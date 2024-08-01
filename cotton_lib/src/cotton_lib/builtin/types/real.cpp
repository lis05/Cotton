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
    Instance *res = new RealInstance(rt);

    if (res == nullptr) {
        rt->signalError("Failed to copy " + this->userRepr(rt), rt->getContext().area);
    }
    icast(res, RealInstance)->value = this->value;
    return res;
}

std::string RealInstance::userRepr(Runtime *rt) {
    ProfilerCAPTURE();
    if (this == nullptr) {
        return "Real(nullptr)";
    }
    return "Real(value = " + std::to_string(this->value) + ")";
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

    rt->verifyIsInstanceObject(self, rt->builtin_types.real, Runtime::SUB0_CTX);

    if (!execution_result_matters) {
        return nullptr;
    }

    auto res = self->type->copy(self, rt);
    return res;
}

static Object *RealNegativeAdapter(Object *self, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();

    rt->verifyIsInstanceObject(self, rt->builtin_types.real, Runtime::SUB0_CTX);

    if (!execution_result_matters) {
        return nullptr;
    }

    auto res               = self->type->copy(self, rt);
    getRealValueFast(res) *= -1;
    return res;
}

static Object *RealMultAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();

    rt->verifyIsInstanceObject(self, rt->builtin_types.real, Runtime::SUB0_CTX);
    rt->verifyIsInstanceObject(arg, rt->builtin_types.real, Runtime::SUB1_CTX);

    if (!execution_result_matters) {
        return nullptr;
    }

    auto res = makeRealInstanceObject(getRealValueFast(self) * getRealValueFast(arg), rt);
    return res;
}

static Object *RealDivAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();

    rt->verifyIsInstanceObject(self, rt->builtin_types.real, Runtime::SUB0_CTX);
    rt->verifyIsInstanceObject(arg, rt->builtin_types.real, Runtime::SUB1_CTX);

    if (!execution_result_matters) {
        return nullptr;
    }

    auto res = makeRealInstanceObject(getRealValueFast(self) / getRealValueFast(arg), rt);
    return res;
}

static Object *RealAddAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();

    rt->verifyIsInstanceObject(self, rt->builtin_types.real, Runtime::SUB0_CTX);
    rt->verifyIsInstanceObject(arg, rt->builtin_types.real, Runtime::SUB1_CTX);

    if (!execution_result_matters) {
        return nullptr;
    }

    auto res = makeRealInstanceObject(getRealValueFast(self) + getRealValueFast(arg), rt);
    return res;
}

static Object *RealSubAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();

    rt->verifyIsInstanceObject(self, rt->builtin_types.real, Runtime::SUB0_CTX);
    rt->verifyIsInstanceObject(arg, rt->builtin_types.real, Runtime::SUB1_CTX);

    if (!execution_result_matters) {
        return nullptr;
    }

    auto res = makeRealInstanceObject(getRealValueFast(self) - getRealValueFast(arg), rt);
    return res;
}

static Object *RealLtAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();

    rt->verifyIsInstanceObject(self, rt->builtin_types.real, Runtime::SUB0_CTX);
    rt->verifyIsInstanceObject(arg, rt->builtin_types.real, Runtime::SUB1_CTX);

    return rt->protectedBoolean(getRealValueFast(self) < getRealValueFast(arg));
}

static Object *RealLeqAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();

    rt->verifyIsInstanceObject(self, rt->builtin_types.real, Runtime::SUB0_CTX);
    rt->verifyIsInstanceObject(arg, rt->builtin_types.real, Runtime::SUB1_CTX);

    return rt->protectedBoolean(getRealValueFast(self) <= getRealValueFast(arg));
}

static Object *RealGtAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();

    rt->verifyIsInstanceObject(self, rt->builtin_types.real, Runtime::SUB0_CTX);
    rt->verifyIsInstanceObject(arg, rt->builtin_types.real, Runtime::SUB1_CTX);

    return rt->protectedBoolean(getRealValueFast(self) > getRealValueFast(arg));
}

static Object *RealGeqAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();

    rt->verifyIsInstanceObject(self, rt->builtin_types.real, Runtime::SUB0_CTX);
    rt->verifyIsInstanceObject(arg, rt->builtin_types.real, Runtime::SUB1_CTX);

    return rt->protectedBoolean(getRealValueFast(self) >= getRealValueFast(arg));
}

static Object *RealEqAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyIsOfType(self, rt->builtin_types.real, Runtime::SUB0_CTX);
    rt->verifyIsValidObject(arg, Runtime::SUB1_CTX);

    if (!rt->isOfType(arg, rt->builtin_types.real)) {
        return rt->protectedBoolean(false);
    }

    if (rt->isInstanceObject(self, rt->builtin_types.real)) {
        if (!rt->isInstanceObject(arg, rt->builtin_types.real)) {
            return rt->protectedBoolean(false);
        }
        return rt->protectedBoolean(getRealValueFast(self) == getRealValueFast(arg));
    }
    else if (rt->isTypeObject(self, rt->builtin_types.real)) {
        if (!rt->isTypeObject(arg, rt->builtin_types.real)) {
            return rt->protectedBoolean(false);
        }
        return rt->protectedBoolean(true);
    }

    return rt->protectedBoolean(false);
}

static Object *RealNeqAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    auto res = RealEqAdapter(self, arg, rt, execution_result_matters);
    return rt->protectedBoolean(!getBooleanValueFast(res));
}

static Object *mm__bool__(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];
    rt->verifyIsInstanceObject(self, rt->builtin_types.real, Runtime::SUB1_CTX);

    if (!execution_result_matters) {
        return self;
    }

    return rt->protectedBoolean(getRealValueFast(self));
}

static Object *mm__char__(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];
    rt->verifyIsInstanceObject(self, rt->builtin_types.real, Runtime::SUB1_CTX);

    if (!execution_result_matters) {
        return self;
    }

    return makeCharacterInstanceObject(getRealValueFast(self), rt);
}

static Object *mm__int__(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];
    rt->verifyIsInstanceObject(self, rt->builtin_types.real, Runtime::SUB1_CTX);

    if (!execution_result_matters) {
        return self;
    }

    return makeIntegerInstanceObject(getRealValueFast(self), rt);
}

static Object *mm__real__(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];
    rt->verifyIsInstanceObject(self, rt->builtin_types.real, Runtime::SUB1_CTX);

    if (!execution_result_matters) {
        return self;
    }

    return makeRealInstanceObject(getRealValueFast(self), rt);
}

static Object *mm__string__(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];
    rt->verifyIsInstanceObject(self, rt->builtin_types.real, Runtime::SUB1_CTX);

    if (!execution_result_matters) {
        return self;
    }

    return makeStringInstanceObject(std::to_string(getRealValueFast(self)), rt);
}

static Object *mm__repr__(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];
    rt->verifyIsOfType(self, rt->builtin_types.real, Runtime::SUB1_CTX);

    if (!execution_result_matters) {
        return self;
    }

    if (rt->isTypeObject(self, nullptr)) {
        return makeStringInstanceObject("Real", rt);
    }

    return makeStringInstanceObject(std::to_string(getRealValueFast(self)), rt);
}

static Object *mm__read__(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];
    rt->verifyIsOfType(self, rt->builtin_types.real, Runtime::SUB1_CTX);

    double v;
    std::cin >> v;

    if (!execution_result_matters) {
        return self;
    }

    return makeRealInstanceObject(v, rt);
}

void installRealMethods(Type *type, Runtime *rt) {
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
    Instance *ins = new RealInstance(rt);
    Object   *obj = new Object(true, ins, this, rt);
    return obj;
}

Object *RealType::copy(Object *obj, Runtime *rt) {
    ProfilerCAPTURE();
    rt->verifyIsOfType(obj, rt->builtin_types.real);
    if (obj->instance == nullptr) {
        return new Object(false, nullptr, this, rt);
    }
    auto ins = obj->instance->copy(rt);
    auto res = new Object(true, ins, this, rt);
    return res;
}

std::string RealType::userRepr(Runtime *rt) {
    ProfilerCAPTURE();
    if (this == nullptr) {
        return "RealType(nullptr)";
    }
    return "RealType";
}

double &getRealValue(Object *obj, Runtime *rt) {
    ProfilerCAPTURE();
    rt->verifyIsInstanceObject(obj, rt->builtin_types.real);
    return icast(obj->instance, RealInstance)->value;
}

double &getRealValue(Object *obj, Runtime *rt, Runtime::ContextId ctx_id) {
    ProfilerCAPTURE();
    rt->verifyIsInstanceObject(obj, rt->builtin_types.real, Runtime::SUB0_CTX);
    return icast(obj->instance, RealInstance)->value;
}

Object *makeRealInstanceObject(double value, Runtime *rt) {
    ProfilerCAPTURE();
    auto res                                  = rt->make(rt->builtin_types.real, Runtime::INSTANCE_OBJECT);
    icast(res->instance, RealInstance)->value = value;
    return res;
}

}    // namespace Cotton::Builtin
