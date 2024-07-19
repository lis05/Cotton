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

// make(type) - makes an object of a given type
static Object *CF_make(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountFunc(args, 1);
    auto arg = args[0];
    rt->verifyIsTypeObject(arg, NULL, rt->getContext().sub_areas[1]);

    auto res = rt->make(arg->type, Runtime::INSTANCE_OBJECT);

    if (rt->isValidObject(res) && res->type->hasMethod(MagicMethods::mm__make__())) {
        rt->runMethod(MagicMethods::mm__make__(), res, {res}, false);
    }
    return res;
}

// copy(obj) - copies obj
static Object *CF_copy(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountFunc(args, 1);
    auto arg = args[0];
    rt->verifyIsValidObject(arg, rt->getContext().sub_areas[1]);

    if (arg->type->hasMethod(MagicMethods::mm__copy__())) {
        return rt->runMethod(MagicMethods::mm__copy__(), arg, {arg}, true);
    }

    return rt->copy(args[0]);
}

// bool(obj) - converts obj to Boolean
static Object *CF_bool(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountFunc(args, 1);
    auto arg = args[0];
    rt->verifyIsValidObject(arg, rt->getContext().sub_areas[1]);

    rt->verifyHasMethod(arg, MagicMethods::mm__bool__(), rt->getContext().sub_areas[1]);
    return rt->runMethod(MagicMethods::mm__bool__(), arg, {arg}, execution_result_matters);
}

// char(obj) - converts obj to Character
static Object *CF_char(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountFunc(args, 1);
    auto arg = args[0];
    rt->verifyIsValidObject(arg, rt->getContext().sub_areas[1]);

    rt->verifyHasMethod(arg, MagicMethods::mm__char__(), rt->getContext().sub_areas[1]);
    return rt->runMethod(MagicMethods::mm__char__(), arg, {arg}, execution_result_matters);
}

// int(obj) - converts obj to Integer
static Object *CF_int(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountFunc(args, 1);
    auto arg = args[0];
    rt->verifyIsValidObject(arg, rt->getContext().sub_areas[1]);

    rt->verifyHasMethod(arg, MagicMethods::mm__int__(), rt->getContext().sub_areas[1]);
    return rt->runMethod(MagicMethods::mm__int__(), arg, {arg}, execution_result_matters);
}

// real(obj) - converts obj to Real
static Object *CF_real(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountFunc(args, 1);
    auto arg = args[0];
    rt->verifyIsValidObject(arg, rt->getContext().sub_areas[1]);

    rt->verifyHasMethod(arg, MagicMethods::mm__real__(), rt->getContext().sub_areas[1]);
    return rt->runMethod(MagicMethods::mm__real__(), arg, {arg}, execution_result_matters);
}

// string(obj) - converts obj to String
static Object *CF_string(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountFunc(args, 1);
    auto arg = args[0];
    rt->verifyIsValidObject(arg, rt->getContext().sub_areas[1]);

    rt->verifyHasMethod(arg, MagicMethods::mm__string__(), rt->getContext().sub_areas[1]);
    return rt->runMethod(MagicMethods::mm__string__(), arg, {arg}, execution_result_matters);
}

// printraw(...) - prints arguments without adding any spaces or new lines
static Object *CF_printraw(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    int64_t index = 0;
    for (auto arg : args) {
        index++;
        rt->verifyIsValidObject(arg, rt->getContext().sub_areas[index]);
        if (rt->isInstanceObject(arg, rt->string_type)) {
            std::cout << getStringDataFast(arg);
            continue;
        }

        rt->verifyHasMethod(arg, MagicMethods::mm__repr__(), rt->getContext().sub_areas[index]);

        auto &ta = rt->getContext().sub_areas[index];
        rt->newContext();
        rt->getContext().area      = ta;
        rt->getContext().sub_areas = {ta, ta};
        auto res                   = rt->runMethod(MagicMethods::mm__repr__(), arg, {arg}, true);
        CF_printraw({res}, rt, false);
        rt->popContext();
    }
    return rt->protected_nothing;
}

// print(...) - prints arguments with adding spaces in between them, but not adding the trailing newline
static Object *CF_print(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    bool    f     = false;
    int64_t index = 0;
    for (auto arg : args) {
        index++;
        if (f) {
            std::cout << " ";
        }
        f         = true;
        auto &ata = rt->getContext().area;
        auto &fta = rt->getContext().sub_areas[0];
        auto &ita = rt->getContext().sub_areas[index];
        rt->newContext();
        rt->getContext().area      = ata;
        rt->getContext().sub_areas = {fta, ita};
        CF_printraw({arg}, rt, false);
        rt->popContext();
    }
    return rt->protected_nothing;
}

// printf(fmt, ...) - prints arguments with a given format
static Object *CF_printf(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    // TODO
}

// println(...) - prints arguments with adding spaces in between them, as well as adding the trailing newline
static Object *CF_println(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    if (args[0]->id > 1e14) {
        std::cout << "FUCKED UP!\n";
    }
    CF_print(args, rt, execution_result_matters);
    std::cout << std::endl;
    return rt->protected_nothing;
}

// readraw() - scans a single character, including spaces and other special characters
static Object *CF_readraw(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountFunc(args, 0);

    char c;
    std::cin >> c;

    if (!execution_result_matters) {
        return rt->protected_nothing;
    }
    return Builtin::makeCharacterInstanceObject(c, rt);
}

// read(type) - scans a value of given type
static Object *CF_read(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountFunc(args, 1);
    auto arg = args[0];
    rt->verifyIsTypeObject(arg, NULL, rt->getContext().sub_areas[1]);

    rt->verifyHasMethod(arg, MagicMethods::mm__read__(), rt->getContext().sub_areas[1]);
    return rt->runMethod(MagicMethods::mm__read__(), arg, {arg}, execution_result_matters);
}

// readln() - scans an entire line
static Object *CF_readln(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountFunc(args, 0);

    std::string str;
    getline(std::cin, str);

    if (!execution_result_matters) {
        return rt->protected_nothing;
    }
    return Builtin::makeStringInstanceObject(str, rt);
}

// exit(code) - exits with a given code
static Object *CF_exit(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountFunc(args, 1);
    auto arg = args[0];
    rt->verifyIsInstanceObject(arg, rt->integer_type, rt->getContext().sub_areas[1]);

    exit(getIntegerValueFast(arg));
}

// fork() - does fork()
static Object *CF_fork(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountFunc(args, 0);

    fork();
}

// system(str) - does system(str)
static Object *CF_system(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountFunc(args, 1);
    auto arg = args[0];
    rt->verifyIsInstanceObject(arg, rt->string_type, rt->getContext().sub_areas[1]);

    auto ret = system(getStringDataFast(arg).c_str());
    if (!execution_result_matters) {
        return rt->protected_nothing;
    }

    return Builtin::makeIntegerInstanceObject(ret, rt);
}

// error(msg) - signals an error with the given message
static Object *CF_error(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountFunc(args, 1);
    auto arg = args[0];
    rt->verifyIsInstanceObject(arg, rt->string_type, rt->getContext().sub_areas[1]);

    rt->signalError(getStringDataFast(arg), rt->getContext().area);
}

// cotton(str) - executes cotton code given in the string
static Object *CF_cotton(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountFunc(args, 1);
    auto arg = args[0];
    rt->verifyIsInstanceObject(arg, rt->string_type, rt->getContext().sub_areas[1]);

    Lexer        lexer(rt->error_manager);
    std::string &str = getStringDataFast(arg);

    std::vector<Token> tokens = lexer.process(str);
    for (auto &token : tokens) {
        token.nameid     = NameId(&token).id;
        // yeah this is awful but it works so shut up
        token.begin_pos += rt->getContext().sub_areas[1].first_char + 1;
        token.end_pos   += rt->getContext().sub_areas[1].first_char + 1;
    }

    Parser    parser(rt->error_manager);
    StmtNode *program = parser.parse(tokens);

    return rt->execute(program, execution_result_matters);
}

// argc() - returns the number of arguments passed to the current function
static Object *CF_argc(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountFunc(args, 0);

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

// argv() - returns an array made of arguments passed to the current functions
static Object *CF_argv(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountFunc(args, 0);

    if (!execution_result_matters) {
        return NULL;
    }

    auto s = rt->scope->prev;
    while (s != NULL && s->can_access_prev) {
        s = s->prev;
    }
    if (s == NULL) {
        return Builtin::makeArrayInstanceObject({}, rt);
    }
    return Builtin::makeArrayInstanceObject(s->arguments, rt);
}

// argg(index) - returns a function arguments at position index
static Object *CF_argg(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountFunc(args, 1);
    auto arg = args[0];
    rt->verifyIsInstanceObject(arg, rt->integer_type, rt->getContext().sub_areas[1]);

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
    if (i < s->arguments.size()) {
        return s->arguments[i];
    }
    else {
        rt->signalError("Argument index is out of range:" + arg->userRepr(), rt->getContext().sub_areas[1]);
    }
}

// typeof(obj) - returns a type object with type of obj
static Object *CF_typeof(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountFunc(args, 1);
    auto arg = args[0];
    rt->verifyIsValidObject(arg, rt->getContext().sub_areas[1]);

    return rt->getTypeObject(arg->type);
}

// isinsobj(obj, type) - tells whether obj is an instance object of the given type (or any type if nothing is
// given)
static Object *CF_isinsobj(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyMinArgsAmountFunc(args, 1);
    auto arg = args[0];
    rt->verifyIsValidObject(arg, rt->getContext().sub_areas[1]);

    if (args.size() == 2) {
        auto type = args[1];
        rt->verifyIsTypeObject(type, NULL, rt->getContext().sub_areas[2]);

        if (!execution_result_matters) {
            return rt->protected_nothing;
        }

        return rt->protectedBoolean(rt->isInstanceObject(arg, type->type));
    }

    return rt->protectedBoolean(rt->isInstanceObject(arg, NULL));
}

// istypeobj(obj, type) - tells whether obj is a type object of the given type (or any type if nothing is
// given)
static Object *CF_istypeobj(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyMinArgsAmountFunc(args, 1);
    auto arg = args[0];
    rt->verifyIsValidObject(arg, rt->getContext().sub_areas[1]);

    if (args.size() == 2) {
        auto type = args[1];
        rt->verifyIsTypeObject(type, NULL, rt->getContext().sub_areas[2]);

        if (!execution_result_matters) {
            return rt->protected_nothing;
        }

        return rt->protectedBoolean(rt->isInstanceObject(arg, type->type));
    }

    return rt->protectedBoolean(rt->isInstanceObject(arg, NULL));
}

// repr(obj) - gives a string representation of obj
static Object *CF_repr(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountFunc(args, 1);
    auto arg = args[0];
    rt->verifyIsValidObject(arg, rt->getContext().sub_areas[1]);

    rt->verifyHasMethod(arg, MagicMethods::mm__repr__(), rt->getContext().sub_areas[1]);
    return rt->runMethod(MagicMethods::mm__repr__(), arg, {arg}, execution_result_matters);
}

// hasfield(obj, str) - tells whether obj has field given in str
static Object *CF_hasfield(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountFunc(args, 2);
    auto obj = args[0];
    auto str = args[1];
    rt->verifyIsInstanceObject(obj, NULL, rt->getContext().sub_areas[1]);
    rt->verifyIsInstanceObject(str, rt->string_type, rt->getContext().sub_areas[2]);

    if (!execution_result_matters) {
        return rt->protected_nothing;
    }
    return rt->protectedBoolean(obj->instance->hasField(NameId(getStringDataFast(str)).id, rt));
}

// hasmethod(obj, str) - tells whether obj has method given in str
static Object *CF_hasmethod(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountFunc(args, 2);
    auto obj = args[0];
    auto str = args[1];
    rt->verifyIsValidObject(obj, rt->getContext().sub_areas[1]);
    rt->verifyIsInstanceObject(str, rt->string_type, rt->getContext().sub_areas[2]);

    if (!execution_result_matters) {
        return rt->protected_nothing;
    }
    return rt->protectedBoolean(obj->type->hasMethod(NameId(getStringDataFast(str)).id));
}

// assert(val, str) - raises an error given in str(or "assertion error" is str is absent) if value is not true
static Object *CF_assert(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyMinArgsAmountFunc(args, 1);
    auto val = args[0];
    rt->verifyIsInstanceObject(val, rt->boolean_type, rt->getContext().sub_areas[1]);

    std::string message = "Assertion error";

    if (args.size() == 2) {
        auto str = args[1];
        rt->verifyIsInstanceObject(str, rt->string_type, rt->getContext().sub_areas[2]);

        message += ": " + getStringDataFast(str);
    }

    if (!getBooleanValueFast(val)) {
        rt->signalError(message, rt->getContext().area);
    }

    if (!execution_result_matters) {
        return rt->protected_nothing;
    }
}

void installBuiltinFunctions(Runtime *rt) {
    ProfilerCAPTURE();
    rt->scope->addVariable(NameId("make").id, makeFunctionInstanceObject(true, CF_make, NULL, rt), rt);
    rt->scope->addVariable(NameId("copy").id, makeFunctionInstanceObject(true, CF_copy, NULL, rt), rt);
    rt->scope->addVariable(NameId("bool").id, makeFunctionInstanceObject(true, CF_bool, NULL, rt), rt);
    rt->scope->addVariable(NameId("char").id, makeFunctionInstanceObject(true, CF_char, NULL, rt), rt);
    rt->scope->addVariable(NameId("int").id, makeFunctionInstanceObject(true, CF_int, NULL, rt), rt);
    rt->scope->addVariable(NameId("real").id, makeFunctionInstanceObject(true, CF_real, NULL, rt), rt);
    rt->scope->addVariable(NameId("string").id, makeFunctionInstanceObject(true, CF_string, NULL, rt), rt);
    rt->scope->addVariable(NameId("printraw").id, makeFunctionInstanceObject(true, CF_printraw, NULL, rt), rt);
    rt->scope->addVariable(NameId("print").id, makeFunctionInstanceObject(true, CF_print, NULL, rt), rt);
    rt->scope->addVariable(NameId("printf").id, makeFunctionInstanceObject(true, CF_printf, NULL, rt), rt);
    rt->scope->addVariable(NameId("println").id, makeFunctionInstanceObject(true, CF_println, NULL, rt), rt);
    rt->scope->addVariable(NameId("readraw").id, makeFunctionInstanceObject(true, CF_readraw, NULL, rt), rt);
    rt->scope->addVariable(NameId("read").id, makeFunctionInstanceObject(true, CF_read, NULL, rt), rt);
    rt->scope->addVariable(NameId("readln").id, makeFunctionInstanceObject(true, CF_readln, NULL, rt), rt);
    rt->scope->addVariable(NameId("exit").id, makeFunctionInstanceObject(true, CF_exit, NULL, rt), rt);
    rt->scope->addVariable(NameId("fork").id, makeFunctionInstanceObject(true, CF_fork, NULL, rt), rt);
    rt->scope->addVariable(NameId("system").id, makeFunctionInstanceObject(true, CF_system, NULL, rt), rt);
    rt->scope->addVariable(NameId("error").id, makeFunctionInstanceObject(true, CF_error, NULL, rt), rt);
    rt->scope->addVariable(NameId("cotton").id, makeFunctionInstanceObject(true, CF_cotton, NULL, rt), rt);
    rt->scope->addVariable(NameId("argc").id, makeFunctionInstanceObject(true, CF_argc, NULL, rt), rt);
    rt->scope->addVariable(NameId("argv").id, makeFunctionInstanceObject(true, CF_argv, NULL, rt), rt);
    rt->scope->addVariable(NameId("argg").id, makeFunctionInstanceObject(true, CF_argg, NULL, rt), rt);
    rt->scope->addVariable(NameId("typeof").id, makeFunctionInstanceObject(true, CF_typeof, NULL, rt), rt);
    rt->scope->addVariable(NameId("isinsobj").id, makeFunctionInstanceObject(true, CF_isinsobj, NULL, rt), rt);
    rt->scope->addVariable(NameId("istypeobj").id, makeFunctionInstanceObject(true, CF_istypeobj, NULL, rt), rt);
    rt->scope->addVariable(NameId("repr").id, makeFunctionInstanceObject(true, CF_repr, NULL, rt), rt);
    rt->scope->addVariable(NameId("hasfield").id, makeFunctionInstanceObject(true, CF_hasfield, NULL, rt), rt);
    rt->scope->addVariable(NameId("hasmethod").id, makeFunctionInstanceObject(true, CF_hasmethod, NULL, rt), rt);
    rt->scope->addVariable(NameId("assert").id, makeFunctionInstanceObject(true, CF_assert, NULL, rt), rt);
}
}    // namespace Cotton::Builtin
