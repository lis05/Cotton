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

static Object *print(const std::vector<Object *> &args, Runtime *rt) {
    for (auto arg : args) {
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
                rt->runMethod(MagicMethods::__iprint__(), arg, {});
            }
        }
        else if (rt->isTypeObject(arg)) {
            if (arg->type->id == rt->boolean_type->id) {
                std::cout << "Boolean";
            }
            else if (arg->type->id == rt->character_type->id) {
                std::cout << "Character";
            }
            else if (arg->type->id == rt->function_type->id) {
                std::cout << "Function";
            }
            else if (arg->type->id == rt->integer_type->id) {
                std::cout << "Integer";
            }
            else if (arg->type->id == rt->nothing_type->id) {
                std::cout << "Nothing";
            }
            else if (arg->type->id == rt->real_type->id) {
                std::cout << "Real";
            }
            else if (arg->type->id == rt->string_type->id) {
                std::cout << "String";
            }
            else {
                rt->runMethod(MagicMethods::__tprint__(), arg, {});
            }
        }
        else {
            rt->signalError("Invalid object " + arg->shortRepr());
        }
    }
    return makeNothingInstanceObject(rt);
}

void installBuiltinFunctions(Runtime *rt) {
    rt->scope->addVariable(NameId("print").id, makeFunctionInstanceObject(true, print, NULL, rt), rt);
}
}    // namespace Cotton::Builtin
