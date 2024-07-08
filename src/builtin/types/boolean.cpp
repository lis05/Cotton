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
    Instance *res = new (rt->alloc(sizeof(BooleanInstance))) BooleanInstance(rt);
    if (res == NULL) {
        rt->signalError("Failed to copy " + this->shortRepr());
    }
    icast(res, BooleanInstance)->value = this->value;
    return res;
}

std::string BooleanInstance::shortRepr() {
    ProfilerCAPTURE();
    if (this == NULL) {
        return "BooleanInstance(NULL)";
    }
    return "BooleanInstance(id = " + std::to_string(this->id) + ", value = " + (this->value ? "true" : "false")
           + ")";
}

void BooleanInstance::destroy(Runtime *rt) {
    rt->dealloc(this, sizeof(BooleanInstance));
}

size_t BooleanInstance::getSize() {
    ProfilerCAPTURE();
    return sizeof(BooleanInstance);
}

size_t BooleanType::getInstanceSize() {
    ProfilerCAPTURE();
    return sizeof(BooleanInstance);
}

static Object *BooleanUnsupportedAdapter(Object *self, const std::vector<Object *> &others, Runtime *rt) {
    ProfilerCAPTURE();
    rt->signalError(self->shortRepr() + " does not support that operator");
}

static Object *BooleanNotAdapter(Object *self, const std::vector<Object *> &others, Runtime *rt) {
    ProfilerCAPTURE();

    auto res                 = rt->make(rt->boolean_type, Runtime::INSTANCE_OBJECT);
    getBooleanValue(res, rt) = !getBooleanValue(self, rt);
    return res;
}

static Object *BooleanEqAdapter(Object *self, const std::vector<Object *> &others, Runtime *rt) {
    ProfilerCAPTURE();

    if (others.size() != 1) {
        rt->signalError("Expected exactly one right-side argument");
        return NULL;
    }
    auto &arg1 = others[0];
    if (!isTypeObject(arg1)) {
        rt->signalError("Right-side object is invalid: " + arg1->shortRepr());
    }
    auto isSelfI = isInstanceObject(self);
    auto isArg1I = arg1->instance != NULL;
    if (isSelfI && isArg1I) {
        if (self->type->id != arg1->type->id) {
            return makeBooleanInstanceObject(false, rt);
        }
        return makeBooleanInstanceObject(getBooleanValueFast(self) == getBooleanValueFast(arg1), rt);
    }
    else if (!isSelfI && !isArg1I) {
        return makeBooleanInstanceObject(self->type->id == arg1->type->id, rt);
    }
    else {
        return makeBooleanInstanceObject(false, rt);
    }
}

static Object *BooleanNeqAdapter(Object *self, const std::vector<Object *> &others, Runtime *rt) {
    ProfilerCAPTURE();
    auto res                 = BooleanEqAdapter(self, others, rt);
    getBooleanValue(res, rt) = !getBooleanValue(res, rt);
    return res;
}

static Object *BooleanAndAdapter(Object *self, const std::vector<Object *> &others, Runtime *rt) {
    ProfilerCAPTURE();

    if (others.size() != 1) {
        rt->signalError("Expected exactly one right-side argument");
        return NULL;
    }
    auto &arg1 = others[0];
    if (!isTypeObject(arg1)) {
        rt->signalError("Right-side object is invalid: " + arg1->shortRepr());
    }
    if (arg1->type->id != rt->boolean_type->id) {
        rt->signalError("Right-side object is not Boolean: " + arg1->shortRepr());
    }
    if (arg1->instance == NULL) {
        rt->signalError("Right-side object is not an instance object: " + arg1->shortRepr());
    }

    return makeBooleanInstanceObject(getBooleanValue(self, rt) && getBooleanValue(arg1, rt), rt);
}

static Object *BooleanOrAdapter(Object *self, const std::vector<Object *> &others, Runtime *rt) {
    ProfilerCAPTURE();

    if (others.size() != 1) {
        rt->signalError("Expected exactly one right-side argument");
        return NULL;
    }
    auto &arg1 = others[0];
    if (!isTypeObject(arg1)) {
        rt->signalError("Right-side object is invalid: " + arg1->shortRepr());
    }
    if (arg1->type->id != rt->boolean_type->id) {
        rt->signalError("Right-side object is not Boolean: " + arg1->shortRepr());
    }
    if (arg1->instance == NULL) {
        rt->signalError("Right-side object is not an instance object: " + arg1->shortRepr());
    }

    return makeBooleanInstanceObject(getBooleanValue(self, rt) || getBooleanValue(arg1, rt), rt);
}

// TODO: add all operators to function and nothing
BooleanType::BooleanType(Runtime *rt)
    : Type(rt) {
    ProfilerCAPTURE();
    this->addOperator(OperatorNode::POST_PLUS_PLUS, BooleanUnsupportedAdapter);
    this->addOperator(OperatorNode::POST_MINUS_MINUS, BooleanUnsupportedAdapter);
    this->addOperator(OperatorNode::CALL, BooleanUnsupportedAdapter);
    this->addOperator(OperatorNode::INDEX, BooleanUnsupportedAdapter);
    this->addOperator(OperatorNode::PRE_PLUS_PLUS, BooleanUnsupportedAdapter);
    this->addOperator(OperatorNode::PRE_MINUS_MINUS, BooleanUnsupportedAdapter);
    this->addOperator(OperatorNode::PRE_PLUS, BooleanUnsupportedAdapter);
    this->addOperator(OperatorNode::PRE_MINUS, BooleanUnsupportedAdapter);
    this->addOperator(OperatorNode::NOT, BooleanNotAdapter);
    this->addOperator(OperatorNode::INVERSE, BooleanUnsupportedAdapter);
    this->addOperator(OperatorNode::MULT, BooleanUnsupportedAdapter);
    this->addOperator(OperatorNode::DIV, BooleanUnsupportedAdapter);
    this->addOperator(OperatorNode::REM, BooleanUnsupportedAdapter);
    this->addOperator(OperatorNode::RIGHT_SHIFT, BooleanUnsupportedAdapter);
    this->addOperator(OperatorNode::LEFT_SHIFT, BooleanUnsupportedAdapter);
    this->addOperator(OperatorNode::PLUS, BooleanUnsupportedAdapter);
    this->addOperator(OperatorNode::MINUS, BooleanUnsupportedAdapter);
    this->addOperator(OperatorNode::LESS, BooleanUnsupportedAdapter);
    this->addOperator(OperatorNode::LESS_EQUAL, BooleanUnsupportedAdapter);
    this->addOperator(OperatorNode::GREATER, BooleanUnsupportedAdapter);
    this->addOperator(OperatorNode::GREATER_EQUAL, BooleanUnsupportedAdapter);
    this->addOperator(OperatorNode::EQUAL, BooleanEqAdapter);
    this->addOperator(OperatorNode::NOT_EQUAL, BooleanNeqAdapter);
    this->addOperator(OperatorNode::BITAND, BooleanUnsupportedAdapter);
    this->addOperator(OperatorNode::BITXOR, BooleanUnsupportedAdapter);
    this->addOperator(OperatorNode::BITOR, BooleanUnsupportedAdapter);
    this->addOperator(OperatorNode::AND, BooleanAndAdapter);
    this->addOperator(OperatorNode::OR, BooleanOrAdapter);
}

Object *BooleanType::create(Runtime *rt) {
    ProfilerCAPTURE();
    Instance *ins = new (rt->alloc(sizeof(BooleanInstance))) BooleanInstance(rt);
    Object   *obj = newObject(true, ins, this, rt);
    return obj;
}

Object *BooleanType::copy(Object *obj, Runtime *rt) {
    ProfilerCAPTURE();
    if (!isTypeObject(obj) || obj->type->id != rt->boolean_type->id) {
        rt->signalError("Failed to copy an invalid object: " + obj->shortRepr());
    }
    if (obj->instance == NULL) {
        return newObject(false, NULL, this, rt);
    }
    auto ins = obj->instance->copy(rt);
    auto res = newObject(true, ins, this, rt);
    return res;
}

std::string BooleanType::shortRepr() {
    ProfilerCAPTURE();
    if (this == NULL) {
        return "BooleanType(NULL)";
    }
    return "BooleanType(id = " + std::to_string(this->id) + ")";
}

bool &getBooleanValue(Object *obj, Runtime *rt) {
    ProfilerCAPTURE();
    if (!isInstanceObject(obj)) {
        rt->signalError(obj->shortRepr() + " is not an instance object");
    }
    if (obj->type->id != rt->boolean_type->id) {
        rt->signalError(obj->shortRepr() + " is not Boolean");
    }
    return icast(obj->instance, BooleanInstance)->value;
}

Object *makeBooleanInstanceObject(bool value, Runtime *rt) {
    ProfilerCAPTURE();
    auto res                                     = rt->make(rt->boolean_type, Runtime::INSTANCE_OBJECT);
    icast(res->instance, BooleanInstance)->value = value;
    return res;
}

}    // namespace Cotton::Builtin
