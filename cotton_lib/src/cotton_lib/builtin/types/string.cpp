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

#include "string.h"
#include "../../profiler.h"
#include "api.h"

namespace Cotton::Builtin {
StringInstance::StringInstance(Runtime *rt)
    : Instance(rt, sizeof(StringInstance)) {
    ProfilerCAPTURE();
    this->data = "";
}

StringInstance::~StringInstance() {
    ProfilerCAPTURE();
}

Instance *StringInstance::copy(Runtime *rt) {
    ProfilerCAPTURE();
    Instance *res = new StringInstance(rt);

    if (res == nullptr) {
        rt->signalError("Failed to copy " + this->userRepr(rt), rt->getContext().area);
    }
    ((StringInstance *)res)->data = this->data;
    return res;
}

std::string StringInstance::userRepr(Runtime *rt) {
    ProfilerCAPTURE();
    if (this == nullptr) {
        return "String(nullptr)";
    }
    return "StringInstance(size = " + std::to_string(this->data.size()) + ", data = ...)";
}

size_t StringInstance::getSize() {
    ProfilerCAPTURE();
    return sizeof(StringInstance);
}

size_t StringType::getInstanceSize() {
    ProfilerCAPTURE();
    return sizeof(StringInstance);
}

static Object *
StringIndexAdapter(Object *self, const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();

    rt->verifyIsInstanceObject(self, rt->builtin_types.string, Runtime::SUB0_CTX);
    rt->verifyExactArgsAmountFunc(args, 1);
    auto &arg = args[0];
    rt->verifyIsInstanceObject(arg, rt->builtin_types.integer, Runtime::SUB1_CTX);
    if (!(0 <= getIntegerValueFast(arg) && getIntegerValueFast(arg) < getStringDataFast(self).size())) {
        rt->signalError("Index " + arg->userRepr(rt) + " is out of string " + self->userRepr(rt) + " range",
                        rt->getContext().sub_areas[1]);
    }
    if (!execution_result_matters) {
        return nullptr;
    }
    return makeCharacterInstanceObject(getStringDataFast(self)[getIntegerValueFast(arg)], rt);
}

static Object *StringAddAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();

    rt->verifyIsInstanceObject(self, rt->builtin_types.string, Runtime::SUB0_CTX);
    rt->verifyIsInstanceObject(arg, rt->builtin_types.string, Runtime::SUB1_CTX);

    if (!execution_result_matters) {
        return nullptr;
    }

    auto res                = rt->copy(self);
    getStringDataFast(res) += getStringDataFast(arg);

    return res;
}

static Object *StringEqAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyIsOfType(self, rt->builtin_types.string, Runtime::SUB0_CTX);
    rt->verifyIsValidObject(arg, Runtime::SUB1_CTX);

    if (!rt->isOfType(arg, rt->builtin_types.string)) {
        return rt->protectedBoolean(false);
    }

    if (rt->isInstanceObject(self, rt->builtin_types.string)) {
        if (!rt->isInstanceObject(arg, rt->builtin_types.string)) {
            return rt->protectedBoolean(false);
        }
        return rt->protectedBoolean(getStringDataFast(self) == getStringDataFast(arg));
    }
    else if (rt->isTypeObject(self, rt->builtin_types.string)) {
        if (!rt->isTypeObject(arg, rt->builtin_types.string)) {
            return rt->protectedBoolean(false);
        }
        return rt->protectedBoolean(true);
    }

    return rt->protectedBoolean(false);
}

static Object *StringNeqAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    auto res = StringEqAdapter(self, arg, rt, execution_result_matters);
    return rt->protectedBoolean(!getBooleanValueFast(res));
}

static Object *stringSizeMethod(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];
    rt->verifyIsInstanceObject(self, rt->builtin_types.string, Runtime::SUB1_CTX);

    if (!execution_result_matters) {
        return nullptr;
    }

    return makeIntegerInstanceObject(getStringDataFast(self).size(), rt);
}

static Object *stringSetMethod(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 2);
    auto self  = args[0];
    auto index = args[1];
    auto value = args[2];

    rt->verifyIsInstanceObject(self, rt->builtin_types.string, Runtime::SUB1_CTX);
    rt->verifyIsInstanceObject(index, rt->builtin_types.integer, Runtime::SUB2_CTX);
    rt->verifyIsInstanceObject(value, rt->builtin_types.character, Runtime::SUB3_CTX);

    int64_t ind  = getIntegerValueFast(index);
    auto   &data = getStringDataFast(self);
    if (!(0 <= ind && ind < data.size())) {
        rt->signalError("Index is out of range: " + index->userRepr(rt), rt->getContext().sub_areas[2]);
    }
    data[ind] = getCharacterValueFast(value);
    return rt->protectedNothing();
}

static Object *mm__bool__(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];
    rt->verifyIsInstanceObject(self, rt->builtin_types.string, Runtime::SUB1_CTX);

    if (!execution_result_matters) {
        return self;
    }

    if (getStringDataFast(self) == "true") {
        return rt->protectedBoolean(true);
    }
    else if (getStringDataFast(self) == "true") {
        return rt->protectedBoolean(false);
    }
    rt->signalError("Unsupported conversion: " + getStringDataFast(self), rt->getContext().area);
}

static Object *mm__int__(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];
    rt->verifyIsInstanceObject(self, rt->builtin_types.string, Runtime::SUB1_CTX);

    if (!execution_result_matters) {
        return self;
    }

    return makeIntegerInstanceObject(atoll(getStringDataFast(self).c_str()), rt);
}

static Object *mm__real__(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];
    rt->verifyIsInstanceObject(self, rt->builtin_types.string, Runtime::SUB1_CTX);

    if (!execution_result_matters) {
        return self;
    }

    return makeRealInstanceObject(atof(getStringDataFast(self).c_str()), rt);
}

static Object *mm__string__(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];
    rt->verifyIsInstanceObject(self, rt->builtin_types.string, Runtime::SUB1_CTX);

    if (!execution_result_matters) {
        return self;
    }

    return makeStringInstanceObject(getStringDataFast(self), rt);
}

static Object *mm__repr__(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];
    rt->verifyIsOfType(self, rt->builtin_types.string, Runtime::SUB1_CTX);

    if (!execution_result_matters) {
        return self;
    }

    if (rt->isTypeObject(self, nullptr)) {
        return makeStringInstanceObject("String", rt);
    }

    return makeStringInstanceObject(getStringDataFast(self), rt);
}

static Object *mm__read__(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];
    rt->verifyIsOfType(self, rt->builtin_types.string, Runtime::SUB1_CTX);

    std::string v;
    std::cin >> v;

    if (!execution_result_matters) {
        return self;
    }

    return makeStringInstanceObject(v, rt);
}

void installStringMethods(Type *type, Runtime *rt) {
    ProfilerCAPTURE();
    type->addMethod(MagicMethods::mm__bool__(rt),
                    Builtin::makeFunctionInstanceObject(true, mm__bool__, nullptr, rt));
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

    type->addMethod(rt->nmgr->getId("size"), makeFunctionInstanceObject(true, stringSizeMethod, nullptr, rt));
    type->addMethod(rt->nmgr->getId("set"), makeFunctionInstanceObject(true, stringSetMethod, nullptr, rt));
}

StringType::StringType(Runtime *rt)
    : Type(rt) {
    ProfilerCAPTURE();
    this->index_op = StringIndexAdapter;
    this->add_op   = StringAddAdapter;
    this->eq_op    = StringEqAdapter;
    this->neq_op   = StringNeqAdapter;
}

Object *StringType::create(Runtime *rt) {
    ProfilerCAPTURE();
    Instance *ins = new StringInstance(rt);
    Object   *obj = new Object(true, ins, this, rt);
    return obj;
}

Object *StringType::copy(Object *obj, Runtime *rt) {
    ProfilerCAPTURE();
    rt->verifyIsOfType(obj, rt->builtin_types.string);
    if (obj->instance == nullptr) {
        return new Object(false, nullptr, this, rt);
    }
    auto ins = obj->instance->copy(rt);
    auto res = new Object(true, ins, this, rt);
    return res;
}

std::string StringType::userRepr(Runtime *rt) {
    ProfilerCAPTURE();
    if (this == nullptr) {
        return "StringType(nullptr)";
    }
    return "StringType";
}

std::string &getStringData(Object *obj, Runtime *rt) {
    ProfilerCAPTURE();
    rt->verifyIsInstanceObject(obj, rt->builtin_types.string);
    return icast(obj->instance, StringInstance)->data;
}

std::string &getStringData(Object *obj, Runtime *rt, Runtime::ContextId ctx_id) {
    ProfilerCAPTURE();
    rt->verifyIsInstanceObject(obj, rt->builtin_types.string, Runtime::SUB0_CTX);
    return icast(obj->instance, StringInstance)->data;
}

Object *makeStringInstanceObject(const std::string &value, Runtime *rt) {
    ProfilerCAPTURE();
    auto res               = rt->make(rt->builtin_types.string, Runtime::INSTANCE_OBJECT);
    getStringDataFast(res) = value;
    return res;
}

}    // namespace Cotton::Builtin
