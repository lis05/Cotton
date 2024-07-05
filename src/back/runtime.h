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

namespace Builtin {
    class NothingType;
    class BooleanType;
    class FunctionType;
    class IntegerType;
    class RealType;
    class CharacterType;
    class StringType;
}    // namespace Builtin

class Runtime {
public:
    Runtime(size_t stack_size, GCStrategy *gc_strategy, ErrorManager *error_manager);
    ~Runtime() = default;

    Builtin::NothingType   *nothing_type;
    Builtin::BooleanType   *boolean_type;
    Builtin::FunctionType  *function_type;
    Builtin::IntegerType   *integer_type;
    Builtin::RealType      *real_type;
    Builtin::CharacterType *character_type;
    Builtin::StringType    *string_type;

    Scope *scope;
    // creates a new scope, as well as a new stack frame
    void   newFrame(bool can_access_prev_scope = true);
    // pops the last scope, as well as clears the last stack frame
    void   popFrame();

    Stack *stack;
    GC    *gc;

    // checks whether obj is an instance object (is non-NULL and has non-NULL type and non-NULL instance)
    bool isInstanceObject(Object *obj);
    bool isInstanceObject(Object *obj, Type *type);
    // checks whether obj is a type object (is non-NULL and has non-NULL type)
    bool isTypeObject(Object *obj);

    ErrorManager *error_manager;
    Token        *current_token;
    // marks token as the current token
    void          highlight(Token *token);
    // signals an error
    [[noreturn]]
    void signalError(const std::string &message, bool include_token = true);

    class ExecutionResult {
    public:
        static uint8_t CONTINUE;
        static uint8_t BREAK;
        static uint8_t RETURN;
        static uint8_t DIRECT_PASS;
        uint8_t        flags;
        Object        *result;
        Object *caller;

        ExecutionResult(uint8_t flags, Object *result, Object *caller = NULL);
    };

    ExecutionResult execute(ExprNode *node);
    ExecutionResult execute(FuncDefNode *node);
    ExecutionResult execute(TypeDefNode *node);
    ExecutionResult execute(OperatorNode *node);
    ExecutionResult execute(AtomNode *node);
    ExecutionResult execute(ParExprNode *node);
    ExecutionResult execute(StmtNode *node);
    ExecutionResult execute(WhileStmtNode *node);
    ExecutionResult execute(ForStmtNode *node);
    ExecutionResult execute(IfStmtNode *node);
    ExecutionResult execute(ReturnStmtNode *node);
    ExecutionResult execute(BlockStmtNode *node);

    enum ObjectOptions { INSTANCE_OBJECT, TYPE_OBJECT };

    // makes an object of given type based on object_opt; calls __make__ if object_opt is INSTANCE_OBJECT
    // if fails, signals an error. therefore, returns a valid object (non-null, satisfies object_opt)
    Object *make(Type *type, ObjectOptions object_opt);
    // returns a copy of obj (type is the same; if instance is present, it is copied)
    // if fails, signals an error. therefore, returns a valid object (non-null, is a copy of obj)
    Object *copy(Object *obj);
    // runs operator on the object. returns a valid object(non-null); if fails, signals an error
    Object *runOperator(OperatorNode::OperatorId id, Object *obj, const std::vector<Object *> &args);
    // runs method on the object. returns a valid object(non-null); if fails, signals an error
    Object *runMethod(int64_t id, Object *obj, const std::vector<Object *> &args);
};

// tries to create instance (on stack first if try_on_stack)

#define createInstance(rt, try_on_stack, I)                                                                       \
    ({                                                                                                            \
        Instance *ins = NULL;                                                                                     \
        if (try_on_stack)                                                                                         \
            ins = rt->stack->allocAndInitInstance<I>(sizeof(I), rt);                                              \
        if (ins == NULL) {                                                                                        \
            ins = new (std::nothrow) I(rt, false);                                                                \
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

// casts instance to another instance
#define icast(ins, type) ((type *)ins)
}    // namespace Cotton
