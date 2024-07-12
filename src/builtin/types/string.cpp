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
        rt->signalError("Failed to copy " + this->userRepr());
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
    if (!(0 <= getIntegerValueFast(arg) && getIntegerValueFast(arg) < getStringDataFast(self).size())) {
        rt->signalError("Index " + arg->userRepr() + " is out of string " + self->userRepr() + " range");
    }
    if (!execution_result_matters) {
        return NULL;
    }
    return makeCharacterInstanceObject(getStringDataFast(self)[getIntegerValueFast(arg)], rt);
}

static Object *StringAddAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();

    if (!isInstanceObject(self)) {
        rt->signalError(self->userRepr() + " does not support that operator");
    }

    if (!isTypeObject(arg)) {
        rt->signalError("Right-side object is invalid: " + arg->userRepr());
    }
    if (arg->instance == NULL || arg->type->id != rt->string_type->id) {
        rt->signalError("Right-side object " + arg->userRepr() + " must be a String instance object");
    }

    if (!execution_result_matters) {
        return NULL;
    }

    auto res                = rt->copy(self);
    getStringDataFast(res) += getStringDataFast(arg);

    return res;
}

static Object *StringEqAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
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
        if (getStringDataFast(self).size() != getStringDataFast(arg).size()) {
            return rt->protected_false;
        }
        return (getStringDataFast(self) == getStringDataFast(arg)) ? rt->protected_true : rt->protected_false;
    }
    else if (!i1 && !i2) {
        return (self->type->id == arg->type->id) ? rt->protected_true : rt->protected_false;
    }
    else {
        return rt->protected_false;
    }
}

static Object *StringNeqAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    auto res = StringEqAdapter(self, arg, rt, execution_result_matters);
    return (!getBooleanValueFast(res)) ? rt->protected_true : rt->protected_false;
}

static Object *stringSizeMethod(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    if (args.size() < 1) {
        rt->signalError("Expected a caller object");
    }
    auto self = args[0];
    if (!isTypeObject(self)) {
        rt->signalError("Caller is invalid: " + self->userRepr());
    }
    if (self->instance == NULL || self->type->id != rt->string_type->id) {
        rt->signalError("Caller must be a String instance object: " + self->userRepr());
    }

    if (!execution_result_matters) {
        return NULL;
    }

    return makeIntegerInstanceObject(getStringDataFast(self).size(), rt);
}

static Object *stringSetMethod(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    if (args.size() < 3) {
        rt->signalError("Expected two arguments");
    }
    auto self  = args[0];
    auto index = args[1];
    auto value = args[2];

    if (!isInstanceObjectOfType(self, rt->string_type)) {
        rt->signalError("Caller must be a String instance object: " + self->userRepr());
    }
    if (!isInstanceObjectOfType(index, rt->integer_type)) {
        rt->signalError("Index must be an Integer instance object: " + index->userRepr());
    }
    if (!isInstanceObjectOfType(value, rt->character_type)) {
        rt->signalError("Value must be a Character instance object: " + value->userRepr());
    }

    int64_t ind  = getIntegerValueFast(index);
    auto   &data = getStringDataFast(self);
    if (!(0 <= ind && ind < data.size())) {
        rt->signalError("Index is out of range: " + index->userRepr());
    }
    data[ind] = getCharacterValueFast(value);
    return rt->protected_nothing;
}

// TODO: add all operators to function and nothing
StringType::StringType(Runtime *rt)
    : Type(rt) {
    ProfilerCAPTURE();
    this->index_op = StringIndexAdapter;
    this->add_op = StringAddAdapter;
    this->eq_op  = StringEqAdapter;
    this->neq_op = StringNeqAdapter;

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
    if (!isTypeObject(obj) || obj->type->id != rt->string_type->id) {
        rt->signalError("Failed to copy an invalid object: " + obj->userRepr());
    }
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
    if (!isInstanceObject(obj)) {
        rt->signalError(obj->userRepr() + " is not an instance object");
    }
    if (obj->type->id != rt->string_type->id) {
        rt->signalError(obj->userRepr() + " is not String");
    }
    return icast(obj->instance, StringInstance)->data;
}

Object *makeStringInstanceObject(const std::string &value, Runtime *rt) {
    ProfilerCAPTURE();
    auto res = rt->make(rt->string_type, Runtime::INSTANCE_OBJECT);
    getStringDataFast(res) = value;
    return res;
}

}    // namespace Cotton::Builtin
