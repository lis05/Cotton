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

#include "type.h"
#include "../profiler.h"
#include "gc.h"
#include "nameid.h"
#include "runtime.h"
#include <string>
#include <vector>

namespace Cotton {

int64_t Type::total_types = 0;

Type::Type(Runtime *rt) {
    ProfilerCAPTURE();
    this->rt = rt;
    this->id = ++Type::total_types;
    for (auto &op : this->operators) {
        op = NULL;
    }
    this->gc_mark = !rt->gc->gc_mark;

    rt->gc->track(this);
}

Type::~Type() {
    ProfilerCAPTURE();
    this->id = -1;
    for (auto &op : this->operators) {
        delete op;
        op = NULL;
    }
    // we don't do anything else, because the GC will take care of that
}

void Type::addOperator(OperatorNode::OperatorId id, OperatorAdapter *op) {
    ProfilerCAPTURE();
    this->operators[id] = op;
}

void Type::addMethod(int64_t id, Object *method) {
    ProfilerCAPTURE();
    this->methods[id] = method;
}

OperatorAdapter *Type::getOperator(OperatorNode::OperatorId id) {
    ProfilerCAPTURE();
    auto res = this->operators[id];
    if (res == NULL) {
        auto method_id = MagicMethods::getMagicOperator(id);
        if (method_id == -1) {
            rt->signalError("Failed to get operator " + std::to_string(id) + " in " + this->shortRepr()
                            + " because the appropriate magic method could not be found");
        }
        auto method = this->getMethod(method_id);
        if (method->type == NULL) {
            rt->signalError("Failed to get operator " + std::to_string(id) + " in " + this->shortRepr()
                            + " because the appropriate magic method is an invalid object");
        }
        auto res = method->type->getOperator(OperatorNode::CALL);
        return res;
    }
    else {
        return res;
    }
}

Object *Type::getMethod(int64_t id) {
    ProfilerCAPTURE();
    auto it = this->methods.find(id);
    if (it != this->methods.end()) {
        if (!isInstanceObject(it->second)) {
            rt->signalError("Method " + NameId::shortRepr(id) + " from " + this->shortRepr()
                            + " is not an instance object");
        }
        return it->second;
    }
    rt->signalError(this->shortRepr() + " doesn't have method " + NameId::shortRepr(id));
}

bool Type::hasOperator(OperatorNode::OperatorId id) {
    ProfilerCAPTURE();
    auto res = this->operators[id];
    if (res == NULL) {
        auto method_id = MagicMethods::getMagicOperator(id);
        if (method_id == -1) {
            rt->signalError("Invalid operator");
        }
        if (!this->hasMethod(method_id)) {
            return false;
        }
        auto method = this->getMethod(method_id);
        return method->type->getOperator(OperatorNode::CALL);
    }
    else {
        return true;
    }
}

bool Type::hasMethod(int64_t id) {
    ProfilerCAPTURE();
    auto it = this->methods.find(id);
    return it != this->methods.end();
}

Object *Type::getMethodOrNULL(int64_t id) {
    ProfilerCAPTURE();
    auto it = this->methods.find(id);
    if (it != this->methods.end()) {
        if (!isInstanceObject(it->second)) {
            rt->signalError("Method " + NameId::shortRepr(id) + " from " + this->shortRepr()
                            + " is not an instance object");
        }
        return it->second;
    }
    return NULL;
}

std::vector<Object *> Type::getGCReachable() {
    ProfilerCAPTURE();
    std::vector<Object *> res;
    for (auto &elem : this->methods) {
        res.push_back(elem.second);
    }
    return res;
}

namespace MagicMethods {
    int64_t __make__() {
        ProfilerCAPTURE();
        static NameId res("__make__");
        return res.id;
    }

    int64_t __print__() {
        ProfilerCAPTURE();
        static NameId res("__print__");
        return res.id;
    }

    // operators
    int64_t __postinc__() {
        ProfilerCAPTURE();
        static NameId res("__postinc__");
        return res.id;
    }

    int64_t __postdec__() {
        ProfilerCAPTURE();
        static NameId res("__postdec__");
        return res.id;
    }

    int64_t __call__() {
        ProfilerCAPTURE();
        static NameId res("__call__");
        return res.id;
    }

    int64_t __index__() {
        ProfilerCAPTURE();
        static NameId res("__index__");
        return res.id;
    }

    int64_t __preinc__() {
        ProfilerCAPTURE();
        static NameId res("__preinc__");
        return res.id;
    }

    int64_t __predec__() {
        ProfilerCAPTURE();
        static NameId res("__predec__");
        return res.id;
    }

    int64_t __positive__() {
        ProfilerCAPTURE();
        static NameId res("__positive__");
        return res.id;
    }

    int64_t __negative__() {
        ProfilerCAPTURE();
        static NameId res("__negative__");
        return res.id;
    }

    int64_t __not__() {
        ProfilerCAPTURE();
        static NameId res("__not__");
        return res.id;
    }

    int64_t __inverse__() {
        ProfilerCAPTURE();
        static NameId res("__inverse__");
        return res.id;
    }

    int64_t __mult__() {
        ProfilerCAPTURE();
        static NameId res("__mult__");
        return res.id;
    }

    int64_t __div__() {
        ProfilerCAPTURE();
        static NameId res("__div__");
        return res.id;
    }

    int64_t __rem__() {
        ProfilerCAPTURE();
        static NameId res("__rem__");
        return res.id;
    }

    int64_t __rshift__() {
        ProfilerCAPTURE();
        static NameId res("__rshift__");
        return res.id;
    }

    int64_t __lshift__() {
        ProfilerCAPTURE();
        static NameId res("__lshift__");
        return res.id;
    }

    int64_t __add__() {
        ProfilerCAPTURE();
        static NameId res("__add__");
        return res.id;
    }

    int64_t __sub__() {
        ProfilerCAPTURE();
        static NameId res("__sub__");
        return res.id;
    }

    int64_t __lt__() {
        ProfilerCAPTURE();
        static NameId res("__lt__");
        return res.id;
    }

    int64_t __leq__() {
        ProfilerCAPTURE();
        static NameId res("__leq__");
        return res.id;
    }

    int64_t __gt__() {
        ProfilerCAPTURE();
        static NameId res("__gt__");
        return res.id;
    }

    int64_t __geq__() {
        ProfilerCAPTURE();
        static NameId res("__geq__");
        return res.id;
    }

    int64_t __eq__() {
        ProfilerCAPTURE();
        static NameId res("__eq__");
        return res.id;
    }

    int64_t __neq__() {
        ProfilerCAPTURE();
        static NameId res("__neq__");
        return res.id;
    }

    int64_t __bitand__() {
        ProfilerCAPTURE();
        static NameId res("__bitand__");
        return res.id;
    }

    int64_t __bitxor__() {
        ProfilerCAPTURE();
        static NameId res("__bitxor__");
        return res.id;
    }

    int64_t __bitor__() {
        ProfilerCAPTURE();
        static NameId res("__bitor__");
        return res.id;
    }

    int64_t __and__() {
        ProfilerCAPTURE();
        static NameId res("__and__");
        return res.id;
    }

    int64_t __or__() {
        ProfilerCAPTURE();
        static NameId res("__or__");
        return res.id;
    }

    int64_t getMagicOperator(OperatorNode::OperatorId id) {
        ProfilerCAPTURE();
        switch (id) {
        case OperatorNode::POST_PLUS_PLUS   : return __postinc__();
        case OperatorNode::POST_MINUS_MINUS : return __postdec__();
        case OperatorNode::CALL             : return __call__();
        case OperatorNode::INDEX            : return __index__();
        case OperatorNode::PRE_PLUS_PLUS    : return __preinc__();
        case OperatorNode::PRE_MINUS_MINUS  : return __predec__();
        case OperatorNode::PRE_PLUS         : return __positive__();
        case OperatorNode::PRE_MINUS        : return __negative__();
        case OperatorNode::NOT              : return __not__();
        case OperatorNode::INVERSE          : return __inverse__();
        case OperatorNode::MULT             : return __mult__();
        case OperatorNode::DIV              : return __div__();
        case OperatorNode::REM              : return __rem__();
        case OperatorNode::RIGHT_SHIFT      : return __rshift__();
        case OperatorNode::LEFT_SHIFT       : return __lshift__();
        case OperatorNode::PLUS             : return __add__();
        case OperatorNode::MINUS            : return __sub__();
        case OperatorNode::LESS             : return __lt__();
        case OperatorNode::LESS_EQUAL       : return __leq__();
        case OperatorNode::GREATER          : return __gt__();
        case OperatorNode::GREATER_EQUAL    : return __geq__();
        case OperatorNode::EQUAL            : return __eq__();
        case OperatorNode::NOT_EQUAL        : return __neq__();
        case OperatorNode::BITAND           : return __bitand__();
        case OperatorNode::BITXOR           : return __bitxor__();
        case OperatorNode::BITOR            : return __bitor__();
        case OperatorNode::AND              : return __and__();
        case OperatorNode::OR               : return __or__();
        default                             : return -1;
        }
    }
}    // namespace MagicMethods

OperatorAdapter::OperatorAdapter(Runtime *rt) {
    ProfilerCAPTURE();
    this->rt = rt;
}
};    // namespace Cotton
