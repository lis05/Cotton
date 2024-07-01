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
#include "gc.h"
#include "nameid.h"
#include "runtime.h"
#include <string>
#include <vector>

namespace Cotton {

int64_t Type::total_types = 0;

Type::Type(bool is_simple, Runtime *rt) {
    this->id = ++Type::total_types;
    for (auto &op : this->operators) {
        op = NULL;
    }
    this->is_simple = is_simple;
    this->gc_mark   = !rt->gc->gc_mark;

    rt->gc->track(this, rt);
}

Type::~Type() {
    this->id = -1;
    for (auto &op : this->operators) {
        delete op;
        op = NULL;
    }
    // we don't do anything else, because the GC will take care of that
}

void Type::addOperator(OperatorNode::OperatorId id, OperatorAdapter *op) {
    this->operators[id] = op;
}

void Type::addMethod(int64_t id, Object *method) {
    this->methods[id] = method;
}

OperatorAdapter *Type::getOperator(OperatorNode::OperatorId id) {
    auto res = this->operators[id];
    if (res == NULL) {
        auto method_id = MagicMethods::getMagicOperator(id);
        if (method_id == -1) {
            return NULL;
        }
        auto method = this->getMethod(method_id);
        if (method == NULL || method->type == NULL) {
            return NULL;
        }    
        // all overloaded operators must be overloaded via the call operator
        return method->type->getOperator(OperatorNode::CALL);
    }
    else {
        return res;
    }
}

Object *Type::getMethod(int64_t id) {
    auto it = this->methods.find(id);
    if (it != this->methods.end()) {
        return it->second;
    }
    return NULL;
}

bool Type::hasMethod(int64_t id) {
    auto it = this->methods.find(id);
    return it != this->methods.end();
}

std::vector<Object *> Type::getGCReachable() {
    std::vector<Object *> res;
    for (auto &elem : this->methods) {
        res.push_back(elem.second);
    }
    return res;
}

namespace MagicMethods {
    int64_t __make__() {
        static NameId res("__make__");
        return res.id;
    }

    // operators
    int64_t __postinc__() {
        static NameId res("__postinc__");
        return res.id;
    }

    int64_t __postdec__() {
        static NameId res("__postdec__");
        return res.id;
    }

    int64_t __call__() {
        static NameId res("__call__");
        return res.id;
    }

    int64_t __index__() {
        static NameId res("__index__");
        return res.id;
    }

    int64_t __preinc__() {
        static NameId res("__preinc__");
        return res.id;
    }

    int64_t __predec__() {
        static NameId res("__predec__");
        return res.id;
    }

    int64_t __positive__() {
        static NameId res("__positive__");
        return res.id;
    }

    int64_t __negative__() {
        static NameId res("__negative__");
        return res.id;
    }

    int64_t __not__() {
        static NameId res("__not__");
        return res.id;
    }

    int64_t __inverse__() {
        static NameId res("__inverse__");
        return res.id;
    }

    int64_t __mult__() {
        static NameId res("__mult__");
        return res.id;
    }

    int64_t __div__() {
        static NameId res("__div__");
        return res.id;
    }

    int64_t __rem__() {
        static NameId res("__rem__");
        return res.id;
    }

    int64_t __rshift__() {
        static NameId res("__rshift__");
        return res.id;
    }

    int64_t __lshift__() {
        static NameId res("__lshift__");
        return res.id;
    }

    int64_t __add__() {
        static NameId res("__add__");
        return res.id;
    }

    int64_t __sub__() {
        static NameId res("__sub__");
        return res.id;
    }

    int64_t __lt__() {
        static NameId res("__lt__");
        return res.id;
    }

    int64_t __leq__() {
        static NameId res("__leq__");
        return res.id;
    }

    int64_t __gt__() {
        static NameId res("__gt__");
        return res.id;
    }

    int64_t __geq__() {
        static NameId res("__geq__");
        return res.id;
    }

    int64_t __eq__() {
        static NameId res("__eq__");
        return res.id;
    }

    int64_t __neq__() {
        static NameId res("__neq__");
        return res.id;
    }

    int64_t __bitand__() {
        static NameId res("__bitand__");
        return res.id;
    }

    int64_t __bitxor__() {
        static NameId res("__bitxor__");
        return res.id;
    }

    int64_t __bitor__() {
        static NameId res("__bitor__");
        return res.id;
    }

    int64_t __and__() {
        static NameId res("__and__");
        return res.id;
    }

    int64_t __or__() {
        static NameId res("__or__");
        return res.id;
    }

    int64_t getMagicOperator(OperatorNode::OperatorId id) {
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

};    // namespace Cotton
