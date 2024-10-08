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

#include "../front/api.h"
#include "../util.h"
#include "nameid.h"

namespace Cotton {

class Object;
class Runtime;
class Type;

typedef Object *(*UnaryOperatorAdapter)(Object *self, Runtime *rt, bool execution_result_matters);
typedef Object *(*BinaryOperatorAdapter)(Object *self, Object *arg, Runtime *rt, bool execution_result_matters);
typedef Object *(*NaryOperatorAdapter)(Object                      *self,
                                       const std::vector<Object *> &args,
                                       Runtime                     *rt,
                                       bool                         execution_result_matters);

/// @brief Class representing a type in Cotton.
class Type {
    friend class Runtime;

public:    // TODOs
    static int64_t total_types;

    // i could make an array of operators, but this is fine
    UnaryOperatorAdapter postinc_op, postdec_op, preinc_op, predec_op, positive_op, negative_op, not_op,
    inverse_op;
    BinaryOperatorAdapter mult_op, div_op, rem_op, rshift_op, lshift_op, add_op, sub_op, lt_op, leq_op, gt_op,
    geq_op, eq_op, neq_op, bitand_op, bitxor_op, bitor_op, and_op, or_op;
    NaryOperatorAdapter call_op, index_op;

    HashTable<NameId, Object *> methods;

public:
    /// @brief id of the type
    int64_t id;

    /// @brief current garbage collector mark of this type
    bool gc_mark : 1;

    /**
     * @brief Construct a new Type object
     *
     * @param rt The runtime. Must be valid.
     */
    Type(Runtime *rt);

    /// @brief Destroy the Type object
    ~Type();

    /**
     * @brief Adds a new method to the type.
     *
     * @param id Nameid of the method.
     * @param method Must be valid.
     */
    void addMethod(NameId id, Object *method);

    /**
     * @brief Returns the requested method. If not found, signals an error.
     *
     * @param id Nameid of the method
     * @param rt The runtime. Must be valid.
     * @return Object*
     */
    Object *getMethod(NameId id, Runtime *rt);

    /**
     * @brief Returns whether the type has a method.
     *
     * @param id Nameid of the method.
     * @return `true` if the type has the method, `false` otherwise
     */
    bool hasMethod(NameId id);

    /**
     * TODO
     * @brief Adds an unary operator to the type. Signals an error if the provided `id` is not of a unary operator.
     *
     * @param id Must be an unary operator.
     * @param op
     */
    void addOperator(OperatorNode::OperatorId id, UnaryOperatorAdapter op);
    void addOperator(OperatorNode::OperatorId id, BinaryOperatorAdapter op);
    void addOperator(OperatorNode::OperatorId id, NaryOperatorAdapter op);

    virtual std::vector<Object *> getGCReachable();
    virtual size_t                getInstanceSize() = 0;    // for placement on stack in case of is_simple

    // creates a valid (non-null) object
    virtual Object *create(Runtime *rt)            = 0;
    // returns a valid (non-null) copy of the object
    virtual Object *copy(Object *obj, Runtime *rt) = 0;

    virtual std::string userRepr(Runtime *rt) = 0;
};

};    // namespace Cotton
