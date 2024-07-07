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
    this->data = {};
}

StringInstance::~StringInstance() {
    ProfilerCAPTURE();
}

Instance *StringInstance::copy(Runtime *rt) {
    ProfilerCAPTURE();
    Instance *res = new (rt->alloc(sizeof(StringInstance))) StringInstance(rt);

    if (res == NULL) {
        rt->signalError("Failed to copy " + this->shortRepr());
    }
    for (auto obj : this->data) {
        ((StringInstance *)res)->data.push_back(rt->copy(obj));
    }
    return res;
}

std::string StringInstance::shortRepr() {
    ProfilerCAPTURE();
    if (this == NULL) {
        return "StringInstance(NULL)";
    }
    return "StringInstance(id = " + std::to_string(this->id) + ", size = " + std::to_string(this->data.size())
           + ", data = ...)";
}

void StringInstance::destroy(Runtime *rt) {
    rt->dealloc(this, sizeof(StringInstance));
}

size_t StringInstance::getSize() {
    ProfilerCAPTURE();
    return sizeof(StringInstance);
}

std::vector<Object *> StringInstance::getGCReachable() {
    ProfilerCAPTURE();
    auto res = Instance::getGCReachable();
    for (auto obj : this->data) {
        res.push_back(obj);
    }
    return res;
}

void StringInstance::spreadSingleUse() {
    for (auto obj : this->data) {
        obj->spreadSingleUse();
    }
}

void StringInstance::spreadMultiUse() {
    for (auto obj : this->data) {
        obj->spreadMultiUse();
    }
}

size_t StringType::getInstanceSize() {
    ProfilerCAPTURE();
    return sizeof(StringInstance);
}

class StringUnsupportedAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        ProfilerCAPTURE();
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class StringIndexAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        ProfilerCAPTURE();

        if (!isInstanceObject(self)) {
            rt->signalError(self->shortRepr() + " does not support that operator");
        }
        if (others.size() != 1) {
            rt->signalError("Expected exactly one right-side argument");
            return NULL;
        }
        auto &arg1 = others[0];
        if (!isTypeObject(arg1)) {
            rt->signalError("Right-side object is invalid: " + arg1->shortRepr());
        }
        if (arg1->instance == NULL || arg1->type->id != rt->integer_type->id) {
            rt->signalError("Index " + arg1->shortRepr() + " must be an integer instance object");
        }
        if (!(0 <= getIntegerValueFast(arg1) && getIntegerValueFast(arg1) < getStringDataFast(self).size())) {
            rt->signalError("Index " + arg1->shortRepr() + " is out of string " + self->shortRepr() + " range");
        }
        return getStringDataFast(self)[getIntegerValueFast(arg1)];
    }
};

class StringAddAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        ProfilerCAPTURE();

        if (!isInstanceObject(self)) {
            rt->signalError(self->shortRepr() + " does not support that operator");
        }
        if (others.size() != 1) {
            rt->signalError("Expected exactly one right-side argument");
            return NULL;
        }
        auto &arg1 = others[0];
        if (!isTypeObject(arg1)) {
            rt->signalError("Right-side object is invalid: " + arg1->shortRepr());
        }
        if (arg1->instance == NULL || arg1->type->id != rt->string_type->id) {
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be a String instance object");
        }

        auto res = rt->copy(self);
        for (auto obj : getStringDataFast(arg1)) {
            getStringDataFast(res).push_back(obj);
        }

        return res;
    }
};

class StringEqAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        ProfilerCAPTURE();
        if (others.size() != 1) {
            rt->signalError("Expected exactly one right-side argument");
            return NULL;
        }
        auto &arg1 = others[0];
        if (!isTypeObject(arg1)) {
            rt->signalError("Right-side object is invalid: " + arg1->shortRepr());
        }

        bool i1 = isInstanceObject(self);
        bool i2 = arg1->instance != NULL;

        if (i1 && i2) {
            if (self->type->id != arg1->type->id) {
                return makeBooleanInstanceObject(false, rt);
            }
            if (getStringDataFast(self).size() != getStringDataFast(arg1).size()) {
                return makeBooleanInstanceObject(false, rt);
            }
            for (int i = 0; i < getStringDataFast(self).size(); i++) {
                if (!rt->runOperator(OperatorNode::EQUAL,
                                     getStringDataFast(self)[i],
                                     {getStringDataFast(arg1)[i]}))
                {
                    return makeBooleanInstanceObject(false, rt);
                }
            }
            return makeBooleanInstanceObject(true, rt);
        }
        else if (!i1 && !i2) {
            return makeBooleanInstanceObject(self->type->id == arg1->type->id, rt);
        }
        else {
            return makeBooleanInstanceObject(false, rt);
        }
    }
};

class StringNeqAdapter: public StringEqAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        ProfilerCAPTURE();
        auto res                 = StringEqAdapter::operator()(self, others, rt);
        getBooleanValueFast(res) = !getBooleanValueFast(res);
        return res;
    }
};

static Object *stringSizeMethod(const std::vector<Object *> &args, Runtime *rt) {
    ProfilerCAPTURE();
    if (args.size() < 1) {
        rt->signalError("Expected a caller object");
    }
    auto self = args[0];
    if (!isTypeObject(self)) {
        rt->signalError("Caller is invalid: " + self->shortRepr());
    }
    if (self->instance == NULL || self->type->id != rt->string_type->id) {
        rt->signalError("Caller must be a String instance object: " + self->shortRepr());
    }
    return makeIntegerInstanceObject(getStringDataFast(self).size(), rt);
}

// TODO: add all operators to function and nothing
StringType::StringType(Runtime *rt)
    : Type(rt) {
    ProfilerCAPTURE();
    this->addOperator(OperatorNode::POST_PLUS_PLUS, new StringUnsupportedAdapter());
    this->addOperator(OperatorNode::POST_MINUS_MINUS, new StringUnsupportedAdapter());
    this->addOperator(OperatorNode::CALL, new StringUnsupportedAdapter());
    this->addOperator(OperatorNode::INDEX, new StringIndexAdapter());
    this->addOperator(OperatorNode::PRE_PLUS_PLUS, new StringUnsupportedAdapter());
    this->addOperator(OperatorNode::PRE_MINUS_MINUS, new StringUnsupportedAdapter());
    this->addOperator(OperatorNode::PRE_PLUS, new StringUnsupportedAdapter());
    this->addOperator(OperatorNode::PRE_MINUS, new StringUnsupportedAdapter());
    this->addOperator(OperatorNode::NOT, new StringUnsupportedAdapter());
    this->addOperator(OperatorNode::INVERSE, new StringUnsupportedAdapter());
    this->addOperator(OperatorNode::MULT, new StringUnsupportedAdapter());
    this->addOperator(OperatorNode::DIV, new StringUnsupportedAdapter());
    this->addOperator(OperatorNode::REM, new StringUnsupportedAdapter());
    this->addOperator(OperatorNode::RIGHT_SHIFT, new StringUnsupportedAdapter());
    this->addOperator(OperatorNode::LEFT_SHIFT, new StringUnsupportedAdapter());
    this->addOperator(OperatorNode::PLUS, new StringAddAdapter());
    this->addOperator(OperatorNode::MINUS, new StringUnsupportedAdapter());
    this->addOperator(OperatorNode::LESS, new StringUnsupportedAdapter());
    this->addOperator(OperatorNode::LESS_EQUAL, new StringUnsupportedAdapter());
    this->addOperator(OperatorNode::GREATER, new StringUnsupportedAdapter());
    this->addOperator(OperatorNode::GREATER_EQUAL, new StringUnsupportedAdapter());
    this->addOperator(OperatorNode::EQUAL, new StringEqAdapter());
    this->addOperator(OperatorNode::NOT_EQUAL, new StringNeqAdapter());
    this->addOperator(OperatorNode::BITAND, new StringUnsupportedAdapter());
    this->addOperator(OperatorNode::BITXOR, new StringUnsupportedAdapter());
    this->addOperator(OperatorNode::BITOR, new StringUnsupportedAdapter());
    this->addOperator(OperatorNode::AND, new StringUnsupportedAdapter());
    this->addOperator(OperatorNode::OR, new StringUnsupportedAdapter());

    this->addMethod(NameId("size").id, makeFunctionInstanceObject(true, stringSizeMethod, NULL, rt));
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
        rt->signalError("Failed to copy an invalid object: " + obj->shortRepr());
    }
    if (obj->instance == NULL) {
        return newObject(false, NULL, this, rt);
    }
    auto ins = obj->instance->copy(rt);
    auto res = newObject(true, ins, this, rt);
    return res;
}

std::string StringType::shortRepr() {
    ProfilerCAPTURE();
    if (this == NULL) {
        return "StringType(NULL)";
    }
    return "StringType(id = " + std::to_string(this->id) + ")";
}

std::vector<Object *> &getStringData(Object *obj, Runtime *rt) {
    ProfilerCAPTURE();
    if (!isInstanceObject(obj)) {
        rt->signalError(obj->shortRepr() + " is not an instance object");
    }
    if (obj->type->id != rt->string_type->id) {
        rt->signalError(obj->shortRepr() + " is not String");
    }
    return icast(obj->instance, StringInstance)->data;
}

Object *makeStringInstanceObject(const std::string &value, Runtime *rt) {
    ProfilerCAPTURE();
    auto res = rt->make(rt->string_type, Runtime::INSTANCE_OBJECT);
    for (auto c : value) {
        getStringDataFast(res).push_back(makeCharacterInstanceObject(c, rt));
    }
    return res;
}

}    // namespace Cotton::Builtin
