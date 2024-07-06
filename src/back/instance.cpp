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
#include "gc.h"
#include "nameid.h"
#include "object.h"
#include "runtime.h"

namespace Cotton {
int64_t Instance::total_instances = 0;

Object *Cotton::Instance::selectField(int64_t id, Runtime *rt) {
    auto it = this->fields.find(id);
    if (it != this->fields.end()) {
        return it->second;
    }
    rt->signalError(this->shortRepr() + "doesn't have field " + NameId::shortRepr(id));
}

bool Instance::hasField(int64_t id, Runtime *rt) {
    return this->fields.find(id) != this->fields.end();
}

void Instance::addField(int64_t id, Object *obj, Runtime *rt) {
    this->fields[id] = obj;
}

Instance::Instance(Runtime *rt, size_t bytes) {
    this->rt = rt;
    this->gc_mark  = !rt->gc->gc_mark;
    this->id       = ++total_instances;
    rt->gc->track(this, bytes);
}

std::vector<Object *> Instance::getGCReachable() {
    std::vector<Object *> res;
    for (auto &elem : this->fields) {
        res.push_back(elem.second);
    }
    return res;
}
};    // namespace Cotton
