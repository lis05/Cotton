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

#include "instance.h"
#include "../profiler.h"
#include "gc.h"
#include "nameid.h"
#include "object.h"
#include "runtime.h"

namespace Cotton {
int64_t Instance::total_instances = 0;

Object *Cotton::Instance::selectField(int64_t id, Runtime *rt) {
    ProfilerCAPTURE();
    rt->signalError(this->userRepr(rt) + "doesn't have field " + rt->nds->userRepr(id), rt->getContext().area);
}

bool Instance::hasField(int64_t id, Runtime *rt) {
    ProfilerCAPTURE();
    return false;
}

void Instance::addField(int64_t id, Object *obj, Runtime *rt) {
    ProfilerCAPTURE();
}

Instance::Instance(Runtime *rt, size_t bytes) {
    ProfilerCAPTURE();
    this->gc_mark = !rt->gc->gc_mark;
    this->id      = ++total_instances;
    rt->gc->track(this, bytes);
}

std::vector<Object *> Instance::getGCReachable() {
    ProfilerCAPTURE();
    return {};
}

void Instance::spreadSingleUse() {
    ProfilerCAPTURE();
}

void Instance::spreadMultiUse() {
    ProfilerCAPTURE();
}
};    // namespace Cotton
