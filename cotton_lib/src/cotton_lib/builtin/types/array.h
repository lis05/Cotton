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
#include "../../back/api.h"
#include "../../front/api.h"

namespace Cotton::Builtin {


class ArrayInstance: public Instance {
public:
    std::vector<Object *> data;

    ArrayInstance(Runtime *rt);
    ~ArrayInstance();

    Instance             *copy(Runtime *rt);
    size_t                getSize();
    std::string           userRepr(Runtime *rt);
    std::vector<Object *> getGCReachable();
    void                  spreadSingleUse();
    void                  spreadMultiUse();
};

class ArrayType: public Type {
public:
    size_t getInstanceSize();
    ArrayType(Runtime *rt);
    ~ArrayType() = default;
    Object     *create(Runtime *rt);
    Object     *copy(Object *obj, Runtime *rt);
    std::string userRepr(Runtime *rt);
};

class ArrayIteratorInstance: public Instance {
public:
    Object *array;
    int64_t        position;

    ArrayIteratorInstance(Runtime *rt);
    ~ArrayIteratorInstance();

    Instance             *copy(Runtime *rt);
    size_t                getSize();
    std::string           userRepr(Runtime *rt);
    std::vector<Object *> getGCReachable();
    void                  spreadSingleUse();
    void                  spreadMultiUse();
};

class ArrayIteratorType: public Type {
public:
    size_t getInstanceSize();
    ArrayIteratorType(Runtime *rt);
    ~ArrayIteratorType() = default;
    Object     *create(Runtime *rt);
    Object     *copy(Object *obj, Runtime *rt);
    std::string userRepr(Runtime *rt);
};

void    installArrayIteratorMethods(Type *type, Runtime *rt);
Object *makeArrayIteratorInstanceObject(Object *array, int64_t position, Runtime *rt);

void    installArrayMethods(Type *type, Runtime *rt);
Object *makeArrayInstanceObject(const std::vector<Object *> &data, Runtime *rt);

std::vector<Object *> &getArrayData(Object *obj, Runtime *rt);
std::vector<Object *> &getArrayData(Object *obj, Runtime *rt, Runtime::ContextId ctx_id);
#define getArrayDataFast(obj) (icast(obj->instance, ArrayInstance)->data)
}    // namespace Cotton::Builtin
