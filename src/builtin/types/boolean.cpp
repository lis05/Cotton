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

#include "api.h"

namespace Cotton::Builtin {
BooleanInstance::BooleanInstance(Runtime *rt)
    : Instance(rt, sizeof(BooleanInstance)) {
    this->value = false;
}

BooleanInstance::~BooleanInstance() {}

Instance *BooleanInstance::copy() {
    Instance *res = new (std::nothrow) BooleanInstance(rt);
    if (res == NULL) {
        rt->signalError("Failed to copy " + this->shortRepr());
    }
    icast(res, BooleanInstance)->value = this->value;
    return res;
}

std::string BooleanInstance::shortRepr() {
    if (this == NULL) {
        return "BooleanInstance(NULL)";
    }
    return "BooleanInstance(id = " + std::to_string(this->id) + ", value = " + (this->value ? "true" : "false")
           + ")";
}

size_t BooleanInstance::getSize() {
    return sizeof(BooleanInstance);
}

size_t BooleanType::getInstanceSize() {
    return sizeof(BooleanInstance);
}

class BooleanUnsupportedAdapter: public OperatorAdapter {
public:
    BooleanUnsupportedAdapter(Runtime *rt)
        : OperatorAdapter(rt) {}

    Object *operator()(Object *self, const std::vector<Object *> &others) {
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class BooleanNotAdapter: public OperatorAdapter {
public:
    BooleanNotAdapter(Runtime *rt)
        : OperatorAdapter(rt) {}

    Object *operator()(Object *self, const std::vector<Object *> &others) {
        if (!rt->isTypeObject(self) || self->type->id != rt->boolean_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        auto res                 = rt->make(rt->boolean_type, Runtime::INSTANCE_OBJECT);
        getBooleanValue(res, rt) = !getBooleanValue(self, rt);
        return res;
    }
};

class BooleanEqAdapter: public OperatorAdapter {
public:
    BooleanEqAdapter(Runtime *rt)
        : OperatorAdapter(rt) {}

    Object *operator()(Object *self, const std::vector<Object *> &others) {
        if (!rt->isTypeObject(self) || self->type->id != rt->boolean_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        if (others.size() != 1) {
            rt->signalError("Expected exactly one right-side argument");
            return NULL;
        }
        auto &arg1 = others[0];
        if (!rt->isTypeObject(arg1)) {
            rt->signalError("Right-side object is invalid: " + arg1->shortRepr());
        }

        if (rt->isInstanceObject(self) && rt->isInstanceObject(arg1)) {
            if (self->type->id != arg1->type->id) {
                return makeBooleanInstanceObject(false, rt);
            }
            return makeBooleanInstanceObject(getBooleanValueFast(self) == getBooleanValueFast(arg1), rt);
        }
        else if (!rt->isInstanceObject(self) && !rt->isInstanceObject(arg1)) {
            return makeBooleanInstanceObject(self->type->id == arg1->type->id, rt);
        }
        else {
            return makeBooleanInstanceObject(false, rt);
        }
    }
};

class BooleanNeqAdapter: public BooleanEqAdapter {
public:
    BooleanNeqAdapter(Runtime *rt)
        : BooleanEqAdapter(rt) {}

    Object *operator()(Object *self, const std::vector<Object *> &others) {
        auto res                 = BooleanEqAdapter::operator()(self, others);
        getBooleanValue(res, rt) = !getBooleanValue(res, rt);
        return res;
    }
};

class BooleanAndAdapter: public OperatorAdapter {
public:
    BooleanAndAdapter(Runtime *rt)
        : OperatorAdapter(rt) {}

    Object *operator()(Object *self, const std::vector<Object *> &others) {
        if (!rt->isTypeObject(self) || self->type->id != rt->boolean_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        if (others.size() != 1) {
            rt->signalError("Expected exactly one right-side argument");
            return NULL;
        }
        auto &arg1 = others[0];
        if (!rt->isTypeObject(arg1)) {
            rt->signalError("Right-side object is invalid: " + arg1->shortRepr());
        }
        if (arg1->type->id != rt->boolean_type->id) {
            rt->signalError("Right-side object is not Boolean: " + arg1->shortRepr());
        }
        if (!rt->isInstanceObject(arg1)) {
            rt->signalError("Right-side object is not an instance object: " + arg1->shortRepr());
        }

        return makeBooleanInstanceObject(getBooleanValue(self, rt) && getBooleanValue(arg1, rt), rt);
    }
};

class BooleanOrAdapter: public OperatorAdapter {
public:
    BooleanOrAdapter(Runtime *rt)
        : OperatorAdapter(rt) {}

    Object *operator()(Object *self, const std::vector<Object *> &others) {
        if (!rt->isTypeObject(self) || self->type->id != rt->boolean_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        if (others.size() != 1) {
            rt->signalError("Expected exactly one right-side argument");
            return NULL;
        }
        auto &arg1 = others[0];
        if (!rt->isTypeObject(arg1)) {
            rt->signalError("Right-side object is invalid: " + arg1->shortRepr());
        }
        if (arg1->type->id != rt->boolean_type->id) {
            rt->signalError("Right-side object is not Boolean: " + arg1->shortRepr());
        }
        if (!rt->isInstanceObject(arg1)) {
            rt->signalError("Right-side object is not an instance object: " + arg1->shortRepr());
        }

        return makeBooleanInstanceObject(getBooleanValue(self, rt) || getBooleanValue(arg1, rt), rt);
    }
};

// TODO: add all operators to function and nothing
BooleanType::BooleanType(Runtime *rt)
    : Type(rt) {
    this->addOperator(OperatorNode::POST_PLUS_PLUS, new BooleanUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::POST_MINUS_MINUS, new BooleanUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::CALL, new BooleanUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::INDEX, new BooleanUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::PRE_PLUS_PLUS, new BooleanUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::PRE_MINUS_MINUS, new BooleanUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::PRE_PLUS, new BooleanUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::PRE_MINUS, new BooleanUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::NOT, new BooleanNotAdapter(rt));
    this->addOperator(OperatorNode::INVERSE, new BooleanUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::MULT, new BooleanUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::DIV, new BooleanUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::REM, new BooleanUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::RIGHT_SHIFT, new BooleanUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::LEFT_SHIFT, new BooleanUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::PLUS, new BooleanUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::MINUS, new BooleanUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::LESS, new BooleanUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::LESS_EQUAL, new BooleanUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::GREATER, new BooleanUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::GREATER_EQUAL, new BooleanUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::EQUAL, new BooleanEqAdapter(rt));
    this->addOperator(OperatorNode::NOT_EQUAL, new BooleanNeqAdapter(rt));
    this->addOperator(OperatorNode::BITAND, new BooleanUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::BITXOR, new BooleanUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::BITOR, new BooleanUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::AND, new BooleanAndAdapter(rt));
    this->addOperator(OperatorNode::OR, new BooleanOrAdapter(rt));
}

Object *BooleanType::create() {
    auto ins = createInstance(rt, BooleanInstance);
    auto obj = createObject(rt, true, ins, this);
    return obj;
}

Object *BooleanType::copy(Object *obj) {
    if (!rt->isTypeObject(obj) || obj->type->id != rt->boolean_type->id) {
        rt->signalError("Failed to copy an invalid object: " + obj->shortRepr());
    }
    if (!rt->isInstanceObject(obj)) {
        return createObject(rt, false, NULL, this);
    }
    auto ins = obj->instance->copy();
    auto res = createObject(rt, true, ins, this);
    return res;
}

std::string BooleanType::shortRepr() {
    if (this == NULL) {
        return "BooleanType(NULL)";
    }
    return "BooleanType(id = " + std::to_string(this->id) + ")";
}

bool &getBooleanValue(Object *obj, Runtime *rt) {
    if (!rt->isInstanceObject(obj)) {
        rt->signalError(obj->shortRepr() + " is not an instance object");
    }
    if (obj->type->id != rt->boolean_type->id) {
        rt->signalError(obj->shortRepr() + " is not Boolean");
    }
    return icast(obj->instance, BooleanInstance)->value;
}

Object *makeBooleanInstanceObject(bool value, Runtime *rt) {
    auto res                                     = rt->make(rt->boolean_type, Runtime::INSTANCE_OBJECT);
    icast(res->instance, BooleanInstance)->value = value;
    return res;
}

}    // namespace Cotton::Builtin
