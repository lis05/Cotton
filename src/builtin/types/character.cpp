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
        rt->signalError("Failed to copy " + this->shortRepr());
    }
    icast(res, CharacterInstance)->value = this->value;
    return res;
}

std::string CharacterInstance::shortRepr() {
    ProfilerCAPTURE();
    if (this == NULL) {
        return "CharacterInstance(NULL)";
    }
    return "CharacterInstance(id = " + std::to_string(this->id) + ", value = " + std::to_string(this->value)
           + ": '" + (char)this->value + "')";
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

static Object *CharacterPostincAdapter(Object *self, Runtime *rt) {
    ProfilerCAPTURE();

    if (!isInstanceObject(self)) {
        rt->signalError(self->shortRepr() + " does not support that operator");
    }

    auto res = self->type->copy(self, rt);
    getCharacterValueFast(self)++;
    return res;
}

static Object *CharacterPostdecAdapter(Object *self, Runtime *rt) {
    ProfilerCAPTURE();

    if (!isInstanceObject(self)) {
        rt->signalError(self->shortRepr() + " does not support that operator");
    }

    auto res = self->type->copy(self, rt);
    getCharacterValueFast(self)--;
    return res;
}

static Object *CharacterPreincAdapter(Object *self, Runtime *rt) {
    ProfilerCAPTURE();

    if (!isInstanceObject(self)) {
        rt->signalError(self->shortRepr() + " does not support that operator");
    }

    getCharacterValueFast(self)++;
    auto res = self->type->copy(self, rt);
    return res;
}

static Object *CharacterPredecAdapter(Object *self, Runtime *rt) {
    ProfilerCAPTURE();

    if (!isInstanceObject(self)) {
        rt->signalError(self->shortRepr() + " does not support that operator");
    }

    getCharacterValueFast(self)--;
    auto res = self->type->copy(self, rt);
    return res;
}

static Object *CharacterPositiveAdapter(Object *self, Runtime *rt) {
    ProfilerCAPTURE();

    if (!isInstanceObject(self)) {
        rt->signalError(self->shortRepr() + " does not support that operator");
    }

    auto res = self->type->copy(self, rt);
    return res;
}

static Object *CharacterNegativeAdapter(Object *self, Runtime *rt) {
    ProfilerCAPTURE();

    if (!isInstanceObject(self)) {
        rt->signalError(self->shortRepr() + " does not support that operator");
    }

    auto res                    = self->type->copy(self, rt);
    getCharacterValueFast(res) *= -1;
    return res;
}

static Object *CharacterAddAdapter(Object *self, Object *arg, Runtime *rt) {
    ProfilerCAPTURE();

    if (!isInstanceObject(self)) {
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
    if (!isTypeObject(arg)) {
        rt->signalError("Right-side object is invalid: " + arg->shortRepr());
    }
    if (arg->instance == NULL || arg->type->id != rt->character_type->id) {
        rt->signalError("Right-side object " + arg->shortRepr() + " must be a Character instance object");
    }

    auto res = makeCharacterInstanceObject(getCharacterValueFast(self) + getCharacterValueFast(arg), rt);
    return res;
}

static Object *CharacterSubAdapter(Object *self, Object *arg, Runtime *rt) {
    ProfilerCAPTURE();

    if (!isInstanceObject(self)) {
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
    if (!isTypeObject(arg)) {
        rt->signalError("Right-side object is invalid: " + arg->shortRepr());
    }
    if (arg->instance == NULL || arg->type->id != rt->character_type->id) {
        rt->signalError("Right-side object " + arg->shortRepr() + " must be a Character instance object");
    }

    auto res = makeCharacterInstanceObject(getCharacterValueFast(self) - getCharacterValueFast(arg), rt);
    return res;
}

static Object *CharacterLtAdapter(Object *self, Object *arg, Runtime *rt) {
    ProfilerCAPTURE();

    if (!isInstanceObject(self)) {
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
    if (!isTypeObject(arg)) {
        rt->signalError("Right-side object is invalid: " + arg->shortRepr());
    }
    if (arg->instance == NULL || arg->type->id != rt->character_type->id) {
        rt->signalError("Right-side object " + arg->shortRepr() + " must be a Character instance object");
    }

    return (getCharacterValueFast(self) < getCharacterValueFast(arg)) ? rt->protected_true : rt->protected_false;
}

static Object *CharacterLeqAdapter(Object *self, Object *arg, Runtime *rt) {
    ProfilerCAPTURE();

    if (!isInstanceObject(self)) {
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
    if (!isTypeObject(arg)) {
        rt->signalError("Right-side object is invalid: " + arg->shortRepr());
    }
    if (arg->instance == NULL || arg->type->id != rt->character_type->id) {
        rt->signalError("Right-side object " + arg->shortRepr() + " must be a Character instance object");
    }

    return (getCharacterValueFast(self) <= getCharacterValueFast(arg)) ? rt->protected_true : rt->protected_false;
}

static Object *CharacterGtAdapter(Object *self, Object *arg, Runtime *rt) {
    ProfilerCAPTURE();

    if (!isInstanceObject(self)) {
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
    if (!isTypeObject(arg)) {
        rt->signalError("Right-side object is invalid: " + arg->shortRepr());
    }
    if (arg->instance == NULL || arg->type->id != rt->character_type->id) {
        rt->signalError("Right-side object " + arg->shortRepr() + " must be a Character instance object");
    }

    return (getCharacterValueFast(self) > getCharacterValueFast(arg)) ? rt->protected_true : rt->protected_false;
}

static Object *CharacterGeqAdapter(Object *self, Object *arg, Runtime *rt) {
    ProfilerCAPTURE();

    if (!isInstanceObject(self)) {
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
    if (!isTypeObject(arg)) {
        rt->signalError("Right-side object is invalid: " + arg->shortRepr());
    }
    if (arg->instance == NULL || arg->type->id != rt->character_type->id) {
        rt->signalError("Right-side object " + arg->shortRepr() + " must be a Character instance object");
    }

    return (getCharacterValueFast(self) >= getCharacterValueFast(arg)) ? rt->protected_true : rt->protected_false;
}

static Object *CharacterEqAdapter(Object *self, Object *arg, Runtime *rt) {
    ProfilerCAPTURE();
    if (!isTypeObject(arg)) {
        rt->signalError("Right-side object is invalid: " + arg->shortRepr());
    }
    bool i1 = isInstanceObject(self);
    bool i2 = arg->instance != NULL;

    if (i1 && i2) {
        if (self->type->id != arg->type->id) {
            return rt->protected_false;
        }
        return (getCharacterValueFast(self) == getCharacterValueFast(arg)) ? rt->protected_true
                                                                           : rt->protected_false;
    }
    else if (!i1 && !i2) {
        return (self->type->id == arg->type->id) ? rt->protected_true : rt->protected_false;
    }
    else {
        return rt->protected_false;
    }
}

static Object *CharacterNeqAdapter(Object *self, Object *arg, Runtime *rt) {
    ProfilerCAPTURE();
    auto res = CharacterEqAdapter(self, arg, rt);
    return (!getBooleanValueFast(res)) ? rt->protected_true : rt->protected_false;
}

// TODO: add all operators to function and nothing
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
    if (!isTypeObject(obj) || obj->type->id != rt->character_type->id) {
        rt->signalError("Failed to copy an invalid object: " + obj->shortRepr());
    }
    if (obj->instance == NULL) {
        return newObject(false, NULL, this, rt);
    }
    auto ins = obj->instance->copy(rt);
    auto res = newObject(true, ins, this, rt);
    return res;
}

std::string CharacterType::shortRepr() {
    ProfilerCAPTURE();
    if (this == NULL) {
        return "CharacterType(NULL)";
    }
    return "CharacterType(id = " + std::to_string(this->id) + ")";
}

uint8_t &getCharacterValue(Object *obj, Runtime *rt) {
    ProfilerCAPTURE();
    if (!isInstanceObject(obj)) {
        rt->signalError(obj->shortRepr() + " is not an instance object");
    }
    if (obj->type->id != rt->character_type->id) {
        rt->signalError(obj->shortRepr() + " is not Character");
    }
    return icast(obj->instance, CharacterInstance)->value;
}

Object *makeCharacterInstanceObject(uint8_t value, Runtime *rt) {
    ProfilerCAPTURE();
    auto res                                       = rt->make(rt->character_type, Runtime::INSTANCE_OBJECT);
    icast(res->instance, CharacterInstance)->value = value;
    return res;
}

}    // namespace Cotton::Builtin
