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
IntegerInstance::IntegerInstance(Runtime *rt)
    : Instance(rt, sizeof(IntegerInstance)) {
    ProfilerCAPTURE();
    this->value = 0;
}

IntegerInstance::~IntegerInstance() {
    ProfilerCAPTURE();
}

Instance *IntegerInstance::copy(Runtime *rt) {
    ProfilerCAPTURE();
    Instance *res = new (rt->alloc(sizeof(IntegerInstance))) IntegerInstance(rt);

    if (res == NULL) {
        rt->signalError("Failed to copy " + this->shortRepr());
    }
    icast(res, IntegerInstance)->value = this->value;
    return res;
}

std::string IntegerInstance::shortRepr() {
    ProfilerCAPTURE();
    if (this == NULL) {
        return "IntegerInstance(NULL)";
    }
    return "IntegerInstance(id = " + std::to_string(this->id) + ", value = " + std::to_string(this->value) + ")";
}

void IntegerInstance::destroy(Runtime *rt) {
    rt->dealloc(this, sizeof(IntegerInstance));
}

size_t IntegerInstance::getSize() {
    ProfilerCAPTURE();
    return sizeof(IntegerInstance);
}

size_t IntegerType::getInstanceSize() {
    ProfilerCAPTURE();
    return sizeof(IntegerInstance);
}

class IntegerPostincAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        ProfilerCAPTURE();

        if (!isInstanceObject(self)) {
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
        ProfilerCAPTURE();

        if (!isInstanceObject(self)) {
            rt->signalError(self->shortRepr() + " does not support that operator");
        }

        auto res = self->type->copy(self, rt);
        getIntegerValueFast(self)--;
        return res;
    }
};

class IntegerUnsupportedAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        ProfilerCAPTURE();
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class IntegerPreincAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        ProfilerCAPTURE();

        if (!isInstanceObject(self)) {
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
        ProfilerCAPTURE();

        if (!isInstanceObject(self)) {
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
        ProfilerCAPTURE();

        if (!isInstanceObject(self)) {
            rt->signalError(self->shortRepr() + " does not support that operator");
        }

        auto res = self->type->copy(self, rt);
        return res;
    }
};

class IntegerNegativeAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        ProfilerCAPTURE();

        if (!isInstanceObject(self)) {
            rt->signalError(self->shortRepr() + " does not support that operator");
        }

        auto res                  = self->type->copy(self, rt);
        getIntegerValueFast(res) *= -1;
        return res;
    }
};

class IntegerInverseAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        ProfilerCAPTURE();

        if (!isInstanceObject(self)) {
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
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be an integer instance object");
        }

        auto res = makeIntegerInstanceObject(getIntegerValueFast(self) * getIntegerValueFast(arg1), rt);
        return res;
    }
};

class IntegerDivAdapter: public OperatorAdapter {
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
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be an integer instance object");
        }

        auto res = makeIntegerInstanceObject(getIntegerValueFast(self) / getIntegerValueFast(arg1), rt);
        return res;
    }
};

class IntegerRemAdapter: public OperatorAdapter {
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
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be an integer instance object");
        }

        auto res = makeIntegerInstanceObject(getIntegerValueFast(self) % getIntegerValueFast(arg1), rt);
        return res;
    }
};

class IntegerRshiftAdapter: public OperatorAdapter {
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
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be an integer instance object");
        }

        auto res = makeIntegerInstanceObject(getIntegerValueFast(self) >> getIntegerValueFast(arg1), rt);
        return res;
    }
};

class IntegerLshiftAdapter: public OperatorAdapter {
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
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be an integer instance object");
        }

        auto res = makeIntegerInstanceObject(getIntegerValueFast(self) << getIntegerValueFast(arg1), rt);
        return res;
    }
};

class IntegerAddAdapter: public OperatorAdapter {
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
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be an integer instance object");
        }

        auto res = makeIntegerInstanceObject(getIntegerValueFast(self) + getIntegerValueFast(arg1), rt);
        return res;
    }
};

class IntegerSubAdapter: public OperatorAdapter {
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
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be an integer instance object");
        }

        auto res = makeIntegerInstanceObject(getIntegerValueFast(self) - getIntegerValueFast(arg1), rt);
        return res;
    }
};

class IntegerLtAdapter: public OperatorAdapter {
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
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be an integer instance object");
        }

        auto res = makeBooleanInstanceObject(getIntegerValueFast(self) < getIntegerValueFast(arg1), rt);
        return res;
    }
};

class IntegerLeqAdapter: public OperatorAdapter {
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
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be an integer instance object");
        }

        auto res = makeBooleanInstanceObject(getIntegerValueFast(self) <= getIntegerValueFast(arg1), rt);
        return res;
    }
};

class IntegerGtAdapter: public OperatorAdapter {
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
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be an integer instance object");
        }

        auto res = makeBooleanInstanceObject(getIntegerValueFast(self) > getIntegerValueFast(arg1), rt);
        return res;
    }
};

class IntegerGeqAdapter: public OperatorAdapter {
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
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be an integer instance object");
        }

        auto res = makeBooleanInstanceObject(getIntegerValueFast(self) >= getIntegerValueFast(arg1), rt);
        return res;
    }
};

class IntegerEqAdapter: public OperatorAdapter {
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
            return makeBooleanInstanceObject(getIntegerValueFast(self) == getIntegerValueFast(arg1), rt);
        }
        else if (!i1 && !i2) {
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
        ProfilerCAPTURE();
        auto res                 = IntegerEqAdapter::operator()(self, others, rt);
        getBooleanValueFast(res) = !getBooleanValueFast(res);
        return res;
    }
};

class IntegerBitandAdapter: public OperatorAdapter {
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
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be an integer instance object");
        }

        auto res = makeIntegerInstanceObject(getIntegerValueFast(self) & getIntegerValueFast(arg1), rt);
        return res;
    }
};

class IntegerBitxorAdapter: public OperatorAdapter {
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
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be an integer instance object");
        }

        auto res = makeIntegerInstanceObject(getIntegerValueFast(self) ^ getIntegerValueFast(arg1), rt);
        return res;
    }
};

class IntegerBitorAdapter: public OperatorAdapter {
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
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be an integer instance object");
        }

        auto res = makeIntegerInstanceObject(getIntegerValueFast(self) | getIntegerValueFast(arg1), rt);
        return res;
    }
};

// TODO: add all operators to function and nothing
IntegerType::IntegerType(Runtime *rt)
    : Type(rt) {
    ProfilerCAPTURE();
    this->addOperator(OperatorNode::POST_PLUS_PLUS, new IntegerPostincAdapter());
    this->addOperator(OperatorNode::POST_MINUS_MINUS, new IntegerPostdecAdapter());
    this->addOperator(OperatorNode::CALL, new IntegerUnsupportedAdapter());
    this->addOperator(OperatorNode::INDEX, new IntegerUnsupportedAdapter());
    this->addOperator(OperatorNode::PRE_PLUS_PLUS, new IntegerPreincAdapter());
    this->addOperator(OperatorNode::PRE_MINUS_MINUS, new IntegerPredecAdapter());
    this->addOperator(OperatorNode::PRE_PLUS, new IntegerPositiveAdapter());
    this->addOperator(OperatorNode::PRE_MINUS, new IntegerNegativeAdapter());
    this->addOperator(OperatorNode::NOT, new IntegerUnsupportedAdapter());
    this->addOperator(OperatorNode::INVERSE, new IntegerInverseAdapter());
    this->addOperator(OperatorNode::MULT, new IntegerMultAdapter());
    this->addOperator(OperatorNode::DIV, new IntegerDivAdapter());
    this->addOperator(OperatorNode::REM, new IntegerRemAdapter());
    this->addOperator(OperatorNode::RIGHT_SHIFT, new IntegerRshiftAdapter());
    this->addOperator(OperatorNode::LEFT_SHIFT, new IntegerLshiftAdapter());
    this->addOperator(OperatorNode::PLUS, new IntegerAddAdapter());
    this->addOperator(OperatorNode::MINUS, new IntegerSubAdapter());
    this->addOperator(OperatorNode::LESS, new IntegerLtAdapter());
    this->addOperator(OperatorNode::LESS_EQUAL, new IntegerLeqAdapter());
    this->addOperator(OperatorNode::GREATER, new IntegerGtAdapter());
    this->addOperator(OperatorNode::GREATER_EQUAL, new IntegerGeqAdapter());
    this->addOperator(OperatorNode::EQUAL, new IntegerEqAdapter());
    this->addOperator(OperatorNode::NOT_EQUAL, new IntegerNeqAdapter());
    this->addOperator(OperatorNode::BITAND, new IntegerBitandAdapter());
    this->addOperator(OperatorNode::BITXOR, new IntegerBitxorAdapter());
    this->addOperator(OperatorNode::BITOR, new IntegerBitorAdapter());
    this->addOperator(OperatorNode::AND, new IntegerUnsupportedAdapter());
    this->addOperator(OperatorNode::OR, new IntegerUnsupportedAdapter());
}

Object *IntegerType::create(Runtime *rt) {
    ProfilerCAPTURE();
    Instance *ins = new (rt->alloc(sizeof(IntegerInstance))) IntegerInstance(rt);
    Object   *obj = newObject(true, ins, this, rt);
    return obj;
}

Object *IntegerType::copy(Object *obj, Runtime *rt) {
    ProfilerCAPTURE();
    if (!isTypeObject(obj) || obj->type->id != rt->integer_type->id) {
        rt->signalError("Failed to copy an invalid object: " + obj->shortRepr());
    }
    if (obj->instance == NULL) {
        return newObject(false, NULL, this, rt);
    }
    auto ins = obj->instance->copy(rt);
    auto res = newObject(true, ins, this, rt);
    return res;
}

std::string IntegerType::shortRepr() {
    ProfilerCAPTURE();
    if (this == NULL) {
        return "IntegerType(NULL)";
    }
    return "IntegerType(id = " + std::to_string(this->id) + ")";
}

int64_t &getIntegerValue(Object *obj, Runtime *rt) {
    ProfilerCAPTURE();
    if (!isInstanceObject(obj)) {
        rt->signalError(obj->shortRepr() + " is not an instance object");
    }
    if (obj->type->id != rt->integer_type->id) {
        rt->signalError(obj->shortRepr() + " is not Integer");
    }
    return icast(obj->instance, IntegerInstance)->value;
}

Object *makeIntegerInstanceObject(int64_t value, Runtime *rt) {
    ProfilerCAPTURE();
    auto res                                     = rt->make(rt->integer_type, Runtime::INSTANCE_OBJECT);
    icast(res->instance, IntegerInstance)->value = value;
    return res;
}

}    // namespace Cotton::Builtin
