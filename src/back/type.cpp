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

#include "type.h"
#include "gc.h"
#include "nameid.h"
#include "runtime.h"

namespace Cotton {

int64_t Type::total_types = 0;

Type::Type(bool is_simple, Runtime *rt) {
    this->id = ++Type::total_types;
    for (auto &op : this->unary_operators) {
        op = NULL;
    }
    for (auto &op : this->binary_operators) {
        op = NULL;
    }
    for (auto &op : this->nary_operators) {
        op = NULL;
    }
    this->is_simple = is_simple;
    this->gc_mark   = !rt->gc->gc_mark;
}

Type::~Type() {
    this->id = -1;
    for (auto &op : this->unary_operators) {
        delete op;
        op = NULL;
    }
    for (auto &op : this->binary_operators) {
        delete op;
        op = NULL;
    }
    for (auto &op : this->nary_operators) {
        delete op;
        op = NULL;
    }
    // we don't do anything else, because the GC will take care of that
}

void Type::addOperator(OperatorNode::OperatorId id, UnaryOperatorAdapter *unary_op) {
    this->unary_operators[id] = unary_op;
}

void Type::addOperator(OperatorNode::OperatorId id, BinaryOperatorAdapter *binary_op) {
    this->binary_operators[id] = binary_op;
}

void Type::addOperator(OperatorNode::OperatorId id, NaryOperatorAdapter *nary_op) {
    this->nary_operators[id] = nary_op;
}

void Type::addMethod(int64_t id, Object *method) {
    this->methods[id] = method;
}

inline UnaryOperatorAdapter *Type::getUnaryOperator(OperatorNode::OperatorId id) {
    return this->unary_operators[id];
}

inline BinaryOperatorAdapter *Type::getBinaryOperator(OperatorNode::OperatorId id) {
    return this->binary_operators[id];
}

inline NaryOperatorAdapter *Type::getNaryOperator(OperatorNode::OperatorId id) {
    return this->nary_operators[id];
}

inline Object *Type::getMethod(int64_t id) {
    auto it = this->methods.find(id);
    if (it != this->methods.end()) {
        return it->second;
    }
    return NULL;
}

std::vector<Object *> Type::getGCReachable() {
    std::vector<Object *> res;
    for (auto &elem : this->methods) {
        res.push_back(elem.second);
    }
    return res;
}
};    // namespace Cotton
