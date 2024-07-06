#include "nothing.h"

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

UserDefinedInstance::UserDefinedInstance(Runtime *rt)
    : Instance(rt, sizeof(UserDefinedInstance)) {}

UserDefinedInstance::~UserDefinedInstance() {}

Instance *UserDefinedInstance::copy() {
    return this;    // because record is a complex data type
}

size_t UserDefinedInstance::getSize() {
    return sizeof(UserDefinedInstance);
}

size_t UserDefinedType::getInstanceSize() {
    return sizeof(UserDefinedInstance);
}

std::string UserDefinedInstance::shortRepr() {
    if (this == NULL) {
        return "NULL";
    }
    return NameId::shortRepr(this->nameid) + "Instance(id = " + std::to_string(this->id) + ")";
}

class UserDefinedDefaultAdapter: public OperatorAdapter {
public:
    UserDefinedDefaultAdapter(Runtime *rt)
        : OperatorAdapter(rt) {}

    Object *operator()(Object *self, const std::vector<Object *> &others) {
        rt->signalError(self->shortRepr() + " does not support that operator");
    }
};

UserDefinedType::UserDefinedType(Runtime *rt)
    : Type(rt) {
    this->addOperator(OperatorNode::POST_PLUS_PLUS, new UserDefinedDefaultAdapter(rt));
    this->addOperator(OperatorNode::POST_MINUS_MINUS, new UserDefinedDefaultAdapter(rt));
    this->addOperator(OperatorNode::CALL, new UserDefinedDefaultAdapter(rt));
    this->addOperator(OperatorNode::INDEX, new UserDefinedDefaultAdapter(rt));
    this->addOperator(OperatorNode::PRE_PLUS_PLUS, new UserDefinedDefaultAdapter(rt));
    this->addOperator(OperatorNode::PRE_MINUS_MINUS, new UserDefinedDefaultAdapter(rt));
    this->addOperator(OperatorNode::PRE_PLUS, new UserDefinedDefaultAdapter(rt));
    this->addOperator(OperatorNode::PRE_MINUS, new UserDefinedDefaultAdapter(rt));
    this->addOperator(OperatorNode::NOT, new UserDefinedDefaultAdapter(rt));
    this->addOperator(OperatorNode::INVERSE, new UserDefinedDefaultAdapter(rt));
    this->addOperator(OperatorNode::MULT, new UserDefinedDefaultAdapter(rt));
    this->addOperator(OperatorNode::DIV, new UserDefinedDefaultAdapter(rt));
    this->addOperator(OperatorNode::REM, new UserDefinedDefaultAdapter(rt));
    this->addOperator(OperatorNode::RIGHT_SHIFT, new UserDefinedDefaultAdapter(rt));
    this->addOperator(OperatorNode::LEFT_SHIFT, new UserDefinedDefaultAdapter(rt));
    this->addOperator(OperatorNode::PLUS, new UserDefinedDefaultAdapter(rt));
    this->addOperator(OperatorNode::MINUS, new UserDefinedDefaultAdapter(rt));
    this->addOperator(OperatorNode::LESS, new UserDefinedDefaultAdapter(rt));
    this->addOperator(OperatorNode::LESS_EQUAL, new UserDefinedDefaultAdapter(rt));
    this->addOperator(OperatorNode::GREATER, new UserDefinedDefaultAdapter(rt));
    this->addOperator(OperatorNode::GREATER_EQUAL, new UserDefinedDefaultAdapter(rt));
    this->addOperator(OperatorNode::EQUAL, new UserDefinedDefaultAdapter(rt));
    this->addOperator(OperatorNode::NOT_EQUAL, new UserDefinedDefaultAdapter(rt));
    this->addOperator(OperatorNode::BITAND, new UserDefinedDefaultAdapter(rt));
    this->addOperator(OperatorNode::BITXOR, new UserDefinedDefaultAdapter(rt));
    this->addOperator(OperatorNode::BITOR, new UserDefinedDefaultAdapter(rt));
    this->addOperator(OperatorNode::AND, new UserDefinedDefaultAdapter(rt));
    this->addOperator(OperatorNode::OR, new UserDefinedDefaultAdapter(rt));
}

Object *UserDefinedType::create() {
    auto ins = createInstance(rt, UserDefinedInstance);
    for (auto f : this->instance_fields) {
        ins->addField(NameId(f).id, makeNothingInstanceObject(rt), rt);
    }
    auto obj = createObject(rt, true, ins, this);
    return obj;
}

std::string UserDefinedType::shortRepr() {
    if (this == NULL) {
        return "NULL";
    }
    return "UserDefinedType(id = " + std::to_string(this->id) + ", name = " + NameId::shortRepr(this->nameid)
           + ")";
}

Object *UserDefinedType::copy(Object *obj) {
    if (!rt->isTypeObject(obj)) {
        rt->signalError("Failed to copy an invalid object: " + obj->shortRepr());
    }
    if (!rt->isInstanceObject(obj)) {
        return createObject(rt, false, NULL, this);
    }
    auto ins = obj->instance->copy();
    auto res = createObject(rt, true, ins, this);
    return res;
}

Object *makeUserDefinedInstanceObject(Runtime *rt) {
    auto res = rt->make(rt->nothing_type, rt->INSTANCE_OBJECT);
    return res;
}
}    // namespace Cotton::Builtin
