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
IntegerInstance::IntegerInstance(Runtime *rt)
    : Instance(rt, sizeof(IntegerInstance)) {
    this->value = 0;
}

IntegerInstance::~IntegerInstance() {}

Instance *IntegerInstance::copy() {
    Instance *res = new (std::nothrow) IntegerInstance(rt);
    if (res == NULL) {
        rt->signalError("Failed to copy " + this->shortRepr());
    }
    icast(res, IntegerInstance)->value = this->value;
    return res;
}

std::string IntegerInstance::shortRepr() {
    if (this == NULL) {
        return "IntegerInstance(NULL)";
    }
    return "IntegerInstance(id = " + std::to_string(this->id) + ", value = " + std::to_string(this->value) + ")";
}

size_t IntegerInstance::getSize() {
    return sizeof(IntegerInstance);
}

size_t IntegerType::getInstanceSize() {
    return sizeof(IntegerInstance);
}

class IntegerPostincAdapter: public OperatorAdapter {
public:
    IntegerPostincAdapter(Runtime *rt)
        : OperatorAdapter(rt) {}

    Object *operator()(Object *self, const std::vector<Object *> &others) {
        if (!rt->isTypeObject(self) || self->type->id != rt->integer_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        if (!rt->isInstanceObject(self)) {
            rt->signalError(self->shortRepr() + " does not support that operator");
        }

        auto res = self->type->copy(self);
        getIntegerValueFast(self)++;
        return res;
    }
};

class IntegerPostdecAdapter: public OperatorAdapter {
public:
    IntegerPostdecAdapter(Runtime *rt)
        : OperatorAdapter(rt) {}

    Object *operator()(Object *self, const std::vector<Object *> &others) {
        if (!rt->isTypeObject(self) || self->type->id != rt->integer_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        if (!rt->isInstanceObject(self)) {
            rt->signalError(self->shortRepr() + " does not support that operator");
        }

        auto res = self->type->copy(self);
        getIntegerValueFast(self)--;
        return res;
    }
};

class IntegerUnsupportedAdapter: public OperatorAdapter {
public:
    IntegerUnsupportedAdapter(Runtime *rt)
        : OperatorAdapter(rt) {}

    Object *operator()(Object *self, const std::vector<Object *> &others) {
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class IntegerPreincAdapter: public OperatorAdapter {
public:
    IntegerPreincAdapter(Runtime *rt)
        : OperatorAdapter(rt) {}

    Object *operator()(Object *self, const std::vector<Object *> &others) {
        if (!rt->isTypeObject(self) || self->type->id != rt->integer_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        if (!rt->isInstanceObject(self)) {
            rt->signalError(self->shortRepr() + " does not support that operator");
        }

        getIntegerValueFast(self)++;
        auto res = self->type->copy(self);
        return res;
    }
};

class IntegerPredecAdapter: public OperatorAdapter {
public:
    IntegerPredecAdapter(Runtime *rt)
        : OperatorAdapter(rt) {}

    Object *operator()(Object *self, const std::vector<Object *> &others) {
        if (!rt->isTypeObject(self) || self->type->id != rt->integer_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        if (!rt->isInstanceObject(self)) {
            rt->signalError(self->shortRepr() + " does not support that operator");
        }

        getIntegerValueFast(self)--;
        auto res = self->type->copy(self);
        return res;
    }
};

class IntegerPositiveAdapter: public OperatorAdapter {
public:
    IntegerPositiveAdapter(Runtime *rt)
        : OperatorAdapter(rt) {}

    Object *operator()(Object *self, const std::vector<Object *> &others) {
        if (!rt->isTypeObject(self) || self->type->id != rt->integer_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        if (!rt->isInstanceObject(self)) {
            rt->signalError(self->shortRepr() + " does not support that operator");
        }

        auto res = self->type->copy(self);
        return res;
    }
};

class IntegerNegativeAdapter: public OperatorAdapter {
public:
    IntegerNegativeAdapter(Runtime *rt)
        : OperatorAdapter(rt) {}

    Object *operator()(Object *self, const std::vector<Object *> &others) {
        if (!rt->isTypeObject(self) || self->type->id != rt->integer_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        if (!rt->isInstanceObject(self)) {
            rt->signalError(self->shortRepr() + " does not support that operator");
        }

        auto res                  = self->type->copy(self);
        getIntegerValueFast(res) *= -1;
        return res;
    }
};

class IntegerInverseAdapter: public OperatorAdapter {
public:
    IntegerInverseAdapter(Runtime *rt)
        : OperatorAdapter(rt) {}

    Object *operator()(Object *self, const std::vector<Object *> &others) {
        if (!rt->isTypeObject(self) || self->type->id != rt->integer_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        if (!rt->isInstanceObject(self)) {
            rt->signalError(self->shortRepr() + " does not support that operator");
        }

        auto res                 = self->type->copy(self);
        getIntegerValueFast(res) = ~getIntegerValueFast(res);
        return res;
    }
};

class IntegerMultAdapter: public OperatorAdapter {
public:
    IntegerMultAdapter(Runtime *rt)
        : OperatorAdapter(rt) {}

    Object *operator()(Object *self, const std::vector<Object *> &others) {
        if (!rt->isTypeObject(self) || self->type->id != rt->integer_type->id) {
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
        if (!rt->isInstanceObject(arg1) || arg1->type->id != rt->integer_type->id) {
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be an integer instance object");
        }

        auto res = makeIntegerInstanceObject(getIntegerValueFast(self) * getIntegerValueFast(arg1), rt);
        return res;
    }
};

class IntegerDivAdapter: public OperatorAdapter {
public:
    IntegerDivAdapter(Runtime *rt)
        : OperatorAdapter(rt) {}

    Object *operator()(Object *self, const std::vector<Object *> &others) {
        if (!rt->isTypeObject(self) || self->type->id != rt->integer_type->id) {
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
        if (!rt->isInstanceObject(arg1) || arg1->type->id != rt->integer_type->id) {
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be an integer instance object");
        }

        auto res = makeIntegerInstanceObject(getIntegerValueFast(self) / getIntegerValueFast(arg1), rt);
        return res;
    }
};

class IntegerRemAdapter: public OperatorAdapter {
public:
    IntegerRemAdapter(Runtime *rt)
        : OperatorAdapter(rt) {}

    Object *operator()(Object *self, const std::vector<Object *> &others) {
        if (!rt->isTypeObject(self) || self->type->id != rt->integer_type->id) {
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
        if (!rt->isInstanceObject(arg1) || arg1->type->id != rt->integer_type->id) {
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be an integer instance object");
        }

        auto res = makeIntegerInstanceObject(getIntegerValueFast(self) % getIntegerValueFast(arg1), rt);
        return res;
    }
};

class IntegerRshiftAdapter: public OperatorAdapter {
public:
    IntegerRshiftAdapter(Runtime *rt)
        : OperatorAdapter(rt) {}

    Object *operator()(Object *self, const std::vector<Object *> &others) {
        if (!rt->isTypeObject(self) || self->type->id != rt->integer_type->id) {
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
        if (!rt->isInstanceObject(arg1) || arg1->type->id != rt->integer_type->id) {
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be an integer instance object");
        }

        auto res = makeIntegerInstanceObject(getIntegerValueFast(self) >> getIntegerValueFast(arg1), rt);
        return res;
    }
};

class IntegerLshiftAdapter: public OperatorAdapter {
public:
    IntegerLshiftAdapter(Runtime *rt)
        : OperatorAdapter(rt) {}

    Object *operator()(Object *self, const std::vector<Object *> &others) {
        if (!rt->isTypeObject(self) || self->type->id != rt->integer_type->id) {
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
        if (!rt->isInstanceObject(arg1) || arg1->type->id != rt->integer_type->id) {
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be an integer instance object");
        }

        auto res = makeIntegerInstanceObject(getIntegerValueFast(self) << getIntegerValueFast(arg1), rt);
        return res;
    }
};

class IntegerAddAdapter: public OperatorAdapter {
public:
    IntegerAddAdapter(Runtime *rt)
        : OperatorAdapter(rt) {}

    Object *operator()(Object *self, const std::vector<Object *> &others) {
        if (!rt->isTypeObject(self) || self->type->id != rt->integer_type->id) {
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
        if (!rt->isInstanceObject(arg1) || arg1->type->id != rt->integer_type->id) {
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be an integer instance object");
        }

        auto res = makeIntegerInstanceObject(getIntegerValueFast(self) + getIntegerValueFast(arg1), rt);
        return res;
    }
};

class IntegerSubAdapter: public OperatorAdapter {
public:
    IntegerSubAdapter(Runtime *rt)
        : OperatorAdapter(rt) {}

    Object *operator()(Object *self, const std::vector<Object *> &others) {
        if (!rt->isTypeObject(self) || self->type->id != rt->integer_type->id) {
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
        if (!rt->isInstanceObject(arg1) || arg1->type->id != rt->integer_type->id) {
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be an integer instance object");
        }

        auto res = makeIntegerInstanceObject(getIntegerValueFast(self) - getIntegerValueFast(arg1), rt);
        return res;
    }
};

class IntegerLtAdapter: public OperatorAdapter {
public:
    IntegerLtAdapter(Runtime *rt)
        : OperatorAdapter(rt) {}

    Object *operator()(Object *self, const std::vector<Object *> &others) {
        if (!rt->isTypeObject(self) || self->type->id != rt->integer_type->id) {
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
        if (!rt->isInstanceObject(arg1) || arg1->type->id != rt->integer_type->id) {
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be an integer instance object");
        }

        auto res = makeBooleanInstanceObject(getIntegerValueFast(self) < getIntegerValueFast(arg1), rt);
        return res;
    }
};

class IntegerLeqAdapter: public OperatorAdapter {
public:
    IntegerLeqAdapter(Runtime *rt)
        : OperatorAdapter(rt) {}

    Object *operator()(Object *self, const std::vector<Object *> &others) {
        if (!rt->isTypeObject(self) || self->type->id != rt->integer_type->id) {
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
        if (!rt->isInstanceObject(arg1) || arg1->type->id != rt->integer_type->id) {
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be an integer instance object");
        }

        auto res = makeBooleanInstanceObject(getIntegerValueFast(self) <= getIntegerValueFast(arg1), rt);
        return res;
    }
};

class IntegerGtAdapter: public OperatorAdapter {
public:
    IntegerGtAdapter(Runtime *rt)
        : OperatorAdapter(rt) {}

    Object *operator()(Object *self, const std::vector<Object *> &others) {
        if (!rt->isTypeObject(self) || self->type->id != rt->integer_type->id) {
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
        if (!rt->isInstanceObject(arg1) || arg1->type->id != rt->integer_type->id) {
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be an integer instance object");
        }

        auto res = makeBooleanInstanceObject(getIntegerValueFast(self) > getIntegerValueFast(arg1), rt);
        return res;
    }
};

class IntegerGeqAdapter: public OperatorAdapter {
public:
    IntegerGeqAdapter(Runtime *rt)
        : OperatorAdapter(rt) {}

    Object *operator()(Object *self, const std::vector<Object *> &others) {
        if (!rt->isTypeObject(self) || self->type->id != rt->integer_type->id) {
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
        if (!rt->isInstanceObject(arg1) || arg1->type->id != rt->integer_type->id) {
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be an integer instance object");
        }

        auto res = makeBooleanInstanceObject(getIntegerValueFast(self) >= getIntegerValueFast(arg1), rt);
        return res;
    }
};

class IntegerEqAdapter: public OperatorAdapter {
public:
    IntegerEqAdapter(Runtime *rt)
        : OperatorAdapter(rt) {}

    Object *operator()(Object *self, const std::vector<Object *> &others) {
        if (!rt->isTypeObject(self) || self->type->id != rt->integer_type->id) {
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
            return makeBooleanInstanceObject(getIntegerValueFast(self) == getIntegerValueFast(arg1), rt);
        }
        else if (!rt->isInstanceObject(self) && !rt->isInstanceObject(arg1)) {
            return makeBooleanInstanceObject(self->type->id == arg1->type->id, rt);
        }
        else {
            return makeBooleanInstanceObject(false, rt);
        }
    }
};

class IntegerNeqAdapter: public IntegerEqAdapter {
public:
    IntegerNeqAdapter(Runtime *rt)
        : IntegerEqAdapter(rt) {}

    Object *operator()(Object *self, const std::vector<Object *> &others) {
        auto res                 = IntegerEqAdapter::operator()(self, others);
        getBooleanValueFast(res) = !getBooleanValueFast(res);
        return res;
    }
};

class IntegerBitandAdapter: public OperatorAdapter {
public:
    IntegerBitandAdapter(Runtime *rt)
        : OperatorAdapter(rt) {}

    Object *operator()(Object *self, const std::vector<Object *> &others) {
        if (!rt->isTypeObject(self) || self->type->id != rt->integer_type->id) {
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
        if (!rt->isInstanceObject(arg1) || arg1->type->id != rt->integer_type->id) {
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be an integer instance object");
        }

        auto res = makeIntegerInstanceObject(getIntegerValueFast(self) & getIntegerValueFast(arg1), rt);
        return res;
    }
};

class IntegerBitxorAdapter: public OperatorAdapter {
public:
    IntegerBitxorAdapter(Runtime *rt)
        : OperatorAdapter(rt) {}

    Object *operator()(Object *self, const std::vector<Object *> &others) {
        if (!rt->isTypeObject(self) || self->type->id != rt->integer_type->id) {
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
        if (!rt->isInstanceObject(arg1) || arg1->type->id != rt->integer_type->id) {
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be an integer instance object");
        }

        auto res = makeIntegerInstanceObject(getIntegerValueFast(self) ^ getIntegerValueFast(arg1), rt);
        return res;
    }
};

class IntegerBitorAdapter: public OperatorAdapter {
public:
    IntegerBitorAdapter(Runtime *rt)
        : OperatorAdapter(rt) {}

    Object *operator()(Object *self, const std::vector<Object *> &others) {
        if (!rt->isTypeObject(self) || self->type->id != rt->integer_type->id) {
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
        if (!rt->isInstanceObject(arg1) || arg1->type->id != rt->integer_type->id) {
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be an integer instance object");
        }

        auto res = makeIntegerInstanceObject(getIntegerValueFast(self) | getIntegerValueFast(arg1), rt);
        return res;
    }
};

// TODO: add all operators to function and nothing
IntegerType::IntegerType(Runtime *rt)
    : Type(rt) {
    this->addOperator(OperatorNode::POST_PLUS_PLUS, new IntegerPostincAdapter(rt));
    this->addOperator(OperatorNode::POST_MINUS_MINUS, new IntegerPostdecAdapter(rt));
    this->addOperator(OperatorNode::CALL, new IntegerUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::INDEX, new IntegerUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::PRE_PLUS_PLUS, new IntegerPreincAdapter(rt));
    this->addOperator(OperatorNode::PRE_MINUS_MINUS, new IntegerPredecAdapter(rt));
    this->addOperator(OperatorNode::PRE_PLUS, new IntegerPositiveAdapter(rt));
    this->addOperator(OperatorNode::PRE_MINUS, new IntegerNegativeAdapter(rt));
    this->addOperator(OperatorNode::NOT, new IntegerUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::INVERSE, new IntegerInverseAdapter(rt));
    this->addOperator(OperatorNode::MULT, new IntegerMultAdapter(rt));
    this->addOperator(OperatorNode::DIV, new IntegerDivAdapter(rt));
    this->addOperator(OperatorNode::REM, new IntegerRemAdapter(rt));
    this->addOperator(OperatorNode::RIGHT_SHIFT, new IntegerRshiftAdapter(rt));
    this->addOperator(OperatorNode::LEFT_SHIFT, new IntegerLshiftAdapter(rt));
    this->addOperator(OperatorNode::PLUS, new IntegerAddAdapter(rt));
    this->addOperator(OperatorNode::MINUS, new IntegerSubAdapter(rt));
    this->addOperator(OperatorNode::LESS, new IntegerLtAdapter(rt));
    this->addOperator(OperatorNode::LESS_EQUAL, new IntegerLeqAdapter(rt));
    this->addOperator(OperatorNode::GREATER, new IntegerGtAdapter(rt));
    this->addOperator(OperatorNode::GREATER_EQUAL, new IntegerGeqAdapter(rt));
    this->addOperator(OperatorNode::EQUAL, new IntegerEqAdapter(rt));
    this->addOperator(OperatorNode::NOT_EQUAL, new IntegerNeqAdapter(rt));
    this->addOperator(OperatorNode::BITAND, new IntegerBitandAdapter(rt));
    this->addOperator(OperatorNode::BITXOR, new IntegerBitxorAdapter(rt));
    this->addOperator(OperatorNode::BITOR, new IntegerBitorAdapter(rt));
    this->addOperator(OperatorNode::AND, new IntegerUnsupportedAdapter(rt));
    this->addOperator(OperatorNode::OR, new IntegerUnsupportedAdapter(rt));
}

Object *IntegerType::create() {
    auto ins = createInstance(rt, IntegerInstance);
    auto obj = createObject(rt, true, ins, this);
    return obj;
}

Object *IntegerType::copy(Object *obj) {
    if (!rt->isTypeObject(obj) || obj->type->id != rt->integer_type->id) {
        rt->signalError("Failed to copy an invalid object: " + obj->shortRepr());
    }
    if (!rt->isInstanceObject(obj)) {
        return createObject(rt, false, NULL, this);
    }
    auto ins = obj->instance->copy();
    auto res = createObject(rt, true, ins, this);
    return res;
}

std::string IntegerType::shortRepr() {
    if (this == NULL) {
        return "IntegerType(NULL)";
    }
    return "IntegerType(id = " + std::to_string(this->id) + ")";
}

int64_t &getIntegerValue(Object *obj, Runtime *rt) {
    if (!rt->isInstanceObject(obj)) {
        rt->signalError(obj->shortRepr() + " is not an instance object");
    }
    if (obj->type->id != rt->integer_type->id) {
        rt->signalError(obj->shortRepr() + " is not Integer");
    }
    return icast(obj->instance, IntegerInstance)->value;
}

Object *makeIntegerInstanceObject(int64_t value, Runtime *rt) {
    auto res                                     = rt->make(rt->integer_type, Runtime::INSTANCE_OBJECT);
    icast(res->instance, IntegerInstance)->value = value;
    return res;
}

}    // namespace Cotton::Builtin
