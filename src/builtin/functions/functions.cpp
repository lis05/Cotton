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
#include <cstdlib>
#include <unistd.h>

namespace Cotton::Builtin {

static Object *make(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    if (args.size() != 1) {
        rt->signalError("Expected exactly one argument");
    }
    auto arg = args[0];
    if (!isTypeObject(arg) || isInstanceObject(arg)) {
        rt->signalError("Expected a type object but got " + arg->userRepr());
    }

    if (!execution_result_matters) {
        return NULL;
    }

    return rt->make(arg->type, Runtime::INSTANCE_OBJECT);
}

static Object *exit(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    if (args.size() != 1) {
        rt->signalError("Expected exactly one argument");
    }
    auto arg = args[0];
    if (!isInstanceObject(arg) || arg->type->id != rt->integer_type->id) {
        rt->signalError("Expected an integer object but got " + arg->userRepr());
    }

    ::exit(getIntegerValue(arg, rt));
}

static Object *print(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    bool f = false;
    for (auto arg : args) {
        if (f) {
            std::cout << " ";
        }
        f = true;
        if (isInstanceObject(arg)) {
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
                std::cout << getStringData(arg, rt);
            }
            else {
                rt->signalError("Cannot print " + arg->userRepr());
            }
        }
        else {
            rt->signalError("Cannot print " + arg->userRepr());
        }
    }
    std::cout << std::endl;
    return rt->protected_nothing;
}

static Object *printr(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    for (auto arg : args) {
        if (isInstanceObject(arg)) {
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
                std::cout << getStringData(arg, rt);
            }
            else {
                rt->signalError("Cannot print " + arg->userRepr());
            }
        }
        else {
            rt->signalError("Cannot print " + arg->userRepr());
        }
    }
    return rt->protected_nothing;
}

static Object *exec(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    if (args.size() != 1) {
        rt->signalError("Expected exactly one argument");
    }
    auto arg = args[0];
    if (!isInstanceObject(arg) || arg->type->id != rt->string_type->id) {
        rt->signalError("Expected a string object but got " + arg->userRepr());
    }

    Lexer        lexer(rt->error_manager);
    std::string &str = getStringDataFast(arg);

    std::vector<Token> tokens = lexer.process(str);
    for (auto &token : tokens) {
        token.nameid = NameId(&token).id;
    }

    Parser    parser(rt->error_manager);
    StmtNode *program = parser.parse(tokens);

    return rt->execute(program, true);
}

static Object *system(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    if (args.size() != 1) {
        rt->signalError("Expected exactly one argument");
    }
    auto arg = args[0];
    if (!isInstanceObject(arg) || arg->type->id != rt->string_type->id) {
        rt->signalError("Expected a string object but got " + arg->userRepr());
    }

    Lexer        lexer(rt->error_manager);
    std::string &str = getStringDataFast(arg);
    auto         ret = ::system(str.c_str());
    if (!execution_result_matters) {
        return NULL;
    }
    return Builtin::makeIntegerInstanceObject(ret, rt);
}

static Object *fork(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    if (args.size() != 0) {
        rt->signalError("Expected exactly zero arguments");
    }

    auto ret = ::fork();

    if (!execution_result_matters) {
        return NULL;
    }
    return Builtin::makeIntegerInstanceObject(ret, rt);
}

static Object *argc(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    if (args.size() != 0) {
        rt->signalError("Expected exactly zero arguments");
    }

    if (!execution_result_matters) {
        return NULL;
    }

    auto s = rt->scope->prev;
    while (s != NULL && s->can_access_prev) {
        s = s->prev;
    }
    if (s == NULL) {
        return Builtin::makeIntegerInstanceObject(0, rt);
    }
    return Builtin::makeIntegerInstanceObject(s->arguments.size(), rt);
}

static Object *argg(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    if (args.size() != 1) {
        rt->signalError("Expected exactly one argument");
    }

    auto arg = args[0];
    if (!isInstanceObject(arg) || arg->type->id != rt->integer_type->id) {
        rt->signalError("Expected an integer object but got " + arg->userRepr());
    }

    if (!execution_result_matters) {
        return NULL;
    }

    int64_t i = getIntegerValue(arg, rt);

    auto s = rt->scope->prev;
    while (s != NULL && s->can_access_prev) {
        s = s->prev;
    }
    if (s == NULL || i >= s->arguments.size()) {
        return rt->protected_nothing;
    }

    return s->arguments[i];
}

static Object *toInt(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    if (args.size() != 1) {
        rt->signalError("Expected exactly one argument");
    }
    auto arg = args[0];
    if (!isInstanceObject(arg)) {
        rt->signalError("Expected an instance object but got  " + arg->userRepr());
    }
    if (!execution_result_matters) {
        return NULL;
    }

    if (arg->type->id == rt->boolean_type->id) {
        return Builtin::makeIntegerInstanceObject(getBooleanValue(arg, rt), rt);
    }
    else if (arg->type->id == rt->character_type->id) {
        return Builtin::makeIntegerInstanceObject(getCharacterValue(arg, rt), rt);
    }
    else if (arg->type->id == rt->function_type->id) {
        // unsupported
    }
    else if (arg->type->id == rt->integer_type->id) {
        return Builtin::makeIntegerInstanceObject(getIntegerValue(arg, rt), rt);
    }
    else if (arg->type->id == rt->nothing_type->id) {
        return Builtin::makeIntegerInstanceObject(0, rt);
    }
    else if (arg->type->id == rt->real_type->id) {
        return Builtin::makeIntegerInstanceObject(getRealValue(arg, rt), rt);
    }
    else if (arg->type->id == rt->string_type->id) {
        std::string &str = getStringDataFast(arg);
        try {
            size_t  ptr;
            int64_t res = std::stoll(str, &ptr);
            if (ptr == str.size()) {
                return Builtin::makeIntegerInstanceObject(res, rt);
            }
        } catch (...) {
        }
    }

    rt->signalError("Unsupported conversion");
}

static Object *toBool(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    if (args.size() != 1) {
        rt->signalError("Expected exactly one argument");
    }
    auto arg = args[0];
    if (!isInstanceObject(arg)) {
        rt->signalError("Expected an instance object but got  " + arg->userRepr());
    }
    if (!execution_result_matters) {
        return NULL;
    }

    if (arg->type->id == rt->boolean_type->id) {
        return Builtin::makeBooleanInstanceObject(getBooleanValue(arg, rt), rt);
    }
    else if (arg->type->id == rt->character_type->id) {
        return Builtin::makeBooleanInstanceObject(getCharacterValue(arg, rt), rt);
    }
    else if (arg->type->id == rt->function_type->id) {
        // unsupported
    }
    else if (arg->type->id == rt->integer_type->id) {
        return Builtin::makeBooleanInstanceObject(getIntegerValue(arg, rt), rt);
    }
    else if (arg->type->id == rt->nothing_type->id) {
        return Builtin::makeBooleanInstanceObject(0, rt);
    }
    else if (arg->type->id == rt->real_type->id) {
        return Builtin::makeBooleanInstanceObject(getRealValue(arg, rt), rt);
    }
    else if (arg->type->id == rt->string_type->id) {
        std::string &str = getStringDataFast(arg);
        if (str == "true") {
            return Builtin::makeBooleanInstanceObject(true, rt);
        }
        else if (str == "false") {
            return Builtin::makeBooleanInstanceObject(false, rt);
        }
    }

    rt->signalError("Unsupported conversion");
}

void installBuiltinFunctions(Runtime *rt) {
    ProfilerCAPTURE();
    rt->scope->addVariable(NameId("print").id, makeFunctionInstanceObject(true, print, NULL, rt), rt);
    rt->scope->addVariable(NameId("printr").id, makeFunctionInstanceObject(true, printr, NULL, rt), rt);
    rt->scope->addVariable(NameId("make").id, makeFunctionInstanceObject(true, make, NULL, rt), rt);
    rt->scope->addVariable(NameId("exit").id, makeFunctionInstanceObject(true, exit, NULL, rt), rt);
    rt->scope->addVariable(NameId("exec").id, makeFunctionInstanceObject(true, exec, NULL, rt), rt);
    rt->scope->addVariable(NameId("system").id, makeFunctionInstanceObject(true, system, NULL, rt), rt);
    rt->scope->addVariable(NameId("fork").id, makeFunctionInstanceObject(true, fork, NULL, rt), rt);
    rt->scope->addVariable(NameId("int").id, makeFunctionInstanceObject(true, toInt, NULL, rt), rt);
    rt->scope->addVariable(NameId("bool").id, makeFunctionInstanceObject(true, toBool, NULL, rt), rt);
    rt->scope->addVariable(NameId("argc").id, makeFunctionInstanceObject(true, argc, NULL, rt), rt);
    rt->scope->addVariable(NameId("argg").id, makeFunctionInstanceObject(true, argg, NULL, rt), rt);
}
}    // namespace Cotton::Builtin
