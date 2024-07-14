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
    Instance *res = new (rt->alloc(sizeof(StringInstance))) StringInstance(rt);

    if (res == NULL) {
        rt->signalError("Failed to copy " + this->userRepr(), rt->getContext().area);
    }
    ((StringInstance *)res)->data = this->data;
    return res;
}

std::string StringInstance::userRepr() {
    ProfilerCAPTURE();
    if (this == NULL) {
        return "String(NULL)";
    }
    return "StringInstance(size = " + std::to_string(this->data.size()) + ", data = ...)";
}

void StringInstance::destroy(Runtime *rt) {
    rt->dealloc(this, sizeof(StringInstance));
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

    rt->verifyIsInstanceObject(self, rt->string_type, rt->getContext().sub_areas[0]);
    rt->verifyExactArgsAmountFunc(args, 1);
    auto &arg = args[0];
    rt->verifyIsInstanceObject(arg, rt->integer_type, rt->getContext().sub_areas[1]);
    if (!(0 <= getIntegerValueFast(arg) && getIntegerValueFast(arg) < getStringDataFast(self).size())) {
        rt->signalError("Index " + arg->userRepr() + " is out of string " + self->userRepr() + " range",
                        rt->getContext().sub_areas[1]);
    }
    if (!execution_result_matters) {
        return NULL;
    }
    return makeCharacterInstanceObject(getStringDataFast(self)[getIntegerValueFast(arg)], rt);
}

static Object *StringAddAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();

    rt->verifyIsInstanceObject(self, rt->string_type, rt->getContext().sub_areas[0]);
    rt->verifyIsInstanceObject(arg, rt->string_type, rt->getContext().sub_areas[1]);

    if (!execution_result_matters) {
        return NULL;
    }

    auto res                = rt->copy(self);
    getStringDataFast(res) += getStringDataFast(arg);

    return res;
}

static Object *StringEqAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyIsOfType(self, rt->string_type, rt->getContext().sub_areas[0]);
    rt->verifyIsValidObject(arg, rt->getContext().sub_areas[1]);

    if (!rt->isOfType(arg, rt->string_type)) {
        return rt->protected_false;
    }

    if (rt->isInstanceObject(self, rt->integer_type)) {
        if (!rt->isInstanceObject(arg, rt->integer_type)) {
            return rt->protected_false;
        }
        return rt->protectedBoolean(getStringDataFast(self) == getStringDataFast(arg));
    }
    else if (rt->isTypeObject(self, rt->integer_type)) {
        if (!rt->isTypeObject(arg, rt->integer_type)) {
            return rt->protected_false;
        }
        return rt->protected_true;
    }

    return rt->protected_false;
}

static Object *StringNeqAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    auto res = StringEqAdapter(self, arg, rt, execution_result_matters);
    return (!getBooleanValueFast(res)) ? rt->protected_true : rt->protected_false;
}

static Object *stringSizeMethod(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];
    rt->verifyIsInstanceObject(self, rt->string_type, rt->getContext().sub_areas[0]);

    if (!execution_result_matters) {
        return NULL;
    }

    return makeIntegerInstanceObject(getStringDataFast(self).size(), rt);
}

static Object *stringSetMethod(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 2);
    auto self  = args[0];
    auto index = args[1];
    auto value = args[2];

    rt->verifyIsInstanceObject(self, rt->string_type, rt->getContext().sub_areas[0]);
    rt->verifyIsInstanceObject(index, rt->integer_type, rt->getContext().sub_areas[1]);
    rt->verifyIsInstanceObject(value, rt->character_type, rt->getContext().sub_areas[2]);

    int64_t ind  = getIntegerValueFast(index);
    auto   &data = getStringDataFast(self);
    if (!(0 <= ind && ind < data.size())) {
        rt->signalError("Index is out of range: " + index->userRepr(), rt->getContext().sub_areas[1]);
    }
    data[ind] = getCharacterValueFast(value);
    return rt->protected_nothing;
}

// TODO: add all operators to function and nothing
StringType::StringType(Runtime *rt)
    : Type(rt) {
    ProfilerCAPTURE();
    this->index_op = StringIndexAdapter;
    this->add_op   = StringAddAdapter;
    this->eq_op    = StringEqAdapter;
    this->neq_op   = StringNeqAdapter;

    this->addMethod(NameId("size").id, makeFunctionInstanceObject(true, stringSizeMethod, NULL, rt));
    this->addMethod(NameId("set").id, makeFunctionInstanceObject(true, stringSetMethod, NULL, rt));
}

Object *StringType::create(Runtime *rt) {
    ProfilerCAPTURE();
    Instance *ins = new (rt->alloc(sizeof(StringInstance))) StringInstance(rt);
    Object   *obj = newObject(true, ins, this, rt);
    return obj;
}

Object *StringType::copy(Object *obj, Runtime *rt) {
    ProfilerCAPTURE();
    rt->verifyIsOfType(obj, rt->string_type);
    if (obj->instance == NULL) {
        return newObject(false, NULL, this, rt);
    }
    auto ins = obj->instance->copy(rt);
    auto res = newObject(true, ins, this, rt);
    return res;
}

std::string StringType::userRepr() {
    ProfilerCAPTURE();
    if (this == NULL) {
        return "StringType(NULL)";
    }
    return "StringType";
}

std::string &getStringData(Object *obj, Runtime *rt) {
    ProfilerCAPTURE();
    rt->verifyIsInstanceObject(obj, rt->string_type);
    return icast(obj->instance, StringInstance)->data;
}

std::string &getStringData(Object *obj, Runtime *rt, const TextArea &ta) {
    ProfilerCAPTURE();
    rt->verifyIsInstanceObject(obj, rt->string_type, ta);
    return icast(obj->instance, StringInstance)->data;
}

Object *makeStringInstanceObject(const std::string &value, Runtime *rt) {
    ProfilerCAPTURE();
    auto res               = rt->make(rt->string_type, Runtime::INSTANCE_OBJECT);
    getStringDataFast(res) = value;
    return res;
}

}    // namespace Cotton::Builtin
