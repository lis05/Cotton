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

#include "array.h"
#include "../../profiler.h"
#include "api.h"

namespace Cotton::Builtin {
ArrayInstance::ArrayInstance(Runtime *rt)
    : Instance(rt, sizeof(ArrayInstance)) {
    ProfilerCAPTURE();
    this->data = {};
}

ArrayInstance::~ArrayInstance() {
    ProfilerCAPTURE();
}

Instance *ArrayInstance::copy(Runtime *rt) {
    ProfilerCAPTURE();
    Instance *res = new (rt->alloc(sizeof(ArrayInstance))) ArrayInstance(rt);

    if (res == NULL) {
        rt->signalError("Failed to copy " + this->userRepr());
    }
    for (auto obj : this->data) {
        ((ArrayInstance *)res)->data.push_back(rt->copy(obj));
    }
    return res;
}

std::string ArrayInstance::userRepr() {
    ProfilerCAPTURE();
    if (this == NULL) {
        return "Array(NULL)";
    }
    return "ArrayInstance(size = " + std::to_string(this->data.size()) + ", data = ...)";
}

void ArrayInstance::destroy(Runtime *rt) {
    rt->dealloc(this, sizeof(ArrayInstance));
}

size_t ArrayInstance::getSize() {
    ProfilerCAPTURE();
    return sizeof(ArrayInstance);
}

std::vector<Object *> ArrayInstance::getGCReachable() {
    ProfilerCAPTURE();
    auto res = Instance::getGCReachable();
    for (auto obj : this->data) {
        res.push_back(obj);
    }
    return res;
}

void ArrayInstance::spreadSingleUse() {
    for (auto obj : this->data) {
        obj->spreadSingleUse();
    }
}

void ArrayInstance::spreadMultiUse() {
    for (auto obj : this->data) {
        obj->spreadMultiUse();
    }
}

size_t ArrayType::getInstanceSize() {
    ProfilerCAPTURE();
    return sizeof(ArrayInstance);
}

static Object *
ArrayIndexAdapter(Object *self, const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();

    if (!isInstanceObject(self)) {
        rt->signalError(self->userRepr() + " does not support that operator");
    }
    if (args.size() != 1) {
        rt->signalError("Expected exactly one right-side argument");
        return NULL;
    }
    auto &arg = args[0];
    if (!isTypeObject(arg)) {
        rt->signalError("Right-side object is invalid: " + arg->userRepr());
    }
    if (arg->instance == NULL || arg->type->id != rt->integer_type->id) {
        rt->signalError("Index " + arg->userRepr() + " must be an integer instance object");
    }
    if (!(0 <= getIntegerValueFast(arg) && getIntegerValueFast(arg) < getArrayDataFast(self).size())) {
        rt->signalError("Index " + arg->userRepr() + " is out of string " + self->userRepr() + " range");
    }
    return getArrayDataFast(self)[getIntegerValueFast(arg)];
}

static Object *ArrayEqAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();

    if (!isTypeObject(arg)) {
        rt->signalError("Right-side object is invalid: " + arg->userRepr());
    }

    if (!execution_result_matters) {
        return NULL;
    }

    bool i1 = isInstanceObject(self);
    bool i2 = arg->instance != NULL;

    if (i1 && i2) {
        if (self->type->id != arg->type->id) {
            return rt->protected_false;
        }
        if (getArrayDataFast(self).size() != getArrayDataFast(arg).size()) {
            return rt->protected_false;
        }
        for (int i = 0; i < getArrayDataFast(self).size(); i++) {
            if (!rt->runOperator(OperatorNode::EQUAL, getArrayDataFast(self)[i], getArrayDataFast(arg)[i], true)) {
                return rt->protected_false;
            }
        }
        return rt->protected_true;
    }
    else if (!i1 && !i2) {
        return (self->type->id == arg->type->id) ? rt->protected_true : rt->protected_false;
    }
    else {
        return rt->protected_false;
    }
}

static Object *ArrayNeqAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    auto res = ArrayEqAdapter(self, arg, rt, execution_result_matters);
    return (!getBooleanValueFast(res)) ? rt->protected_true : rt->protected_false;
}

static Object *arraySizeMethod(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    if (args.size() != 1) {
        rt->signalError("Expected a caller object");
    }
    auto self = args[0];
    if (!isTypeObject(self)) {
        rt->signalError("Caller is invalid: " + self->userRepr());
    }
    if (self->instance == NULL || self->type->id != rt->array_type->id) {
        rt->signalError("Caller must be an Array instance object: " + self->userRepr());
    }

    if (!execution_result_matters) {
        return NULL;
    }

    return makeIntegerInstanceObject(getArrayDataFast(self).size(), rt);
}

static Object *arrayResizeMethod(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    if (args.size() != 2) {
        rt->signalError("Expected an argument");
    }
    auto self     = args[0];
    auto new_size = args[0];
    if (!isInstanceObjectOfType(self, rt->array_type)) {
        rt->signalError("Caller must be an Array instance object: " + self->userRepr());
    }
    if (!isInstanceObjectOfType(new_size, rt->integer_type)) {
        rt->signalError("New size must be an Integer instance object: " + new_size->userRepr());
    }

    int64_t oldn = getArrayDataFast(self).size();
    int64_t newn = getIntegerValueFast(new_size);
    getArrayDataFast(self).resize(newn);

    for (int64_t i = oldn; i < newn; i++) {
        getArrayDataFast(self)[i] = makeNothingInstanceObject(rt);
    }

    return rt->protected_nothing;
}

// TODO: add all operators to function and nothing
ArrayType::ArrayType(Runtime *rt)
    : Type(rt) {
    ProfilerCAPTURE();
    this->index_op = ArrayIndexAdapter;
    this->eq_op    = ArrayEqAdapter;
    this->neq_op   = ArrayNeqAdapter;

    this->addMethod(NameId("size").id, makeFunctionInstanceObject(true, arraySizeMethod, NULL, rt));
    this->addMethod(NameId("resize").id, makeFunctionInstanceObject(true, arrayResizeMethod, NULL, rt));
}

Object *ArrayType::create(Runtime *rt) {
    ProfilerCAPTURE();
    Instance *ins = new (rt->alloc(sizeof(ArrayInstance))) ArrayInstance(rt);
    Object   *obj = newObject(true, ins, this, rt);
    return obj;
}

Object *ArrayType::copy(Object *obj, Runtime *rt) {
    ProfilerCAPTURE();
    if (!isTypeObject(obj) || obj->type->id != rt->array_type->id) {
        rt->signalError("Failed to copy an invalid object: " + obj->userRepr());
    }
    if (obj->instance == NULL) {
        return newObject(false, NULL, this, rt);
    }
    auto ins = obj->instance->copy(rt);
    auto res = newObject(true, ins, this, rt);
    return res;
}

std::string ArrayType::userRepr() {
    ProfilerCAPTURE();
    if (this == NULL) {
        return "ArrayType(NULL)";
    }
    return "ArrayType";
}

std::vector<Object *> &getArrayData(Object *obj, Runtime *rt) {
    ProfilerCAPTURE();
    if (!isInstanceObject(obj)) {
        rt->signalError(obj->userRepr() + " is not an instance object");
    }
    if (obj->type->id != rt->array_type->id) {
        rt->signalError(obj->userRepr() + " is not Array");
    }
    return icast(obj->instance, ArrayInstance)->data;
}

Object *makeArrayInstanceObject(const std::vector<Object*> &data, Runtime *rt) {
    ProfilerCAPTURE();
    auto res = rt->make(rt->array_type, Runtime::INSTANCE_OBJECT);
    getArrayDataFast(res) = data;
    return res;
}

}    // namespace Cotton::Builtin
