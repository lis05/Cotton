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
    Instance *res = new ArrayInstance(rt);

    if (res == NULL) {
        rt->signalError("Failed to copy " + this->userRepr(rt), rt->getContext().area);
    }
    for (auto obj : this->data) {
        ((ArrayInstance *)res)->data.push_back(rt->copy(obj));
    }
    return res;
}

std::string ArrayInstance::userRepr(Runtime *rt) {
    ProfilerCAPTURE();
    if (this == NULL) {
        return "Array(NULL)";
    }
    return "Array(size = " + std::to_string(this->data.size()) + ", data = ...)";
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
    ProfilerCAPTURE();
    for (auto obj : this->data) {
        obj->spreadSingleUse();
    }
}

void ArrayInstance::spreadMultiUse() {
    ProfilerCAPTURE();
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

    rt->verifyIsInstanceObject(self, rt->array_type, Runtime::SUB0_CTX);
    rt->verifyExactArgsAmountFunc(args, 1);
    auto &arg = args[0];
    rt->verifyIsInstanceObject(arg, rt->integer_type, Runtime::SUB1_CTX);

    if (!(0 <= getIntegerValueFast(arg) && getIntegerValueFast(arg) < getArrayDataFast(self).size())) {
        rt->signalError("Index " + arg->userRepr(rt) + " is out of array " + self->userRepr(rt) + " range",
                        rt->getContext().sub_areas[1]);
    }
    return getArrayDataFast(self)[getIntegerValueFast(arg)];
}

static Object *ArrayEqAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyIsOfType(self, rt->array_type, Runtime::SUB0_CTX);
    rt->verifyIsValidObject(arg, Runtime::SUB1_CTX);

    if (!execution_result_matters) {
        return rt->protected_false;
    }

    if (!rt->isOfType(arg, rt->string_type)) {
        return rt->protected_false;
    }

    if (rt->isInstanceObject(self, rt->integer_type)) {
        if (!rt->isInstanceObject(arg, rt->integer_type)) {
            return rt->protected_false;
        }
        auto a1 = getArrayDataFast(self);
        auto a2 = getArrayDataFast(self);
        if (a1.size() != a2.size()) {
            return rt->protected_false;
        }
        for (int64_t i = 0; i < a1.size(); i++) {
            auto &ta = rt->getContext().area;
            rt->newContext();
            rt->getContext().area      = ta;
            rt->getContext().sub_areas = {ta, ta};
            auto res                   = rt->runOperator(OperatorNode::EQUAL, a1[i], a2[i], true);
            rt->popContext();
            if (!getBooleanValueFast(res)) {
                return rt->protected_false;
            }
        }
        return rt->protected_true;
    }
    else if (rt->isTypeObject(self, rt->integer_type)) {
        if (!rt->isTypeObject(arg, rt->integer_type)) {
            return rt->protected_false;
        }
        return rt->protected_true;
    }

    return rt->protected_false;
}

static Object *ArrayNeqAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    auto res = ArrayEqAdapter(self, arg, rt, execution_result_matters);
    return (!getBooleanValueFast(res)) ? rt->protected_true : rt->protected_false;
}

static Object *arraySizeMethod(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];
    rt->verifyIsInstanceObject(self, rt->array_type, Runtime::SUB0_CTX);

    if (!execution_result_matters) {
        return NULL;
    }

    return makeIntegerInstanceObject(getArrayDataFast(self).size(), rt);
}

static Object *arrayResizeMethod(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 1);
    auto self     = args[0];
    auto new_size = args[1];
    rt->verifyIsInstanceObject(self, rt->array_type, Runtime::SUB0_CTX);
    rt->verifyIsInstanceObject(new_size, rt->integer_type, Runtime::SUB1_CTX);

    int64_t oldn = getArrayDataFast(self).size();
    int64_t newn = getIntegerValueFast(new_size);
    if (newn <= 0) {
        rt->signalError("New array size must be positive: " + new_size->userRepr(rt),
                        rt->getContext().sub_areas[1]);
    }
    getArrayDataFast(self).resize(newn);

    for (int64_t i = oldn; i < newn; i++) {
        getArrayDataFast(self)[i] = makeNothingInstanceObject(rt);
    }

    return self;
}

static Object *mm__repr__(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];
    rt->verifyIsOfType(self, rt->array_type, Runtime::SUB1_CTX);

    if (!execution_result_matters) {
        return self;
    }

    if (rt->isTypeObject(self, NULL)) {
        return makeStringInstanceObject("Array", rt);
    }

    auto       &arr = getArrayDataFast(self);
    std::string res = "{";
    if (arr.size() != 0) {
        auto o = rt->runMethod(MagicMethods::mm__repr__(rt), arr[0], {arr[0]}, true);
        rt->verifyIsInstanceObject(o, rt->string_type, Runtime::SUB1_CTX);
        res += getStringDataFast(o);
    }

    for (int64_t i = 1; i < arr.size(); i++) {
        auto o = rt->runMethod(MagicMethods::mm__repr__(rt), arr[i], {arr[i]}, true);
        rt->verifyIsInstanceObject(o, rt->string_type, Runtime::SUB1_CTX);
        res += ", " + getStringDataFast(o);
    }

    res += "}";

    return makeStringInstanceObject(res, rt);
}

void installArrayMethods(Type *type, Runtime *rt) {
    ProfilerCAPTURE();
    type->addMethod(MagicMethods::mm__repr__(rt), Builtin::makeFunctionInstanceObject(true, mm__repr__, NULL, rt));

    type->addMethod(rt->nds->get("size").id, makeFunctionInstanceObject(true, arraySizeMethod, NULL, rt));
    type->addMethod(rt->nds->get("resize").id, makeFunctionInstanceObject(true, arrayResizeMethod, NULL, rt));
}

ArrayType::ArrayType(Runtime *rt)
    : Type(rt) {
    ProfilerCAPTURE();
    this->index_op = ArrayIndexAdapter;
    this->eq_op    = ArrayEqAdapter;
    this->neq_op   = ArrayNeqAdapter;
}

Object *ArrayType::create(Runtime *rt) {
    ProfilerCAPTURE();
    Instance *ins = new ArrayInstance(rt);
    Object   *obj = newObject(true, ins, this, rt);
    return obj;
}

Object *ArrayType::copy(Object *obj, Runtime *rt) {
    ProfilerCAPTURE();
    rt->verifyIsOfType(obj, rt->array_type);
    if (obj->instance == NULL) {
        return newObject(false, NULL, this, rt);
    }
    auto ins = obj->instance->copy(rt);
    auto res = newObject(true, ins, this, rt);
    return res;
}

std::string ArrayType::userRepr(Runtime *rt) {
    ProfilerCAPTURE();
    if (this == NULL) {
        return "ArrayType(NULL)";
    }
    return "ArrayType";
}

std::vector<Object *> &getArrayData(Object *obj, Runtime *rt) {
    ProfilerCAPTURE();
    rt->verifyIsInstanceObject(obj, rt->array_type);
    return icast(obj->instance, ArrayInstance)->data;
}

std::vector<Object *> &getArrayData(Object *obj, Runtime *rt, Runtime::ContextId ctx_id) {
    ProfilerCAPTURE();
    rt->verifyIsInstanceObject(obj, rt->array_type, Runtime::SUB0_CTX);
    return icast(obj->instance, ArrayInstance)->data;
}

Object *makeArrayInstanceObject(const std::vector<Object *> &data, Runtime *rt) {
    ProfilerCAPTURE();
    auto res              = rt->make(rt->array_type, Runtime::INSTANCE_OBJECT);
    getArrayDataFast(res) = data;
    return res;
}

}    // namespace Cotton::Builtin
