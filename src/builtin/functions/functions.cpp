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
#include "../../api.h"
#include "../api.h"

namespace Cotton::Builtin {

static Object *make(const std::vector<Object *> &args, Runtime *rt) {
    if (args.size() != 1) {
        rt->signalError("Expected exactly one argument");
    }
    auto arg = args[0];
    if (!rt->isTypeObject(arg) || rt->isInstanceObject(arg)) {
        rt->signalError("Expected a type object but got " + arg->shortRepr());
    }

    return rt->make(arg->type, Runtime::INSTANCE_OBJECT);
}

static Object *print(const std::vector<Object *> &args, Runtime *rt) {
    bool f = false;
    for (auto arg : args) {
        if (f) {
            std::cout << " ";
        }
        f = true;
        if (rt->isInstanceObject(arg)) {
            if (arg->type->id == rt->boolean_type->id) {
                std::cout << (getBooleanValue(arg, rt) ? "true" : "false");
            }
            else if (arg->type->id == rt->character_type->id) {
                std::cout << (char)getCharacterValue(arg, rt);
            }
            else if (arg->type->id == rt->function_type->id) {
                std::cout << "function";
            }
            else if (arg->type->id == rt->integer_type->id) {
                std::cout << getIntegerValue(arg, rt);
            }
            else if (arg->type->id == rt->nothing_type->id) {
                std::cout << "nothing";
            }
            else if (arg->type->id == rt->real_type->id) {
                std::cout << getRealValue(arg, rt);
            }
            else if (arg->type->id == rt->string_type->id) {
                for (auto obj : getStringData(arg, rt)) {
                    std::cout << (char)getCharacterValue(obj, rt);
                }
            }
            else {
                rt->runMethod(MagicMethods::__print__(), arg, {});
            }
        }
        else {
            rt->signalError("Cannot print " + arg->shortRepr());
        }
    }
    std::cout << std::endl;
    return makeNothingInstanceObject(rt);
}

void installBuiltinFunctions(Runtime *rt) {
    rt->scope->addVariable(NameId("print").id, makeFunctionInstanceObject(true, print, NULL, rt), rt);
    rt->scope->addVariable(NameId("make").id, makeFunctionInstanceObject(true, make, NULL, rt), rt);
}
}    // namespace Cotton::Builtin
