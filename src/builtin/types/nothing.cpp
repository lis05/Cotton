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

#include "api.h"

namespace Cotton::Builtin {

NothingInstance::NothingInstance(Runtime *rt, bool on_stack)
    : Instance(rt, sizeof(NothingInstance), on_stack) {}

NothingInstance::~NothingInstance() {}

Instance *NothingInstance::copy(Runtime *rt) {
    Instance *res = rt->stack->allocAndInitInstance<NothingInstance>(sizeof(NothingInstance), rt);
    if (res != NULL) {
        res->on_stack = true;
        return res;
    }
    res = new (std::nothrow) NothingInstance(rt, false);
    if (res == NULL) {
        rt->signalError("Failed to copy " + this->shortRepr());
    }
    res->on_stack = false;
    return res;
}

size_t NothingInstance::getSize() {
    return sizeof(NothingInstance);
}

size_t NothingType::getInstanceSize() {
    return sizeof(NothingInstance);
}

std::string NothingInstance::shortRepr() {
    if (this == NULL) {
        return "NULL";
    }
    return "NothingInstance(id = " + std::to_string(this->id) + ")";
}

class NothingPostincAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->nothing_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class NothingPostdecAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->nothing_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class NothingCallAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->nothing_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class NothingIndexAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->nothing_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class NothingPreincAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->nothing_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class NothingPredecAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->nothing_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class NothingPositiveAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->nothing_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class NothingNegativeAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->nothing_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class NothingNotAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->nothing_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class NothingInverseAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->nothing_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class NothingMultAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->nothing_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class NothingDivAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->nothing_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class NothingRemAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->nothing_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class NothingRshiftAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->nothing_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class NothingLshiftAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->nothing_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class NothingAddAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->nothing_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class NothingSubAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->nothing_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }

        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class NothingLtAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->nothing_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }

        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class NothingLeqAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->nothing_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }

        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class NothingGtAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->nothing_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }

        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class NothingGeqAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->nothing_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }

        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class NothingEqAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->nothing_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        if (others.size() != 1) {
            rt->signalError("Expected exactly one right-side argument");
        }
        auto &arg1 = others[0];
        if (!rt->isTypeObject(arg1)) {
            rt->signalError("Right-side object is invalid: " + arg1->shortRepr());
        }

        if (rt->isInstanceObject(self) && rt->isInstanceObject(arg1)) {
            auto res                 = rt->make(rt->boolean_type, Runtime::INSTANCE_OBJECT);
            getBooleanValue(res, rt) = (arg1->type->id == self->type->id);
            return res;
        }
        else if (!rt->isInstanceObject(self) && !rt->isInstanceObject(arg1)) {
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
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        auto res                                     = NothingEqAdapter::operator()(self, others, rt);
        getBooleanValue(res, rt) = !getBooleanValue(res, rt);
        return res;
    }
};

class NothingBitandAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->nothing_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }

        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class NothingBitxorAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->nothing_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }

        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class NothingBitorAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->nothing_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }

        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class NothingAndAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->nothing_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }

        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class NothingOrAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->nothing_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }

        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

// TODO: add all operators to function and nothing
NothingType::NothingType(Runtime *rt)
    : Type(true, rt) {
    this->addOperator(OperatorNode::POST_PLUS_PLUS, new NothingPostincAdapter(), rt);
    this->addOperator(OperatorNode::POST_MINUS_MINUS, new NothingPostdecAdapter(), rt);
    this->addOperator(OperatorNode::CALL, new NothingCallAdapter(), rt);
    this->addOperator(OperatorNode::INDEX, new NothingIndexAdapter(), rt);
    this->addOperator(OperatorNode::PRE_PLUS_PLUS, new NothingPreincAdapter(), rt);
    this->addOperator(OperatorNode::PRE_MINUS_MINUS, new NothingPredecAdapter(), rt);
    this->addOperator(OperatorNode::PRE_PLUS, new NothingPositiveAdapter(), rt);
    this->addOperator(OperatorNode::PRE_MINUS, new NothingNegativeAdapter(), rt);
    this->addOperator(OperatorNode::NOT, new NothingNotAdapter(), rt);
    this->addOperator(OperatorNode::INVERSE, new NothingInverseAdapter(), rt);
    this->addOperator(OperatorNode::MULT, new NothingMultAdapter(), rt);
    this->addOperator(OperatorNode::DIV, new NothingDivAdapter(), rt);
    this->addOperator(OperatorNode::REM, new NothingRemAdapter(), rt);
    this->addOperator(OperatorNode::RIGHT_SHIFT, new NothingRshiftAdapter(), rt);
    this->addOperator(OperatorNode::LEFT_SHIFT, new NothingLshiftAdapter(), rt);
    this->addOperator(OperatorNode::PLUS, new NothingAddAdapter(), rt);
    this->addOperator(OperatorNode::MINUS, new NothingSubAdapter(), rt);
    this->addOperator(OperatorNode::LESS, new NothingLtAdapter(), rt);
    this->addOperator(OperatorNode::LESS_EQUAL, new NothingLeqAdapter(), rt);
    this->addOperator(OperatorNode::GREATER, new NothingGtAdapter(), rt);
    this->addOperator(OperatorNode::GREATER_EQUAL, new NothingGeqAdapter(), rt);
    this->addOperator(OperatorNode::EQUAL, new NothingEqAdapter(), rt);
    this->addOperator(OperatorNode::NOT_EQUAL, new NothingNeqAdapter(), rt);
    this->addOperator(OperatorNode::BITAND, new NothingBitandAdapter(), rt);
    this->addOperator(OperatorNode::BITXOR, new NothingBitxorAdapter(), rt);
    this->addOperator(OperatorNode::BITOR, new NothingBitorAdapter(), rt);
    this->addOperator(OperatorNode::AND, new NothingAndAdapter(), rt);
    this->addOperator(OperatorNode::OR, new NothingOrAdapter(), rt);
}

Object *NothingType::create(Runtime *rt) {
    auto ins = createInstance(rt, true, NothingInstance);
    auto obj = createObject(rt, true, ins, this, true);
    return obj;
}

std::string NothingType::shortRepr() {
    if (this == NULL) {
        return "NULL";
    }
    return "NothingType(id = " + std::to_string(this->id) + ")";
}

Object *NothingType::copy(Object *obj, Runtime *rt) {
    if (!rt->isTypeObject(obj) || obj->type->id != rt->nothing_type->id) {
        rt->signalError("Failed to copy an invalid object: " + obj->shortRepr());
    }
    auto ins = obj->instance->copy(rt);
    auto res = createObject(rt, true, ins, this, true);
    return res;
}

Object *makeNothingInstanceObject(Runtime *rt) {
    auto res = rt->make(rt->nothing_type, rt->INSTANCE_OBJECT);
    return res;
}
}    // namespace Cotton::Builtin
