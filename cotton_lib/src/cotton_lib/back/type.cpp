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
#include "../profiler.h"
#include "gc.h"
#include "nameid.h"
#include "runtime.h"
#include <string>
#include <vector>

namespace Cotton {

int64_t Type::total_types = 0;

Type::Type(Runtime *rt) {
    ProfilerCAPTURE();
    this->id   = ++Type::total_types;
    postinc_op = postdec_op = preinc_op = predec_op = positive_op = negative_op = not_op = inverse_op = nullptr;

    mult_op = div_op = rem_op = rshift_op = lshift_op = add_op = sub_op = lt_op = leq_op = gt_op = geq_op = eq_op
    = neq_op = bitand_op = bitxor_op = bitor_op = and_op = or_op = nullptr;

    call_op = index_op = nullptr;
    this->gc_mark      = !rt->getGC()->gc_mark;

    rt->getGC()->track(this);
}

Type::~Type() {
    ProfilerCAPTURE();
    this->id   = -1;
    postinc_op = postdec_op = preinc_op = predec_op = positive_op = negative_op = not_op = inverse_op = nullptr;

    mult_op = div_op = rem_op = rshift_op = lshift_op = add_op = sub_op = lt_op = leq_op = gt_op = geq_op = eq_op
    = neq_op = bitand_op = bitxor_op = bitor_op = and_op = or_op = nullptr;

    call_op = index_op = nullptr;
    // we don't do anything else, because the GC will take care of that
}

void Type::addMethod(NameId id, Object *method) {
    ProfilerCAPTURE();
    this->methods[id] = method;
}

Object *Type::getMethod(NameId id, Runtime *rt) {
    ProfilerCAPTURE();
    auto it = this->methods.find(id);
    if (it != this->methods.end()) {
        rt->verifyIsValidObject(it->second);
        return it->second;
    }
    rt->signalError(this->userRepr(rt) + " doesn't have method " + rt->nmgr->getString(id), rt->getContext().area);
}

bool Type::hasMethod(NameId id) {
    ProfilerCAPTURE();
    auto it = this->methods.find(id);
    return it != this->methods.end();
}

std::vector<Object *> Type::getGCReachable() {
    ProfilerCAPTURE();
    std::vector<Object *> res;
    for (auto &elem : this->methods) {
        res.push_back(elem.second);
    }
    return res;
}

};    // namespace Cotton
