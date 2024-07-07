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
RealInstance::RealInstance(Runtime *rt)
    : Instance(rt, sizeof(RealInstance)) {
    ProfilerCAPTURE();
    this->value = 0;
}

RealInstance::~RealInstance() {
    ProfilerCAPTURE();
}

Instance *RealInstance::copy(Runtime *rt) {
    ProfilerCAPTURE();
    Instance *res = new (rt->alloc(sizeof(RealInstance))) RealInstance(rt);

    if (res == NULL) {
        rt->signalError("Failed to copy " + this->shortRepr());
    }
    icast(res, RealInstance)->value = this->value;
    return res;
}

std::string RealInstance::shortRepr() {
    ProfilerCAPTURE();
    if (this == NULL) {
        return "RealInstance(NULL)";
    }
    return "RealInstance(id = " + std::to_string(this->id) + ", value = " + std::to_string(this->value) + ")";
}

void RealInstance::destroy(Runtime *rt) {
    rt->dealloc(this, sizeof(RealInstance));
}

size_t RealInstance::getSize() {
    ProfilerCAPTURE();
    return sizeof(RealInstance);
}

size_t RealType::getInstanceSize() {
    ProfilerCAPTURE();
    return sizeof(RealInstance);
}

class RealUnsupportedAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        ProfilerCAPTURE();
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class RealPositiveAdapter: public OperatorAdapter {
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

class RealNegativeAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        ProfilerCAPTURE();

        if (!isInstanceObject(self)) {
            rt->signalError(self->shortRepr() + " does not support that operator");
        }

        auto res               = self->type->copy(self, rt);
        getRealValueFast(res) *= -1;
        return res;
    }
};

class RealMultAdapter: public OperatorAdapter {
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
        if (arg1->instance == NULL || arg1->type->id != rt->real_type->id) {
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be a Real instance object");
        }

        auto res = makeRealInstanceObject(getRealValueFast(self) * getRealValueFast(arg1), rt);
        return res;
    }
};

class RealDivAdapter: public OperatorAdapter {
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
        if (arg1->instance == NULL || arg1->type->id != rt->real_type->id) {
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be a Real instance object");
        }

        auto res = makeRealInstanceObject(getRealValueFast(self) / getRealValueFast(arg1), rt);
        return res;
    }
};

class RealAddAdapter: public OperatorAdapter {
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
        if (arg1->instance == NULL || arg1->type->id != rt->real_type->id) {
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be a Real instance object");
        }

        auto res = makeRealInstanceObject(getRealValueFast(self) + getRealValueFast(arg1), rt);
        return res;
    }
};

class RealSubAdapter: public OperatorAdapter {
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
        if (arg1->instance == NULL || arg1->type->id != rt->real_type->id) {
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be a Real instance object");
        }

        auto res = makeRealInstanceObject(getRealValueFast(self) - getRealValueFast(arg1), rt);
        return res;
    }
};

class RealLtAdapter: public OperatorAdapter {
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
        if (arg1->instance == NULL || arg1->type->id != rt->real_type->id) {
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be a Real instance object");
        }

        auto res = makeBooleanInstanceObject(getRealValueFast(self) < getRealValueFast(arg1), rt);
        return res;
    }
};

class RealLeqAdapter: public OperatorAdapter {
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
        if (arg1->instance == NULL || arg1->type->id != rt->real_type->id) {
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be a Real instance object");
        }

        auto res = makeBooleanInstanceObject(getRealValueFast(self) <= getRealValueFast(arg1), rt);
        return res;
    }
};

class RealGtAdapter: public OperatorAdapter {
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
        if (arg1->instance == NULL || arg1->type->id != rt->real_type->id) {
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be a Real instance object");
        }

        auto res = makeBooleanInstanceObject(getRealValueFast(self) > getRealValueFast(arg1), rt);
        return res;
    }
};

class RealGeqAdapter: public OperatorAdapter {
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
        if (arg1->instance == NULL || arg1->type->id != rt->real_type->id) {
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be a Real instance object");
        }

        auto res = makeBooleanInstanceObject(getRealValueFast(self) >= getRealValueFast(arg1), rt);
        return res;
    }
};

class RealEqAdapter: public OperatorAdapter {
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
            return makeBooleanInstanceObject(getRealValueFast(self) == getRealValueFast(arg1), rt);
        }
        else if (!i1 && !i2) {
            return makeBooleanInstanceObject(self->type->id == arg1->type->id, rt);
        }
        else {
            return makeBooleanInstanceObject(false, rt);
        }
    }
};

class RealNeqAdapter: public RealEqAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        ProfilerCAPTURE();
        auto res              = RealEqAdapter::operator()(self, others, rt);
        getRealValueFast(res) = !getRealValueFast(res);
        return res;
    }
};

// TODO: add all operators to function and nothing
RealType::RealType(Runtime *rt)
    : Type(rt) {
    ProfilerCAPTURE();
    this->addOperator(OperatorNode::POST_PLUS_PLUS, new RealUnsupportedAdapter());
    this->addOperator(OperatorNode::POST_MINUS_MINUS, new RealUnsupportedAdapter());
    this->addOperator(OperatorNode::CALL, new RealUnsupportedAdapter());
    this->addOperator(OperatorNode::INDEX, new RealUnsupportedAdapter());
    this->addOperator(OperatorNode::PRE_PLUS_PLUS, new RealUnsupportedAdapter());
    this->addOperator(OperatorNode::PRE_MINUS_MINUS, new RealUnsupportedAdapter());
    this->addOperator(OperatorNode::PRE_PLUS, new RealPositiveAdapter());
    this->addOperator(OperatorNode::PRE_MINUS, new RealNegativeAdapter());
    this->addOperator(OperatorNode::NOT, new RealUnsupportedAdapter());
    this->addOperator(OperatorNode::INVERSE, new RealUnsupportedAdapter());
    this->addOperator(OperatorNode::MULT, new RealMultAdapter());
    this->addOperator(OperatorNode::DIV, new RealDivAdapter());
    this->addOperator(OperatorNode::REM, new RealUnsupportedAdapter());
    this->addOperator(OperatorNode::RIGHT_SHIFT, new RealUnsupportedAdapter());
    this->addOperator(OperatorNode::LEFT_SHIFT, new RealUnsupportedAdapter());
    this->addOperator(OperatorNode::PLUS, new RealAddAdapter());
    this->addOperator(OperatorNode::MINUS, new RealSubAdapter());
    this->addOperator(OperatorNode::LESS, new RealLtAdapter());
    this->addOperator(OperatorNode::LESS_EQUAL, new RealLeqAdapter());
    this->addOperator(OperatorNode::GREATER, new RealGtAdapter());
    this->addOperator(OperatorNode::GREATER_EQUAL, new RealGeqAdapter());
    this->addOperator(OperatorNode::EQUAL, new RealEqAdapter());
    this->addOperator(OperatorNode::NOT_EQUAL, new RealNeqAdapter());
    this->addOperator(OperatorNode::BITAND, new RealUnsupportedAdapter());
    this->addOperator(OperatorNode::BITXOR, new RealUnsupportedAdapter());
    this->addOperator(OperatorNode::BITOR, new RealUnsupportedAdapter());
    this->addOperator(OperatorNode::AND, new RealUnsupportedAdapter());
    this->addOperator(OperatorNode::OR, new RealUnsupportedAdapter());
}

Object *RealType::create(Runtime *rt) {
    ProfilerCAPTURE();
    Instance *ins = new (rt->alloc(sizeof(RealInstance))) RealInstance(rt);
    Object   *obj = newObject(true, ins, this, rt);
    return obj;
}

Object *RealType::copy(Object *obj, Runtime *rt) {
    ProfilerCAPTURE();
    if (!isTypeObject(obj) || obj->type->id != rt->real_type->id) {
        rt->signalError("Failed to copy an invalid object: " + obj->shortRepr());
    }
    if (obj->instance == NULL) {
        return newObject(false, NULL, this, rt);
    }
    auto ins = obj->instance->copy(rt);
    auto res = newObject(true, ins, this, rt);
    return res;
}

std::string RealType::shortRepr() {
    ProfilerCAPTURE();
    if (this == NULL) {
        return "RealType(NULL)";
    }
    return "RealType(id = " + std::to_string(this->id) + ")";
}

double &getRealValue(Object *obj, Runtime *rt) {
    ProfilerCAPTURE();
    if (!isInstanceObject(obj)) {
        rt->signalError(obj->shortRepr() + " is not an instance object");
    }
    if (obj->type->id != rt->real_type->id) {
        rt->signalError(obj->shortRepr() + " is not Real");
    }
    return icast(obj->instance, RealInstance)->value;
}

Object *makeRealInstanceObject(double value, Runtime *rt) {
    ProfilerCAPTURE();
    auto res                                  = rt->make(rt->real_type, Runtime::INSTANCE_OBJECT);
    icast(res->instance, RealInstance)->value = value;
    return res;
}

}    // namespace Cotton::Builtin
