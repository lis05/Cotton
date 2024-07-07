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
FunctionInstance::FunctionInstance(Runtime *rt)
    : Instance(rt, sizeof(FunctionInstance)) {
    ProfilerCAPTURE();
    this->is_internal  = true;
    this->internal_ptr = NULL;
    this->cotton_ptr   = NULL;
}

FunctionInstance::~FunctionInstance() {
    ProfilerCAPTURE();
}

void FunctionInstance::init(bool is_internal, InternalFunction internal_ptr, FuncDefNode *cotton_ptr) {
    ProfilerCAPTURE();
    this->is_internal  = is_internal;
    this->internal_ptr = internal_ptr;
    this->cotton_ptr   = cotton_ptr;
}

Instance *FunctionInstance::copy(Runtime *rt) {
    ProfilerCAPTURE();
    auto res = new FunctionInstance(rt);
    if (res == NULL) {
        rt->signalError("Failed to copy " + this->shortRepr());
    }
    res->init(this->is_internal, this->internal_ptr, this->cotton_ptr);
    return res;
}

std::string FunctionInstance::shortRepr() {
    ProfilerCAPTURE();
    if (this == NULL) {
        return "NULL";
    }
    return "FunctionInstance(id = " + std::to_string(this->id)
           + ", is_internal = " + (this->is_internal ? "true" : "false") + ")";
}

void FunctionInstance::destroy(Runtime *rt) {
    delete this;
}

size_t FunctionInstance::getSize() {
    ProfilerCAPTURE();
    return sizeof(FunctionInstance);
}

size_t FunctionType::getInstanceSize() {
    ProfilerCAPTURE();
    return sizeof(FunctionInstance);
}

class FunctionUnsupportedAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        ProfilerCAPTURE();
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class FunctionCallAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        ProfilerCAPTURE();

        if (!isInstanceObject(self)) {
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
            auto res = rt->execute(f->cotton_ptr->body);
            rt->popFrame();
            if (res == NULL) {
                rt->signalError("Execution of function " + self->shortRepr() + " has failed");
            }
            return res;
        }
    }
};

class FunctionEqAdapter: public OperatorAdapter {
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
        else if (!i1 && !i2) {
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
        ProfilerCAPTURE();
        auto res                 = FunctionEqAdapter::operator()(self, others, rt);
        getBooleanValue(res, rt) = !getBooleanValue(res, rt);
        return res;
    }
};

// TODO: add all operators to function and nothing
FunctionType::FunctionType(Runtime *rt)
    : Type(rt) {
    ProfilerCAPTURE();
    this->addOperator(OperatorNode::POST_PLUS_PLUS, new FunctionUnsupportedAdapter());
    this->addOperator(OperatorNode::POST_MINUS_MINUS, new FunctionUnsupportedAdapter());
    this->addOperator(OperatorNode::CALL, new FunctionCallAdapter());
    this->addOperator(OperatorNode::INDEX, new FunctionUnsupportedAdapter());
    this->addOperator(OperatorNode::PRE_PLUS_PLUS, new FunctionUnsupportedAdapter());
    this->addOperator(OperatorNode::PRE_MINUS_MINUS, new FunctionUnsupportedAdapter());
    this->addOperator(OperatorNode::PRE_PLUS, new FunctionUnsupportedAdapter());
    this->addOperator(OperatorNode::PRE_MINUS, new FunctionUnsupportedAdapter());
    this->addOperator(OperatorNode::NOT, new FunctionUnsupportedAdapter());
    this->addOperator(OperatorNode::INVERSE, new FunctionUnsupportedAdapter());
    this->addOperator(OperatorNode::MULT, new FunctionUnsupportedAdapter());
    this->addOperator(OperatorNode::DIV, new FunctionUnsupportedAdapter());
    this->addOperator(OperatorNode::REM, new FunctionUnsupportedAdapter());
    this->addOperator(OperatorNode::RIGHT_SHIFT, new FunctionUnsupportedAdapter());
    this->addOperator(OperatorNode::LEFT_SHIFT, new FunctionUnsupportedAdapter());
    this->addOperator(OperatorNode::PLUS, new FunctionUnsupportedAdapter());
    this->addOperator(OperatorNode::MINUS, new FunctionUnsupportedAdapter());
    this->addOperator(OperatorNode::LESS, new FunctionUnsupportedAdapter());
    this->addOperator(OperatorNode::LESS_EQUAL, new FunctionUnsupportedAdapter());
    this->addOperator(OperatorNode::GREATER, new FunctionUnsupportedAdapter());
    this->addOperator(OperatorNode::GREATER_EQUAL, new FunctionUnsupportedAdapter());
    this->addOperator(OperatorNode::EQUAL, new FunctionEqAdapter());
    this->addOperator(OperatorNode::NOT_EQUAL, new FunctionNeqAdapter());
    this->addOperator(OperatorNode::BITAND, new FunctionUnsupportedAdapter());
    this->addOperator(OperatorNode::BITXOR, new FunctionUnsupportedAdapter());
    this->addOperator(OperatorNode::BITOR, new FunctionUnsupportedAdapter());
    this->addOperator(OperatorNode::AND, new FunctionUnsupportedAdapter());
    this->addOperator(OperatorNode::OR, new FunctionUnsupportedAdapter());
}

Object *FunctionType::create(Runtime *rt) {
    ProfilerCAPTURE();
    auto    ins = new FunctionInstance(rt);
    Object *obj = newObject(true, ins, this, rt);
    return obj;
}

Object *FunctionType::copy(Object *obj, Runtime *rt) {
    ProfilerCAPTURE();
    if (!isTypeObject(obj) || obj->type->id != rt->function_type->id) {
        rt->signalError("Failed to copy an invalid object: " + obj->shortRepr());
    }
    if (obj->instance == NULL) {
        return newObject(false, NULL, this, rt);
    }
    auto ins = obj->instance->copy(rt);
    auto res = newObject(true, ins, this, rt);

    return res;
}

std::string FunctionType::shortRepr() {
    ProfilerCAPTURE();
    if (this == NULL) {
        return "NULL";
    }
    return "FunctionType(id = " + std::to_string(this->id) + ")";
}

Object *
makeFunctionInstanceObject(bool is_internal, InternalFunction internal_ptr, FuncDefNode *cotton_ptr, Runtime *rt) {
    ProfilerCAPTURE();
    auto res = rt->make(rt->function_type, rt->INSTANCE_OBJECT);
    icast(res->instance, FunctionInstance)->init(is_internal, internal_ptr, cotton_ptr);
    return res;
}

}    // namespace Cotton::Builtin
