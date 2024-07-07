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
    Instance *res = new (std::nothrow) CharacterInstance(rt);
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

size_t CharacterInstance::getSize() {
    ProfilerCAPTURE();
    return sizeof(CharacterInstance);
}

size_t CharacterType::getInstanceSize() {
    ProfilerCAPTURE();
    return sizeof(CharacterInstance);
}

class CharacterPostincAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        ProfilerCAPTURE();

        if (!isInstanceObject(self)) {
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
        ProfilerCAPTURE();

        if (!isInstanceObject(self)) {
            rt->signalError(self->shortRepr() + " does not support that operator");
        }

        auto res = self->type->copy(self, rt);
        getCharacterValueFast(self)--;
        return res;
    }
};

class CharacterUnsupportedAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        ProfilerCAPTURE();
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class CharacterPreincAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        ProfilerCAPTURE();

        if (!isInstanceObject(self)) {
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
        ProfilerCAPTURE();

        if (!isInstanceObject(self)) {
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
        ProfilerCAPTURE();

        if (!isInstanceObject(self)) {
            rt->signalError(self->shortRepr() + " does not support that operator");
        }

        auto res = self->type->copy(self, rt);
        return res;
    }
};

class CharacterNegativeAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        ProfilerCAPTURE();

        if (!isInstanceObject(self)) {
            rt->signalError(self->shortRepr() + " does not support that operator");
        }

        auto res                    = self->type->copy(self, rt);
        getCharacterValueFast(res) *= -1;
        return res;
    }
};

class CharacterInverseAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        ProfilerCAPTURE();

        if (!isInstanceObject(self)) {
            rt->signalError(self->shortRepr() + " does not support that operator");
        }

        auto res                   = self->type->copy(self, rt);
        getCharacterValueFast(res) = ~getCharacterValueFast(res);
        return res;
    }
};

class CharacterMultAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
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
};

class CharacterDivAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
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
};

class CharacterRemAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
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
};

class CharacterRshiftAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
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
};

class CharacterLshiftAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
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
};

class CharacterAddAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
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
};

class CharacterSubAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
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
};

class CharacterLtAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
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
};

class CharacterLeqAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
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
};

class CharacterGtAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
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
};

class CharacterGeqAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
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
};

class CharacterEqAdapter: public OperatorAdapter {
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
};

class CharacterNeqAdapter: public CharacterEqAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        ProfilerCAPTURE();
        auto res                   = CharacterEqAdapter::operator()(self, others, rt);
        getCharacterValueFast(res) = !getCharacterValueFast(res);
        return res;
    }
};

class CharacterBitandAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
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
};

class CharacterBitxorAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
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
};

class CharacterBitorAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
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
};

// TODO: add all operators to function and nothing
CharacterType::CharacterType(Runtime *rt)
    : Type(rt) {
    ProfilerCAPTURE();
    this->addOperator(OperatorNode::POST_PLUS_PLUS, new CharacterPostincAdapter());
    this->addOperator(OperatorNode::POST_MINUS_MINUS, new CharacterPostdecAdapter());
    this->addOperator(OperatorNode::CALL, new CharacterUnsupportedAdapter());
    this->addOperator(OperatorNode::INDEX, new CharacterUnsupportedAdapter());
    this->addOperator(OperatorNode::PRE_PLUS_PLUS, new CharacterPreincAdapter());
    this->addOperator(OperatorNode::PRE_MINUS_MINUS, new CharacterPredecAdapter());
    this->addOperator(OperatorNode::PRE_PLUS, new CharacterPositiveAdapter());
    this->addOperator(OperatorNode::PRE_MINUS, new CharacterNegativeAdapter());
    this->addOperator(OperatorNode::NOT, new CharacterUnsupportedAdapter());
    this->addOperator(OperatorNode::INVERSE, new CharacterInverseAdapter());
    this->addOperator(OperatorNode::MULT, new CharacterMultAdapter());
    this->addOperator(OperatorNode::DIV, new CharacterDivAdapter());
    this->addOperator(OperatorNode::REM, new CharacterRemAdapter());
    this->addOperator(OperatorNode::RIGHT_SHIFT, new CharacterRshiftAdapter());
    this->addOperator(OperatorNode::LEFT_SHIFT, new CharacterLshiftAdapter());
    this->addOperator(OperatorNode::PLUS, new CharacterAddAdapter());
    this->addOperator(OperatorNode::MINUS, new CharacterSubAdapter());
    this->addOperator(OperatorNode::LESS, new CharacterLtAdapter());
    this->addOperator(OperatorNode::LESS_EQUAL, new CharacterLeqAdapter());
    this->addOperator(OperatorNode::GREATER, new CharacterGtAdapter());
    this->addOperator(OperatorNode::GREATER_EQUAL, new CharacterGeqAdapter());
    this->addOperator(OperatorNode::EQUAL, new CharacterEqAdapter());
    this->addOperator(OperatorNode::NOT_EQUAL, new CharacterNeqAdapter());
    this->addOperator(OperatorNode::BITAND, new CharacterBitandAdapter());
    this->addOperator(OperatorNode::BITXOR, new CharacterBitxorAdapter());
    this->addOperator(OperatorNode::BITOR, new CharacterBitorAdapter());
    this->addOperator(OperatorNode::AND, new CharacterUnsupportedAdapter());
    this->addOperator(OperatorNode::OR, new CharacterUnsupportedAdapter());
}

Object *CharacterType::create(Runtime *rt) {
    ProfilerCAPTURE();
    auto ins = createInstance(rt, CharacterInstance);
    auto obj = createObject(rt, true, ins, this);
    return obj;
}

Object *CharacterType::copy(Object *obj, Runtime *rt) {
    ProfilerCAPTURE();
    if (!isTypeObject(obj) || obj->type->id != rt->character_type->id) {
        rt->signalError("Failed to copy an invalid object: " + obj->shortRepr());
    }
    if (obj->instance == NULL) {
        return createObject(rt, false, NULL, this);
    }
    auto ins = obj->instance->copy(rt);
    auto res = createObject(rt, true, ins, this);
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
