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

#include "api.h"

namespace Cotton::Builtin {
StringInstance::StringInstance(Runtime *rt, bool on_stack)
    : Instance(rt, sizeof(StringInstance), on_stack) {
    this->data = {};
}

StringInstance::~StringInstance() {}

Instance *StringInstance::copy(Runtime *rt, bool force_heap) {
    Instance *res = NULL;
    if (!force_heap) {
        res = rt->stack->allocAndInitInstance<StringInstance>(sizeof(StringInstance), rt);
        if (res != NULL) {
            res->on_stack = true;
            for (auto obj : this->data) {
                ((StringInstance *)res)->data.push_back(rt->copy(obj, force_heap));
            }
            return res;
        }
    }
    res = new (std::nothrow) StringInstance(rt, false);
    if (res == NULL) {
        rt->signalError("Failed to copy " + this->shortRepr());
    }
    for (auto obj : this->data) {
        ((StringInstance *)res)->data.push_back(rt->copy(obj, force_heap));
    }
    res->on_stack = false;
    return res;
}

std::string StringInstance::shortRepr() {
    if (this == NULL) {
        return "StringInstance(NULL)";
    }
    return "StringInstance(id = " + std::to_string(this->id) + ", size = " + std::to_string(this->data.size())
           + ", data = ...)";
}

size_t StringInstance::getSize() {
    return sizeof(StringInstance);
}

std::vector<Object *> StringInstance::getGCReachable(Runtime *rt) {
    auto res = Instance::getGCReachable(rt);
    for (auto obj : this->data) {
        res.push_back(obj);
    }
    return res;
}

size_t StringType::getInstanceSize() {
    return sizeof(StringInstance);
}

class StringPostincAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->string_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class StringPostdecAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->string_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class StringCallAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->string_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class StringIndexAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->string_type->id) {
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
        if (!rt->isInstanceObject(arg1) || arg1->type->id != rt->string_type->id) {
            rt->signalError("Index " + arg1->shortRepr() + " must be an integer instance object");
        }
        if (!(0 <= getIntegerValueFast(arg1) && getIntegerValueFast(arg1) < getStringDataFast(self).size())) {
            rt->signalError("Index " + arg1->shortRepr() + " is out of string " + self->shortRepr() + " range");
        }
        return getStringDataFast(self)[getIntegerValueFast(arg1)];
    }
};

class StringPreincAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->string_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class StringPredecAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->string_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class StringPositiveAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->string_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class StringNegativeAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->string_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class StringNotAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->string_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class StringInverseAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->string_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class StringMultAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->string_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class StringDivAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->string_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class StringRemAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->string_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class StringRshiftAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->string_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class StringLshiftAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->string_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class StringAddAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->string_type->id) {
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
        if (!rt->isInstanceObject(arg1) || arg1->type->id != rt->string_type->id) {
            rt->signalError("Right-side object " + arg1->shortRepr() + " must be a String instance object");
        }

        auto res = rt->copy(self);
        for (auto obj : getStringDataFast(arg1)) {
            getStringDataFast(res).push_back(obj);
        }

        return res;
    }
};

class StringSubAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->string_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class StringLtAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->string_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class StringLeqAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->string_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class StringGtAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->string_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class StringGeqAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->string_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class StringEqAdapter: public OperatorAdapter {
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
            if (getStringDataFast(self).size() != getStringDataFast(arg1).size()) {
                return makeBooleanInstanceObject(false, rt);
            }
            for (int i = 0; i < getStringDataFast(self).size(); i++) {
                if (!rt->runOperator(OperatorNode::EQUAL,
                                     getStringDataFast(self)[i],
                                     {getStringDataFast(arg1)[i]}))
                {
                    return makeBooleanInstanceObject(false, rt);
                }
            }
            return makeBooleanInstanceObject(true, rt);
        }
        else if (!rt->isInstanceObject(self) && !rt->isInstanceObject(arg1)) {
            return makeBooleanInstanceObject(self->type->id == arg1->type->id, rt);
        }
        else {
            return makeBooleanInstanceObject(false, rt);
        }
    }
};

class StringNeqAdapter: public StringEqAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        auto res                 = StringEqAdapter::operator()(self, others, rt);
        getBooleanValueFast(res) = !getBooleanValueFast(res);
        return res;
    }
};

class StringBitandAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->string_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class StringBitxorAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->string_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class StringBitorAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->string_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class StringAndAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->string_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

class StringOrAdapter: public OperatorAdapter {
public:
    Object *operator()(Object *self, const std::vector<Object *> &others, Runtime *rt) {
        if (!rt->isTypeObject(self) || self->type->id != rt->string_type->id) {
            rt->signalError("Left-side object is invalid: " + self->shortRepr());
        }
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

static Object *stringSizeMethod(const std::vector<Object *> &args, Runtime *rt) {
    if (args.size() < 1) {
        rt->signalError("Expected a caller object");
    }
    auto self = args[0];
    if (!rt->isTypeObject(self)) {
        rt->signalError("Caller is invalid: " + self->shortRepr());
    }
    if (!rt->isInstanceObject(self) || self->type->id != rt->string_type->id) {
        rt->signalError("Caller must be a String instance object: " + self->shortRepr());
    }
    return makeIntegerInstanceObject(getStringDataFast(self).size(), rt);
}

// TODO: add all operators to function and nothing
StringType::StringType(Runtime *rt)
    : Type(true, rt) {
    this->addOperator(OperatorNode::POST_PLUS_PLUS, new StringPostincAdapter(), rt);
    this->addOperator(OperatorNode::POST_MINUS_MINUS, new StringPostdecAdapter(), rt);
    this->addOperator(OperatorNode::CALL, new StringCallAdapter(), rt);
    this->addOperator(OperatorNode::INDEX, new StringIndexAdapter(), rt);
    this->addOperator(OperatorNode::PRE_PLUS_PLUS, new StringPreincAdapter(), rt);
    this->addOperator(OperatorNode::PRE_MINUS_MINUS, new StringPredecAdapter(), rt);
    this->addOperator(OperatorNode::PRE_PLUS, new StringPositiveAdapter(), rt);
    this->addOperator(OperatorNode::PRE_MINUS, new StringNegativeAdapter(), rt);
    this->addOperator(OperatorNode::NOT, new StringNotAdapter(), rt);
    this->addOperator(OperatorNode::INVERSE, new StringInverseAdapter(), rt);
    this->addOperator(OperatorNode::MULT, new StringMultAdapter(), rt);
    this->addOperator(OperatorNode::DIV, new StringDivAdapter(), rt);
    this->addOperator(OperatorNode::REM, new StringRemAdapter(), rt);
    this->addOperator(OperatorNode::RIGHT_SHIFT, new StringRshiftAdapter(), rt);
    this->addOperator(OperatorNode::LEFT_SHIFT, new StringLshiftAdapter(), rt);
    this->addOperator(OperatorNode::PLUS, new StringAddAdapter(), rt);
    this->addOperator(OperatorNode::MINUS, new StringSubAdapter(), rt);
    this->addOperator(OperatorNode::LESS, new StringLtAdapter(), rt);
    this->addOperator(OperatorNode::LESS_EQUAL, new StringLeqAdapter(), rt);
    this->addOperator(OperatorNode::GREATER, new StringGtAdapter(), rt);
    this->addOperator(OperatorNode::GREATER_EQUAL, new StringGeqAdapter(), rt);
    this->addOperator(OperatorNode::EQUAL, new StringEqAdapter(), rt);
    this->addOperator(OperatorNode::NOT_EQUAL, new StringNeqAdapter(), rt);
    this->addOperator(OperatorNode::BITAND, new StringBitandAdapter(), rt);
    this->addOperator(OperatorNode::BITXOR, new StringBitxorAdapter(), rt);
    this->addOperator(OperatorNode::BITOR, new StringBitorAdapter(), rt);
    this->addOperator(OperatorNode::AND, new StringAndAdapter(), rt);
    this->addOperator(OperatorNode::OR, new StringOrAdapter(), rt);

    this->addMethod(NameId("size").id, makeFunctionInstanceObject(true, stringSizeMethod, NULL, rt), rt);
}

Object *StringType::create(Runtime *rt) {
    auto ins = createInstance(rt, false, StringInstance);
    auto obj = createObject(rt, true, ins, this, false);
    return obj;
}

Object *StringType::copy(Object *obj, Runtime *rt, bool force_heap) {
    if (!rt->isTypeObject(obj) || obj->type->id != rt->string_type->id) {
        rt->signalError("Failed to copy an invalid object: " + obj->shortRepr());
    }
    if (!rt->isInstanceObject(obj)) {
        return createObject(rt, false, NULL, this, !force_heap);
    }
    auto ins = obj->instance->copy(rt, force_heap);
    auto res = createObject(rt, true, ins, this, !force_heap);
    return res;
}

std::string StringType::shortRepr() {
    if (this == NULL) {
        return "StringType(NULL)";
    }
    return "StringType(id = " + std::to_string(this->id) + ")";
}

std::vector<Object *> &getStringData(Object *obj, Runtime *rt) {
    if (!rt->isInstanceObject(obj)) {
        rt->signalError(obj->shortRepr() + " is not an instance object");
    }
    if (obj->type->id != rt->string_type->id) {
        rt->signalError(obj->shortRepr() + " is not String");
    }
    return icast(obj->instance, StringInstance)->data;
}

Object *makeStringInstanceObject(const std::string &value, Runtime *rt) {
    auto res = rt->make(rt->string_type, Runtime::INSTANCE_OBJECT);
    for (auto c : value) {
        getStringDataFast(res).push_back(makeCharacterInstanceObject(c, rt));
    }
    return res;
}

}    // namespace Cotton::Builtin
