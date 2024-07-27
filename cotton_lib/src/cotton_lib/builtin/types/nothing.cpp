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

    if (res == NULL) {
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
    if (this == NULL) {
        return "Nothing(NULL)";
    }
    return "Nothing";
}

static Object *NothingEqAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyIsOfType(self, rt->nothing_type, rt->getContext().sub_areas[0]);
    rt->verifyIsValidObject(arg, rt->getContext().sub_areas[1]);

    if (!rt->isOfType(arg, rt->nothing_type)) {
        return rt->protected_false;
    }

    return rt->protected_true;
}

static Object *NothingNeqAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    auto res = NothingEqAdapter(self, arg, rt, execution_result_matters);
    return (!getBooleanValueFast(res)) ? rt->protected_true : rt->protected_false;
}

static Object *mm__repr__(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];
    rt->verifyIsOfType(self, rt->nothing_type, rt->getContext().sub_areas[1]);

    if (!execution_result_matters) {
        return self;
    }

    if (rt->isTypeObject(self, NULL)) {
        return makeStringInstanceObject("Nothing", rt);
    }

    return makeStringInstanceObject("nothing", rt);
}

void installNothingMethods(Type *type, Runtime *rt) {
    type->addMethod(MagicMethods::mm__repr__(rt), Builtin::makeFunctionInstanceObject(true, mm__repr__, NULL, rt));
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
    Object   *obj = newObject(true, ins, this, rt);
    return obj;
}

std::string NothingType::userRepr(Runtime *rt) {
    ProfilerCAPTURE();
    if (this == NULL) {
        return "NothingType(NULL)";
    }
    return "NothingType";
}

Object *NothingType::copy(Object *obj, Runtime *rt) {
    ProfilerCAPTURE();
    rt->verifyIsOfType(obj, rt->nothing_type);
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
