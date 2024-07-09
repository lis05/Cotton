#include "nothing.h"

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

#include "../../profiler.h"
#include "api.h"

namespace Cotton::Builtin {

NothingInstance::NothingInstance(Runtime *rt)
    : Instance(rt, sizeof(NothingInstance)) {
    ProfilerCAPTURE();
}

NothingInstance::~NothingInstance() {
    ProfilerCAPTURE();
}

Instance *NothingInstance::copy(Runtime *rt) {
    ProfilerCAPTURE();
    Instance *res = new (rt->alloc(sizeof(NothingInstance))) NothingInstance(rt);

    if (res == NULL) {
        rt->signalError("Failed to copy " + this->shortRepr());
    }
    return res;
}

size_t NothingInstance::getSize() {
    ProfilerCAPTURE();
    return sizeof(NothingInstance);
}

size_t NothingType::getInstanceSize() {
    ProfilerCAPTURE();
    return sizeof(NothingInstance);
}

std::string NothingInstance::shortRepr() {
    ProfilerCAPTURE();
    if (this == NULL) {
        return "NULL";
    }
    return "NothingInstance(id = " + std::to_string(this->id) + ")";
}

void NothingInstance::destroy(Runtime *rt) {
    rt->dealloc(this, sizeof(NothingInstance));
}

static Object *NothingEqAdapter(Object *self, Object *arg, Runtime *rt) {
    ProfilerCAPTURE();
    if (!isTypeObject(arg)) {
        rt->signalError("Right-side object is invalid: " + arg->shortRepr());
    }

    bool i1 = isInstanceObject(self);
    bool i2 = arg->instance != NULL;

    if (i1 && i2) {
        return (arg->type->id == self->type->id) ? rt->protected_true : rt->protected_false;
    }
    else if (!i1 && !i2) {
        return (arg->type->id == self->type->id) ? rt->protected_true : rt->protected_false;
    }
    else {
        return rt->protected_false;
    }
}

static Object *NothingNeqAdapter(Object *self, Object *arg, Runtime *rt) {
    ProfilerCAPTURE();
    auto res = NothingEqAdapter(self, arg, rt);
    return (!getBooleanValue(res, rt)) ? rt->protected_true : rt->protected_false;
}

// TODO: add all operators to function and nothing
NothingType::NothingType(Runtime *rt)
    : Type(rt) {
    ProfilerCAPTURE();
    this->eq_op  = NothingEqAdapter;
    this->neq_op = NothingNeqAdapter;
}

Object *NothingType::create(Runtime *rt) {
    ProfilerCAPTURE();
    Instance *ins = new (rt->alloc(sizeof(NothingInstance))) NothingInstance(rt);
    Object   *obj = newObject(true, ins, this, rt);
    return obj;
}

std::string NothingType::shortRepr() {
    ProfilerCAPTURE();
    if (this == NULL) {
        return "NULL";
    }
    return "NothingType(id = " + std::to_string(this->id) + ")";
}

Object *NothingType::copy(Object *obj, Runtime *rt) {
    ProfilerCAPTURE();
    if (!isTypeObject(obj) || obj->type->id != rt->nothing_type->id) {
        rt->signalError("Failed to copy an invalid object: " + obj->shortRepr());
    }
    if (obj->instance == NULL) {
        return newObject(false, NULL, this, rt);
    }
    auto ins = obj->instance->copy(rt);
    auto res = newObject(true, ins, this, rt);
    return res;
}

Object *makeNothingInstanceObject(Runtime *rt) {
    ProfilerCAPTURE();
    auto res = rt->make(rt->nothing_type, rt->INSTANCE_OBJECT);
    return res;
}
}    // namespace Cotton::Builtin
