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
        rt->signalError("Failed to copy " + this->userRepr(rt), rt->getContext().area);
    }
    res->init(this->is_internal, this->internal_ptr, this->cotton_ptr);
    return res;
}

std::string FunctionInstance::userRepr(Runtime *rt) {
    ProfilerCAPTURE();
    if (this == NULL) {
        return "Function(NULL)";
    }
    return (this->is_internal) ? "Function(internal)" : "Function";
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
    rt->verifyIsInstanceObject(self, rt->function_type, Runtime::SUB0_CTX);
    auto f = icast(self->instance, FunctionInstance);
    if (f->is_internal) {
        if (f->internal_ptr == NULL) {
            rt->signalError("Failed to execute NULL internal function: " + self->userRepr(rt),
                            rt->getContext().area);
        }
        auto res = f->internal_ptr(args, rt, execution_result_matters);
        if (execution_result_matters && res == NULL) {
            rt->signalError("Execution of internal function " + self->userRepr(rt) + " has failed",
                            rt->getContext().area);
        }
        return res;
    }
    else {
        if (f->cotton_ptr == NULL || f->cotton_ptr->body == NULL) {
            rt->signalError("Failed to execute NULL function " + self->userRepr(rt), rt->getContext().area);
        }
        rt->newFrame(false);
        // rt->scope->arguments.push_back(self); // is it needed?
        for (auto arg : args) {
            rt->scope->arguments.push_back(arg);
        }
        if (f->cotton_ptr->params != NULL) {
            int i = 0;
            for (auto token : f->cotton_ptr->params->list) {
                if (i >= args.size()) {
                    rt->scope->addVariable(token->nameid, makeNothingInstanceObject(rt), rt);
                    continue;
                }
                rt->scope->addVariable(token->nameid, args[i], rt);
                i++;
            }
        }
        auto res = rt->execute(f->cotton_ptr->body, execution_result_matters);
        rt->popFrame();
        if (execution_result_matters && res == NULL) {
            rt->signalError("Execution of function " + self->userRepr(rt) + " has failed",
                            rt->getContext().sub_areas[0]);
        }
        return res;
    }
}

static Object *FunctionEqAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyIsOfType(self, rt->function_type, Runtime::SUB0_CTX);
    rt->verifyIsValidObject(arg, Runtime::SUB1_CTX);

    if (!rt->isOfType(arg, rt->function_type)) {
        return rt->protected_false;
    }

    if (rt->isInstanceObject(self, rt->function_type)) {
        if (!rt->isInstanceObject(arg, rt->function_type)) {
            return rt->protected_false;
        }
        auto f1 = icast(self, FunctionInstance);
        auto f2 = icast(arg, FunctionInstance);
        if (f1->is_internal && f2->is_internal) {
            return rt->protectedBoolean(f1->internal_ptr == f2->internal_ptr);
        }
        else if (!f1->is_internal && !f2->is_internal) {
            return rt->protectedBoolean(f1->cotton_ptr == f2->cotton_ptr);
        }
        return rt->protected_false;
    }
    else if (rt->isTypeObject(self, rt->function_type)) {
        if (!rt->isTypeObject(arg, rt->function_type)) {
            return rt->protected_false;
        }
        return rt->protected_true;
    }

    return rt->protected_false;
}

static Object *FunctionNeqAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    auto res = FunctionEqAdapter(self, arg, rt, execution_result_matters);
    return (!getBooleanValueFast(res)) ? rt->protected_true : rt->protected_false;
}

static Object *mm__repr__(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];
    rt->verifyIsOfType(self, rt->function_type, Runtime::SUB1_CTX);

    if (!execution_result_matters) {
        return self;
    }

    if (rt->isTypeObject(self, NULL)) {
        return makeStringInstanceObject("Function", rt);
    }

    return makeStringInstanceObject("function", rt);
}

void installFunctionMethods(Type *type, Runtime *rt) {
    type->addMethod(MagicMethods::mm__repr__(rt), Builtin::makeFunctionInstanceObject(true, mm__repr__, NULL, rt));
}

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
    rt->verifyIsOfType(obj, rt->function_type);
    if (obj->instance == NULL) {
        return newObject(false, NULL, this, rt);
    }
    auto ins = obj->instance->copy(rt);
    auto res = newObject(true, ins, this, rt);

    return res;
}

std::string FunctionType::userRepr(Runtime *rt) {
    ProfilerCAPTURE();
    if (this == NULL) {
        return "FunctionType(NULL)";
    }
    return "FunctionType";
}

Object *
makeFunctionInstanceObject(bool is_internal, InternalFunction internal_ptr, FuncDefNode *cotton_ptr, Runtime *rt) {
    ProfilerCAPTURE();
    auto res = rt->make(rt->function_type, rt->INSTANCE_OBJECT);
    icast(res->instance, FunctionInstance)->init(is_internal, internal_ptr, cotton_ptr);
    return res;
}

}    // namespace Cotton::Builtin
