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
        ((BooleanInstance *)res)->value = this->value;
        res->on_stack                   = true;
        return res;
    }
    res = new (std::nothrow) BooleanInstance(rt, false);
    if (res == NULL) {
        return NULL;
    }
    ((BooleanInstance *)res)->value = this->value;
    res->on_stack                   = false;
    return res;
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
        rt->signalError("Boolean does not support that operator");
        return NULL;
    }
};

class BooleanPostdecAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        rt->signalError("Boolean does not support that operator");
        return NULL;
    }
};

class BooleanCallAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        rt->signalError("Boolean does not support that operator");
        return NULL;
    }
};

class BooleanIndexAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        rt->signalError("Boolean does not support that operator");
        return NULL;
    }
};

class BooleanPreincAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        rt->signalError("Boolean does not support that operator");
        return NULL;
    }
};

class BooleanPredecAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        rt->signalError("Boolean does not support that operator");
        return NULL;
    }
};

class BooleanPositiveAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        rt->signalError("Boolean does not support that operator");
        return NULL;
    }
};

class BooleanNegativeAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        rt->signalError("Boolean does not support that operator");
        return NULL;
    }
};

class BooleanNotAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->validate(self, Runtime::INSTANCE_OBJECT)) {
            return NULL;
        }
        auto res = self->copy(rt);
        if (!rt->validate(res, Runtime::INSTANCE_OBJECT)) {
            return NULL;
        }

        ((BooleanInstance *)res->instance)->value = !((BooleanInstance *)self->instance)->value;
        return res;
    }
};

class BooleanInverseAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        rt->signalError("Boolean does not support that operator");
        return NULL;
    }
};

class BooleanMultAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        rt->signalError("Boolean does not support that operator");
        return NULL;
    }
};

class BooleanDivAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        rt->signalError("Boolean does not support that operator");
        return NULL;
    }
};

class BooleanRemAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        rt->signalError("Boolean does not support that operator");
        return NULL;
    }
};

class BooleanRshiftAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        rt->signalError("Boolean does not support that operator");
        return NULL;
    }
};

class BooleanLshiftAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        rt->signalError("Boolean does not support that operator");
        return NULL;
    }
};

class BooleanAddAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        rt->signalError("Boolean does not support that operator");
        return NULL;
    }
};

class BooleanSubAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        rt->signalError("Boolean does not support that operator");
        return NULL;
    }
};

class BooleanLtAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        rt->signalError("Boolean does not support that operator");
        return NULL;
    }
};

class BooleanLeqAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        rt->signalError("Boolean does not support that operator");
        return NULL;
    }
};

class BooleanGtAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        rt->signalError("Boolean does not support that operator");
        return NULL;
    }
};

class BooleanGeqAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        rt->signalError("Boolean does not support that operator");
        return NULL;
    }
};

class BooleanEqAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (others.size() != 1) {
            rt->signalError("Expected one right-hand argument");
            return NULL;
        }
        auto arg1 = others[0];
        if (!rt->validate(arg1, Runtime::INSTANCE_OBJECT)) {
            return NULL;
        }

        auto res = rt->make(self->type, Runtime::INSTANCE_OBJECT);
        if (!rt->validate(res, Runtime::INSTANCE_OBJECT)) {
            return NULL;
        }
        
        auto &res_value = ((BooleanInstance *)res->instance)->value;
        if (arg1->type->id != self->type->id) {
            res_value = false;
        }
        else if (((BooleanInstance *)self->instance)->value != ((BooleanInstance *)arg1->instance)->value) {
            res_value = false;
        }
        else {
            res_value = true;
        }

        return res;
    }
};

class BooleanNeqAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (others.size() != 1) {
            rt->signalError("Expected one right-hand argument");
            return NULL;
        }
        auto arg1 = others[0];

        auto  res       = rt->make(self->type, Runtime::INSTANCE_OBJECT);
        auto &res_value = ((BooleanInstance *)res->instance)->value;
        if (arg1->type->id != self->type->id) {
            res_value = true;
        }
        else if (((BooleanInstance *)self->instance)->value != ((BooleanInstance *)arg1->instance)->value) {
            res_value = true;
        }
        else {
            res_value = false;
        }

        return res;
    }
};

class BooleanBitandAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        rt->signalError("Boolean does not support that operator");
        return NULL;
    }
};

class BooleanBitxorAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        rt->signalError("Boolean does not support that operator");
        return NULL;
    }
};

class BooleanBitorAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        rt->signalError("Boolean does not support that operator");
        return NULL;
    }
};

class BooleanAndAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (others.size() != 1) {
            rt->signalError("Expected one right-hand argument");
            return NULL;
        }
        auto arg1 = others[0];

        auto  res       = rt->make(self->type, Runtime::INSTANCE_OBJECT);
        auto &res_value = ((BooleanInstance *)res->instance)->value;
        if (arg1->type->id != self->type->id) {
            rt->signalError("Boolean only supports that operator with another Boolean");
            return NULL;
        }
        res_value = ((BooleanInstance *)self->instance)->value && ((BooleanInstance *)arg1->instance)->value;

        return res;
    }
};

class BooleanOrAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (others.size() != 1) {
            rt->signalError("Expected one right-hand argument");
            return NULL;
        }
        auto arg1 = others[0];

        auto  res       = rt->make(self->type, Runtime::INSTANCE_OBJECT);
        auto &res_value = ((BooleanInstance *)res->instance)->value;
        if (arg1->type->id != self->type->id) {
            rt->signalError("Boolean only supports that operator with another Boolean");
            return NULL;
        }
        res_value = ((BooleanInstance *)self->instance)->value || ((BooleanInstance *)arg1->instance)->value;

        return res;
    }
};

// TODO: add all operators to function and nothing
BooleanType::BooleanType(Runtime *rt)
    : Type(true, rt) {
    this->addOperator(OperatorNode::POST_PLUS_PLUS, new BooleanPostincAdapter());
    this->addOperator(OperatorNode::POST_MINUS_MINUS, new BooleanPostdecAdapter());
    this->addOperator(OperatorNode::CALL, new BooleanCallAdapter());
    this->addOperator(OperatorNode::INDEX, new BooleanIndexAdapter());
    this->addOperator(OperatorNode::PRE_PLUS_PLUS, new BooleanPreincAdapter());
    this->addOperator(OperatorNode::PRE_MINUS_MINUS, new BooleanPredecAdapter());
    this->addOperator(OperatorNode::PRE_PLUS, new BooleanPositiveAdapter());
    this->addOperator(OperatorNode::PRE_MINUS, new BooleanNegativeAdapter());
    this->addOperator(OperatorNode::NOT, new BooleanNotAdapter());
    this->addOperator(OperatorNode::INVERSE, new BooleanInverseAdapter());
    this->addOperator(OperatorNode::MULT, new BooleanMultAdapter());
    this->addOperator(OperatorNode::DIV, new BooleanDivAdapter());
    this->addOperator(OperatorNode::REM, new BooleanRemAdapter());
    this->addOperator(OperatorNode::RIGHT_SHIFT, new BooleanRshiftAdapter());
    this->addOperator(OperatorNode::LEFT_SHIFT, new BooleanLshiftAdapter());
    this->addOperator(OperatorNode::PLUS, new BooleanAddAdapter());
    this->addOperator(OperatorNode::MINUS, new BooleanSubAdapter());
    this->addOperator(OperatorNode::LESS, new BooleanLtAdapter());
    this->addOperator(OperatorNode::LESS_EQUAL, new BooleanLeqAdapter());
    this->addOperator(OperatorNode::GREATER, new BooleanGtAdapter());
    this->addOperator(OperatorNode::GREATER_EQUAL, new BooleanGeqAdapter());
    this->addOperator(OperatorNode::EQUAL, new BooleanEqAdapter());
    this->addOperator(OperatorNode::NOT_EQUAL, new BooleanNeqAdapter());
    this->addOperator(OperatorNode::BITAND, new BooleanBitandAdapter());
    this->addOperator(OperatorNode::BITXOR, new BooleanBitxorAdapter());
    this->addOperator(OperatorNode::BITOR, new BooleanBitorAdapter());
    this->addOperator(OperatorNode::AND, new BooleanAndAdapter());
    this->addOperator(OperatorNode::OR, new BooleanOrAdapter());
}

Object *BooleanType::create(Runtime *rt) {
    auto ins = createInstance(rt, true, BooleanInstance);
    auto obj = createObject(rt, true, ins, this, true);
    return obj;
}
}    // namespace Cotton::Builtin
