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

#include "boolean.h"
#include "../../profiler.h"
#include "api.h"

namespace Cotton::Builtin {
BooleanInstance::BooleanInstance(Runtime *rt)
    : Instance(rt, sizeof(BooleanInstance)) {
    ProfilerCAPTURE();
    this->value = false;
}

BooleanInstance::~BooleanInstance() {
    ProfilerCAPTURE();
}

Instance *BooleanInstance::copy(Runtime *rt) {
    ProfilerCAPTURE();
    Instance *res = new (rt->alloc(sizeof(BooleanInstance))) BooleanInstance(rt);
    if (res == NULL) {
        rt->signalError("Failed to copy " + this->userRepr(), rt->getContext().area);
    }
    icast(res, BooleanInstance)->value = this->value;
    return res;
}

std::string BooleanInstance::userRepr() {
    ProfilerCAPTURE();
    if (this == NULL) {
        return "Boolean(NULL)";
    }
    return std::string("Boolean(value = ") + (this->value ? "true" : "false") + ")";
}

void BooleanInstance::destroy(Runtime *rt) {
    rt->dealloc(this, sizeof(BooleanInstance));
}

size_t BooleanInstance::getSize() {
    ProfilerCAPTURE();
    return sizeof(BooleanInstance);
}

size_t BooleanType::getInstanceSize() {
    ProfilerCAPTURE();
    return sizeof(BooleanInstance);
}

static Object *BooleanNotAdapter(Object *self, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();

    return (getBooleanValue(self, rt)) ? rt->protected_true : rt->protected_false;
}

static Object *BooleanEqAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyIsOfType(self, rt->boolean_type, rt->getContext().sub_areas[0]);
    rt->verifyIsValidObject(arg, rt->getContext().sub_areas[1]);

    if (!rt->isOfType(arg, rt->boolean_type)) {
        return rt->protected_false;
    }

    if (rt->isInstanceObject(self, rt->boolean_type)) {
        if (!rt->isInstanceObject(arg, rt->boolean_type)) {
            return rt->protected_false;
        }
        return rt->protectedBoolean(getBooleanValueFast(self) == getBooleanValueFast(arg));
    }
    else if (rt->isTypeObject(self, rt->boolean_type)) {
        if (!rt->isTypeObject(arg, rt->boolean_type)) {
            return rt->protected_false;
        }
        return rt->protected_true;
    }

    return rt->protected_false;
}

static Object *BooleanNeqAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    auto res = BooleanEqAdapter(self, arg, rt, execution_result_matters);
    return (!getBooleanValue(res, rt)) ? rt->protected_true : rt->protected_false;
}

static Object *BooleanAndAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyIsInstanceObject(self, rt->boolean_type, rt->getContext().sub_areas[0]);
    rt->verifyIsInstanceObject(arg, rt->boolean_type, rt->getContext().sub_areas[1]);

    return rt->protectedBoolean(getBooleanValueFast(self) && getBooleanValueFast(arg));
}

static Object *BooleanOrAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyIsInstanceObject(self, rt->boolean_type, rt->getContext().sub_areas[0]);
    rt->verifyIsInstanceObject(arg, rt->boolean_type, rt->getContext().sub_areas[1]);

    return rt->protectedBoolean(getBooleanValueFast(self) || getBooleanValueFast(arg));
}

// TODO: add all operators to function and nothing
BooleanType::BooleanType(Runtime *rt)
    : Type(rt) {
    ProfilerCAPTURE();

    this->not_op = BooleanNotAdapter;
    this->eq_op  = BooleanEqAdapter;
    this->neq_op = BooleanNeqAdapter;
    this->and_op = BooleanAndAdapter;
    this->or_op  = BooleanOrAdapter;
}

Object *BooleanType::create(Runtime *rt) {
    ProfilerCAPTURE();
    Instance *ins = new (rt->alloc(sizeof(BooleanInstance))) BooleanInstance(rt);
    Object   *obj = newObject(true, ins, this, rt);
    return obj;
}

Object *BooleanType::copy(Object *obj, Runtime *rt) {
    ProfilerCAPTURE();
    rt->verifyIsOfType(obj, rt->boolean_type);
    if (obj->instance == NULL) {
        return newObject(false, NULL, this, rt);
    }
    auto ins = obj->instance->copy(rt);
    auto res = newObject(true, ins, this, rt);
    return res;
}

std::string BooleanType::userRepr() {
    ProfilerCAPTURE();
    if (this == NULL) {
        return "BooleanType(NULL)";
    }
    return "BooleanType";
}

bool &getBooleanValue(Object *obj, Runtime *rt) {
    ProfilerCAPTURE();
    rt->verifyIsInstanceObject(obj, rt->boolean_type);
    return icast(obj->instance, BooleanInstance)->value;
}

bool &getBooleanValue(Object *obj, Runtime *rt, const TextArea &ta) {
    ProfilerCAPTURE();
    rt->verifyIsInstanceObject(obj, rt->boolean_type, ta);
    return icast(obj->instance, BooleanInstance)->value;
}

Object *makeBooleanInstanceObject(bool value, Runtime *rt) {
    ProfilerCAPTURE();
    auto res                                     = rt->make(rt->boolean_type, Runtime::INSTANCE_OBJECT);
    icast(res->instance, BooleanInstance)->value = value;
    return res;
}

}    // namespace Cotton::Builtin
