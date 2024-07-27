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

#include "integer.h"
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
    Instance *res = new IntegerInstance(rt);

    if (res == NULL) {
        rt->signalError("Failed to copy " + this->userRepr(rt), rt->getContext().area);
    }
    icast(res, IntegerInstance)->value = this->value;
    return res;
}

std::string IntegerInstance::userRepr(Runtime *rt) {
    ProfilerCAPTURE();
    if (this == NULL) {
        return "Integer(NULL)";
    }
    return "Integer(value = " + std::to_string(this->value) + ")";
}

size_t IntegerInstance::getSize() {
    ProfilerCAPTURE();
    return sizeof(IntegerInstance);
}

size_t IntegerType::getInstanceSize() {
    ProfilerCAPTURE();
    return sizeof(IntegerInstance);
}

static Object *IntegerPostincAdapter(Object *self, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyIsInstanceObject(self, rt->integer_type, rt->getContext().sub_areas[0]);

    if (!execution_result_matters) {
        getIntegerValueFast(self)++;
        return NULL;
    }

    auto res = self->type->copy(self, rt);
    getIntegerValueFast(self)++;
    return res;
}

static Object *IntegerPostdecAdapter(Object *self, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyIsInstanceObject(self, rt->integer_type, rt->getContext().sub_areas[0]);

    if (!execution_result_matters) {
        getIntegerValueFast(self)--;
        return NULL;
    }

    auto res = self->type->copy(self, rt);
    getIntegerValueFast(self)--;
    return res;
}

static Object *IntegerPreincAdapter(Object *self, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyIsInstanceObject(self, rt->integer_type, rt->getContext().sub_areas[0]);

    if (!execution_result_matters) {
        getIntegerValueFast(self)++;
        return NULL;
    }

    getIntegerValueFast(self)++;
    auto res = self->type->copy(self, rt);
    return res;
}

static Object *IntegerPredecAdapter(Object *self, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyIsInstanceObject(self, rt->integer_type, rt->getContext().sub_areas[0]);

    if (!execution_result_matters) {
        getIntegerValueFast(self)--;
        return NULL;
    }

    getIntegerValueFast(self)--;
    auto res = self->type->copy(self, rt);
    return res;
}

static Object *IntegerPositiveAdapter(Object *self, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyIsInstanceObject(self, rt->integer_type, rt->getContext().sub_areas[0]);

    if (!execution_result_matters) {
        return NULL;
    }

    auto res = self->type->copy(self, rt);
    return res;
}

static Object *IntegerNegativeAdapter(Object *self, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyIsInstanceObject(self, rt->integer_type, rt->getContext().sub_areas[0]);

    if (!execution_result_matters) {
        return NULL;
    }

    auto res                  = self->type->copy(self, rt);
    getIntegerValueFast(res) *= -1;
    return res;
}

static Object *IntegerInverseAdapter(Object *self, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyIsInstanceObject(self, rt->integer_type, rt->getContext().sub_areas[0]);

    if (!execution_result_matters) {
        return NULL;
    }

    auto res                 = self->type->copy(self, rt);
    getIntegerValueFast(res) = ~getIntegerValueFast(res);
    return res;
}

static Object *IntegerMultAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyIsInstanceObject(self, rt->integer_type, rt->getContext().sub_areas[0]);
    rt->verifyIsInstanceObject(arg, rt->integer_type, rt->getContext().sub_areas[1]);

    if (!execution_result_matters) {
        return NULL;
    }

    auto res = makeIntegerInstanceObject(getIntegerValueFast(self) * getIntegerValueFast(arg), rt);
    return res;
}

static Object *IntegerDivAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyIsInstanceObject(self, rt->integer_type, rt->getContext().sub_areas[0]);
    rt->verifyIsInstanceObject(arg, rt->integer_type, rt->getContext().sub_areas[1]);

    if (!execution_result_matters) {
        return NULL;
    }

    auto res = makeIntegerInstanceObject(getIntegerValueFast(self) / getIntegerValueFast(arg), rt);
    return res;
}

static Object *IntegerRemAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyIsInstanceObject(self, rt->integer_type, rt->getContext().sub_areas[0]);
    rt->verifyIsInstanceObject(arg, rt->integer_type, rt->getContext().sub_areas[1]);

    if (!execution_result_matters) {
        return NULL;
    }

    auto res = makeIntegerInstanceObject(getIntegerValueFast(self) % getIntegerValueFast(arg), rt);
    return res;
}

static Object *IntegerRshiftAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyIsInstanceObject(self, rt->integer_type, rt->getContext().sub_areas[0]);
    rt->verifyIsInstanceObject(arg, rt->integer_type, rt->getContext().sub_areas[1]);

    if (!execution_result_matters) {
        return NULL;
    }

    auto res = makeIntegerInstanceObject(getIntegerValueFast(self) >> getIntegerValueFast(arg), rt);
    return res;
}

static Object *IntegerLshiftAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyIsInstanceObject(self, rt->integer_type, rt->getContext().sub_areas[0]);
    rt->verifyIsInstanceObject(arg, rt->integer_type, rt->getContext().sub_areas[1]);

    if (!execution_result_matters) {
        return NULL;
    }

    auto res = makeIntegerInstanceObject(getIntegerValueFast(self) << getIntegerValueFast(arg), rt);
    return res;
}

static Object *IntegerAddAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyIsInstanceObject(self, rt->integer_type, rt->getContext().sub_areas[0]);
    rt->verifyIsInstanceObject(arg, rt->integer_type, rt->getContext().sub_areas[1]);

    if (!execution_result_matters) {
        return NULL;
    }

    auto res = makeIntegerInstanceObject(getIntegerValueFast(self) + getIntegerValueFast(arg), rt);
    return res;
}

static Object *IntegerSubAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyIsInstanceObject(self, rt->integer_type, rt->getContext().sub_areas[0]);
    rt->verifyIsInstanceObject(arg, rt->integer_type, rt->getContext().sub_areas[1]);

    if (!execution_result_matters) {
        return NULL;
    }

    auto res = makeIntegerInstanceObject(getIntegerValueFast(self) - getIntegerValueFast(arg), rt);
    return res;
}

static Object *IntegerLtAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyIsInstanceObject(self, rt->integer_type, rt->getContext().sub_areas[0]);
    rt->verifyIsInstanceObject(arg, rt->integer_type, rt->getContext().sub_areas[1]);

    return rt->protectedBoolean(getIntegerValueFast(self) < getIntegerValueFast(arg));
}

static Object *IntegerLeqAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyIsInstanceObject(self, rt->integer_type, rt->getContext().sub_areas[0]);
    rt->verifyIsInstanceObject(arg, rt->integer_type, rt->getContext().sub_areas[1]);

    return rt->protectedBoolean(getIntegerValueFast(self) <= getIntegerValueFast(arg));
}

static Object *IntegerGtAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyIsInstanceObject(self, rt->integer_type, rt->getContext().sub_areas[0]);
    rt->verifyIsInstanceObject(arg, rt->integer_type, rt->getContext().sub_areas[1]);

    return rt->protectedBoolean(getIntegerValueFast(self) > getIntegerValueFast(arg));
}

static Object *IntegerGeqAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyIsInstanceObject(self, rt->integer_type, rt->getContext().sub_areas[0]);
    rt->verifyIsInstanceObject(arg, rt->integer_type, rt->getContext().sub_areas[1]);

    return rt->protectedBoolean(getIntegerValueFast(self) >= getIntegerValueFast(arg));
}

static Object *IntegerEqAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyIsOfType(self, rt->integer_type, rt->getContext().sub_areas[0]);
    rt->verifyIsValidObject(arg, rt->getContext().sub_areas[1]);

    if (!rt->isOfType(arg, rt->integer_type)) {
        return rt->protected_false;
    }

    if (rt->isInstanceObject(self, rt->integer_type)) {
        if (!rt->isInstanceObject(arg, rt->integer_type)) {
            return rt->protected_false;
        }
        return rt->protectedBoolean(getIntegerValueFast(self) == getIntegerValueFast(arg));
    }
    else if (rt->isTypeObject(self, rt->integer_type)) {
        if (!rt->isTypeObject(arg, rt->integer_type)) {
            return rt->protected_false;
        }
        return rt->protected_true;
    }

    return rt->protected_false;
}

static Object *IntegerNeqAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    auto res = IntegerEqAdapter(self, arg, rt, execution_result_matters);
    return (!getBooleanValueFast(res)) ? rt->protected_true : rt->protected_false;
}

static Object *IntegerBitandAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyIsInstanceObject(self, rt->integer_type, rt->getContext().sub_areas[0]);
    rt->verifyIsInstanceObject(arg, rt->integer_type, rt->getContext().sub_areas[1]);

    if (!execution_result_matters) {
        return NULL;
    }

    auto res = makeIntegerInstanceObject(getIntegerValueFast(self) & getIntegerValueFast(arg), rt);
    return res;
}

static Object *IntegerBitxorAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyIsInstanceObject(self, rt->integer_type, rt->getContext().sub_areas[0]);
    rt->verifyIsInstanceObject(arg, rt->integer_type, rt->getContext().sub_areas[1]);

    if (!execution_result_matters) {
        return NULL;
    }

    auto res = makeIntegerInstanceObject(getIntegerValueFast(self) ^ getIntegerValueFast(arg), rt);
    return res;
}

static Object *IntegerBitorAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyIsInstanceObject(self, rt->integer_type, rt->getContext().sub_areas[0]);
    rt->verifyIsInstanceObject(arg, rt->integer_type, rt->getContext().sub_areas[1]);

    if (!execution_result_matters) {
        return NULL;
    }

    auto res = makeIntegerInstanceObject(getIntegerValueFast(self) | getIntegerValueFast(arg), rt);
    return res;
}

static Object *mm__bool__(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];
    rt->verifyIsInstanceObject(self, rt->integer_type, rt->getContext().sub_areas[1]);

    if (!execution_result_matters) {
        return self;
    }

    return rt->protectedBoolean(getIntegerValueFast(self));
}

static Object *mm__char__(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];
    rt->verifyIsInstanceObject(self, rt->integer_type, rt->getContext().sub_areas[1]);

    if (!execution_result_matters) {
        return self;
    }

    return makeCharacterInstanceObject(getIntegerValueFast(self), rt);
}

static Object *mm__int__(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];
    rt->verifyIsInstanceObject(self, rt->integer_type, rt->getContext().sub_areas[1]);

    if (!execution_result_matters) {
        return self;
    }

    return makeIntegerInstanceObject(getIntegerValueFast(self), rt);
}

static Object *mm__real__(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];
    rt->verifyIsInstanceObject(self, rt->integer_type, rt->getContext().sub_areas[1]);

    if (!execution_result_matters) {
        return self;
    }

    return makeRealInstanceObject(getIntegerValueFast(self), rt);
}

static Object *mm__string__(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];
    rt->verifyIsInstanceObject(self, rt->integer_type, rt->getContext().sub_areas[1]);

    if (!execution_result_matters) {
        return self;
    }

    return makeStringInstanceObject(std::to_string(getIntegerValueFast(self)), rt);
}

static Object *mm__repr__(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];
    rt->verifyIsOfType(self, rt->integer_type, rt->getContext().sub_areas[1]);

    if (!execution_result_matters) {
        return self;
    }

    if (rt->isTypeObject(self, NULL)) {
        return makeStringInstanceObject("Integer", rt);
    }

    return makeStringInstanceObject(std::to_string(getIntegerValueFast(self)), rt);
}

static Object *mm__read__(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];
    rt->verifyIsOfType(self, rt->integer_type, rt->getContext().sub_areas[1]);

    int64_t v;
    std::cin >> v;

    if (!execution_result_matters) {
        return self;
    }

    return makeIntegerInstanceObject(v, rt);
}

void installIntegerMethods(Type *type, Runtime *rt) {
    type->addMethod(MagicMethods::mm__bool__(rt), Builtin::makeFunctionInstanceObject(true, mm__bool__, NULL, rt));
    type->addMethod(MagicMethods::mm__char__(rt), Builtin::makeFunctionInstanceObject(true, mm__char__, NULL, rt));
    type->addMethod(MagicMethods::mm__int__(rt), Builtin::makeFunctionInstanceObject(true, mm__int__, NULL, rt));
    type->addMethod(MagicMethods::mm__real__(rt), Builtin::makeFunctionInstanceObject(true, mm__real__, NULL, rt));
    type->addMethod(MagicMethods::mm__string__(rt),
                    Builtin::makeFunctionInstanceObject(true, mm__string__, NULL, rt));
    type->addMethod(MagicMethods::mm__repr__(rt), Builtin::makeFunctionInstanceObject(true, mm__repr__, NULL, rt));
    type->addMethod(MagicMethods::mm__read__(rt), Builtin::makeFunctionInstanceObject(true, mm__read__, NULL, rt));
}

IntegerType::IntegerType(Runtime *rt)
    : Type(rt) {
    ProfilerCAPTURE();
    this->postinc_op  = IntegerPostincAdapter;
    this->postdec_op  = IntegerPostdecAdapter;
    this->preinc_op   = IntegerPreincAdapter;
    this->predec_op   = IntegerPredecAdapter;
    this->positive_op = IntegerPositiveAdapter;
    this->negative_op = IntegerNegativeAdapter;
    this->inverse_op  = IntegerInverseAdapter;
    this->mult_op     = IntegerMultAdapter;
    this->div_op      = IntegerDivAdapter;
    this->rem_op      = IntegerRemAdapter;
    this->rshift_op   = IntegerRshiftAdapter;
    this->lshift_op   = IntegerLshiftAdapter;
    this->add_op      = IntegerAddAdapter;
    this->sub_op      = IntegerSubAdapter;
    this->lt_op       = IntegerLtAdapter;
    this->leq_op      = IntegerLeqAdapter;
    this->gt_op       = IntegerGtAdapter;
    this->geq_op      = IntegerGeqAdapter;
    this->eq_op       = IntegerEqAdapter;
    this->neq_op      = IntegerNeqAdapter;
    this->bitand_op   = IntegerBitandAdapter;
    this->bitxor_op   = IntegerBitxorAdapter;
    this->bitor_op    = IntegerBitorAdapter;
}

Object *IntegerType::create(Runtime *rt) {
    ProfilerCAPTURE();
    Instance *ins = new IntegerInstance(rt);
    Object   *obj = newObject(true, ins, this, rt);
    return obj;
}

Object *IntegerType::copy(Object *obj, Runtime *rt) {
    ProfilerCAPTURE();
    rt->verifyIsOfType(obj, rt->integer_type);
    if (obj->instance == NULL) {
        return newObject(false, NULL, this, rt);
    }
    auto ins = obj->instance->copy(rt);
    auto res = newObject(true, ins, this, rt);
    return res;
}

std::string IntegerType::userRepr(Runtime *rt) {
    ProfilerCAPTURE();
    if (this == NULL) {
        return "IntegerType(NULL)";
    }
    return "IntegerType";
}

int64_t &getIntegerValue(Object *obj, Runtime *rt) {
    ProfilerCAPTURE();
    rt->verifyIsInstanceObject(obj, rt->integer_type);
    return icast(obj->instance, IntegerInstance)->value;
}

int64_t &getIntegerValue(Object *obj, Runtime *rt, const TextArea &ta) {
    ProfilerCAPTURE();
    rt->verifyIsInstanceObject(obj, rt->integer_type, ta);
    return icast(obj->instance, IntegerInstance)->value;
}

Object *makeIntegerInstanceObject(int64_t value, Runtime *rt) {
    ProfilerCAPTURE();
    auto res                                     = rt->make(rt->integer_type, Runtime::INSTANCE_OBJECT);
    icast(res->instance, IntegerInstance)->value = value;
    return res;
}

}    // namespace Cotton::Builtin
