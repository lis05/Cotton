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

#include "character.h"
#include "../back/type.h"
#include "boolean.h"

namespace Cotton::Builtin {
CharacterInstance::CharacterInstance(Runtime *rt, bool on_stack)
    : Instance(rt, sizeof(CharacterInstance), on_stack) {
    this->value = '\0';
}

CharacterInstance::~CharacterInstance() {}

Instance *CharacterInstance::copy(Runtime *rt) {
    Instance *res = rt->stack->allocAndInitInstance<CharacterInstance>(sizeof(CharacterInstance), rt);
    if (res != NULL) {
        res->on_stack                      = true;
        icast(res, CharacterInstance)->value = this->value;
        return res;
    }
    res = new (std::nothrow) CharacterInstance(rt, false);
    if (res == NULL) {
        rt->signalError("Failed to copy " + this->shortRepr());
    }
    icast(res, CharacterInstance)->value = this->value;
    res->on_stack                      = false;
    return res;
}

std::string CharacterInstance::shortRepr() {
    if (this == NULL) {
        return "CharacterInstance(NULL)";
    }
    return "CharacterInstance(id = " + std::to_string(this->id) + ", value = " + std::to_string(this->value) + ": '" + (char)this->value + "')";
}

size_t CharacterInstance::getSize() {
    return sizeof(CharacterInstance);
}

size_t CharacterType::getInstanceSize() {
    return sizeof(CharacterInstance);
}

class CharacterPostincAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->character_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        if (!rt->isInstanceObject(self)) {
            rt->signalError(self->shortRepr() + " does not support that operator");
        }

        auto res = self->type->copy(self, rt);
        getCharacterValueFast(self)++;
        return res;
    }
};

class CharacterPostdecAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->character_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        if (!rt->isInstanceObject(self)) {
            rt->signalError(self->shortRepr() + " does not support that operator");
        }

        auto res = self->type->copy(self, rt);
        getCharacterValueFast(self)--;
        return res;
    }
};

class CharacterCallAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->character_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }

        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class CharacterIndexAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->character_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }

        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class CharacterPreincAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->character_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        if (!rt->isInstanceObject(self)) {
            rt->signalError(self->shortRepr() + " does not support that operator");
        }

        getCharacterValueFast(self)++;
        auto res = self->type->copy(self, rt);
        return res;
    }
};

class CharacterPredecAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->character_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        if (!rt->isInstanceObject(self)) {
            rt->signalError(self->shortRepr() + " does not support that operator");
        }

        getCharacterValueFast(self)--;
        auto res = self->type->copy(self, rt);
        return res;
    }
};

class CharacterPositiveAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->character_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        if (!rt->isInstanceObject(self)) {
            rt->signalError(self->shortRepr() + " does not support that operator");
        }

        auto res = self->type->copy(self, rt);
        return res;
    }
};

class CharacterNegativeAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->character_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        if (!rt->isInstanceObject(self)) {
            rt->signalError(self->shortRepr() + " does not support that operator");
        }

        auto res                  = self->type->copy(self, rt);
        getCharacterValueFast(res) *= -1;
        return res;
    }
};

class CharacterNotAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->character_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class CharacterInverseAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->character_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        if (!rt->isInstanceObject(self)) {
            rt->signalError(self->shortRepr() + " does not support that operator");
        }

        auto res                 = self->type->copy(self, rt);
        getCharacterValueFast(res) = ~getCharacterValueFast(res);
        return res;
    }
};

class CharacterMultAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->character_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        if (!rt->isInstanceObject(self)) {
            rt->signalError(self->shortRepr() + " does not support that operator");
        }
        if (others.size() != 1) {
            rt->signalError("Expected exactly one right-side argument");
            return NULL;
        }
        auto &arg1 = others[0];
        if (!rt->isTypeObject(arg1)) {
            rt->signalError("Right-side object is invalid: " + arg1->shortRepr());
        }
        if (!rt->isInstanceObject(arg1) || arg1->type->id != rt->character_type->id) {
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be a Character instance object");
        }

        auto res = makeCharacterInstanceObject(getCharacterValueFast(self) * getCharacterValueFast(arg1), rt);
        return res;
    }
};

class CharacterDivAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->character_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        if (!rt->isInstanceObject(self)) {
            rt->signalError(self->shortRepr() + " does not support that operator");
        }
        if (others.size() != 1) {
            rt->signalError("Expected exactly one right-side argument");
            return NULL;
        }
        auto &arg1 = others[0];
        if (!rt->isTypeObject(arg1)) {
            rt->signalError("Right-side object is invalid: " + arg1->shortRepr());
        }
        if (!rt->isInstanceObject(arg1) || arg1->type->id != rt->character_type->id) {
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be a Character instance object");
        }

        auto res = makeCharacterInstanceObject(getCharacterValueFast(self) / getCharacterValueFast(arg1), rt);
        return res;
    }
};

class CharacterRemAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->character_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        if (!rt->isInstanceObject(self)) {
            rt->signalError(self->shortRepr() + " does not support that operator");
        }
        if (others.size() != 1) {
            rt->signalError("Expected exactly one right-side argument");
            return NULL;
        }
        auto &arg1 = others[0];
        if (!rt->isTypeObject(arg1)) {
            rt->signalError("Right-side object is invalid: " + arg1->shortRepr());
        }
        if (!rt->isInstanceObject(arg1) || arg1->type->id != rt->character_type->id) {
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be a Character instance object");
        }

        auto res = makeCharacterInstanceObject(getCharacterValueFast(self) % getCharacterValueFast(arg1), rt);
        return res;
    }
};

class CharacterRshiftAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->character_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        if (!rt->isInstanceObject(self)) {
            rt->signalError(self->shortRepr() + " does not support that operator");
        }
        if (others.size() != 1) {
            rt->signalError("Expected exactly one right-side argument");
            return NULL;
        }
        auto &arg1 = others[0];
        if (!rt->isTypeObject(arg1)) {
            rt->signalError("Right-side object is invalid: " + arg1->shortRepr());
        }
        if (!rt->isInstanceObject(arg1) || arg1->type->id != rt->character_type->id) {
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be a Character instance object");
        }

        auto res = makeCharacterInstanceObject(getCharacterValueFast(self) >> getCharacterValueFast(arg1), rt);
        return res;
    }
};

class CharacterLshiftAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->character_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        if (!rt->isInstanceObject(self)) {
            rt->signalError(self->shortRepr() + " does not support that operator");
        }
        if (others.size() != 1) {
            rt->signalError("Expected exactly one right-side argument");
            return NULL;
        }
        auto &arg1 = others[0];
        if (!rt->isTypeObject(arg1)) {
            rt->signalError("Right-side object is invalid: " + arg1->shortRepr());
        }
        if (!rt->isInstanceObject(arg1) || arg1->type->id != rt->character_type->id) {
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be a Character instance object");
        }

        auto res = makeCharacterInstanceObject(getCharacterValueFast(self) << getCharacterValueFast(arg1), rt);
        return res;
    }
};

class CharacterAddAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->character_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        if (!rt->isInstanceObject(self)) {
            rt->signalError(self->shortRepr() + " does not support that operator");
        }
        if (others.size() != 1) {
            rt->signalError("Expected exactly one right-side argument");
            return NULL;
        }
        auto &arg1 = others[0];
        if (!rt->isTypeObject(arg1)) {
            rt->signalError("Right-side object is invalid: " + arg1->shortRepr());
        }
        if (!rt->isInstanceObject(arg1) || arg1->type->id != rt->character_type->id) {
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be a Character instance object");
        }

        auto res = makeCharacterInstanceObject(getCharacterValueFast(self) + getCharacterValueFast(arg1), rt);
        return res;
    }
};

class CharacterSubAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->character_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        if (!rt->isInstanceObject(self)) {
            rt->signalError(self->shortRepr() + " does not support that operator");
        }
        if (others.size() != 1) {
            rt->signalError("Expected exactly one right-side argument");
            return NULL;
        }
        auto &arg1 = others[0];
        if (!rt->isTypeObject(arg1)) {
            rt->signalError("Right-side object is invalid: " + arg1->shortRepr());
        }
        if (!rt->isInstanceObject(arg1) || arg1->type->id != rt->character_type->id) {
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be a Character instance object");
        }

        auto res = makeCharacterInstanceObject(getCharacterValueFast(self) - getCharacterValueFast(arg1), rt);
        return res;
    }
};

class CharacterLtAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->character_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        if (!rt->isInstanceObject(self)) {
            rt->signalError(self->shortRepr() + " does not support that operator");
        }
        if (others.size() != 1) {
            rt->signalError("Expected exactly one right-side argument");
            return NULL;
        }
        auto &arg1 = others[0];
        if (!rt->isTypeObject(arg1)) {
            rt->signalError("Right-side object is invalid: " + arg1->shortRepr());
        }
        if (!rt->isInstanceObject(arg1) || arg1->type->id != rt->character_type->id) {
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be a Character instance object");
        }

        auto res = makeBooleanInstanceObject(getCharacterValueFast(self) < getCharacterValueFast(arg1), rt);
        return res;
    }
};

class CharacterLeqAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->character_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        if (!rt->isInstanceObject(self)) {
            rt->signalError(self->shortRepr() + " does not support that operator");
        }
        if (others.size() != 1) {
            rt->signalError("Expected exactly one right-side argument");
            return NULL;
        }
        auto &arg1 = others[0];
        if (!rt->isTypeObject(arg1)) {
            rt->signalError("Right-side object is invalid: " + arg1->shortRepr());
        }
        if (!rt->isInstanceObject(arg1) || arg1->type->id != rt->character_type->id) {
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be a Character instance object");
        }

        auto res = makeBooleanInstanceObject(getCharacterValueFast(self) <= getCharacterValueFast(arg1), rt);
        return res;
    }
};

class CharacterGtAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->character_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        if (!rt->isInstanceObject(self)) {
            rt->signalError(self->shortRepr() + " does not support that operator");
        }
        if (others.size() != 1) {
            rt->signalError("Expected exactly one right-side argument");
            return NULL;
        }
        auto &arg1 = others[0];
        if (!rt->isTypeObject(arg1)) {
            rt->signalError("Right-side object is invalid: " + arg1->shortRepr());
        }
        if (!rt->isInstanceObject(arg1) || arg1->type->id != rt->character_type->id) {
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be a Character instance object");
        }

        auto res = makeBooleanInstanceObject(getCharacterValueFast(self) > getCharacterValueFast(arg1), rt);
        return res;
    }
};

class CharacterGeqAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->character_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        if (!rt->isInstanceObject(self)) {
            rt->signalError(self->shortRepr() + " does not support that operator");
        }
        if (others.size() != 1) {
            rt->signalError("Expected exactly one right-side argument");
            return NULL;
        }
        auto &arg1 = others[0];
        if (!rt->isTypeObject(arg1)) {
            rt->signalError("Right-side object is invalid: " + arg1->shortRepr());
        }
        if (!rt->isInstanceObject(arg1) || arg1->type->id != rt->character_type->id) {
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be a Character instance object");
        }

        auto res = makeBooleanInstanceObject(getCharacterValueFast(self) >= getCharacterValueFast(arg1), rt);
        return res;
    }
};

class CharacterEqAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->boolean_type->id) {
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
            if (self->type->id != arg1->type->id) {
                return makeBooleanInstanceObject(false, rt);
            }
            return makeBooleanInstanceObject(getCharacterValueFast(self) == getCharacterValueFast(arg1), rt);
        }
        else if (!rt->isInstanceObject(self) && !rt->isInstanceObject(arg1)) {
            return makeBooleanInstanceObject(self->type->id == arg1->type->id, rt);
        }
        else {
            return makeBooleanInstanceObject(false, rt);
        }
    }
};

class CharacterNeqAdapter: public CharacterEqAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        auto res                 = CharacterEqAdapter::operator()(self, others, rt);
        getCharacterValueFast(res) = !getCharacterValueFast(res);
        return res;
    }
};

class CharacterBitandAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->character_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        if (!rt->isInstanceObject(self)) {
            rt->signalError(self->shortRepr() + " does not support that operator");
        }
        if (others.size() != 1) {
            rt->signalError("Expected exactly one right-side argument");
            return NULL;
        }
        auto &arg1 = others[0];
        if (!rt->isTypeObject(arg1)) {
            rt->signalError("Right-side object is invalid: " + arg1->shortRepr());
        }
        if (!rt->isInstanceObject(arg1) || arg1->type->id != rt->character_type->id) {
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be a Character instance object");
        }

        auto res = makeCharacterInstanceObject(getCharacterValueFast(self) & getCharacterValueFast(arg1), rt);
        return res;
    }
};

class CharacterBitxorAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->character_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        if (!rt->isInstanceObject(self)) {
            rt->signalError(self->shortRepr() + " does not support that operator");
        }
        if (others.size() != 1) {
            rt->signalError("Expected exactly one right-side argument");
            return NULL;
        }
        auto &arg1 = others[0];
        if (!rt->isTypeObject(arg1)) {
            rt->signalError("Right-side object is invalid: " + arg1->shortRepr());
        }
        if (!rt->isInstanceObject(arg1) || arg1->type->id != rt->character_type->id) {
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be a Character instance object");
        }

        auto res = makeCharacterInstanceObject(getCharacterValueFast(self) ^ getCharacterValueFast(arg1), rt);
        return res;
    }
};

class CharacterBitorAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->character_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        if (!rt->isInstanceObject(self)) {
            rt->signalError(self->shortRepr() + " does not support that operator");
        }
        if (others.size() != 1) {
            rt->signalError("Expected exactly one right-side argument");
            return NULL;
        }
        auto &arg1 = others[0];
        if (!rt->isTypeObject(arg1)) {
            rt->signalError("Right-side object is invalid: " + arg1->shortRepr());
        }
        if (!rt->isInstanceObject(arg1) || arg1->type->id != rt->character_type->id) {
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be a Character instance object");
        }

        auto res = makeCharacterInstanceObject(getCharacterValueFast(self) | getCharacterValueFast(arg1), rt);
        return res;
    }
};

class CharacterAndAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->character_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class CharacterOrAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->character_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

// TODO: add all operators to function and nothing
CharacterType::CharacterType(Runtime *rt)
    : Type(true, rt) {
    this->addOperator(OperatorNode::POST_PLUS_PLUS, new CharacterPostincAdapter(), rt);
    this->addOperator(OperatorNode::POST_MINUS_MINUS, new CharacterPostdecAdapter(), rt);
    this->addOperator(OperatorNode::CALL, new CharacterCallAdapter(), rt);
    this->addOperator(OperatorNode::INDEX, new CharacterIndexAdapter(), rt);
    this->addOperator(OperatorNode::PRE_PLUS_PLUS, new CharacterPreincAdapter(), rt);
    this->addOperator(OperatorNode::PRE_MINUS_MINUS, new CharacterPredecAdapter(), rt);
    this->addOperator(OperatorNode::PRE_PLUS, new CharacterPositiveAdapter(), rt);
    this->addOperator(OperatorNode::PRE_MINUS, new CharacterNegativeAdapter(), rt);
    this->addOperator(OperatorNode::NOT, new CharacterNotAdapter(), rt);
    this->addOperator(OperatorNode::INVERSE, new CharacterInverseAdapter(), rt);
    this->addOperator(OperatorNode::MULT, new CharacterMultAdapter(), rt);
    this->addOperator(OperatorNode::DIV, new CharacterDivAdapter(), rt);
    this->addOperator(OperatorNode::REM, new CharacterRemAdapter(), rt);
    this->addOperator(OperatorNode::RIGHT_SHIFT, new CharacterRshiftAdapter(), rt);
    this->addOperator(OperatorNode::LEFT_SHIFT, new CharacterLshiftAdapter(), rt);
    this->addOperator(OperatorNode::PLUS, new CharacterAddAdapter(), rt);
    this->addOperator(OperatorNode::MINUS, new CharacterSubAdapter(), rt);
    this->addOperator(OperatorNode::LESS, new CharacterLtAdapter(), rt);
    this->addOperator(OperatorNode::LESS_EQUAL, new CharacterLeqAdapter(), rt);
    this->addOperator(OperatorNode::GREATER, new CharacterGtAdapter(), rt);
    this->addOperator(OperatorNode::GREATER_EQUAL, new CharacterGeqAdapter(), rt);
    this->addOperator(OperatorNode::EQUAL, new CharacterEqAdapter(), rt);
    this->addOperator(OperatorNode::NOT_EQUAL, new CharacterNeqAdapter(), rt);
    this->addOperator(OperatorNode::BITAND, new CharacterBitandAdapter(), rt);
    this->addOperator(OperatorNode::BITXOR, new CharacterBitxorAdapter(), rt);
    this->addOperator(OperatorNode::BITOR, new CharacterBitorAdapter(), rt);
    this->addOperator(OperatorNode::AND, new CharacterAndAdapter(), rt);
    this->addOperator(OperatorNode::OR, new CharacterOrAdapter(), rt);
}

Object *CharacterType::create(Runtime *rt) {
    auto ins = createInstance(rt, true, CharacterInstance);
    auto obj = createObject(rt, true, ins, this, true);
    return obj;
}

Object *CharacterType::copy(Object *obj, Runtime *rt) {
    if (!rt->isTypeObject(obj) || obj->type->id != rt->character_type->id) {
        rt->signalError("Failed to copy an invalid object: " + obj->shortRepr());
    }
    auto ins = obj->instance->copy(rt);
    auto res = createObject(rt, true, ins, this, true);
    return res;
}

std::string CharacterType::shortRepr() {
    if (this == NULL) {
        return "CharacterType(NULL)";
    }
    return "CharacterType(id = " + std::to_string(this->id) + ")";
}

uint8_t &getCharacterValue(Object *obj, Runtime *rt) {
    if (!rt->isInstanceObject(obj)) {
        rt->signalError(obj->shortRepr() + " is not an instance object");
    }
    if (obj->type->id != rt->character_type->id) {
        rt->signalError(obj->shortRepr() + " is not Character");
    }
    return icast(obj->instance, CharacterInstance)->value;
}

Object *makeCharacterInstanceObject(uint8_t value, Runtime *rt) {
    auto res                                     = rt->make(rt->character_type, Runtime::INSTANCE_OBJECT);
    icast(res->instance, CharacterInstance)->value = value;
    return res;
}

}    // namespace Cotton::Builtin
