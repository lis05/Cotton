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
    Instance *res = new BooleanInstance(rt);
    if (res == nullptr) {
        rt->signalError("Failed to copy " + this->userRepr(rt), rt->getContext().area);
    }
    icast(res, BooleanInstance)->value = this->value;
    return res;
}

std::string BooleanInstance::userRepr(Runtime *rt) {
    ProfilerCAPTURE();
    if (this == nullptr) {
        return "Boolean(nullptr)";
    }
    return std::string("Boolean(value = ") + (this->value ? "true" : "false") + ")";
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
    rt->verifyIsInstanceObject(self, rt->builtin_types.boolean, Runtime::SUB0_CTX);

    return (!getBooleanValueFast(self)) ? rt->protectedBoolean(true) : rt->protectedBoolean(false);
}

static Object *BooleanEqAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyIsOfType(self, rt->builtin_types.boolean, Runtime::SUB0_CTX);
    rt->verifyIsValidObject(arg, Runtime::SUB1_CTX);

    if (!rt->isOfType(arg, rt->builtin_types.boolean)) {
        return rt->protectedBoolean(false);
    }

    if (rt->isInstanceObject(self, rt->builtin_types.boolean)) {
        if (!rt->isInstanceObject(arg, rt->builtin_types.boolean)) {
            return rt->protectedBoolean(false);
        }
        return rt->protectedBoolean(getBooleanValueFast(self) == getBooleanValueFast(arg));
    }
    else if (rt->isTypeObject(self, rt->builtin_types.boolean)) {
        if (!rt->isTypeObject(arg, rt->builtin_types.boolean)) {
            return rt->protectedBoolean(false);
        }
        return rt->protectedBoolean(true);
    }

    return rt->protectedBoolean(false);
}

static Object *BooleanNeqAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    auto res = BooleanEqAdapter(self, arg, rt, execution_result_matters);
    return (!getBooleanValue(res, rt)) ? rt->protectedBoolean(true) : rt->protectedBoolean(false);
}

static Object *BooleanAndAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyIsInstanceObject(self, rt->builtin_types.boolean, Runtime::SUB0_CTX);
    rt->verifyIsInstanceObject(arg, rt->builtin_types.boolean, Runtime::SUB1_CTX);

    return rt->protectedBoolean(getBooleanValueFast(self) && getBooleanValueFast(arg));
}

static Object *BooleanOrAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyIsInstanceObject(self, rt->builtin_types.boolean, Runtime::SUB0_CTX);
    rt->verifyIsInstanceObject(arg, rt->builtin_types.boolean, Runtime::SUB1_CTX);

    return rt->protectedBoolean(getBooleanValueFast(self) || getBooleanValueFast(arg));
}

static Object *mm__bool__(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];
    rt->verifyIsInstanceObject(self, rt->builtin_types.boolean, Runtime::SUB1_CTX);

    if (!execution_result_matters) {
        return self;
    }

    if (rt->isTypeObject(self, nullptr)) {
        return rt->protectedBoolean(false);
    }

    return self;
}

static Object *mm__char__(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];
    rt->verifyIsInstanceObject(self, rt->builtin_types.boolean, Runtime::SUB1_CTX);

    if (!execution_result_matters) {
        return self;
    }

    return makeCharacterInstanceObject('0' + getBooleanValueFast(self), rt);
}

static Object *mm__int__(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];
    rt->verifyIsInstanceObject(self, rt->builtin_types.boolean, Runtime::SUB1_CTX);

    if (!execution_result_matters) {
        return self;
    }

    return makeIntegerInstanceObject(getBooleanValueFast(self), rt);
}

static Object *mm__real__(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];
    rt->verifyIsInstanceObject(self, rt->builtin_types.boolean, Runtime::SUB1_CTX);

    if (!execution_result_matters) {
        return self;
    }

    return makeRealInstanceObject(getBooleanValueFast(self), rt);
}

static Object *mm__string__(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];
    rt->verifyIsOfType(self, rt->builtin_types.boolean, Runtime::SUB1_CTX);

    if (!execution_result_matters) {
        return self;
    }

    if (rt->isTypeObject(self, nullptr)) {
        return makeStringInstanceObject("Boolean", rt);
    }

    return makeStringInstanceObject(getBooleanValueFast(self) ? "true" : "false", rt);
}

static Object *mm__repr__(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];
    rt->verifyIsOfType(self, rt->builtin_types.boolean, Runtime::SUB1_CTX);

    if (!execution_result_matters) {
        return self;
    }

    if (rt->isTypeObject(self, nullptr)) {
        return makeStringInstanceObject("Boolean", rt);
    }

    return makeStringInstanceObject(getBooleanValueFast(self) ? "true" : "false", rt);
}

static Object *mm__read__(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];
    rt->verifyIsOfType(self, rt->builtin_types.boolean, Runtime::SUB1_CTX);

    std::string s;
    std::cin >> s;

    if (!execution_result_matters) {
        return self;
    }

    if (s == "true") {
        return rt->protectedBoolean(true);
    }
    else if (s == "false") {
        return rt->protectedBoolean(false);
    }
    rt->signalError("Not a Boolean: " + s, rt->getContext().area);
}

void installBooleanMethods(Type *type, Runtime *rt) {
    ProfilerCAPTURE();
    type->addMethod(MagicMethods::mm__bool__(rt),
                    Builtin::makeFunctionInstanceObject(true, mm__bool__, nullptr, rt));
    type->addMethod(MagicMethods::mm__char__(rt),
                    Builtin::makeFunctionInstanceObject(true, mm__char__, nullptr, rt));
    type->addMethod(MagicMethods::mm__int__(rt),
                    Builtin::makeFunctionInstanceObject(true, mm__int__, nullptr, rt));
    type->addMethod(MagicMethods::mm__real__(rt),
                    Builtin::makeFunctionInstanceObject(true, mm__real__, nullptr, rt));
    type->addMethod(MagicMethods::mm__string__(rt),
                    Builtin::makeFunctionInstanceObject(true, mm__string__, nullptr, rt));
    type->addMethod(MagicMethods::mm__repr__(rt),
                    Builtin::makeFunctionInstanceObject(true, mm__repr__, nullptr, rt));
    type->addMethod(MagicMethods::mm__read__(rt),
                    Builtin::makeFunctionInstanceObject(true, mm__read__, nullptr, rt));
}

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
    Instance *ins = new BooleanInstance(rt);
    Object   *obj = new Object(true, ins, this, rt);
    return obj;
}

Object *BooleanType::copy(Object *obj, Runtime *rt) {
    ProfilerCAPTURE();
    rt->verifyIsOfType(obj, rt->builtin_types.boolean);
    if (obj->instance == nullptr) {
        return new Object(false, nullptr, this, rt);
    }
    auto ins = obj->instance->copy(rt);
    auto res = new Object(true, ins, this, rt);
    return res;
}

std::string BooleanType::userRepr(Runtime *rt) {
    ProfilerCAPTURE();
    if (this == nullptr) {
        return "BooleanType(nullptr)";
    }
    return "BooleanType";
}

bool &getBooleanValue(Object *obj, Runtime *rt) {
    ProfilerCAPTURE();
    rt->verifyIsInstanceObject(obj, rt->builtin_types.boolean);
    return icast(obj->instance, BooleanInstance)->value;
}

bool &getBooleanValue(Object *obj, Runtime *rt, Runtime::ContextId ctx_id) {
    ProfilerCAPTURE();
    rt->verifyIsInstanceObject(obj, rt->builtin_types.boolean, Runtime::SUB0_CTX);
    return icast(obj->instance, BooleanInstance)->value;
}

Object *makeBooleanInstanceObject(bool value, Runtime *rt) {
    ProfilerCAPTURE();
    auto res                                     = rt->make(rt->builtin_types.boolean, Runtime::INSTANCE_OBJECT);
    icast(res->instance, BooleanInstance)->value = value;
    return res;
}

}    // namespace Cotton::Builtin
