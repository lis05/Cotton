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

#include "function.h"
#include "../back/type.h"

namespace Cotton::Builtin {
class FunctionCallAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->validate(self)) {
            return NULL;
        }
        if (!rt->validate(self->type)) {
            return NULL;
        }
        if (!rt->validate(self->instance)) {
            return NULL;
        }
        auto ins = (FunctionInstance *)self->instance;
        if (ins->is_internal) {
            if (ins->internal_ptr == NULL) {
                rt->signalSubError("Failed to call internal function");
                return NULL;
            }
            return ins->internal_ptr(others, rt);
        }
        else {
            auto res = rt->execute(ins->cotton_ptr);
            if (!res.success) {
                rt->signalSubError("Failed to execute function written in Cotton");
                return NULL;
            }
            return res.res;
        }
    }
};

FunctionInstance::FunctionInstance(Runtime *rt, bool on_stack)
    : Instance(rt, sizeof(FunctionInstance), on_stack) {
    this->is_internal  = true;
    this->internal_ptr = NULL;
    this->cotton_ptr   = NULL;
}

FunctionInstance::~FunctionInstance() {}

void FunctionInstance::init(bool is_internal, InternalFunction internal_ptr, StmtNode *cotton_ptr) {
    this->is_internal  = is_internal;
    this->internal_ptr = internal_ptr;
    this->cotton_ptr   = cotton_ptr;
}

Instance *FunctionInstance::copy(Runtime *rt) {
    auto res = new (std::nothrow) FunctionInstance(rt, false);
    if (res == NULL) {
        return NULL;
    }
    res->on_stack = false;
    res->init(this->is_internal, this->internal_ptr, this->cotton_ptr);
    return res;
}

size_t FunctionInstance::getSize() {
    return sizeof(FunctionInstance);
}

size_t FunctionType::getInstanceSize() {
    return sizeof(FunctionInstance);
}

FunctionType::FunctionType(Runtime *rt)
    : Type(false, rt) {
    FunctionCallAdapter *fca = new FunctionCallAdapter();
    this->addOperator(OperatorNode::CALL, fca);
}

Object *FunctionType::create(Runtime *rt) {
    auto ins = createInstance(rt, false, FunctionInstance);
    auto obj = createObject(rt, true, ins, this, false);
    return obj;
}
}    // namespace Cotton::Builtin
