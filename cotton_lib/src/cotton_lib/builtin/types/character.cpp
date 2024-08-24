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
CharacterInstance::CharacterInstance(Runtime *rt)
    : Instance(rt, sizeof(CharacterInstance)) {
    ProfilerCAPTURE();
    this->value = '\0';
}

CharacterInstance::~CharacterInstance() {
    ProfilerCAPTURE();
}

Instance *CharacterInstance::copy(Runtime *rt) {
    ProfilerCAPTURE();
    Instance *res = new CharacterInstance(rt);
    if (res == nullptr) {
        rt->signalError("Failed to copy " + this->userRepr(rt), rt->getContext().area);
    }
    icast(res, CharacterInstance)->value = this->value;
    return res;
}

static std::string charToString(char c) {
    ProfilerCAPTURE();
    switch (c) {
    case '\a' : return "\\a";
    case '\b' : return "\\b";
    case '\f' : return "\\f";
    case '\n' : return "\\n";
    case '\r' : return "\\r";
    case '\t' : return "\\t";
    case '\v' : return "\\v";
    case '\\' : return "\\\\";
    case '\"' : return "\\\"";
    case '\'' : return "\\\'";
    }
    return std::string() + c;
}

std::string CharacterInstance::userRepr(Runtime *rt) {
    ProfilerCAPTURE();
    if (this == nullptr) {
        return "Character(nullptr)";
    }
    return std::string("Character(value = '") + charToString(this->value) + "')";
}

size_t CharacterInstance::getSize() {
    ProfilerCAPTURE();
    return sizeof(CharacterInstance);
}

size_t CharacterType::getInstanceSize() {
    ProfilerCAPTURE();
    return sizeof(CharacterInstance);
}

static Object *CharacterPostincAdapter(Object *self, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();

    if (!execution_result_matters) {
        getCharacterValueFast(self)++;
        return nullptr;
    }

    auto res = self->type->copy(self, rt);
    getCharacterValueFast(self)++;
    return res;
}

static Object *CharacterPostdecAdapter(Object *self, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();

    if (!execution_result_matters) {
        getCharacterValueFast(self)--;
        return nullptr;
    }

    auto res = self->type->copy(self, rt);
    getCharacterValueFast(self)--;
    return res;
}

static Object *CharacterPreincAdapter(Object *self, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();

    if (!execution_result_matters) {
        getCharacterValueFast(self)++;
        return nullptr;
    }

    getCharacterValueFast(self)++;
    auto res = self->type->copy(self, rt);
    return res;
}

static Object *CharacterPredecAdapter(Object *self, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();

    if (!execution_result_matters) {
        getCharacterValueFast(self)--;
        return nullptr;
    }

    getCharacterValueFast(self)--;
    auto res = self->type->copy(self, rt);
    return res;
}

static Object *CharacterPositiveAdapter(Object *self, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();

    if (!execution_result_matters) {
        return nullptr;
    }

    auto res = self->type->copy(self, rt);
    return res;
}

static Object *CharacterNegativeAdapter(Object *self, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();

    if (!execution_result_matters) {
        return nullptr;
    }

    auto res                    = self->type->copy(self, rt);
    getCharacterValueFast(res) *= -1;
    return res;
}

static Object *CharacterAddAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();

    rt->verifyIsInstanceObject(arg, rt->builtin_types.character, Runtime::SUB1_CTX);

    if (!execution_result_matters) {
        return nullptr;
    }

    auto res = makeCharacterInstanceObject(getCharacterValueFast(self) + getCharacterValueFast(arg), rt);
    return res;
}

static Object *CharacterSubAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();

    rt->verifyIsInstanceObject(arg, rt->builtin_types.character, Runtime::SUB1_CTX);

    if (!execution_result_matters) {
        return nullptr;
    }

    auto res = makeCharacterInstanceObject(getCharacterValueFast(self) - getCharacterValueFast(arg), rt);
    return res;
}

static Object *CharacterLtAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();

    rt->verifyIsInstanceObject(arg, rt->builtin_types.character, Runtime::SUB1_CTX);

    return rt->protectedBoolean(getCharacterValueFast(self) < getCharacterValueFast(arg));
}

static Object *CharacterLeqAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();

    rt->verifyIsInstanceObject(arg, rt->builtin_types.character, Runtime::SUB1_CTX);

    return rt->protectedBoolean(getCharacterValueFast(self) <= getCharacterValueFast(arg));
}

static Object *CharacterGtAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();

    rt->verifyIsInstanceObject(arg, rt->builtin_types.character, Runtime::SUB1_CTX);

    return rt->protectedBoolean(getCharacterValueFast(self) > getCharacterValueFast(arg));
}

static Object *CharacterGeqAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();

    rt->verifyIsInstanceObject(arg, rt->builtin_types.character, Runtime::SUB1_CTX);

    return rt->protectedBoolean(getCharacterValueFast(self) >= getCharacterValueFast(arg));
}

static Object *CharacterEqAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyIsOfType(self, rt->builtin_types.character, Runtime::SUB0_CTX);
    rt->verifyIsValidObject(arg, Runtime::SUB1_CTX);

    if (!rt->isOfType(arg, rt->builtin_types.character)) {
        return rt->protectedBoolean(false);
    }

    if (rt->isInstanceObject(self, rt->builtin_types.character)) {
        if (!rt->isInstanceObject(arg, rt->builtin_types.character)) {
            return rt->protectedBoolean(false);
        }
        return rt->protectedBoolean(getCharacterValueFast(self) == getCharacterValueFast(arg));
    }
    else if (rt->isTypeObject(self, rt->builtin_types.character)) {
        if (!rt->isTypeObject(arg, rt->builtin_types.character)) {
            return rt->protectedBoolean(false);
        }
        return rt->protectedBoolean(true);
    }

    return rt->protectedBoolean(false);
}

static Object *CharacterNeqAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    auto res = CharacterEqAdapter(self, arg, rt, execution_result_matters);
    return rt->protectedBoolean(!getBooleanValueFast(res));
}

static Object *character_mm__bool__(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];

    if (!execution_result_matters) {
        return self;
    }

    return makeBooleanInstanceObject(getCharacterValueFast(self) != '0', rt);
}

static Object *character_mm__char__(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];

    if (!execution_result_matters) {
        return self;
    }

    return makeCharacterInstanceObject(getCharacterValueFast(self), rt);
}

static Object *character_mm__int__(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];

    if (!execution_result_matters) {
        return self;
    }

    return makeIntegerInstanceObject(getCharacterValueFast(self), rt);
}

static Object *character_mm__real__(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];

    if (!execution_result_matters) {
        return self;
    }

    return makeRealInstanceObject(getCharacterValueFast(self), rt);
}

static Object *character_mm__string__(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];
    rt->verifyIsOfType(self, rt->builtin_types.character, Runtime::SUB1_CTX);

    if (!execution_result_matters) {
        return self;
    }

    if (rt->isTypeObject(self, nullptr)) {
        return makeStringInstanceObject("Character", rt);
    }

    return makeStringInstanceObject(std::string() + (char)getCharacterValueFast(self), rt);
}

static Object *character_mm__repr__(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];
    rt->verifyIsOfType(self, rt->builtin_types.character, Runtime::SUB1_CTX);

    if (!execution_result_matters) {
        return self;
    }

    if (rt->isTypeObject(self, nullptr)) {
        return makeStringInstanceObject("Character", rt);
    }

    return makeStringInstanceObject(std::string("\'") + (char)getCharacterValueFast(self) + "\'", rt);
}

static Object *character_mm__read__(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];
    rt->verifyIsOfType(self, rt->builtin_types.character, Runtime::SUB1_CTX);

    char c;
    std::cin >> c;

    if (!execution_result_matters) {
        return self;
    }

    return makeCharacterInstanceObject(c, rt);
}

void installCharacterMethods(Type *type, Runtime *rt) {
    ProfilerCAPTURE();
    type->addMethod(MagicMethods::mm__bool__(rt), Builtin::makeFunctionInstanceObject(true, character_mm__bool__, nullptr, rt));
    type->addMethod(MagicMethods::mm__char__(rt), Builtin::makeFunctionInstanceObject(true, character_mm__char__, nullptr, rt));
    type->addMethod(MagicMethods::mm__int__(rt), Builtin::makeFunctionInstanceObject(true, character_mm__int__, nullptr, rt));
    type->addMethod(MagicMethods::mm__real__(rt), Builtin::makeFunctionInstanceObject(true, character_mm__real__, nullptr, rt));
    type->addMethod(MagicMethods::mm__string__(rt), Builtin::makeFunctionInstanceObject(true, character_mm__string__, nullptr, rt));
    type->addMethod(MagicMethods::mm__repr__(rt), Builtin::makeFunctionInstanceObject(true, character_mm__repr__, nullptr, rt));
    type->addMethod(MagicMethods::mm__read__(rt), Builtin::makeFunctionInstanceObject(true, character_mm__read__, nullptr, rt));
}

CharacterType::CharacterType(Runtime *rt)
    : Type(rt) {
    ProfilerCAPTURE();
    this->postinc_op  = CharacterPostincAdapter;
    this->postdec_op  = CharacterPostdecAdapter;
    this->preinc_op   = CharacterPreincAdapter;
    this->predec_op   = CharacterPredecAdapter;
    this->positive_op = CharacterPositiveAdapter;
    this->negative_op = CharacterNegativeAdapter;
    this->add_op      = CharacterAddAdapter;
    this->sub_op      = CharacterSubAdapter;
    this->lt_op       = CharacterLtAdapter;
    this->leq_op      = CharacterLeqAdapter;
    this->gt_op       = CharacterGtAdapter;
    this->geq_op      = CharacterGeqAdapter;
    this->eq_op       = CharacterEqAdapter;
    this->neq_op      = CharacterNeqAdapter;
}

Object *CharacterType::create(Runtime *rt) {
    ProfilerCAPTURE();
    Instance *ins = new CharacterInstance(rt);
    Object   *obj = new Object(true, ins, this, rt);
    return obj;
}

Object *CharacterType::copy(Object *obj, Runtime *rt) {
    ProfilerCAPTURE();
    rt->verifyIsOfType(obj, rt->builtin_types.character);
    if (obj->instance == nullptr) {
        return new Object(false, nullptr, this, rt);
    }
    auto ins = obj->instance->copy(rt);
    auto res = new Object(true, ins, this, rt);
    return res;
}

std::string CharacterType::userRepr(Runtime *rt) {
    ProfilerCAPTURE();
    if (this == nullptr) {
        return "CharacterType(nullptr)";
    }
    return "CharacterType";
}

uint8_t &getCharacterValue(Object *obj, Runtime *rt) {
    ProfilerCAPTURE();
    rt->verifyIsInstanceObject(obj, rt->builtin_types.character);
    return icast(obj->instance, Cotton::Builtin::CharacterInstance)->value;
}

uint8_t &getCharacterValue(Object *obj, Runtime *rt, Runtime::ContextId ctx_id) {
    ProfilerCAPTURE();
    rt->verifyIsInstanceObject(obj, rt->builtin_types.character, Runtime::SUB0_CTX);
    return icast(obj->instance, Cotton::Builtin::CharacterInstance)->value;
}

Object *makeCharacterInstanceObject(uint8_t value, Runtime *rt) {
    ProfilerCAPTURE();
    auto res                                       = rt->make(rt->builtin_types.character, Runtime::INSTANCE_OBJECT);
    icast(res->instance, CharacterInstance)->value = value;
    return res;
}

}    // namespace Cotton::Builtin
