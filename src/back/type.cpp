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

/*std::vector<int64_t> operator_names = {
    "__postinc_op__",
    "__postdecr_op__",
    "__call_op__",
    "__index_op__",
    "__dot_op__ NOT OVERLOADABLE",
    "__at_op__ NOT OVERLOADABLE",
    "__prefinc_op__",
    "__prefdecr_op__",
    "__positive_op__",
    "__negative_op__",
    "__not_op__",
    "__bitnot_op__",
    "__mult_op__",
    "__div_op__",
    "__rem_op__",
    "__rightshift_op__",
    "__leftshift_op__",
    "__add_op__",
    "__sub_op__",
    "__lt_op__",
    "__leq_op__",
    "__gt_op__",
    "__geq_op__",
    "__eq_op__",
    "__noteq_op__",
    "__bitand_op__",
    "__bitxor_op__",
    "__bitor_op__",
    "__and_op__",
    "__or_op__",
    "__assign_op__",
    "__add_assign_op__",
    "__sub_assign_op__",
    "__mult_assign_op__",
    "__div_assign_op__",
    "__rem_assign_op__",
    "__comma_op__ NOT OVERLOADABLE",
    "go heck yourself",
};*/

OperatorAdapter *Type::getOperator(OperatorNode::OperatorId id) {
    auto res = this->operators[id];
    return res;    // TODO
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
};    // namespace Cotton
