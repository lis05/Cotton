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
#include "../back/api.h"
#include "../front/api.h"

namespace Cotton::Builtin {

typedef Object *(*InternalFunction)(const std::vector<Object *> &args, Runtime *rt);

class FunctionInstance: public Instance {
public:
    bool             is_internal;
    InternalFunction internal_ptr;    // function written in C++
    StmtNode        *cotton_ptr;      // function written in Cotton

    FunctionInstance(Runtime *rt, bool on_stack);
    ~FunctionInstance();

    void      init(bool is_internal, InternalFunction internal_ptr, StmtNode *cotton_ptr);
    Instance *copy(Runtime *rt);
    size_t    getSize();
};

class FunctionType: public Type {
public:
    size_t getInstanceSize();
    FunctionType(Runtime *rt);
    ~FunctionType() = default;
    Object *create(Runtime *rt);
};
}    // namespace Cotton::Builtin
