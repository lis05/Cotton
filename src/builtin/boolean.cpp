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
#include "../back/type.h"
#include "function.h"

namespace Cotton::Builtin {
BooleanInstance::BooleanInstance(Runtime *rt, bool on_stack)
    : Instance(rt, sizeof(BooleanInstance), on_stack) {
    this->value = false;
}

BooleanInstance::~BooleanInstance() {}

Instance *BooleanInstance::copy(Runtime *rt) {
    Instance *res = rt->stack->allocAndInitInstance<BooleanInstance>(sizeof(BooleanInstance), rt);
    if (res != NULL) {
        res->on_stack                      = true;
        icast(res, BooleanInstance)->value = this->value;
        return res;
    }
    res = new (std::nothrow) BooleanInstance(rt, false);
    if (res == NULL) {
        rt->signalError("Failed to copy " + this->shortRepr());
    }
    icast(res, BooleanInstance)->value = this->value;
    res->on_stack                      = false;
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

class BooleanPostincAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->boolean_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError("BooleanType does not support that operator");
    }
};

class BooleanPostdecAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->boolean_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError("BooleanType does not support that operator");
    }
};

class BooleanCallAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->boolean_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError("BooleanType does not support that operator");
    }
};

class BooleanIndexAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->boolean_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError("BooleanType does not support that operator");
    }
};

class BooleanPreincAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->boolean_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError("BooleanType does not support that operator");
    }
};

class BooleanPredecAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->boolean_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError("BooleanType does not support that operator");
    }
};

class BooleanPositiveAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->boolean_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError("BooleanType does not support that operator");
    }
};

class BooleanNegativeAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->boolean_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError("BooleanType does not support that operator");
    }
};

class BooleanNotAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->boolean_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        auto res                 = rt->make(rt->boolean_type, Runtime::INSTANCE_OBJECT);
        getBooleanValue(res, rt) = !getBooleanValue(self, rt);
        return res;
    }
};

class BooleanInverseAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->boolean_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError("BooleanType does not support that operator");
    }
};

class BooleanMultAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->boolean_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError("BooleanType does not support that operator");
    }
};

class BooleanDivAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->boolean_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError("BooleanType does not support that operator");
    }
};

class BooleanRemAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->boolean_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError("BooleanType does not support that operator");
    }
};

class BooleanRshiftAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->boolean_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError("BooleanType does not support that operator");
    }
};

class BooleanLshiftAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->boolean_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError("BooleanType does not support that operator");
    }
};

class BooleanAddAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->boolean_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError("BooleanType does not support that operator");
    }
};

class BooleanSubAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->boolean_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError("BooleanType does not support that operator");
    }
};

class BooleanLtAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->boolean_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError("BooleanType does not support that operator");
    }
};

class BooleanLeqAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->boolean_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError("BooleanType does not support that operator");
    }
};

class BooleanGtAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->boolean_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError("BooleanType does not support that operator");
    }
};

class BooleanGeqAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->boolean_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError("BooleanType does not support that operator");
    }
};

class BooleanEqAdapter: public OperatorAdapter {
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

        auto res                 = rt->make(rt->boolean_type, Runtime::INSTANCE_OBJECT);
        getBooleanValue(res, rt) = false;
        if (self->type->id == arg1->type->id) {
            auto b1                  = getBooleanValue(self, rt);
            auto b2                  = getBooleanValue(self, rt);
            getBooleanValue(res, rt) = (b1 == b2);
        }
        return res;
    }
};

class BooleanNeqAdapter: public BooleanEqAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        auto res                 = BooleanEqAdapter::operator()(self, others, rt);
        getBooleanValue(res, rt) = !getBooleanValue(res, rt);
        return res;
    }
};

class BooleanBitandAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->boolean_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError("BooleanType does not support that operator");
    }
};

class BooleanBitxorAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->boolean_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError("BooleanType does not support that operator");
    }
};

class BooleanBitorAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->boolean_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError("BooleanType does not support that operator");
    }
};

class BooleanAndAdapter: public OperatorAdapter {
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
    : Type(true, rt) {
    this->addOperator(OperatorNode::POST_PLUS_PLUS, new BooleanPostincAdapter(), rt);
    this->addOperator(OperatorNode::POST_MINUS_MINUS, new BooleanPostdecAdapter(), rt);
    this->addOperator(OperatorNode::CALL, new BooleanCallAdapter(), rt);
    this->addOperator(OperatorNode::INDEX, new BooleanIndexAdapter(), rt);
    this->addOperator(OperatorNode::PRE_PLUS_PLUS, new BooleanPreincAdapter(), rt);
    this->addOperator(OperatorNode::PRE_MINUS_MINUS, new BooleanPredecAdapter(), rt);
    this->addOperator(OperatorNode::PRE_PLUS, new BooleanPositiveAdapter(), rt);
    this->addOperator(OperatorNode::PRE_MINUS, new BooleanNegativeAdapter(), rt);
    this->addOperator(OperatorNode::NOT, new BooleanNotAdapter(), rt);
    this->addOperator(OperatorNode::INVERSE, new BooleanInverseAdapter(), rt);
    this->addOperator(OperatorNode::MULT, new BooleanMultAdapter(), rt);
    this->addOperator(OperatorNode::DIV, new BooleanDivAdapter(), rt);
    this->addOperator(OperatorNode::REM, new BooleanRemAdapter(), rt);
    this->addOperator(OperatorNode::RIGHT_SHIFT, new BooleanRshiftAdapter(), rt);
    this->addOperator(OperatorNode::LEFT_SHIFT, new BooleanLshiftAdapter(), rt);
    this->addOperator(OperatorNode::PLUS, new BooleanAddAdapter(), rt);
    this->addOperator(OperatorNode::MINUS, new BooleanSubAdapter(), rt);
    this->addOperator(OperatorNode::LESS, new BooleanLtAdapter(), rt);
    this->addOperator(OperatorNode::LESS_EQUAL, new BooleanLeqAdapter(), rt);
    this->addOperator(OperatorNode::GREATER, new BooleanGtAdapter(), rt);
    this->addOperator(OperatorNode::GREATER_EQUAL, new BooleanGeqAdapter(), rt);
    this->addOperator(OperatorNode::EQUAL, new BooleanEqAdapter(), rt);
    this->addOperator(OperatorNode::NOT_EQUAL, new BooleanNeqAdapter(), rt);
    this->addOperator(OperatorNode::BITAND, new BooleanBitandAdapter(), rt);
    this->addOperator(OperatorNode::BITXOR, new BooleanBitxorAdapter(), rt);
    this->addOperator(OperatorNode::BITOR, new BooleanBitorAdapter(), rt);
    this->addOperator(OperatorNode::AND, new BooleanAndAdapter(), rt);
    this->addOperator(OperatorNode::OR, new BooleanOrAdapter(), rt);
}

Object *BooleanType::create(Runtime *rt) {
    auto ins = createInstance(rt, true, BooleanInstance);
    auto obj = createObject(rt, true, ins, this, true);
    return obj;
}

Object *BooleanType::copy(Object *obj, Runtime *rt) {
    if (!rt->isTypeObject(obj) || obj->type->id != rt->boolean_type->id) {
        rt->signalError("Failed to copy an invalid object: " + obj->shortRepr());
    }
    auto ins = obj->instance->copy(rt);
    auto res = createObject(rt, true, ins, this, false);
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
