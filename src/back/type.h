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
class Type;

class OperatorAdapter {
public:
    virtual Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) = 0;
};

class Type {
private:
    static int64_t total_types;

public:
    static const int num_operators = OperatorNode::TOTAL_OPERATORS;
    int64_t          id;
    OperatorAdapter *operators[num_operators];
    bool             is_simple : 1;    // otherwise complex
    bool             gc_mark   : 1;
    // if is_simple, then its instances are placed on stack

    __gnu_pbds::cc_hash_table<int64_t, Object *> methods;

    Type(bool is_simple, Runtime *rt);
    ~Type();

    void             addOperator(OperatorNode::OperatorId id, OperatorAdapter *op);
    void             addMethod(int64_t id, Object *method);
    OperatorAdapter *getOperator(OperatorNode::OperatorId id);
    Object          *getMethod(int64_t id);
    bool             hasMethod(int64_t id);

    virtual std::vector<Object *> getGCReachable();
    virtual size_t                getInstanceSize() = 0;    // for placement on stack in case of is_simple

    virtual Object *create(Runtime *rt) = 0;                // creates an empty object
};

namespace MagicMethods {
    int64_t __make__();

    // operators
    int64_t __postinc__();
    int64_t __postdec__();
    int64_t __call__();
    int64_t __index__();
    int64_t __preinc__();
    int64_t __predec__();
    int64_t __positive__();
    int64_t __negative__();
    int64_t __not__();
    int64_t __inverse__();
    int64_t __mult__();
    int64_t __div__();
    int64_t __rem__();
    int64_t __rshift__();
    int64_t __lshift__();
    int64_t __add__();
    int64_t __sub__();
    int64_t __lt__();
    int64_t __leq__();
    int64_t __gt__();
    int64_t __geq__();
    int64_t __eq__();
    int64_t __neq__();
    int64_t __bitand__();
    int64_t __bitxor__();
    int64_t __bitor__();
    int64_t __and__();
    int64_t __or__();

    int64_t getMagicOperator(OperatorNode::OperatorId id);
}    // namespace MagicMethods

};    // namespace Cotton
