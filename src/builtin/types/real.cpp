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
RealInstance::RealInstance(Runtime *rt)
    : Instance(rt, sizeof(RealInstance)) {
    ProfilerCAPTURE();
    this->value = 0;
}

RealInstance::~RealInstance() {
    ProfilerCAPTURE();
}

Instance *RealInstance::copy(Runtime *rt) {
    ProfilerCAPTURE();
    Instance *res = new (rt->alloc(sizeof(RealInstance))) RealInstance(rt);

    if (res == NULL) {
        rt->signalError("Failed to copy " + this->userRepr());
    }
    icast(res, RealInstance)->value = this->value;
    return res;
}

std::string RealInstance::userRepr() {
    ProfilerCAPTURE();
    if (this == NULL) {
        return "Real(NULL)";
    }
    return "Real(value = " + std::to_string(this->value) + ")";
}

void RealInstance::destroy(Runtime *rt) {
    rt->dealloc(this, sizeof(RealInstance));
}

size_t RealInstance::getSize() {
    ProfilerCAPTURE();
    return sizeof(RealInstance);
}

size_t RealType::getInstanceSize() {
    ProfilerCAPTURE();
    return sizeof(RealInstance);
}

static Object *RealPositiveAdapter(Object *self, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();

    if (!isInstanceObject(self)) {
        rt->signalError(self->userRepr() + " does not support that operator");
    }

    if (!execution_result_matters) {
        return NULL;
    }

    auto res = self->type->copy(self, rt);
    return res;
}

static Object *RealNegativeAdapter(Object *self, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();

    if (!isInstanceObject(self)) {
        rt->signalError(self->userRepr() + " does not support that operator");
    }

    if (!execution_result_matters) {
        return NULL;
    }

    auto res               = self->type->copy(self, rt);
    getRealValueFast(res) *= -1;
    return res;
}

static Object *RealMultAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();

    if (!isInstanceObject(self)) {
        rt->signalError(self->userRepr() + " does not support that operator");
    }

    if (!isTypeObject(arg)) {
        rt->signalError("Right-side object is invalid: " + arg->userRepr());
    }
    if (arg->instance == NULL || arg->type->id != rt->real_type->id) {
        rt->signalError("Right-side object " + arg->userRepr() + " must be a Real instance object");
    }

    if (!execution_result_matters) {
        return NULL;
    }

    auto res = makeRealInstanceObject(getRealValueFast(self) * getRealValueFast(arg), rt);
    return res;
}

static Object *RealDivAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();

    if (!isInstanceObject(self)) {
        rt->signalError(self->userRepr() + " does not support that operator");
    }

    if (!isTypeObject(arg)) {
        rt->signalError("Right-side object is invalid: " + arg->userRepr());
    }
    if (arg->instance == NULL || arg->type->id != rt->real_type->id) {
        rt->signalError("Right-side object " + arg->userRepr() + " must be a Real instance object");
    }

    if (!execution_result_matters) {
        return NULL;
    }

    auto res = makeRealInstanceObject(getRealValueFast(self) / getRealValueFast(arg), rt);
    return res;
}

static Object *RealAddAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();

    if (!isInstanceObject(self)) {
        rt->signalError(self->userRepr() + " does not support that operator");
    }

    if (!isTypeObject(arg)) {
        rt->signalError("Right-side object is invalid: " + arg->userRepr());
    }
    if (arg->instance == NULL || arg->type->id != rt->real_type->id) {
        rt->signalError("Right-side object " + arg->userRepr() + " must be a Real instance object");
    }

    if (!execution_result_matters) {
        return NULL;
    }

    auto res = makeRealInstanceObject(getRealValueFast(self) + getRealValueFast(arg), rt);
    return res;
}

static Object *RealSubAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();

    if (!isInstanceObject(self)) {
        rt->signalError(self->userRepr() + " does not support that operator");
    }

    if (!isTypeObject(arg)) {
        rt->signalError("Right-side object is invalid: " + arg->userRepr());
    }
    if (arg->instance == NULL || arg->type->id != rt->real_type->id) {
        rt->signalError("Right-side object " + arg->userRepr() + " must be a Real instance object");
    }

    if (!execution_result_matters) {
        return NULL;
    }

    auto res = makeRealInstanceObject(getRealValueFast(self) - getRealValueFast(arg), rt);
    return res;
}

static Object *RealLtAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();

    if (!isInstanceObject(self)) {
        rt->signalError(self->userRepr() + " does not support that operator");
    }

    if (!isTypeObject(arg)) {
        rt->signalError("Right-side object is invalid: " + arg->userRepr());
    }
    if (arg->instance == NULL || arg->type->id != rt->real_type->id) {
        rt->signalError("Right-side object " + arg->userRepr() + " must be a Real instance object");
    }

    if (!execution_result_matters) {
        return NULL;
    }

    return (getRealValueFast(self) < getRealValueFast(arg)) ? rt->protected_true : rt->protected_false;
}

static Object *RealLeqAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();

    if (!isInstanceObject(self)) {
        rt->signalError(self->userRepr() + " does not support that operator");
    }

    if (!isTypeObject(arg)) {
        rt->signalError("Right-side object is invalid: " + arg->userRepr());
    }
    if (arg->instance == NULL || arg->type->id != rt->real_type->id) {
        rt->signalError("Right-side object " + arg->userRepr() + " must be a Real instance object");
    }

    return (getRealValueFast(self) <= getRealValueFast(arg)) ? rt->protected_true : rt->protected_false;
}

static Object *RealGtAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();

    if (!isInstanceObject(self)) {
        rt->signalError(self->userRepr() + " does not support that operator");
    }

    if (!isTypeObject(arg)) {
        rt->signalError("Right-side object is invalid: " + arg->userRepr());
    }
    if (arg->instance == NULL || arg->type->id != rt->real_type->id) {
        rt->signalError("Right-side object " + arg->userRepr() + " must be a Real instance object");
    }

    return (getRealValueFast(self) > getRealValueFast(arg)) ? rt->protected_true : rt->protected_false;
}

static Object *RealGeqAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();

    if (!isInstanceObject(self)) {
        rt->signalError(self->userRepr() + " does not support that operator");
    }

    if (!isTypeObject(arg)) {
        rt->signalError("Right-side object is invalid: " + arg->userRepr());
    }
    if (arg->instance == NULL || arg->type->id != rt->real_type->id) {
        rt->signalError("Right-side object " + arg->userRepr() + " must be a Real instance object");
    }

    return (getRealValueFast(self) >= getRealValueFast(arg)) ? rt->protected_true : rt->protected_false;
}

static Object *RealEqAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();

    if (!isTypeObject(arg)) {
        rt->signalError("Right-side object is invalid: " + arg->userRepr());
    }

    bool i1 = isInstanceObject(self);
    bool i2 = arg->instance != NULL;

    if (i1 && i2) {
        if (self->type->id != arg->type->id) {
            return rt->protected_false;
        }
        return (getRealValueFast(self) == getRealValueFast(arg)) ? rt->protected_true : rt->protected_false;
    }
    else if (!i1 && !i2) {
        return (self->type->id == arg->type->id) ? rt->protected_true : rt->protected_false;
    }
    else {
        return rt->protected_false;
    }
}

static Object *RealNeqAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    auto res = RealEqAdapter(self, arg, rt, execution_result_matters);
    return (!getBooleanValueFast(res)) ? rt->protected_true : rt->protected_false;
}

// TODO: add all operators to function and nothing
RealType::RealType(Runtime *rt)
    : Type(rt) {
    ProfilerCAPTURE();
    this->positive_op = RealPositiveAdapter;
    this->negative_op = RealNegativeAdapter;
    this->mult_op     = RealMultAdapter;
    this->div_op      = RealDivAdapter;
    this->add_op      = RealAddAdapter;
    this->sub_op      = RealSubAdapter;
    this->lt_op       = RealLtAdapter;
    this->leq_op      = RealLeqAdapter;
    this->gt_op       = RealGtAdapter;
    this->geq_op      = RealGeqAdapter;
    this->eq_op       = RealEqAdapter;
    this->neq_op      = RealNeqAdapter;
}

Object *RealType::create(Runtime *rt) {
    ProfilerCAPTURE();
    Instance *ins = new (rt->alloc(sizeof(RealInstance))) RealInstance(rt);
    Object   *obj = newObject(true, ins, this, rt);
    return obj;
}

Object *RealType::copy(Object *obj, Runtime *rt) {
    ProfilerCAPTURE();
    if (!isTypeObject(obj) || obj->type->id != rt->real_type->id) {
        rt->signalError("Failed to copy an invalid object: " + obj->userRepr());
    }
    if (obj->instance == NULL) {
        return newObject(false, NULL, this, rt);
    }
    auto ins = obj->instance->copy(rt);
    auto res = newObject(true, ins, this, rt);
    return res;
}

std::string RealType::userRepr() {
    ProfilerCAPTURE();
    if (this == NULL) {
        return "RealType(NULL)";
    }
    return "RealType";
}

double &getRealValue(Object *obj, Runtime *rt) {
    ProfilerCAPTURE();
    if (!isInstanceObject(obj)) {
        rt->signalError(obj->userRepr() + " is not an instance object");
    }
    if (obj->type->id != rt->real_type->id) {
        rt->signalError(obj->userRepr() + " is not Real");
    }
    return icast(obj->instance, RealInstance)->value;
}

Object *makeRealInstanceObject(double value, Runtime *rt) {
    ProfilerCAPTURE();
    auto res                                  = rt->make(rt->real_type, Runtime::INSTANCE_OBJECT);
    icast(res->instance, RealInstance)->value = value;
    return res;
}

}    // namespace Cotton::Builtin
