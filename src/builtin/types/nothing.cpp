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

Instance *NothingInstance::copy() {
    ProfilerCAPTURE();
    Instance *res = new (std::nothrow) NothingInstance(rt);
    if (res == NULL) {
        rt->signalError("Failed to copy " + this->shortRepr());
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

std::string NothingInstance::shortRepr() {
    ProfilerCAPTURE();
    if (this == NULL) {
        return "NULL";
    }
    return "NothingInstance(id = " + std::to_string(this->id) + ")";
}

class NothingUnsupportedAdapter: public OperatorAdapter {
public:
    NothingUnsupportedAdapter(Runtime *rt)
        : OperatorAdapter(rt) {}

    Object *operator()(Object *self, const std::vector<Object *> &others) {
        ProfilerCAPTURE();
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class NothingEqAdapter: public OperatorAdapter {
public:
    NothingEqAdapter(Runtime *rt)
        : OperatorAdapter(rt) {}

    Object *operator()(Object *self, const std::vector<Object *> &others) {
        ProfilerCAPTURE();
        if (others.size() != 1) {
            rt->signalError("Expected exactly one right-side argument");
        }
        auto &arg1 = others[0];
        if (!rt->isTypeObject(arg1)) {
            rt->signalError("Right-side object is invalid: " + arg1->shortRepr());
        }

        bool i1 = rt->isInstanceObject(self);
        bool i2 = arg1->instance != NULL;

        if (i1 && i2) {
            auto res                 = rt->make(rt->boolean_type, Runtime::INSTANCE_OBJECT);
            getBooleanValue(res, rt) = (arg1->type->id == self->type->id);
            return res;
        }
        else if (!i1 && !i2) {
            auto res                 = rt->make(rt->boolean_type, Runtime::INSTANCE_OBJECT);
            getBooleanValue(res, rt) = (arg1->type->id == self->type->id);
            return res;
        }
        else {
            auto res                 = rt->make(rt->boolean_type, Runtime::INSTANCE_OBJECT);
            getBooleanValue(res, rt) = false;
            return res;
        }
    }
};

class NothingNeqAdapter: public NothingEqAdapter {
public:
    NothingNeqAdapter(Runtime *rt)
        : NothingEqAdapter(rt) {}

    Object *operator()(Object *self, const std::vector<Object *> &others) {
        ProfilerCAPTURE();
        auto res                 = NothingEqAdapter::operator()(self, others);
        getBooleanValue(res, rt) = !getBooleanValue(res, rt);
        return res;
    }
};

// TODO: add all operators to function and nothing
NothingType::NothingType(Runtime *rt)
    : Type(rt) {
    ProfilerCAPTURE();
    this->addOperator(OperatorNode::POST_PLUS_PLUS, new NothingUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::POST_MINUS_MINUS, new NothingUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::CALL, new NothingUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::INDEX, new NothingUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::PRE_PLUS_PLUS, new NothingUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::PRE_MINUS_MINUS, new NothingUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::PRE_PLUS, new NothingUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::PRE_MINUS, new NothingUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::NOT, new NothingUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::INVERSE, new NothingUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::MULT, new NothingUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::DIV, new NothingUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::REM, new NothingUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::RIGHT_SHIFT, new NothingUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::LEFT_SHIFT, new NothingUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::PLUS, new NothingUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::MINUS, new NothingUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::LESS, new NothingUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::LESS_EQUAL, new NothingUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::GREATER, new NothingUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::GREATER_EQUAL, new NothingUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::EQUAL, new NothingEqAdapter(rt));
    this->addOperator(OperatorNode::NOT_EQUAL, new NothingNeqAdapter(rt));
    this->addOperator(OperatorNode::BITAND, new NothingUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::BITXOR, new NothingUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::BITOR, new NothingUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::AND, new NothingUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::OR, new NothingUnsupportedAdapter(rt));
}

Object *NothingType::create() {
    ProfilerCAPTURE();
    auto ins = createInstance(rt, NothingInstance);
    auto obj = createObject(rt, true, ins, this);
    return obj;
}

std::string NothingType::shortRepr() {
    ProfilerCAPTURE();
    if (this == NULL) {
        return "NULL";
    }
    return "NothingType(id = " + std::to_string(this->id) + ")";
}

Object *NothingType::copy(Object *obj) {
    ProfilerCAPTURE();
    if (!rt->isTypeObject(obj) || obj->type->id != rt->nothing_type->id) {
        rt->signalError("Failed to copy an invalid object: " + obj->shortRepr());
    }
    if (obj->instance == NULL) {
        return createObject(rt, false, NULL, this);
    }
    auto ins = obj->instance->copy();
    auto res = createObject(rt, true, ins, this);
    return res;
}

Object *makeNothingInstanceObject(Runtime *rt) {
    ProfilerCAPTURE();
    auto res = rt->make(rt->nothing_type, rt->INSTANCE_OBJECT);
    return res;
}
}    // namespace Cotton::Builtin
