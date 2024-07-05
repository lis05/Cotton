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
FunctionInstance::FunctionInstance(Runtime *rt, bool on_stack)
    : Instance(rt, sizeof(FunctionInstance), on_stack) {
    this->is_internal  = true;
    this->internal_ptr = NULL;
    this->cotton_ptr   = NULL;
}

FunctionInstance::~FunctionInstance() {}

void FunctionInstance::init(bool is_internal, InternalFunction internal_ptr, FuncDefNode *cotton_ptr) {
    this->is_internal  = is_internal;
    this->internal_ptr = internal_ptr;
    this->cotton_ptr   = cotton_ptr;
}

Instance *FunctionInstance::copy(Runtime *rt) {
    auto res = new (std::nothrow) FunctionInstance(rt, false);
    if (res == NULL) {
        rt->signalError("Failed to copy " + this->shortRepr());
    }
    res->on_stack = false;
    res->init(this->is_internal, this->internal_ptr, this->cotton_ptr);
    return res;
}

std::string FunctionInstance::shortRepr() {
    if (this == NULL) {
        return "NULL";
    }
    return "FunctionInstance(id = " + std::to_string(this->id)
           + ", is_internal = " + (this->is_internal ? "true" : "false") + ")";
}

size_t FunctionInstance::getSize() {
    return sizeof(FunctionInstance);
}

size_t FunctionType::getInstanceSize() {
    return sizeof(FunctionInstance);
}

class FunctionPostincAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->function_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }

        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class FunctionPostdecAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->function_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }

        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class FunctionCallAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->function_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        if (!rt->isInstanceObject(self)) {
            rt->signalError(self->shortRepr() + " does not support that operator");
        }
        auto f = icast(self->instance, FunctionInstance);
        if (f->is_internal) {
            if (f->internal_ptr == NULL) {
                rt->signalError("Failed to execute NULL internal function " + self->shortRepr());
            }
            auto res = f->internal_ptr(others, rt);
            if (res == NULL) {
                rt->signalError("Execution of internal function " + self->shortRepr() + " has failed");
            }
            return res;
        }
        else {
            if (f->cotton_ptr == NULL || f->cotton_ptr->body == NULL) {
                rt->signalError("Failed to execute NULL function " + self->shortRepr());
            }
            rt->newFrame(false);
            rt->scope->arguments.push_back(self);
            for (auto arg : others) {
                rt->scope->arguments.push_back(arg);
            }
            if (f->cotton_ptr->params != NULL) {
                int i = 0;
                for (auto token : f->cotton_ptr->params->list) {
                    if (i >= others.size()) {
                        break;
                    }
                    rt->scope->addVariable(token->nameid, others[i], rt);
                    i++;
                }
            }
            auto res = rt->execute(f->cotton_ptr);
            rt->popFrame();
            if (res.result == NULL) {
                rt->signalError("Execution of function " + self->shortRepr() + " has failed");
            }
            return res.result;
        }
    }
};

class FunctionIndexAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->function_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }

        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class FunctionPreincAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->function_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }

        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class FunctionPredecAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->function_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }

        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class FunctionPositiveAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->function_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }

        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class FunctionNegativeAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->function_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }

        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class FunctionNotAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->function_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }

        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class FunctionInverseAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->function_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }

        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class FunctionMultAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->function_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }

        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class FunctionDivAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->function_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }

        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class FunctionRemAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->function_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }

        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class FunctionRshiftAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->function_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }

        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class FunctionLshiftAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->function_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }

        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class FunctionAddAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->function_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }

        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class FunctionSubAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->function_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }

        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class FunctionLtAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->function_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }

        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class FunctionLeqAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->function_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }

        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class FunctionGtAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->function_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }

        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class FunctionGeqAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->function_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }

        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class FunctionEqAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->function_type->id) {
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
            auto res                 = rt->make(rt->boolean_type, Runtime::INSTANCE_OBJECT);
            getBooleanValue(res, rt) = false;
            if (self->type->id == arg1->type->id) {
                auto f1 = icast(self->instance, FunctionInstance);
                auto f2 = icast(arg1->instance, FunctionInstance);
                if (f1->is_internal == f2->is_internal && f1->internal_ptr == f2->internal_ptr
                    && f1->cotton_ptr == f2->cotton_ptr)
                {
                    getBooleanValue(res, rt) = true;
                }
            }
            return res;
        }
        else if (!rt->isInstanceObject(self) && !rt->isInstanceObject(arg1)) {
            auto res                 = rt->make(rt->boolean_type, Runtime::INSTANCE_OBJECT);
            getBooleanValue(res, rt) = self->type->id == arg1->type->id;
            return res;
        }
        else {
            auto res                 = rt->make(rt->boolean_type, Runtime::INSTANCE_OBJECT);
            getBooleanValue(res, rt) = false;
            return res;
        }
    }
};

class FunctionNeqAdapter: public FunctionEqAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        auto res                 = FunctionEqAdapter::operator()(self, others, rt);
        getBooleanValue(res, rt) = !getBooleanValue(res, rt);
        return res;
    }
};

class FunctionBitandAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->function_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }

        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class FunctionBitxorAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->function_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }

        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class FunctionBitorAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->function_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }

        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class FunctionAndAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->function_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }

        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class FunctionOrAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->function_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }

        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

// TODO: add all operators to function and nothing
FunctionType::FunctionType(Runtime *rt)
    : Type(true, rt) {
    this->addOperator(OperatorNode::POST_PLUS_PLUS, new FunctionPostincAdapter(), rt);
    this->addOperator(OperatorNode::POST_MINUS_MINUS, new FunctionPostdecAdapter(), rt);
    this->addOperator(OperatorNode::CALL, new FunctionCallAdapter(), rt);
    this->addOperator(OperatorNode::INDEX, new FunctionIndexAdapter(), rt);
    this->addOperator(OperatorNode::PRE_PLUS_PLUS, new FunctionPreincAdapter(), rt);
    this->addOperator(OperatorNode::PRE_MINUS_MINUS, new FunctionPredecAdapter(), rt);
    this->addOperator(OperatorNode::PRE_PLUS, new FunctionPositiveAdapter(), rt);
    this->addOperator(OperatorNode::PRE_MINUS, new FunctionNegativeAdapter(), rt);
    this->addOperator(OperatorNode::NOT, new FunctionNotAdapter(), rt);
    this->addOperator(OperatorNode::INVERSE, new FunctionInverseAdapter(), rt);
    this->addOperator(OperatorNode::MULT, new FunctionMultAdapter(), rt);
    this->addOperator(OperatorNode::DIV, new FunctionDivAdapter(), rt);
    this->addOperator(OperatorNode::REM, new FunctionRemAdapter(), rt);
    this->addOperator(OperatorNode::RIGHT_SHIFT, new FunctionRshiftAdapter(), rt);
    this->addOperator(OperatorNode::LEFT_SHIFT, new FunctionLshiftAdapter(), rt);
    this->addOperator(OperatorNode::PLUS, new FunctionAddAdapter(), rt);
    this->addOperator(OperatorNode::MINUS, new FunctionSubAdapter(), rt);
    this->addOperator(OperatorNode::LESS, new FunctionLtAdapter(), rt);
    this->addOperator(OperatorNode::LESS_EQUAL, new FunctionLeqAdapter(), rt);
    this->addOperator(OperatorNode::GREATER, new FunctionGtAdapter(), rt);
    this->addOperator(OperatorNode::GREATER_EQUAL, new FunctionGeqAdapter(), rt);
    this->addOperator(OperatorNode::EQUAL, new FunctionEqAdapter(), rt);
    this->addOperator(OperatorNode::NOT_EQUAL, new FunctionNeqAdapter(), rt);
    this->addOperator(OperatorNode::BITAND, new FunctionBitandAdapter(), rt);
    this->addOperator(OperatorNode::BITXOR, new FunctionBitxorAdapter(), rt);
    this->addOperator(OperatorNode::BITOR, new FunctionBitorAdapter(), rt);
    this->addOperator(OperatorNode::AND, new FunctionAndAdapter(), rt);
    this->addOperator(OperatorNode::OR, new FunctionOrAdapter(), rt);
}

Object *FunctionType::create(Runtime *rt) {
    auto ins = createInstance(rt, false, FunctionInstance);
    auto obj = createObject(rt, true, ins, this, false);
    return obj;
}

Object *FunctionType::copy(Object *obj, Runtime *rt) {
    if (!rt->isTypeObject(obj) || obj->type->id != rt->function_type->id) {
        rt->signalError("Failed to copy an invalid object: " + obj->shortRepr());
    }
    auto ins = obj->instance->copy(rt);
    auto res = createObject(rt, true, ins, this, false);
    return res;
}

std::string FunctionType::shortRepr() {
    if (this == NULL) {
        return "NULL";
    }
    return "FunctionType(id = " + std::to_string(this->id) + ")";
}

Object *
makeFunctionInstanceObject(bool is_internal, InternalFunction internal_ptr, FuncDefNode *cotton_ptr, Runtime *rt) {
    auto res = rt->make(rt->function_type, rt->INSTANCE_OBJECT);
    icast(res->instance, FunctionInstance)->init(is_internal, internal_ptr, cotton_ptr);
    return res;
}

}    // namespace Cotton::Builtin
