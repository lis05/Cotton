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

#include "integer.h"
#include "../back/type.h"
#include "boolean.h"

namespace Cotton::Builtin {
IntegerInstance::IntegerInstance(Runtime *rt, bool on_stack)
    : Instance(rt, sizeof(IntegerInstance), on_stack) {
    this->value = 0;
}

IntegerInstance::~IntegerInstance() {}

Instance *IntegerInstance::copy(Runtime *rt) {
    Instance *res = rt->stack->allocAndInitInstance<IntegerInstance>(sizeof(IntegerInstance), rt);
    if (res != NULL) {
        res->on_stack                      = true;
        icast(res, IntegerInstance)->value = this->value;
        return res;
    }
    res = new (std::nothrow) IntegerInstance(rt, false);
    if (res == NULL) {
        rt->signalError("Failed to copy " + this->shortRepr());
    }
    icast(res, IntegerInstance)->value = this->value;
    res->on_stack                      = false;
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
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->integer_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        if (!rt->isInstanceObject(self)) {
            rt->signalError(self->shortRepr() + " does not support that operator");
        }

        auto res = self->type->copy(self, rt);
        getIntegerValueFast(self)++;
        return res;
    }
};

class IntegerPostdecAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->integer_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        if (!rt->isInstanceObject(self)) {
            rt->signalError(self->shortRepr() + " does not support that operator");
        }

        auto res = self->type->copy(self, rt);
        getIntegerValueFast(self)--;
        return res;
    }
};

class IntegerCallAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->integer_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }

        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class IntegerIndexAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->integer_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }

        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class IntegerPreincAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->integer_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        if (!rt->isInstanceObject(self)) {
            rt->signalError(self->shortRepr() + " does not support that operator");
        }

        getIntegerValueFast(self)++;
        auto res = self->type->copy(self, rt);
        return res;
    }
};

class IntegerPredecAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->integer_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        if (!rt->isInstanceObject(self)) {
            rt->signalError(self->shortRepr() + " does not support that operator");
        }

        getIntegerValueFast(self)--;
        auto res = self->type->copy(self, rt);
        return res;
    }
};

class IntegerPositiveAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->integer_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        if (!rt->isInstanceObject(self)) {
            rt->signalError(self->shortRepr() + " does not support that operator");
        }

        auto res = self->type->copy(self, rt);
        return res;
    }
};

class IntegerNegativeAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->integer_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        if (!rt->isInstanceObject(self)) {
            rt->signalError(self->shortRepr() + " does not support that operator");
        }

        auto res                  = self->type->copy(self, rt);
        getIntegerValueFast(res) *= -1;
        return res;
    }
};

class IntegerNotAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->integer_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class IntegerInverseAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->integer_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        if (!rt->isInstanceObject(self)) {
            rt->signalError(self->shortRepr() + " does not support that operator");
        }

        auto res                 = self->type->copy(self, rt);
        getIntegerValueFast(res) = ~getIntegerValueFast(res);
        return res;
    }
};

class IntegerMultAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
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
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
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
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
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
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
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
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
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
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
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
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
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
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
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
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
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
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
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
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
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
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
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
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        auto res                 = IntegerEqAdapter::operator()(self, others, rt);
        getIntegerValueFast(res) = !getIntegerValueFast(res);
        return res;
    }
};

class IntegerBitandAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
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
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
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
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
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

class IntegerAndAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->integer_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class IntegerOrAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->integer_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

// TODO: add all operators to function and nothing
IntegerType::IntegerType(Runtime *rt)
    : Type(true, rt) {
    this->addOperator(OperatorNode::POST_PLUS_PLUS, new IntegerPostincAdapter(), rt);
    this->addOperator(OperatorNode::POST_MINUS_MINUS, new IntegerPostdecAdapter(), rt);
    this->addOperator(OperatorNode::CALL, new IntegerCallAdapter(), rt);
    this->addOperator(OperatorNode::INDEX, new IntegerIndexAdapter(), rt);
    this->addOperator(OperatorNode::PRE_PLUS_PLUS, new IntegerPreincAdapter(), rt);
    this->addOperator(OperatorNode::PRE_MINUS_MINUS, new IntegerPredecAdapter(), rt);
    this->addOperator(OperatorNode::PRE_PLUS, new IntegerPositiveAdapter(), rt);
    this->addOperator(OperatorNode::PRE_MINUS, new IntegerNegativeAdapter(), rt);
    this->addOperator(OperatorNode::NOT, new IntegerNotAdapter(), rt);
    this->addOperator(OperatorNode::INVERSE, new IntegerInverseAdapter(), rt);
    this->addOperator(OperatorNode::MULT, new IntegerMultAdapter(), rt);
    this->addOperator(OperatorNode::DIV, new IntegerDivAdapter(), rt);
    this->addOperator(OperatorNode::REM, new IntegerRemAdapter(), rt);
    this->addOperator(OperatorNode::RIGHT_SHIFT, new IntegerRshiftAdapter(), rt);
    this->addOperator(OperatorNode::LEFT_SHIFT, new IntegerLshiftAdapter(), rt);
    this->addOperator(OperatorNode::PLUS, new IntegerAddAdapter(), rt);
    this->addOperator(OperatorNode::MINUS, new IntegerSubAdapter(), rt);
    this->addOperator(OperatorNode::LESS, new IntegerLtAdapter(), rt);
    this->addOperator(OperatorNode::LESS_EQUAL, new IntegerLeqAdapter(), rt);
    this->addOperator(OperatorNode::GREATER, new IntegerGtAdapter(), rt);
    this->addOperator(OperatorNode::GREATER_EQUAL, new IntegerGeqAdapter(), rt);
    this->addOperator(OperatorNode::EQUAL, new IntegerEqAdapter(), rt);
    this->addOperator(OperatorNode::NOT_EQUAL, new IntegerNeqAdapter(), rt);
    this->addOperator(OperatorNode::BITAND, new IntegerBitandAdapter(), rt);
    this->addOperator(OperatorNode::BITXOR, new IntegerBitxorAdapter(), rt);
    this->addOperator(OperatorNode::BITOR, new IntegerBitorAdapter(), rt);
    this->addOperator(OperatorNode::AND, new IntegerAndAdapter(), rt);
    this->addOperator(OperatorNode::OR, new IntegerOrAdapter(), rt);
}

Object *IntegerType::create(Runtime *rt) {
    auto ins = createInstance(rt, true, IntegerInstance);
    auto obj = createObject(rt, true, ins, this, true);
    return obj;
}

Object *IntegerType::copy(Object *obj, Runtime *rt) {
    if (!rt->isTypeObject(obj) || obj->type->id != rt->integer_type->id) {
        rt->signalError("Failed to copy an invalid object: " + obj->shortRepr());
    }
    auto ins = obj->instance->copy(rt);
    auto res = createObject(rt, true, ins, this, true);
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
