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
RealInstance::RealInstance(Runtime *rt)
    : Instance(rt, sizeof(RealInstance)) {
    this->value = 0;
}

RealInstance::~RealInstance() {}

Instance *RealInstance::copy() {
    Instance *res = new (std::nothrow) RealInstance(rt);
    if (res == NULL) {
        rt->signalError("Failed to copy " + this->shortRepr());
    }
    icast(res, RealInstance)->value = this->value;
    return res;
}

std::string RealInstance::shortRepr() {
    if (this == NULL) {
        return "RealInstance(NULL)";
    }
    return "RealInstance(id = " + std::to_string(this->id) + ", value = " + std::to_string(this->value) + ")";
}

size_t RealInstance::getSize() {
    return sizeof(RealInstance);
}

size_t RealType::getInstanceSize() {
    return sizeof(RealInstance);
}

class RealUnsupportedAdapter: public OperatorAdapter {
public:
    RealUnsupportedAdapter(Runtime *rt)
        : OperatorAdapter(rt) {}

    Object *operator()(Object *self, const std::vector<Object *> &others) {
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class RealPositiveAdapter: public OperatorAdapter {
public:
    RealPositiveAdapter(Runtime *rt)
        : OperatorAdapter(rt) {}

    Object *operator()(Object *self, const std::vector<Object *> &others) {
        if (!rt->isTypeObject(self) || self->type->id != rt->real_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        if (!rt->isInstanceObject(self)) {
            rt->signalError(self->shortRepr() + " does not support that operator");
        }

        auto res = self->type->copy(self);
        return res;
    }
};

class RealNegativeAdapter: public OperatorAdapter {
public:
    RealNegativeAdapter(Runtime *rt)
        : OperatorAdapter(rt) {}

    Object *operator()(Object *self, const std::vector<Object *> &others) {
        if (!rt->isTypeObject(self) || self->type->id != rt->real_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        if (!rt->isInstanceObject(self)) {
            rt->signalError(self->shortRepr() + " does not support that operator");
        }

        auto res               = self->type->copy(self);
        getRealValueFast(res) *= -1;
        return res;
    }
};

class RealMultAdapter: public OperatorAdapter {
public:
    RealMultAdapter(Runtime *rt)
        : OperatorAdapter(rt) {}

    Object *operator()(Object *self, const std::vector<Object *> &others) {
        if (!rt->isTypeObject(self) || self->type->id != rt->real_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        if (!rt->isInstanceObject(self)) {
            rt->signalError(self->shortRepr() + " does not support that operator");
        }
        if (others.size() != 1) {
            rt->signalError("Expected exactly one right-side argument");
            return NULL;
        }
        auto &arg1 = others[0];
        if (!rt->isTypeObject(arg1)) {
            rt->signalError("Right-side object is invalid: " + arg1->shortRepr());
        }
        if (!rt->isInstanceObject(arg1) || arg1->type->id != rt->real_type->id) {
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be a Real instance object");
        }

        auto res = makeRealInstanceObject(getRealValueFast(self) * getRealValueFast(arg1), rt);
        return res;
    }
};

class RealDivAdapter: public OperatorAdapter {
public:
    RealDivAdapter(Runtime *rt)
        : OperatorAdapter(rt) {}

    Object *operator()(Object *self, const std::vector<Object *> &others) {
        if (!rt->isTypeObject(self) || self->type->id != rt->real_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        if (!rt->isInstanceObject(self)) {
            rt->signalError(self->shortRepr() + " does not support that operator");
        }
        if (others.size() != 1) {
            rt->signalError("Expected exactly one right-side argument");
            return NULL;
        }
        auto &arg1 = others[0];
        if (!rt->isTypeObject(arg1)) {
            rt->signalError("Right-side object is invalid: " + arg1->shortRepr());
        }
        if (!rt->isInstanceObject(arg1) || arg1->type->id != rt->real_type->id) {
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be a Real instance object");
        }

        auto res = makeRealInstanceObject(getRealValueFast(self) / getRealValueFast(arg1), rt);
        return res;
    }
};

class RealAddAdapter: public OperatorAdapter {
public:
    RealAddAdapter(Runtime *rt)
        : OperatorAdapter(rt) {}

    Object *operator()(Object *self, const std::vector<Object *> &others) {
        if (!rt->isTypeObject(self) || self->type->id != rt->real_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        if (!rt->isInstanceObject(self)) {
            rt->signalError(self->shortRepr() + " does not support that operator");
        }
        if (others.size() != 1) {
            rt->signalError("Expected exactly one right-side argument");
            return NULL;
        }
        auto &arg1 = others[0];
        if (!rt->isTypeObject(arg1)) {
            rt->signalError("Right-side object is invalid: " + arg1->shortRepr());
        }
        if (!rt->isInstanceObject(arg1) || arg1->type->id != rt->real_type->id) {
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be a Real instance object");
        }

        auto res = makeRealInstanceObject(getRealValueFast(self) + getRealValueFast(arg1), rt);
        return res;
    }
};

class RealSubAdapter: public OperatorAdapter {
public:
    RealSubAdapter(Runtime *rt)
        : OperatorAdapter(rt) {}

    Object *operator()(Object *self, const std::vector<Object *> &others) {
        if (!rt->isTypeObject(self) || self->type->id != rt->real_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        if (!rt->isInstanceObject(self)) {
            rt->signalError(self->shortRepr() + " does not support that operator");
        }
        if (others.size() != 1) {
            rt->signalError("Expected exactly one right-side argument");
            return NULL;
        }
        auto &arg1 = others[0];
        if (!rt->isTypeObject(arg1)) {
            rt->signalError("Right-side object is invalid: " + arg1->shortRepr());
        }
        if (!rt->isInstanceObject(arg1) || arg1->type->id != rt->real_type->id) {
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be a Real instance object");
        }

        auto res = makeRealInstanceObject(getRealValueFast(self) - getRealValueFast(arg1), rt);
        return res;
    }
};

class RealLtAdapter: public OperatorAdapter {
public:
    RealLtAdapter(Runtime *rt)
        : OperatorAdapter(rt) {}

    Object *operator()(Object *self, const std::vector<Object *> &others) {
        if (!rt->isTypeObject(self) || self->type->id != rt->real_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        if (!rt->isInstanceObject(self)) {
            rt->signalError(self->shortRepr() + " does not support that operator");
        }
        if (others.size() != 1) {
            rt->signalError("Expected exactly one right-side argument");
            return NULL;
        }
        auto &arg1 = others[0];
        if (!rt->isTypeObject(arg1)) {
            rt->signalError("Right-side object is invalid: " + arg1->shortRepr());
        }
        if (!rt->isInstanceObject(arg1) || arg1->type->id != rt->real_type->id) {
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be a Real instance object");
        }

        auto res = makeBooleanInstanceObject(getRealValueFast(self) < getRealValueFast(arg1), rt);
        return res;
    }
};

class RealLeqAdapter: public OperatorAdapter {
public:
    RealLeqAdapter(Runtime *rt)
        : OperatorAdapter(rt) {}

    Object *operator()(Object *self, const std::vector<Object *> &others) {
        if (!rt->isTypeObject(self) || self->type->id != rt->real_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        if (!rt->isInstanceObject(self)) {
            rt->signalError(self->shortRepr() + " does not support that operator");
        }
        if (others.size() != 1) {
            rt->signalError("Expected exactly one right-side argument");
            return NULL;
        }
        auto &arg1 = others[0];
        if (!rt->isTypeObject(arg1)) {
            rt->signalError("Right-side object is invalid: " + arg1->shortRepr());
        }
        if (!rt->isInstanceObject(arg1) || arg1->type->id != rt->real_type->id) {
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be a Real instance object");
        }

        auto res = makeBooleanInstanceObject(getRealValueFast(self) <= getRealValueFast(arg1), rt);
        return res;
    }
};

class RealGtAdapter: public OperatorAdapter {
public:
    RealGtAdapter(Runtime *rt)
        : OperatorAdapter(rt) {}

    Object *operator()(Object *self, const std::vector<Object *> &others) {
        if (!rt->isTypeObject(self) || self->type->id != rt->real_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        if (!rt->isInstanceObject(self)) {
            rt->signalError(self->shortRepr() + " does not support that operator");
        }
        if (others.size() != 1) {
            rt->signalError("Expected exactly one right-side argument");
            return NULL;
        }
        auto &arg1 = others[0];
        if (!rt->isTypeObject(arg1)) {
            rt->signalError("Right-side object is invalid: " + arg1->shortRepr());
        }
        if (!rt->isInstanceObject(arg1) || arg1->type->id != rt->real_type->id) {
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be a Real instance object");
        }

        auto res = makeBooleanInstanceObject(getRealValueFast(self) > getRealValueFast(arg1), rt);
        return res;
    }
};

class RealGeqAdapter: public OperatorAdapter {
public:
    RealGeqAdapter(Runtime *rt)
        : OperatorAdapter(rt) {}

    Object *operator()(Object *self, const std::vector<Object *> &others) {
        if (!rt->isTypeObject(self) || self->type->id != rt->real_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        if (!rt->isInstanceObject(self)) {
            rt->signalError(self->shortRepr() + " does not support that operator");
        }
        if (others.size() != 1) {
            rt->signalError("Expected exactly one right-side argument");
            return NULL;
        }
        auto &arg1 = others[0];
        if (!rt->isTypeObject(arg1)) {
            rt->signalError("Right-side object is invalid: " + arg1->shortRepr());
        }
        if (!rt->isInstanceObject(arg1) || arg1->type->id != rt->real_type->id) {
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be a Real instance object");
        }

        auto res = makeBooleanInstanceObject(getRealValueFast(self) >= getRealValueFast(arg1), rt);
        return res;
    }
};

class RealEqAdapter: public OperatorAdapter {
public:
    RealEqAdapter(Runtime *rt)
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
            return makeBooleanInstanceObject(getRealValueFast(self) == getRealValueFast(arg1), rt);
        }
        else if (!rt->isInstanceObject(self) && !rt->isInstanceObject(arg1)) {
            return makeBooleanInstanceObject(self->type->id == arg1->type->id, rt);
        }
        else {
            return makeBooleanInstanceObject(false, rt);
        }
    }
};

class RealNeqAdapter: public RealEqAdapter {
public:
    RealNeqAdapter(Runtime *rt)
        : RealEqAdapter(rt) {}

    Object *operator()(Object *self, const std::vector<Object *> &others) {
        auto res              = RealEqAdapter::operator()(self, others);
        getRealValueFast(res) = !getRealValueFast(res);
        return res;
    }
};

// TODO: add all operators to function and nothing
RealType::RealType(Runtime *rt)
    : Type(rt) {
    this->addOperator(OperatorNode::POST_PLUS_PLUS, new RealUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::POST_MINUS_MINUS, new RealUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::CALL, new RealUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::INDEX, new RealUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::PRE_PLUS_PLUS, new RealUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::PRE_MINUS_MINUS, new RealUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::PRE_PLUS, new RealPositiveAdapter(rt));
    this->addOperator(OperatorNode::PRE_MINUS, new RealNegativeAdapter(rt));
    this->addOperator(OperatorNode::NOT, new RealUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::INVERSE, new RealUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::MULT, new RealMultAdapter(rt));
    this->addOperator(OperatorNode::DIV, new RealDivAdapter(rt));
    this->addOperator(OperatorNode::REM, new RealUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::RIGHT_SHIFT, new RealUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::LEFT_SHIFT, new RealUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::PLUS, new RealAddAdapter(rt));
    this->addOperator(OperatorNode::MINUS, new RealSubAdapter(rt));
    this->addOperator(OperatorNode::LESS, new RealLtAdapter(rt));
    this->addOperator(OperatorNode::LESS_EQUAL, new RealLeqAdapter(rt));
    this->addOperator(OperatorNode::GREATER, new RealGtAdapter(rt));
    this->addOperator(OperatorNode::GREATER_EQUAL, new RealGeqAdapter(rt));
    this->addOperator(OperatorNode::EQUAL, new RealEqAdapter(rt));
    this->addOperator(OperatorNode::NOT_EQUAL, new RealNeqAdapter(rt));
    this->addOperator(OperatorNode::BITAND, new RealUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::BITXOR, new RealUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::BITOR, new RealUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::AND, new RealUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::OR, new RealUnsupportedAdapter(rt));
}

Object *RealType::create() {
    auto ins = createInstance(rt, RealInstance);
    auto obj = createObject(rt, true, ins, this);
    return obj;
}

Object *RealType::copy(Object *obj) {
    if (!rt->isTypeObject(obj) || obj->type->id != rt->real_type->id) {
        rt->signalError("Failed to copy an invalid object: " + obj->shortRepr());
    }
    if (!rt->isInstanceObject(obj)) {
        return createObject(rt, false, NULL, this);
    }
    auto ins = obj->instance->copy();
    auto res = createObject(rt, true, ins, this);
    return res;
}

std::string RealType::shortRepr() {
    if (this == NULL) {
        return "RealType(NULL)";
    }
    return "RealType(id = " + std::to_string(this->id) + ")";
}

double &getRealValue(Object *obj, Runtime *rt) {
    if (!rt->isInstanceObject(obj)) {
        rt->signalError(obj->shortRepr() + " is not an instance object");
    }
    if (obj->type->id != rt->real_type->id) {
        rt->signalError(obj->shortRepr() + " is not Real");
    }
    return icast(obj->instance, RealInstance)->value;
}

Object *makeRealInstanceObject(double value, Runtime *rt) {
    auto res                                  = rt->make(rt->real_type, Runtime::INSTANCE_OBJECT);
    icast(res->instance, RealInstance)->value = value;
    return res;
}

}    // namespace Cotton::Builtin
