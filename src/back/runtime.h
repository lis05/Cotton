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
#include "mempool.h"
#include "object.h"
#include <ext/pb_ds/assoc_container.hpp>
#include <string>
#include <vector>

namespace Cotton {
class Scope;
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
    Runtime(GCStrategy *gc_strategy, ErrorManager *error_manager);
    ~Runtime() = default;

    Builtin::NothingType   *nothing_type;
    Builtin::BooleanType   *boolean_type;
    Builtin::FunctionType  *function_type;
    Builtin::IntegerType   *integer_type;
    Builtin::RealType      *real_type;
    Builtin::CharacterType *character_type;
    Builtin::StringType    *string_type;

    PoolAllocator                                     *object_allocator;
    __gnu_pbds::cc_hash_table<size_t, PoolAllocator *> allocators;
    PoolAllocator                                     *array_of_allocators[4096];    // for small objects

    PoolAllocator *getAllocator(size_t size);

    void *alloc(size_t size);
    void  dealloc(void *ptr, size_t size);

    void destroy(Object *obj);
    void destroy(Instance *ins);

    Object                                      *exec_res_default;
    __gnu_pbds::cc_hash_table<int64_t, Object *> readonly_literals;

    Object *protected_nothing;
    Object *protected_true;
    Object *protected_false;

    Scope *scope;
    // creates a new scope
    void   newFrame(bool can_access_prev_scope = true);
    // pops the last scope
    void   popFrame();

    GC *gc;

    ErrorManager *error_manager;
    Token        *current_token;
    // signals an error
    [[noreturn]]
    void signalError(const std::string &message, bool include_token = true);

    uint8_t execution_flags;

    Object *execute(ExprNode *node);
    Object *execute(FuncDefNode *node);
    Object *execute(TypeDefNode *node);
    Object *execute(OperatorNode *node);
    Object *execute(AtomNode *node);
    Object *execute(ParExprNode *node);
    Object *execute(StmtNode *node);
    Object *execute(WhileStmtNode *node);
    Object *execute(ForStmtNode *node);
    Object *execute(IfStmtNode *node);
    Object *execute(ReturnStmtNode *node);
    Object *execute(BlockStmtNode *node);

    enum ObjectOptions { INSTANCE_OBJECT, TYPE_OBJECT };

    // makes an object of given type based on object_opt; calls __make__ if object_opt is INSTANCE_OBJECT
    // if fails, signals an error. therefore, returns a valid object (non-null, satisfies object_opt)
    Object *make(Type *type, ObjectOptions object_opt);
    // returns a copy of obj (type is the same; if instance is present, it is copied)
    // if fails, signals an error. therefore, returns a valid object (non-null, is a copy of obj)
    Object *copy(Object *obj);
    // runs operator on the object. returns a valid object(non-null); if fails, signals an error
    Object *runOperator(OperatorNode::OperatorId id, Object *obj);
    Object *runOperator(OperatorNode::OperatorId id, Object *obj, Object *arg);
    Object *runOperator(OperatorNode::OperatorId id, Object *obj, const std::vector<Object *> &args);
    // runs method on the object. returns a valid object(non-null); if fails, signals an error
    Object *runMethod(int64_t id, Object *obj, const std::vector<Object *> &args);
};

#define newObject(is_instance, instance, type, rt)                                                                \
    new (rt->object_allocator->allocate(sizeof(Object))) Object(is_instance, instance, type, rt)

#define highlight(rt, token) rt->current_token = token;

// checks whether obj is an instance object (is non-NULL and has non-NULL type and non-NULL instance)
#define isInstanceObject(obj)             ((obj) != NULL && (obj)->instance != NULL && (obj)->type != NULL)
#define isInstanceObjectOfType(obj, type) ((obj) != NULL && (obj)->instance != NULL && (obj)->type == (type))
// checks whether obj is a type object (is non-NULL and has non-NULL type)
#define isTypeObject(obj)                 (obj != NULL && (obj)->type != NULL)

#define setExecFlagNONE(rt)        rt->execution_flags = 0;
#define setExecFlagCONTINUE(rt)    rt->execution_flags = 1;
#define setExecFlagBREAK(rt)       rt->execution_flags = 2;
#define setExecFlagRETURN(rt)      rt->execution_flags = 4;
#define setExecFlagDIRECT_PASS(rt) rt->execution_flags = 8;

#define isExecFlagNONE(rt)        (rt->execution_flags == 0)
#define isExecFlagCONTINUE(rt)    (rt->execution_flags == 1)
#define isExecFlagBREAK(rt)       (rt->execution_flags == 2)
#define isExecFlagRETURN(rt)      (rt->execution_flags == 4)
#define isExecFlagDIRECT_PASS(rt) (rt->execution_flags == 8)

// casts instance to another instance
#define icast(ins, type) ((type *)ins)
}    // namespace Cotton
