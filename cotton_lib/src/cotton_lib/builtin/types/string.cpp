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

#include "string.h"
#include "../../profiler.h"
#include "api.h"

namespace Cotton::Builtin {
StringInstance::StringInstance(Runtime *rt)
    : Instance(rt, sizeof(StringInstance)) {
    ProfilerCAPTURE();
    this->data = "";
}

StringInstance::~StringInstance() {
    ProfilerCAPTURE();
}

Instance *StringInstance::copy(Runtime *rt) {
    ProfilerCAPTURE();
    Instance *res = new StringInstance(rt);

    if (res == nullptr) {
        rt->signalError("Failed to copy " + this->userRepr(rt), rt->getContext().area);
    }
    ((StringInstance *)res)->data = this->data;
    return res;
}

std::string StringInstance::userRepr(Runtime *rt) {
    ProfilerCAPTURE();
    if (this == nullptr) {
        return "String(nullptr)";
    }
    return "StringInstance(size = " + std::to_string(this->data.size()) + ", data = ...)";
}

size_t StringInstance::getSize() {
    ProfilerCAPTURE();
    return sizeof(StringInstance);
}

size_t StringType::getInstanceSize() {
    ProfilerCAPTURE();
    return sizeof(StringInstance);
}

static Object *StringIndexAdapter(Object *self, const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();

    rt->verifyExactArgsAmountFunc(args, 1);
    auto &arg = args[0];
    rt->verifyIsInstanceObject(arg, rt->builtin_types.integer, OperatorArgCtx(1));
    if (!(0 <= getIntegerValueFast(arg) && getIntegerValueFast(arg) < getStringDataFast(self).size())) {
        rt->signalError("Index " + arg->userRepr(rt) + " is out of string " + self->userRepr(rt) + " range", rt->getContext().sub_areas[1]);
    }
    if (!execution_result_matters) {
        return nullptr;
    }
    return makeCharacterInstanceObject(getStringDataFast(self)[getIntegerValueFast(arg)], rt);
}

static Object *StringAddAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();

    rt->verifyIsInstanceObject(arg, rt->builtin_types.string, OperatorArgCtx(1));

    if (!execution_result_matters) {
        return nullptr;
    }

    auto res                = rt->copy(self);
    getStringDataFast(res) += getStringDataFast(arg);

    return res;
}

static Object *StringEqAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyIsValidObject(arg, OperatorArgCtx(1));

    if (!rt->isOfType(arg, rt->builtin_types.string)) {
        return rt->protectedBoolean(false);
    }

    if (rt->isInstanceObject(self, rt->builtin_types.string)) {
        if (!rt->isInstanceObject(arg, rt->builtin_types.string)) {
            return rt->protectedBoolean(false);
        }
        return rt->protectedBoolean(getStringDataFast(self) == getStringDataFast(arg));
    }
    else if (rt->isTypeObject(self, rt->builtin_types.string)) {
        if (!rt->isTypeObject(arg, rt->builtin_types.string)) {
            return rt->protectedBoolean(false);
        }
        return rt->protectedBoolean(true);
    }

    return rt->protectedBoolean(false);
}

static Object *StringNeqAdapter(Object *self, Object *arg, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    auto res = StringEqAdapter(self, arg, rt, execution_result_matters);
    return rt->protectedBoolean(!getBooleanValueFast(res));
}

static Object *stringSizeMethod(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];

    if (!execution_result_matters) {
        return nullptr;
    }

    return makeIntegerInstanceObject(getStringDataFast(self).size(), rt);
}

static Object *stringSetMethod(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 2);
    auto self  = args[0];
    auto index = args[1];
    auto value = args[2];

    rt->verifyIsInstanceObject(index, rt->builtin_types.integer, MethodArgCtx(0));
    rt->verifyIsInstanceObject(value, rt->builtin_types.character, MethodArgCtx(1));

    int64_t ind  = getIntegerValueFast(index);
    auto   &data = getStringDataFast(self);
    if (!(0 <= ind && ind < data.size())) {
        rt->signalError("Index is out of range: " + index->userRepr(rt), rt->getContext().sub_areas[2]);
    }
    data[ind] = getCharacterValueFast(value);
    return rt->protectedNothing();
}

static Object *stringClearMethod(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];

    getStringDataFast(self).clear();
    return self;
}

static Object *stringEmptyMethod(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];

    return rt->protectedBoolean(getStringDataFast(self).empty());
}

static Object *stringReverseMethod(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];

    std::reverse(getStringDataFast(self).begin(), getStringDataFast(self).end());
    return self;
}

static Object *stringPrependMethod(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 1);
    auto self = args[0];
    auto arg  = args[1];

    rt->verifyIsInstanceObject(arg, rt->builtin_types.string, MethodArgCtx(0));

    getStringDataFast(self) = getStringDataFast(arg) + getStringDataFast(self);

    return self;
}

static Object *stringAppendMethod(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 1);
    auto self = args[0];
    auto arg  = args[1];

    rt->verifyIsInstanceObject(arg, rt->builtin_types.string, MethodArgCtx(0));

    getStringDataFast(self) += getStringDataFast(arg);

    return self;
}

static Object *stringDelprefMethod(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 1);
    auto self = args[0];
    auto arg  = args[1];

    rt->verifyIsInstanceObject(arg, rt->builtin_types.string, MethodArgCtx(0));

    auto &str  = getStringDataFast(self);
    auto &pref = getStringDataFast(arg);

    if (str.starts_with(pref)) {
        str.erase(0, pref.size());
    }

    return self;
}

static Object *stringDelsufMethod(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 1);
    auto self = args[0];
    auto arg  = args[1];

    rt->verifyIsInstanceObject(arg, rt->builtin_types.string, MethodArgCtx(0));

    auto &str  = getStringDataFast(self);
    auto &pref = getStringDataFast(arg);

    if (str.ends_with(pref)) {
        str.erase(str.size() - pref.size(), pref.size());
    }

    return self;
}

static Object *stringCopyMethod(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];

    auto &str = getStringDataFast(self);

    return makeStringInstanceObject(str, rt);
}

static Object *stringSubstrMethod(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 2);
    auto self  = args[0];
    auto begin = args[1];
    auto end   = args[2];

    rt->verifyIsInstanceObject(begin, rt->builtin_types.integer, MethodArgCtx(0));
    rt->verifyIsInstanceObject(end, rt->builtin_types.integer, MethodArgCtx(1));

    int64_t p1  = getIntegerValueFast(begin);
    int64_t p2  = getIntegerValueFast(end);
    auto   &str = getStringDataFast(self);

    if (p1 < 0) {
        p1 = 0;
    }
    if (p2 > str.size()) {
        p2 = str.size();
    }
    if (p1 >= p2) {
        return makeStringInstanceObject("", rt);
    }

    auto substr = str.substr(p1, p2 - p1);

    return makeStringInstanceObject(substr, rt);
}

static Object *stringArrayMethod(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];

    std::vector<Object *> data;
    for (auto c : getStringDataFast(self)) {
        data.push_back(makeCharacterInstanceObject(c, rt));
    }
    return makeArrayInstanceObject(data, rt);
}

static Object *stringSplitMethod(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyMinArgsAmountMethod(args, 0);
    auto self = args[0];

    auto &str = getStringDataFast(self);

    if (args.size() >= 2) {
        auto arg = args[1];
        rt->verifyIsInstanceObject(arg, rt->builtin_types.string, MethodArgCtx(0));
        auto &splitter = getStringDataFast(arg);

        std::vector<std::string> res;
        size_t                   prev_pos = 0;

        while (true) {
            size_t pos = str.find(splitter, prev_pos);
            if (pos == str.npos) {
                res.push_back(str.substr(prev_pos));
                break;
            }

            res.push_back(str.substr(prev_pos, pos - prev_pos));
            prev_pos = pos + splitter.size();
        }

        std::vector<Object *> arr;
        for (auto &str : res) {
            arr.push_back(makeStringInstanceObject(str, rt));
        }
        return makeArrayInstanceObject(arr, rt);
    }
    std::vector<std::string> res;
    size_t                   prev_pos = 0;
    for (size_t i = 0; i < str.size(); i++) {
        if (isspace(str[i])) {
            res.push_back(str.substr(prev_pos, i - prev_pos));
            prev_pos = i + 1;
        }
    }
    res.push_back(str.substr(prev_pos));

    std::vector<Object *> arr;
    for (auto &str : res) {
        arr.push_back(makeStringInstanceObject(str, rt));
    }
    return makeArrayInstanceObject(arr, rt);
}

static Object *stringSplitfMethod(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 1);
    auto self = args[0];
    auto arg  = args[1];

    rt->verifyIsInstanceObject(arg, rt->builtin_types.function, MethodArgCtx(0));

    auto &str = getStringDataFast(self);

    std::vector<std::string> res;
    size_t                   prev_pos = 0;
    auto                     ctx      = rt->getContext();
    rt->newContext();
    rt->getContext().area      = ctx.area;
    rt->getContext().sub_areas = {ctx.area, ctx.area};
    for (size_t i = 0; i < str.size(); i++) {
        auto r = rt->runOperator(OperatorNode::CALL, arg, std::vector<Object *> {makeCharacterInstanceObject(str[i], rt)}, true);
        if (getBooleanValue(r, rt)) {
            res.push_back(str.substr(prev_pos, i - prev_pos));
            prev_pos = i + 1;
        }
    }
    rt->popContext();
    res.push_back(str.substr(prev_pos));

    std::vector<Object *> arr;
    for (auto &str : res) {
        arr.push_back(makeStringInstanceObject(str, rt));
    }
    return makeArrayInstanceObject(arr, rt);
}

static Object *stringStartswithMethod(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 1);
    auto self = args[0];
    auto arg  = args[1];

    rt->verifyIsInstanceObject(arg, rt->builtin_types.string, MethodArgCtx(0));

    auto &str    = getStringDataFast(self);
    auto &target = getStringDataFast(arg);

    return rt->protectedBoolean(str.substr(0, target.size()) == target);
}

static Object *stringEndswithMethod(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 1);
    auto self = args[0];
    auto arg  = args[1];

    rt->verifyIsInstanceObject(arg, rt->builtin_types.string, MethodArgCtx(0));

    auto &str    = getStringDataFast(self);
    auto &target = getStringDataFast(arg);

    return rt->protectedBoolean(str.substr(std::max((int64_t)0, (int64_t)str.size() - int64_t(target.size()))) == target);
}

static Object *stringSplitlinesMethod(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];

    auto &str = getStringDataFast(self);

    std::vector<std::string> res;
    size_t                   prev_pos = 0;
    for (size_t i = 0; i < str.size(); i++) {
        if (str[i] == '\n') {
            res.push_back(str.substr(prev_pos, i - prev_pos));
            prev_pos = i + 1;
        }
    }
    res.push_back(str.substr(prev_pos));

    std::vector<Object *> arr;
    for (auto &str : res) {
        arr.push_back(makeStringInstanceObject(str, rt));
    }
    return makeArrayInstanceObject(arr, rt);
}

static Object *stringJoinMethod(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 1);
    auto self = args[0];
    auto arg  = args[1];

    rt->verifyIsInstanceObject(arg, rt->builtin_types.array, MethodArgCtx(0));

    auto &str = getStringDataFast(self);
    auto &arr = getArrayDataFast(arg);

    std::string res;
    bool        flag = false;
    for (auto item : arr) {
        if (flag) {
            res += str;
        }
        if (rt->isInstanceObject(item, rt->builtin_types.string)) {
            res  += getStringDataFast(item);
            flag  = true;
        }
        else {
            rt->verifyHasMethod(item, MagicMethods::mm__string__(rt), Runtime::AREA_CTX);
            auto s  = rt->runMethod(MagicMethods::mm__string__(rt), item, std::vector<Object *> {item}, true);
            res    += getStringData(s, rt);
        }
    }

    return makeStringInstanceObject(res, rt);
}

static Object *stringReplaceMethod(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 2);
    auto self = args[0];
    auto arg1 = args[1];
    auto arg2 = args[2];

    rt->verifyIsInstanceObject(arg1, rt->builtin_types.string, MethodArgCtx(0));
    rt->verifyIsInstanceObject(arg2, rt->builtin_types.string, MethodArgCtx(1));

    auto &str  = getStringDataFast(self);
    auto &what = getStringDataFast(arg1);
    auto &with = getStringDataFast(arg2);

    std::string res;
    size_t      prev_pos = 0;

    while (true) {
        size_t pos = str.find(what, prev_pos);
        if (pos == str.npos) {
            res += str.substr(prev_pos);
            break;
        }

        res      += str.substr(prev_pos, pos - prev_pos);
        res      += with;
        prev_pos  = pos + what.size();
    }

    str = res;
    return self;
}

static Object *stringConcatwithMethod(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 1);
    auto self = args[0];
    auto arg  = args[1];
    rt->verifyIsInstanceObject(arg, rt->builtin_types.string, MethodArgCtx(0));

    auto &str    = getStringDataFast(self);
    auto &other  = getStringDataFast(arg);
    str         += other;
    return self;
}

static Object *string_mm__bool__(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];

    if (!execution_result_matters) {
        return self;
    }

    if (getStringDataFast(self) == "true") {
        return rt->protectedBoolean(true);
    }
    else if (getStringDataFast(self) == "true") {
        return rt->protectedBoolean(false);
    }
    rt->signalError("Unsupported conversion: " + getStringDataFast(self), rt->getContext().area);
}

static Object *string_mm__int__(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];

    if (!execution_result_matters) {
        return self;
    }

    return makeIntegerInstanceObject(atoll(getStringDataFast(self).c_str()), rt);
}

static Object *string_mm__real__(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];

    if (!execution_result_matters) {
        return self;
    }

    return makeRealInstanceObject(atof(getStringDataFast(self).c_str()), rt);
}

static Object *string_mm__string__(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];

    if (!execution_result_matters) {
        return self;
    }

    return makeStringInstanceObject(getStringDataFast(self), rt);
}

static Object *string_mm__repr__(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];
    rt->verifyIsOfType(self, rt->builtin_types.string, MethodArgCtx(0));

    if (!execution_result_matters) {
        return self;
    }

    if (rt->isTypeObject(self, nullptr)) {
        return makeStringInstanceObject("String", rt);
    }

    return makeStringInstanceObject("\"" + getStringDataFast(self) + "\"", rt);
}

static Object *string_mm__read__(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];
    rt->verifyIsOfType(self, rt->builtin_types.string, MethodArgCtx(0));

    std::string v;
    std::cin >> v;

    if (!execution_result_matters) {
        return self;
    }

    return makeStringInstanceObject(v, rt);
}

void installStringMethods(Type *type, Runtime *rt) {
    ProfilerCAPTURE();
    type->addMethod(MagicMethods::mm__bool__(rt), Builtin::makeFunctionInstanceObject(true, string_mm__bool__, nullptr, rt));
    type->addMethod(MagicMethods::mm__int__(rt), Builtin::makeFunctionInstanceObject(true, string_mm__int__, nullptr, rt));
    type->addMethod(MagicMethods::mm__real__(rt), Builtin::makeFunctionInstanceObject(true, string_mm__real__, nullptr, rt));
    type->addMethod(MagicMethods::mm__string__(rt), Builtin::makeFunctionInstanceObject(true, string_mm__string__, nullptr, rt));
    type->addMethod(MagicMethods::mm__repr__(rt), Builtin::makeFunctionInstanceObject(true, string_mm__repr__, nullptr, rt));
    type->addMethod(MagicMethods::mm__read__(rt), Builtin::makeFunctionInstanceObject(true, string_mm__read__, nullptr, rt));

    type->addMethod(rt->nmgr->getId("size"), makeFunctionInstanceObject(true, stringSizeMethod, nullptr, rt));
    type->addMethod(rt->nmgr->getId("set"), makeFunctionInstanceObject(true, stringSetMethod, nullptr, rt));
    type->addMethod(rt->nmgr->getId("clear"), makeFunctionInstanceObject(true, stringClearMethod, nullptr, rt));
    type->addMethod(rt->nmgr->getId("empty"), makeFunctionInstanceObject(true, stringEmptyMethod, nullptr, rt));
    type->addMethod(rt->nmgr->getId("reverse"), makeFunctionInstanceObject(true, stringReverseMethod, nullptr, rt));
    type->addMethod(rt->nmgr->getId("prepend"), makeFunctionInstanceObject(true, stringPrependMethod, nullptr, rt));
    type->addMethod(rt->nmgr->getId("append"), makeFunctionInstanceObject(true, stringAppendMethod, nullptr, rt));
    type->addMethod(rt->nmgr->getId("delpref"), makeFunctionInstanceObject(true, stringDelprefMethod, nullptr, rt));
    type->addMethod(rt->nmgr->getId("delsuf"), makeFunctionInstanceObject(true, stringDelsufMethod, nullptr, rt));
    type->addMethod(rt->nmgr->getId("copy"), makeFunctionInstanceObject(true, stringCopyMethod, nullptr, rt));
    type->addMethod(rt->nmgr->getId("substr"), makeFunctionInstanceObject(true, stringSubstrMethod, nullptr, rt));
    type->addMethod(rt->nmgr->getId("array"), makeFunctionInstanceObject(true, stringArrayMethod, nullptr, rt));
    type->addMethod(rt->nmgr->getId("split"), makeFunctionInstanceObject(true, stringSplitMethod, nullptr, rt));
    type->addMethod(rt->nmgr->getId("splitf"), makeFunctionInstanceObject(true, stringSplitfMethod, nullptr, rt));
    type->addMethod(rt->nmgr->getId("startswith"), makeFunctionInstanceObject(true, stringStartswithMethod, nullptr, rt));
    type->addMethod(rt->nmgr->getId("endswith"), makeFunctionInstanceObject(true, stringEndswithMethod, nullptr, rt));
    type->addMethod(rt->nmgr->getId("splitlines"), makeFunctionInstanceObject(true, stringSplitlinesMethod, nullptr, rt));
    type->addMethod(rt->nmgr->getId("join"), makeFunctionInstanceObject(true, stringJoinMethod, nullptr, rt));
    type->addMethod(rt->nmgr->getId("replace"), makeFunctionInstanceObject(true, stringReplaceMethod, nullptr, rt));
    type->addMethod(rt->nmgr->getId("concatwith"), makeFunctionInstanceObject(true, stringConcatwithMethod, nullptr, rt));
}

StringType::StringType(Runtime *rt)
    : Type(rt) {
    ProfilerCAPTURE();
    this->index_op = StringIndexAdapter;
    this->add_op   = StringAddAdapter;
    this->eq_op    = StringEqAdapter;
    this->neq_op   = StringNeqAdapter;
}

Object *StringType::create(Runtime *rt) {
    ProfilerCAPTURE();
    Instance *ins = new StringInstance(rt);
    Object   *obj = new Object(true, ins, this, rt);
    return obj;
}

Object *StringType::copy(Object *obj, Runtime *rt) {
    ProfilerCAPTURE();
    rt->verifyIsOfType(obj, rt->builtin_types.string);
    if (obj->instance == nullptr) {
        return new Object(false, nullptr, this, rt);
    }
    auto ins = obj->instance->copy(rt);
    auto res = new Object(true, ins, this, rt);
    return res;
}

std::string StringType::userRepr(Runtime *rt) {
    ProfilerCAPTURE();
    if (this == nullptr) {
        return "StringType(nullptr)";
    }
    return "StringType";
}

std::string &getStringData(Object *obj, Runtime *rt) {
    ProfilerCAPTURE();
    rt->verifyIsInstanceObject(obj, rt->builtin_types.string);
    return icast(obj->instance, Cotton::Builtin::StringInstance)->data;
}

std::string &getStringData(Object *obj, Runtime *rt, Runtime::ContextId ctx_id) {
    ProfilerCAPTURE();
    rt->verifyIsInstanceObject(obj, rt->builtin_types.string, Runtime::SUB0_CTX);
    return icast(obj->instance, Cotton::Builtin::StringInstance)->data;
}

Object *makeStringInstanceObject(const std::string &value, Runtime *rt) {
    ProfilerCAPTURE();
    auto res               = rt->make(rt->builtin_types.string, Runtime::INSTANCE_OBJECT);
    getStringDataFast(res) = value;
    return res;
}

}    // namespace Cotton::Builtin
