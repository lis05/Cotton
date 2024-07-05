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

#include "stack.h"
#include "instance.h"
#include "object.h"
#include <cstdlib>

namespace Cotton {

Stack::Stack(size_t mem_limit) {
    this->mem_limit = mem_limit;
    this->base_ptr  = (char *)malloc(this->mem_limit);
    this->cur_ptr   = this->base_ptr;
}

Stack::~Stack() {
    while (!this->frame_ptrs.empty()) {
        this->popFrame();
    }

    if (this->base_ptr != NULL) {
        free(this->base_ptr);
    }
    this->base_ptr = NULL;
    this->cur_ptr  = NULL;
}

void Stack::newFrame() {
    this->frame_ptrs.push_back(this->cur_ptr);
    this->frame_objects.push_back({});
    this->frame_instances.push_back({});
}

void Stack::popFrame() {
    if (this->frame_ptrs.empty()) {
        this->cur_ptr = NULL;
        return;
    }
    for (auto &obj : this->frame_objects.back()) {
        if (obj != NULL) {
            obj->~Object();
        }
    }
    for (auto &ins : this->frame_instances.back()) {
        if (ins != NULL) {
            ins->~Instance();
        }
    }
    this->cur_ptr = this->frame_ptrs.back();
    this->frame_ptrs.pop_back();
    this->frame_objects.pop_back();
    this->frame_instances.pop_back();
}

void *Stack::alloc(size_t size) {
    int64_t free_space = (int64_t)this->mem_limit - ((int64_t)this->cur_ptr - (int64_t)this->base_ptr);
    if (size <= free_space) {
        void *res      = this->cur_ptr;
        this->cur_ptr += size;
        return res;
    }
    else {
        return NULL;
    }
}

Object *Stack::allocAndInitObject(bool is_instance, Instance *instance, Type *type, Runtime *rt) {
    void *ptr = this->alloc(sizeof(Object));
    if (ptr == NULL) {
        return NULL;
    }

    Object *res = new (ptr) Object(is_instance, true, instance, type, rt);
    this->frame_objects.back().push_back(res);
    return res;
}

}    // namespace Cotton
