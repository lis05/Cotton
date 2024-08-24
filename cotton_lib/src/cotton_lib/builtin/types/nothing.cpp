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
    Instance *res = new NothingInstance(rt);

    if (res == nullptr) {
        rt->signalError("Failed to copy " + this->userRepr(rt), rt->getContext().area);
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

std::string NothingInstance::userRepr(Runtime *rt) {
    ProfilerCAPTURE();
    if (this == nullptr) {
        return "Nothing(nullptr)";
    }
    return "Nothing";
}

static Object *NothingEqAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyIsOfType(self, rt->builtin_types.nothing, Runtime::SUB0_CTX);
    rt->verifyIsValidObject(arg, Runtime::SUB1_CTX);

    if (!rt->isOfType(arg, rt->builtin_types.nothing)) {
        return rt->protectedBoolean(false);
    }

    return rt->protectedBoolean(true);
}

static Object *NothingNeqAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    auto res = NothingEqAdapter(self, arg, rt, execution_result_matters);
    return rt->protectedBoolean(!getBooleanValueFast(res));
}

static Object *nothing_mm__repr__(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];
    rt->verifyIsOfType(self, rt->builtin_types.nothing, Runtime::SUB1_CTX);

    if (!execution_result_matters) {
        return self;
    }

    if (rt->isTypeObject(self, nullptr)) {
        return makeStringInstanceObject("Nothing", rt);
    }

    return makeStringInstanceObject("nothing", rt);
}

void installNothingMethods(Type *type, Runtime *rt) {
    ProfilerCAPTURE();
    type->addMethod(MagicMethods::mm__repr__(rt),
                    Builtin::makeFunctionInstanceObject(true, nothing_mm__repr__, nullptr, rt));
    type->addMethod(MagicMethods::mm__string__(rt),
                    Builtin::makeFunctionInstanceObject(true, nothing_mm__repr__, nullptr, rt));
}

NothingType::NothingType(Runtime *rt)
    : Type(rt) {
    ProfilerCAPTURE();
    this->eq_op  = NothingEqAdapter;
    this->neq_op = NothingNeqAdapter;
}

Object *NothingType::create(Runtime *rt) {
    ProfilerCAPTURE();
    Instance *ins = new NothingInstance(rt);
    Object   *obj = new Object(true, ins, this, rt);
    return obj;
}

std::string NothingType::userRepr(Runtime *rt) {
    ProfilerCAPTURE();
    if (this == nullptr) {
        return "NothingType(nullptr)";
    }
    return "NothingType";
}

Object *NothingType::copy(Object *obj, Runtime *rt) {
    ProfilerCAPTURE();
    rt->verifyIsOfType(obj, rt->builtin_types.nothing);
    if (obj->instance == nullptr) {
        return new Object(false, nullptr, this, rt);
    }
    auto ins = obj->instance->copy(rt);
    auto res = new Object(true, ins, this, rt);
    return res;
}

Object *makeNothingInstanceObject(Runtime *rt) {
    ProfilerCAPTURE();
    auto res = rt->make(rt->builtin_types.nothing, rt->INSTANCE_OBJECT);
    return res;
}
}    // namespace Cotton::Builtin
