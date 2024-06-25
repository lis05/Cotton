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
#include <cstdint>
#include <ext/pb_ds/assoc_container.hpp>
#include <vector>

namespace Cotton {

class Object;
class Runtime;
class NameId;

class UnaryOperatorAdapter {
public:
    virtual Object *operator()(Object *self, Runtime *rt) = 0;
};

class BinaryOperatorAdapter {
public:
    virtual Object *operator()(Object *self, Object *other, Runtime *rt) = 0;
};

class NaryOperatorAdapter {
public:
    virtual Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) = 0;
};

class Type {
private:
    static int64_t total_types;

public:
    static const int       num_operators = OperatorNode::TOTAL_OPERATORS;
    int64_t                id;
    UnaryOperatorAdapter  *unary_operators[num_operators];
    BinaryOperatorAdapter *binary_operators[num_operators];
    NaryOperatorAdapter   *nary_operators[num_operators];
    bool                   is_simple;    // otherwise complex
    // if is_simple, then its instances are placed on stack

    __gnu_pbds::cc_hash_table<int64_t, Object *> methods;

    Type(bool is_simple);
    ~Type();

    void addOperator(OperatorNode::OperatorId id, UnaryOperatorAdapter *unary_op);
    void addOperator(OperatorNode::OperatorId id, BinaryOperatorAdapter *binary_op);
    void addOperator(OperatorNode::OperatorId id, NaryOperatorAdapter *nary_op);
    void addMethod(int64_t id, Object *method);

    inline UnaryOperatorAdapter  *getUnaryOperator(OperatorNode::OperatorId id);
    inline BinaryOperatorAdapter *getBinaryOperator(OperatorNode::OperatorId id);
    inline NaryOperatorAdapter   *getNaryOperator(OperatorNode::OperatorId id);
    inline Object                *getMethod(int64_t id);

    virtual std::vector<Object *> getGCReachable();
    virtual size_t                getIntanceSize() = 0;    // for placement on stack in case of is_simple
};

};    // namespace Cotton
