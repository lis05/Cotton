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

static Object *CharacterPostincAdapter(Object *self, const std::vector<Object *> &others, Runtime *rt) {
    ProfilerCAPTURE();

    if (!isInstanceObject(self)) {
        rt->signalError(self->shortRepr() + " does not support that operator");
    }

    auto res = self->type->copy(self, rt);
    getCharacterValueFast(self)++;
    return res;
}

static Object *CharacterPostdecAdapter(Object *self, const std::vector<Object *> &others, Runtime *rt) {
    ProfilerCAPTURE();

    if (!isInstanceObject(self)) {
        rt->signalError(self->shortRepr() + " does not support that operator");
    }

    auto res = self->type->copy(self, rt);
    getCharacterValueFast(self)--;
    return res;
}

static Object *CharacterUnsupportedAdapter(Object *self, const std::vector<Object *> &others, Runtime *rt) {
    ProfilerCAPTURE();
    rt->signalError(self->shortRepr() + " does not support that operator");
}

static Object *CharacterPreincAdapter(Object *self, const std::vector<Object *> &others, Runtime *rt) {
    ProfilerCAPTURE();

    if (!isInstanceObject(self)) {
        rt->signalError(self->shortRepr() + " does not support that operator");
    }

    getCharacterValueFast(self)++;
    auto res = self->type->copy(self, rt);
    return res;
}

static Object *CharacterPredecAdapter(Object *self, const std::vector<Object *> &others, Runtime *rt) {
    ProfilerCAPTURE();

    if (!isInstanceObject(self)) {
        rt->signalError(self->shortRepr() + " does not support that operator");
    }

    getCharacterValueFast(self)--;
    auto res = self->type->copy(self, rt);
    return res;
}

static Object *CharacterPositiveAdapter(Object *self, const std::vector<Object *> &others, Runtime *rt) {
    ProfilerCAPTURE();

    if (!isInstanceObject(self)) {
        rt->signalError(self->shortRepr() + " does not support that operator");
    }

    auto res = self->type->copy(self, rt);
    return res;
}

static Object *CharacterNegativeAdapter(Object *self, const std::vector<Object *> &others, Runtime *rt) {
    ProfilerCAPTURE();

    if (!isInstanceObject(self)) {
        rt->signalError(self->shortRepr() + " does not support that operator");
    }

    auto res                    = self->type->copy(self, rt);
    getCharacterValueFast(res) *= -1;
    return res;
}

static Object *CharacterInverseAdapter(Object *self, const std::vector<Object *> &others, Runtime *rt) {
    ProfilerCAPTURE();

    if (!isInstanceObject(self)) {
        rt->signalError(self->shortRepr() + " does not support that operator");
    }

    auto res                   = self->type->copy(self, rt);
    getCharacterValueFast(res) = ~getCharacterValueFast(res);
    return res;
}

static Object *CharacterMultAdapter(Object *self, const std::vector<Object *> &others, Runtime *rt) {
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
    if (arg1->instance == NULL || arg1->type->id != rt->character_type->id) {
        rt->signalError("Right-side object " + arg1->shortRepr() + " must be a Character instance object");
    }

    auto res = makeCharacterInstanceObject(getCharacterValueFast(self) * getCharacterValueFast(arg1), rt);
    return res;
}

static Object *CharacterDivAdapter(Object *self, const std::vector<Object *> &others, Runtime *rt) {
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
    if (arg1->instance == NULL || arg1->type->id != rt->character_type->id) {
        rt->signalError("Right-side object " + arg1->shortRepr() + " must be a Character instance object");
    }

    auto res = makeCharacterInstanceObject(getCharacterValueFast(self) / getCharacterValueFast(arg1), rt);
    return res;
}

static Object *CharacterRemAdapter(Object *self, const std::vector<Object *> &others, Runtime *rt) {
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
    if (arg1->instance == NULL || arg1->type->id != rt->character_type->id) {
        rt->signalError("Right-side object " + arg1->shortRepr() + " must be a Character instance object");
    }

    auto res = makeCharacterInstanceObject(getCharacterValueFast(self) % getCharacterValueFast(arg1), rt);
    return res;
}

static Object *CharacterRshiftAdapter(Object *self, const std::vector<Object *> &others, Runtime *rt) {
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
    if (arg1->instance == NULL || arg1->type->id != rt->character_type->id) {
        rt->signalError("Right-side object " + arg1->shortRepr() + " must be a Character instance object");
    }

    auto res = makeCharacterInstanceObject(getCharacterValueFast(self) >> getCharacterValueFast(arg1), rt);
    return res;
}

static Object *CharacterLshiftAdapter(Object *self, const std::vector<Object *> &others, Runtime *rt) {
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
    if (arg1->instance == NULL || arg1->type->id != rt->character_type->id) {
        rt->signalError("Right-side object " + arg1->shortRepr() + " must be a Character instance object");
    }

    auto res = makeCharacterInstanceObject(getCharacterValueFast(self) << getCharacterValueFast(arg1), rt);
    return res;
}

static Object *CharacterAddAdapter(Object *self, const std::vector<Object *> &others, Runtime *rt) {
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
    if (arg1->instance == NULL || arg1->type->id != rt->character_type->id) {
        rt->signalError("Right-side object " + arg1->shortRepr() + " must be a Character instance object");
    }

    auto res = makeCharacterInstanceObject(getCharacterValueFast(self) + getCharacterValueFast(arg1), rt);
    return res;
}

static Object *CharacterSubAdapter(Object *self, const std::vector<Object *> &others, Runtime *rt) {
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
    if (arg1->instance == NULL || arg1->type->id != rt->character_type->id) {
        rt->signalError("Right-side object " + arg1->shortRepr() + " must be a Character instance object");
    }

    auto res = makeCharacterInstanceObject(getCharacterValueFast(self) - getCharacterValueFast(arg1), rt);
    return res;
}

static Object *CharacterLtAdapter(Object *self, const std::vector<Object *> &others, Runtime *rt) {
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
    if (arg1->instance == NULL || arg1->type->id != rt->character_type->id) {
        rt->signalError("Right-side object " + arg1->shortRepr() + " must be a Character instance object");
    }

    auto res = makeBooleanInstanceObject(getCharacterValueFast(self) < getCharacterValueFast(arg1), rt);
    return res;
}

static Object *CharacterLeqAdapter(Object *self, const std::vector<Object *> &others, Runtime *rt) {
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
    if (arg1->instance == NULL || arg1->type->id != rt->character_type->id) {
        rt->signalError("Right-side object " + arg1->shortRepr() + " must be a Character instance object");
    }

    auto res = makeBooleanInstanceObject(getCharacterValueFast(self) <= getCharacterValueFast(arg1), rt);
    return res;
}

static Object *CharacterGtAdapter(Object *self, const std::vector<Object *> &others, Runtime *rt) {
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
    if (arg1->instance == NULL || arg1->type->id != rt->character_type->id) {
        rt->signalError("Right-side object " + arg1->shortRepr() + " must be a Character instance object");
    }

    auto res = makeBooleanInstanceObject(getCharacterValueFast(self) > getCharacterValueFast(arg1), rt);
    return res;
}

static Object *CharacterGeqAdapter(Object *self, const std::vector<Object *> &others, Runtime *rt) {
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
    if (arg1->instance == NULL || arg1->type->id != rt->character_type->id) {
        rt->signalError("Right-side object " + arg1->shortRepr() + " must be a Character instance object");
    }

    auto res = makeBooleanInstanceObject(getCharacterValueFast(self) >= getCharacterValueFast(arg1), rt);
    return res;
}

static Object *CharacterEqAdapter(Object *self, const std::vector<Object *> &others, Runtime *rt) {
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
        return makeBooleanInstanceObject(getCharacterValueFast(self) == getCharacterValueFast(arg1), rt);
    }
    else if (!i1 && !i2) {
        return makeBooleanInstanceObject(self->type->id == arg1->type->id, rt);
    }
    else {
        return makeBooleanInstanceObject(false, rt);
    }
}

static Object *CharacterNeqAdapter(Object *self, const std::vector<Object *> &others, Runtime *rt) {
    ProfilerCAPTURE();
    auto res                   = CharacterEqAdapter(self, others, rt);
    getCharacterValueFast(res) = !getCharacterValueFast(res);
    return res;
}

static Object *CharacterBitandAdapter(Object *self, const std::vector<Object *> &others, Runtime *rt) {
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
    if (arg1->instance == NULL || arg1->type->id != rt->character_type->id) {
        rt->signalError("Right-side object " + arg1->shortRepr() + " must be a Character instance object");
    }

    auto res = makeCharacterInstanceObject(getCharacterValueFast(self) & getCharacterValueFast(arg1), rt);
    return res;
}

static Object *CharacterBitxorAdapter(Object *self, const std::vector<Object *> &others, Runtime *rt) {
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
    if (arg1->instance == NULL || arg1->type->id != rt->character_type->id) {
        rt->signalError("Right-side object " + arg1->shortRepr() + " must be a Character instance object");
    }

    auto res = makeCharacterInstanceObject(getCharacterValueFast(self) ^ getCharacterValueFast(arg1), rt);
    return res;
}

static Object *CharacterBitorAdapter(Object *self, const std::vector<Object *> &others, Runtime *rt) {
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
    if (arg1->instance == NULL || arg1->type->id != rt->character_type->id) {
        rt->signalError("Right-side object " + arg1->shortRepr() + " must be a Character instance object");
    }

    auto res = makeCharacterInstanceObject(getCharacterValueFast(self) | getCharacterValueFast(arg1), rt);
    return res;
}

// TODO: add all operators to function and nothing
CharacterType::CharacterType(Runtime *rt)
    : Type(rt) {
    ProfilerCAPTURE();
    this->addOperator(OperatorNode::POST_PLUS_PLUS, CharacterPostincAdapter);
    this->addOperator(OperatorNode::POST_MINUS_MINUS, CharacterPostdecAdapter);
    this->addOperator(OperatorNode::CALL, CharacterUnsupportedAdapter);
    this->addOperator(OperatorNode::INDEX, CharacterUnsupportedAdapter);
    this->addOperator(OperatorNode::PRE_PLUS_PLUS, CharacterPreincAdapter);
    this->addOperator(OperatorNode::PRE_MINUS_MINUS, CharacterPredecAdapter);
    this->addOperator(OperatorNode::PRE_PLUS, CharacterPositiveAdapter);
    this->addOperator(OperatorNode::PRE_MINUS, CharacterNegativeAdapter);
    this->addOperator(OperatorNode::NOT, CharacterUnsupportedAdapter);
    this->addOperator(OperatorNode::INVERSE, CharacterInverseAdapter);
    this->addOperator(OperatorNode::MULT, CharacterMultAdapter);
    this->addOperator(OperatorNode::DIV, CharacterDivAdapter);
    this->addOperator(OperatorNode::REM, CharacterRemAdapter);
    this->addOperator(OperatorNode::RIGHT_SHIFT, CharacterRshiftAdapter);
    this->addOperator(OperatorNode::LEFT_SHIFT, CharacterLshiftAdapter);
    this->addOperator(OperatorNode::PLUS, CharacterAddAdapter);
    this->addOperator(OperatorNode::MINUS, CharacterSubAdapter);
    this->addOperator(OperatorNode::LESS, CharacterLtAdapter);
    this->addOperator(OperatorNode::LESS_EQUAL, CharacterLeqAdapter);
    this->addOperator(OperatorNode::GREATER, CharacterGtAdapter);
    this->addOperator(OperatorNode::GREATER_EQUAL, CharacterGeqAdapter);
    this->addOperator(OperatorNode::EQUAL, CharacterEqAdapter);
    this->addOperator(OperatorNode::NOT_EQUAL, CharacterNeqAdapter);
    this->addOperator(OperatorNode::BITAND, CharacterBitandAdapter);
    this->addOperator(OperatorNode::BITXOR, CharacterBitxorAdapter);
    this->addOperator(OperatorNode::BITOR, CharacterBitorAdapter);
    this->addOperator(OperatorNode::AND, CharacterUnsupportedAdapter);
    this->addOperator(OperatorNode::OR, CharacterUnsupportedAdapter);
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
