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

static Object *
FunctionCallAdapter(Object *self, const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();

    if (!isInstanceObject(self)) {
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
    auto f = icast(self->instance, FunctionInstance);
    if (f->is_internal) {
        if (f->internal_ptr == NULL) {
            rt->signalError("Failed to execute NULL internal function " + self->shortRepr());
        }
        auto res = f->internal_ptr(args, rt, execution_result_matters);
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
        for (auto arg : args) {
            rt->scope->arguments.push_back(arg);
        }
        if (f->cotton_ptr->params != NULL) {
            int i = 0;
            for (auto token : f->cotton_ptr->params->list) {
                if (i >= args.size()) {
                    break;
                }
                rt->scope->addVariable(token->nameid, args[i], rt);
                i++;
            }
        }
        auto res = rt->execute(f->cotton_ptr->body, execution_result_matters);
        rt->popFrame();
        if (res == NULL) {
            rt->signalError("Execution of function " + self->shortRepr() + " has failed");
        }
        return res;
    }
}

static Object *FunctionEqAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();

    if (!isTypeObject(arg)) {
        rt->signalError("Right-side object is invalid: " + arg->shortRepr());
    }

    bool i1 = isInstanceObject(self);
    bool i2 = arg->instance != NULL;

    if (i1 && i2) {
        if (self->type->id == arg->type->id) {
            auto f1 = icast(self->instance, FunctionInstance);
            auto f2 = icast(arg->instance, FunctionInstance);
            if (f1->is_internal == f2->is_internal && f1->internal_ptr == f2->internal_ptr
                && f1->cotton_ptr == f2->cotton_ptr)
            {
                return rt->protected_true;
            }
        }
        return rt->protected_false;
    }
    else if (!i1 && !i2) {
        return (self->type->id == arg->type->id) ? rt->protected_true : rt->protected_false;
    }
    else {
        return rt->protected_false;
    }
}

static Object *FunctionNeqAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    auto res = FunctionEqAdapter(self, arg, rt, execution_result_matters);
    return (!getBooleanValueFast(res)) ? rt->protected_true : rt->protected_false;
}

// TODO: add all operators to function and nothing
FunctionType::FunctionType(Runtime *rt)
    : Type(rt) {
    ProfilerCAPTURE();
    this->call_op = FunctionCallAdapter;
    this->eq_op   = FunctionEqAdapter;
    this->neq_op  = FunctionNeqAdapter;
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
