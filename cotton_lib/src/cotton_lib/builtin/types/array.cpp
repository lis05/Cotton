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

    if (res == nullptr) {
        rt->signalError("Failed to copy " + this->userRepr(rt), rt->getContext().area);
    }
    for (auto obj : this->data) {
        ((ArrayInstance *)res)->data.push_back(rt->copy(obj));
    }
    return res;
}

std::string ArrayInstance::userRepr(Runtime *rt) {
    ProfilerCAPTURE();
    if (this == nullptr) {
        return "Array(nullptr)";
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

static Object *ArrayIndexAdapter(Object *self, const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();

    rt->verifyIsInstanceObject(self, rt->builtin_types.array, Runtime::SUB0_CTX);
    rt->verifyExactArgsAmountFunc(args, 1);
    auto &arg = args[0];
    rt->verifyIsInstanceObject(arg, rt->builtin_types.integer, Runtime::SUB1_CTX);

    if (!(0 <= getIntegerValueFast(arg) && getIntegerValueFast(arg) < getArrayDataFast(self).size())) {
        rt->signalError("Index " + arg->userRepr(rt) + " is out of array " + self->userRepr(rt) + " range", rt->getContext().sub_areas[1]);
    }
    return getArrayDataFast(self)[getIntegerValueFast(arg)];
}

static Object *ArrayEqAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyIsOfType(self, rt->builtin_types.array, Runtime::SUB0_CTX);
    rt->verifyIsValidObject(arg, Runtime::SUB1_CTX);

    if (!execution_result_matters) {
        return rt->protectedBoolean(false);
    }

    if (!rt->isOfType(arg, rt->builtin_types.array)) {
        return rt->protectedBoolean(false);
    }

    if (rt->isInstanceObject(self, rt->builtin_types.array)) {
        if (!rt->isInstanceObject(arg, rt->builtin_types.array)) {
            return rt->protectedBoolean(false);
        }
        auto a1 = getArrayDataFast(self);
        auto a2 = getArrayDataFast(self);
        if (a1.size() != a2.size()) {
            return rt->protectedBoolean(false);
        }
        for (int64_t i = 0; i < a1.size(); i++) {
            auto &ta = rt->getContext().area;
            rt->newContext();
            rt->getContext().area      = ta;
            rt->getContext().sub_areas = {ta, ta};
            auto res                   = rt->runOperator(OperatorNode::EQUAL, a1[i], a2[i], true);
            rt->popContext();
            if (!getBooleanValueFast(res)) {
                return rt->protectedBoolean(false);
            }
        }
        return rt->protectedBoolean(true);
    }
    else if (rt->isTypeObject(self, rt->builtin_types.array)) {
        if (!rt->isTypeObject(arg, rt->builtin_types.array)) {
            return rt->protectedBoolean(false);
        }
        return rt->protectedBoolean(true);
    }

    return rt->protectedBoolean(false);
}

static Object *ArrayNeqAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    auto res = ArrayEqAdapter(self, arg, rt, execution_result_matters);
    return rt->protectedBoolean(!getBooleanValueFast(res));
}

static Object *arraySizeMethod(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];
    rt->verifyIsInstanceObject(self, rt->builtin_types.array, Runtime::SUB0_CTX);

    if (!execution_result_matters) {
        return nullptr;
    }

    return makeIntegerInstanceObject(getArrayDataFast(self).size(), rt);
}

static Object *arrayResizeMethod(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 1);
    auto self     = args[0];
    auto new_size = args[1];
    rt->verifyIsInstanceObject(self, rt->builtin_types.array, Runtime::SUB0_CTX);
    rt->verifyIsInstanceObject(new_size, rt->builtin_types.integer, Runtime::SUB1_CTX);

    int64_t oldn = getArrayDataFast(self).size();
    int64_t newn = getIntegerValueFast(new_size);
    if (newn <= 0) {
        rt->signalError("New array size must be positive: " + new_size->userRepr(rt), rt->getContext().sub_areas[1]);
    }
    getArrayDataFast(self).resize(newn);

    for (int64_t i = oldn; i < newn; i++) {
        getArrayDataFast(self)[i] = makeNothingInstanceObject(rt);
    }

    if (!self->single_use) {
        self->spreadMultiUse();
    }

    return self;
}

static Object *arrayAppendMethod(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    rt->verifyMinArgsAmountMethod(args, 1);
    auto self = args[0];
    rt->verifyIsInstanceObject(self, rt->builtin_types.array, Runtime::SUB1_CTX);
    auto &data = getArrayDataFast(self);
    for (int64_t i = 1; i < args.size(); i++) {
        rt->verifyIsValidObject(args[i], (Runtime::ContextId)i);
        data.push_back(args[i]);
    }
    return self;
}

static Object *arrayPrependMethod(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    rt->verifyMinArgsAmountMethod(args, 1);
    auto self = args[0];
    rt->verifyIsInstanceObject(self, rt->builtin_types.array, Runtime::SUB1_CTX);
    auto                 &data = getArrayDataFast(self);
    std::vector<Object *> pref;
    for (int64_t i = 1; i < args.size(); i++) {
        rt->verifyIsValidObject(args[i], (Runtime::ContextId)i);
        pref.push_back(args[i]);
    }
    for (auto &item : data) {
        pref.push_back(item);
    }
    data = pref;
    return self;
}

static Object *arrayPoplastMethod(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];
    rt->verifyIsInstanceObject(self, rt->builtin_types.array, Runtime::SUB1_CTX);
    auto &data = getArrayDataFast(self);
    if (data.empty()) {
        return self;
    }
    data.pop_back();
    return self;
}

static Object *arrayPopfirstMethod(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];
    rt->verifyIsInstanceObject(self, rt->builtin_types.array, Runtime::SUB1_CTX);
    auto &data = getArrayDataFast(self);
    if (data.empty()) {
        return self;
    }
    data.erase(data.begin());
    return self;
}

static Object *arrayFirstMethod(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];
    rt->verifyIsInstanceObject(self, rt->builtin_types.array, Runtime::SUB1_CTX);
    auto &data = getArrayDataFast(self);
    if (data.empty()) {
        return rt->protectedNothing();
    }
    return data[0];
}

static Object *arrayLastMethod(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];
    rt->verifyIsInstanceObject(self, rt->builtin_types.array, Runtime::SUB1_CTX);
    auto &data = getArrayDataFast(self);
    if (data.empty()) {
        return rt->protectedNothing();
    }
    return data.back();
}

static Object *arrayEmptyMethod(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];
    rt->verifyIsInstanceObject(self, rt->builtin_types.array, Runtime::SUB1_CTX);
    auto &data = getArrayDataFast(self);
    return rt->protectedBoolean(data.empty());
}

static Object *arrayClearMethod(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];
    rt->verifyIsInstanceObject(self, rt->builtin_types.array, Runtime::SUB1_CTX);
    auto &data = getArrayDataFast(self);
    data.clear();
    return self;
}

static Object *arrayCopyMethod(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];

    rt->verifyIsInstanceObject(self, rt->builtin_types.array, Runtime::SUB1_CTX);
    return rt->copy(self);
}

static Object *arrayFilterMethod(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    rt->verifyExactArgsAmountMethod(args, 1);
    auto self = args[0];
    auto arg  = args[1];
    rt->verifyIsInstanceObject(self, rt->builtin_types.array, Runtime::SUB1_CTX);
    rt->verifyIsInstanceObject(arg, rt->builtin_types.function, Runtime::SUB2_CTX);

    std::vector<Object *> new_data;
    auto                  ctx = rt->getContext();
    rt->newContext();
    rt->getContext().area      = ctx.area;
    rt->getContext().sub_areas = {ctx.area, ctx.area};
    for (auto &obj : getArrayDataFast(self)) {
        auto res = rt->runOperator(OperatorNode::CALL, arg, std::vector<Object *> {obj}, true);
        if (getBooleanValue(res, rt)) {
            new_data.push_back(obj);
        }
    }
    rt->popContext();

    getArrayDataFast(self) = new_data;
    return self;
}

static Object *arrayApplyMethod(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    rt->verifyExactArgsAmountMethod(args, 1);
    auto self = args[0];
    auto arg  = args[1];
    rt->verifyIsInstanceObject(self, rt->builtin_types.array, Runtime::SUB1_CTX);
    rt->verifyIsInstanceObject(arg, rt->builtin_types.function, Runtime::SUB2_CTX);

    auto ctx = rt->getContext();
    rt->newContext();
    rt->getContext().area      = ctx.area;
    rt->getContext().sub_areas = {ctx.area, ctx.area};
    for (auto &obj : getArrayDataFast(self)) {
        rt->runOperator(OperatorNode::CALL, arg, std::vector<Object *> {obj}, true);
    }
    rt->popContext();

    return self;
}

static Object *arrayReverseMethod(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];
    rt->verifyIsInstanceObject(self, rt->builtin_types.array, Runtime::SUB1_CTX);

    std::reverse(getArrayDataFast(self).begin(), getArrayDataFast(self).end());
    return self;
}

static Object *arraySortMethod(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    rt->verifyExactArgsAmountMethod(args, 1);
    auto self = args[0];
    auto arg  = args[1];
    rt->verifyIsInstanceObject(self, rt->builtin_types.array, Runtime::SUB1_CTX);
    rt->verifyIsInstanceObject(arg, rt->builtin_types.function, Runtime::SUB2_CTX);

    auto ctx = rt->getContext();
    rt->newContext();
    rt->getContext().area      = ctx.area;
    rt->getContext().sub_areas = {ctx.area, ctx.area};
    std::sort(getArrayDataFast(self).begin(), getArrayDataFast(self).end(), [rt, arg](const auto &a, const auto &b) {
        auto res = rt->runOperator(OperatorNode::CALL, arg, std::vector<Object *> {a, b}, true);
        return getBooleanValue(res, rt);
    });
    rt->popContext();
    return self;
}

static Object *arrayCombineMethod(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    rt->verifyExactArgsAmountMethod(args, 2);
    auto self = args[0];
    auto arg  = args[1];
    auto init = args[2];
    rt->verifyIsInstanceObject(self, rt->builtin_types.array, Runtime::SUB1_CTX);
    rt->verifyIsInstanceObject(arg, rt->builtin_types.function, Runtime::SUB2_CTX);
    rt->verifyIsValidObject(init, Runtime::SUB3_CTX);

    auto ctx = rt->getContext();
    rt->newContext();
    rt->getContext().area      = ctx.area;
    rt->getContext().sub_areas = {ctx.area, ctx.area};
    for (auto obj : getArrayDataFast(self)) {
        init = rt->runOperator(OperatorNode::CALL, arg, {init, obj}, true);
    }
    rt->popContext();
    return init;
}

static Object *array_mm__repr__(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];
    rt->verifyIsOfType(self, rt->builtin_types.array, Runtime::SUB1_CTX);

    if (!execution_result_matters) {
        return self;
    }

    if (rt->isTypeObject(self, nullptr)) {
        return makeStringInstanceObject("Array", rt);
    }

    auto       &arr = getArrayDataFast(self);
    std::string res = "{";
    if (arr.size() != 0) {
        auto o = rt->runMethod(MagicMethods::mm__repr__(rt), arr[0], {arr[0]}, true);
        rt->verifyIsInstanceObject(o, rt->builtin_types.string, Runtime::SUB1_CTX);
        res += getStringDataFast(o);
    }

    for (int64_t i = 1; i < arr.size(); i++) {
        auto o = rt->runMethod(MagicMethods::mm__repr__(rt), arr[i], {arr[i]}, true);
        rt->verifyIsInstanceObject(o, rt->builtin_types.string, Runtime::SUB1_CTX);
        res += ", " + getStringDataFast(o);
    }

    res += "}";

    return makeStringInstanceObject(res, rt);
}

void installArrayMethods(Type *type, Runtime *rt) {
    ProfilerCAPTURE();
    type->addMethod(MagicMethods::mm__repr__(rt), Builtin::makeFunctionInstanceObject(true, array_mm__repr__, nullptr, rt));
    type->addMethod(MagicMethods::mm__string__(rt), Builtin::makeFunctionInstanceObject(true, array_mm__repr__, nullptr, rt));

    type->addMethod(rt->nmgr->getId("size"), makeFunctionInstanceObject(true, arraySizeMethod, nullptr, rt));
    type->addMethod(rt->nmgr->getId("resize"), makeFunctionInstanceObject(true, arrayResizeMethod, nullptr, rt));
    type->addMethod(rt->nmgr->getId("append"), makeFunctionInstanceObject(true, arrayAppendMethod, nullptr, rt));
    type->addMethod(rt->nmgr->getId("prepend"), makeFunctionInstanceObject(true, arrayPrependMethod, nullptr, rt));
    type->addMethod(rt->nmgr->getId("poplast"), makeFunctionInstanceObject(true, arrayPoplastMethod, nullptr, rt));
    type->addMethod(rt->nmgr->getId("popfirst"), makeFunctionInstanceObject(true, arrayPopfirstMethod, nullptr, rt));
    type->addMethod(rt->nmgr->getId("first"), makeFunctionInstanceObject(true, arrayFirstMethod, nullptr, rt));
    type->addMethod(rt->nmgr->getId("last"), makeFunctionInstanceObject(true, arrayLastMethod, nullptr, rt));
    type->addMethod(rt->nmgr->getId("empty"), makeFunctionInstanceObject(true, arrayEmptyMethod, nullptr, rt));
    type->addMethod(rt->nmgr->getId("clear"), makeFunctionInstanceObject(true, arrayClearMethod, nullptr, rt));
    type->addMethod(rt->nmgr->getId("copy"), makeFunctionInstanceObject(true, arrayCopyMethod, nullptr, rt));
    type->addMethod(rt->nmgr->getId("filter"), makeFunctionInstanceObject(true, arrayFilterMethod, nullptr, rt));
    type->addMethod(rt->nmgr->getId("apply"), makeFunctionInstanceObject(true, arrayApplyMethod, nullptr, rt));
    type->addMethod(rt->nmgr->getId("reverse"), makeFunctionInstanceObject(true, arrayReverseMethod, nullptr, rt));
    type->addMethod(rt->nmgr->getId("sort"), makeFunctionInstanceObject(true, arraySortMethod, nullptr, rt));
    type->addMethod(rt->nmgr->getId("combine"), makeFunctionInstanceObject(true, arrayCombineMethod, nullptr, rt));
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
    Object   *obj = new Object(true, ins, this, rt);
    return obj;
}

Object *ArrayType::copy(Object *obj, Runtime *rt) {
    ProfilerCAPTURE();
    rt->verifyIsOfType(obj, rt->builtin_types.array);
    if (obj->instance == nullptr) {
        return new Object(false, nullptr, this, rt);
    }
    auto ins = obj->instance->copy(rt);
    auto res = new Object(true, ins, this, rt);
    return res;
}

std::string ArrayType::userRepr(Runtime *rt) {
    ProfilerCAPTURE();
    if (this == nullptr) {
        return "ArrayType(nullptr)";
    }
    return "ArrayType";
}

std::vector<Object *> &getArrayData(Object *obj, Runtime *rt) {
    ProfilerCAPTURE();
    rt->verifyIsInstanceObject(obj, rt->builtin_types.array);
    return icast(obj->instance, Cotton::Builtin::ArrayInstance)->data;
}

std::vector<Object *> &getArrayData(Object *obj, Runtime *rt, Runtime::ContextId ctx_id) {
    ProfilerCAPTURE();
    rt->verifyIsInstanceObject(obj, rt->builtin_types.array, Runtime::SUB0_CTX);
    return icast(obj->instance, Cotton::Builtin::ArrayInstance)->data;
}

Object *makeArrayInstanceObject(const std::vector<Object *> &data, Runtime *rt) {
    ProfilerCAPTURE();
    auto res              = rt->make(rt->builtin_types.array, Runtime::INSTANCE_OBJECT);
    getArrayDataFast(res) = data;
    return res;
}
}    // namespace Cotton::Builtin
