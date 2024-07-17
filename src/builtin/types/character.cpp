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
    Instance *res = new (rt->alloc(sizeof(CharacterInstance))) CharacterInstance(rt);
    if (res == NULL) {
        rt->signalError("Failed to copy " + this->userRepr(), rt->getContext().area);
    }
    icast(res, CharacterInstance)->value = this->value;
    return res;
}

static std::string charToString(char c) {
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

std::string CharacterInstance::userRepr() {
    ProfilerCAPTURE();
    if (this == NULL) {
        return "Character(NULL)";
    }
    return std::string("Character(value = '") + charToString(this->value) + "')";
}

void CharacterInstance::destroy(Runtime *rt) {
    rt->dealloc(this, sizeof(CharacterInstance));
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

    rt->verifyIsInstanceObject(self, rt->character_type, rt->getContext().sub_areas[0]);

    if (!execution_result_matters) {
        getCharacterValueFast(self)++;
        return NULL;
    }

    auto res = self->type->copy(self, rt);
    getCharacterValueFast(self)++;
    return res;
}

static Object *CharacterPostdecAdapter(Object *self, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();

    rt->verifyIsInstanceObject(self, rt->character_type, rt->getContext().sub_areas[0]);

    if (!execution_result_matters) {
        getCharacterValueFast(self)--;
        return NULL;
    }

    auto res = self->type->copy(self, rt);
    getCharacterValueFast(self)--;
    return res;
}

static Object *CharacterPreincAdapter(Object *self, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();

    rt->verifyIsInstanceObject(self, rt->character_type, rt->getContext().sub_areas[0]);

    if (!execution_result_matters) {
        getCharacterValueFast(self)++;
        return NULL;
    }

    getCharacterValueFast(self)++;
    auto res = self->type->copy(self, rt);
    return res;
}

static Object *CharacterPredecAdapter(Object *self, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();

    rt->verifyIsInstanceObject(self, rt->character_type, rt->getContext().sub_areas[0]);

    if (!execution_result_matters) {
        getCharacterValueFast(self)--;
        return NULL;
    }

    getCharacterValueFast(self)--;
    auto res = self->type->copy(self, rt);
    return res;
}

static Object *CharacterPositiveAdapter(Object *self, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();

    rt->verifyIsInstanceObject(self, rt->character_type, rt->getContext().sub_areas[0]);

    if (!execution_result_matters) {
        return NULL;
    }

    auto res = self->type->copy(self, rt);
    return res;
}

static Object *CharacterNegativeAdapter(Object *self, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();

    rt->verifyIsInstanceObject(self, rt->character_type, rt->getContext().sub_areas[0]);

    if (!execution_result_matters) {
        return NULL;
    }

    auto res                    = self->type->copy(self, rt);
    getCharacterValueFast(res) *= -1;
    return res;
}

static Object *CharacterAddAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();

    rt->verifyIsInstanceObject(self, rt->character_type, rt->getContext().sub_areas[0]);
    rt->verifyIsInstanceObject(arg, rt->character_type, rt->getContext().sub_areas[1]);

    if (!execution_result_matters) {
        return NULL;
    }

    auto res = makeCharacterInstanceObject(getCharacterValueFast(self) + getCharacterValueFast(arg), rt);
    return res;
}

static Object *CharacterSubAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    rt->verifyIsInstanceObject(self, rt->character_type, rt->getContext().sub_areas[0]);
    rt->verifyIsInstanceObject(arg, rt->character_type, rt->getContext().sub_areas[1]);

    if (!execution_result_matters) {
        return NULL;
    }

    auto res = makeCharacterInstanceObject(getCharacterValueFast(self) - getCharacterValueFast(arg), rt);
    return res;
}

static Object *CharacterLtAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();

    rt->verifyIsInstanceObject(self, rt->character_type, rt->getContext().sub_areas[0]);
    rt->verifyIsInstanceObject(arg, rt->character_type, rt->getContext().sub_areas[1]);

    return rt->protectedBoolean(getCharacterValueFast(self) < getCharacterValueFast(arg));
}

static Object *CharacterLeqAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();

    rt->verifyIsInstanceObject(self, rt->character_type, rt->getContext().sub_areas[0]);
    rt->verifyIsInstanceObject(arg, rt->character_type, rt->getContext().sub_areas[1]);

    return rt->protectedBoolean(getCharacterValueFast(self) <= getCharacterValueFast(arg));
}

static Object *CharacterGtAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();

    rt->verifyIsInstanceObject(self, rt->character_type, rt->getContext().sub_areas[0]);
    rt->verifyIsInstanceObject(arg, rt->character_type, rt->getContext().sub_areas[1]);

    return rt->protectedBoolean(getCharacterValueFast(self) > getCharacterValueFast(arg));
}

static Object *CharacterGeqAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();

    rt->verifyIsInstanceObject(self, rt->character_type, rt->getContext().sub_areas[0]);
    rt->verifyIsInstanceObject(arg, rt->character_type, rt->getContext().sub_areas[1]);

    return rt->protectedBoolean(getCharacterValueFast(self) >= getCharacterValueFast(arg));
}

static Object *CharacterEqAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyIsOfType(self, rt->character_type, rt->getContext().sub_areas[0]);
    rt->verifyIsValidObject(arg, rt->getContext().sub_areas[1]);

    if (!rt->isOfType(arg, rt->character_type)) {
        return rt->protected_false;
    }

    if (rt->isInstanceObject(self, rt->character_type)) {
        if (!rt->isInstanceObject(arg, rt->character_type)) {
            return rt->protected_false;
        }
        return rt->protectedBoolean(getCharacterValueFast(self) == getCharacterValueFast(arg));
    }
    else if (rt->isTypeObject(self, rt->character_type)) {
        if (!rt->isTypeObject(arg, rt->character_type)) {
            return rt->protected_false;
        }
        return rt->protected_true;
    }

    return rt->protected_false;
}

static Object *CharacterNeqAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    auto res = CharacterEqAdapter(self, arg, rt, execution_result_matters);
    return (!getBooleanValueFast(res)) ? rt->protected_true : rt->protected_false;
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
    Instance *ins = new (rt->alloc(sizeof(CharacterInstance))) CharacterInstance(rt);
    Object   *obj = newObject(true, ins, this, rt);
    return obj;
}

Object *CharacterType::copy(Object *obj, Runtime *rt) {
    ProfilerCAPTURE();
    rt->verifyIsOfType(obj, rt->character_type);
    if (obj->instance == NULL) {
        return newObject(false, NULL, this, rt);
    }
    auto ins = obj->instance->copy(rt);
    auto res = newObject(true, ins, this, rt);
    return res;
}

std::string CharacterType::userRepr() {
    ProfilerCAPTURE();
    if (this == NULL) {
        return "CharacterType(NULL)";
    }
    return "CharacterType";
}

uint8_t &getCharacterValue(Object *obj, Runtime *rt) {
    ProfilerCAPTURE();
    rt->verifyIsInstanceObject(obj, rt->character_type, rt->getContext().area);
    return icast(obj->instance, CharacterInstance)->value;
}

uint8_t &getCharacterValue(Object *obj, Runtime *rt, const TextArea &ta) {
    ProfilerCAPTURE();
    rt->verifyIsInstanceObject(obj, rt->character_type, ta);
    return icast(obj->instance, CharacterInstance)->value;
}

Object *makeCharacterInstanceObject(uint8_t value, Runtime *rt) {
    ProfilerCAPTURE();
    auto res                                       = rt->make(rt->character_type, Runtime::INSTANCE_OBJECT);
    icast(res->instance, CharacterInstance)->value = value;
    return res;
}

}    // namespace Cotton::Builtin
