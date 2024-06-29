/*
 Copyright (c) 2024 Ihor Lukianov (lis05)

 Permission is hereby granted, free of charge, to any person obtaining a copy of
 this software and associated documentation files (the "Software"), to deal in
 the Software without restriction, including without limitation the rights to
 use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 the Software, and to permit persons to whom the Software is furnished to do so,
 subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once

#include "../front/parser.h"
#include "object.h"
#include "stack.h"
#include <string>
#include <vector>

namespace Cotton {
class Scope;
class Stack;
class GC;
class Object;
class Instance;
class Type;
class ErrorManager;
class Token;
class ExprNode;
class StmtNode;
class GCStrategy;

class Runtime {
public:
    Runtime(size_t stack_size, GCStrategy *gc_strategy, ErrorManager *error_manager);
    ~Runtime() = default;

    Scope *scope;
    void   newScope();
    void   popScope();

    Stack *stack;
    GC    *gc;

    ErrorManager                                *error_manager;
    std::vector<std::pair<std::string, Token *>> error_messages;
    Token                                       *current_token;
    void                                         highlight(Token *token);
    void                                         signalSubError(const std::string &message);    // doesn't exit
    void                                         signalError(const std::string &message);    // exits immidiately

    class ExecutionResult {
    public:
        bool    success;
        Object *res;

        bool directly_passed : 1;    // if passed via @A operator

        ExecutionResult(bool success, Object *res, bool directly_passed);
        ~ExecutionResult() = default;
    };

    ExecutionResult execute(ExprNode *expr);
    ExecutionResult execute(StmtNode *stmt);

    // check for null
    bool validate(Object *obj);
    bool validate(Instance *ins);
    bool validate(Type *type);

    // if copied on heap, adds to the GC
    // if errors happen, reports them
    Object *copy(Object *obj);

    // if on heap, removes from GC
    void destroy(Object *obj);

    // crates a new object, if needed adds to the GC, and calles __make__
    Object *make(Type *type);

    Object *runOperator(OperatorNode::OperatorId id, Object *obj, std::vector<Object *> args);
    Object *runMethod(int64_t id, Object *obj, std::vector<Object *> args);
};

// tries to create instance (on stack first if try_on_stack)

#define createInstance(rt, try_on_stack, I)                                                                       \
    ({                                                                                                            \
        Instance *ins = NULL;                                                                                     \
        if (try_on_stack)                                                                                         \
            ins = rt->stack->allocAndInitInstance<I>(sizeof(I), rt);                                              \
        if (ins == NULL) {                                                                                        \
            ins = new (std::nothrow) I(rt);                                                                       \
            if (ins == NULL) {                                                                                    \
                return NULL;                                                                                      \
            }                                                                                                     \
            ins->on_stack = false;                                                                                \
        }                                                                                                         \
        else {                                                                                                    \
            ins->on_stack = true;                                                                                 \
        }                                                                                                         \
        ins;                                                                                                      \
    })

// tries to create object (on stack first if try_on_stack)
#define createObject(rt, is_instance, ins, type, try_on_stack)                                                    \
    ({                                                                                                            \
        Object *obj = NULL;                                                                                       \
        if (try_on_stack)                                                                                         \
            obj = rt->stack->allocAndInitObject(is_instance, ins, type, rt);                                      \
        if (obj == NULL) {                                                                                        \
            obj = new (std::nothrow) Object(is_instance, false, ins, type, rt);                                   \
            if (ins == NULL) {                                                                                    \
                return NULL;                                                                                      \
            }                                                                                                     \
            obj->on_stack = false;                                                                                \
        }                                                                                                         \
        else {                                                                                                    \
            obj->on_stack = true;                                                                                 \
        }                                                                                                         \
        obj;                                                                                                      \
    })

}    // namespace Cotton
