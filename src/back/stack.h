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
#include <cstdint>
#include <cstdlib>
#include <vector>

namespace Cotton {

class Object;
class Instance;
class Type;
class Runtime;

class Stack {
public:
    size_t mem_limit;
    char  *base_ptr;
    char  *cur_ptr;

    std::vector<char *>     frame_ptrs;
    std::vector<Object *>   frame_objects;
    std::vector<Instance *> frame_instances;

    Stack(size_t mem_limit);
    ~Stack();

    void newFrame();
    void popFrame();

    void *alloc(size_t bytes);

    template<class I>
    Instance *allocAndInitInstance(size_t size, Runtime *rt);

    Object *allocAndInitObject(bool is_instance, Instance *instance, Type *type, Runtime *rt);
};

template<class I>
inline Instance *Stack::allocAndInitInstance(size_t size, Runtime *rt) {
    void *ptr = this->alloc(size);
    if (ptr == NULL) {
        return NULL;
    }

    I *res = new (ptr) I(rt);
    this->frame_instances.push_back(res);
    return res;
}
}    // namespace Cotton
