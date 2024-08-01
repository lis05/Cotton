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
#include <dlfcn.h>
#include <filesystem>
#include <unistd.h>

namespace Cotton::Builtin {

// make(type) - makes an object of a given type
static Object *CF_make(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountFunc(args, 1);
    auto arg = args[0];
    rt->verifyIsTypeObject(arg, nullptr, Runtime::SUB1_CTX);

    auto res = rt->make(arg->type, Runtime::INSTANCE_OBJECT);

    if (rt->isValidObject(res) && res->type->hasMethod(MagicMethods::mm__make__(rt))) {
        rt->runMethod(MagicMethods::mm__make__(rt), res, {res}, false);
    }
    return res;
}

// copy(obj) - copies obj
static Object *CF_copy(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountFunc(args, 1);
    auto arg = args[0];
    rt->verifyIsValidObject(arg, Runtime::SUB1_CTX);

    if (arg->type->hasMethod(MagicMethods::mm__copy__(rt))) {
        return rt->runMethod(MagicMethods::mm__copy__(rt), arg, {arg}, true);
    }

    return rt->copy(args[0]);
}

// bool(obj) - converts obj to Boolean
static Object *CF_bool(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountFunc(args, 1);
    auto arg = args[0];
    rt->verifyIsValidObject(arg, Runtime::SUB1_CTX);

    rt->verifyHasMethod(arg, MagicMethods::mm__bool__(rt), Runtime::SUB1_CTX);
    return rt->runMethod(MagicMethods::mm__bool__(rt), arg, {arg}, execution_result_matters);
}

// char(obj) - converts obj to Character
static Object *CF_char(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountFunc(args, 1);
    auto arg = args[0];
    rt->verifyIsValidObject(arg, Runtime::SUB1_CTX);

    rt->verifyHasMethod(arg, MagicMethods::mm__char__(rt), Runtime::SUB1_CTX);
    return rt->runMethod(MagicMethods::mm__char__(rt), arg, {arg}, execution_result_matters);
}

// int(obj) - converts obj to Integer
static Object *CF_int(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountFunc(args, 1);
    auto arg = args[0];
    rt->verifyIsValidObject(arg, Runtime::SUB1_CTX);

    rt->verifyHasMethod(arg, MagicMethods::mm__int__(rt), Runtime::SUB1_CTX);
    return rt->runMethod(MagicMethods::mm__int__(rt), arg, {arg}, execution_result_matters);
}

// real(obj) - converts obj to Real
static Object *CF_real(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountFunc(args, 1);
    auto arg = args[0];
    rt->verifyIsValidObject(arg, Runtime::SUB1_CTX);

    rt->verifyHasMethod(arg, MagicMethods::mm__real__(rt), Runtime::SUB1_CTX);
    return rt->runMethod(MagicMethods::mm__real__(rt), arg, {arg}, execution_result_matters);
}

// string(obj) - converts obj to String
static Object *CF_string(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountFunc(args, 1);
    auto arg = args[0];
    rt->verifyIsValidObject(arg, Runtime::SUB1_CTX);

    rt->verifyHasMethod(arg, MagicMethods::mm__string__(rt), Runtime::SUB1_CTX);
    return rt->runMethod(MagicMethods::mm__string__(rt), arg, {arg}, execution_result_matters);
}

// printraw(...) - prints arguments without adding any spaces or new lines
static Object *CF_printraw(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    int64_t index = 0;
    for (auto arg : args) {
        index++;
        rt->verifyIsValidObject(arg, (Runtime::ContextId)index);
        if (rt->isInstanceObject(arg, rt->builtin_types.string)) {
            std::cout << getStringDataFast(arg);
            continue;
        }

        rt->verifyHasMethod(arg, MagicMethods::mm__repr__(rt), (Runtime::ContextId)index);

        auto &ta = rt->getContext().sub_areas[index];
        rt->newContext();
        rt->getContext().area      = ta;
        rt->getContext().sub_areas = {ta, ta};
        auto res                   = rt->runMethod(MagicMethods::mm__repr__(rt), arg, {arg}, true);
        CF_printraw({res}, rt, false);
        rt->popContext();
    }
    return rt->protectedNothing();
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
    return rt->protectedNothing();
}

// printf(fmt, ...) - prints arguments with a given format
static Object *CF_printf(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    // TODO
}

// println(...) - prints arguments with adding spaces in between them, as well as adding the trailing newline
static Object *CF_println(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    CF_print(args, rt, execution_result_matters);
    std::cout << std::endl;
    return rt->protectedNothing();
}

// readraw() - scans a single character, including spaces and other special characters
static Object *CF_readraw(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountFunc(args, 0);

    char c;
    std::cin >> c;

    if (!execution_result_matters) {
        return rt->protectedNothing();
    }
    return Builtin::makeCharacterInstanceObject(c, rt);
}

// read(type) - scans a value of given type
static Object *CF_read(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountFunc(args, 1);
    auto arg = args[0];
    rt->verifyIsTypeObject(arg, nullptr, Runtime::SUB1_CTX);

    rt->verifyHasMethod(arg, MagicMethods::mm__read__(rt), Runtime::SUB1_CTX);
    return rt->runMethod(MagicMethods::mm__read__(rt), arg, {arg}, execution_result_matters);
}

// readln() - scans an entire line
static Object *CF_readln(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountFunc(args, 0);

    std::string str;
    getline(std::cin, str);

    if (!execution_result_matters) {
        return rt->protectedNothing();
    }
    return Builtin::makeStringInstanceObject(str, rt);
}

// exit(code) - exits with a given code
static Object *CF_exit(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountFunc(args, 1);
    auto arg = args[0];
    rt->verifyIsInstanceObject(arg, rt->builtin_types.integer, Runtime::SUB1_CTX);

    exit(getIntegerValueFast(arg));
}

// fork() - does fork()
static Object *CF_fork(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountFunc(args, 0);

    int res = fork();
    if (!execution_result_matters) {
        return rt->protectedNothing();
    }

    return makeIntegerInstanceObject(res, rt);
}

// system(str) - does system(str)
static Object *CF_system(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountFunc(args, 1);
    auto arg = args[0];
    rt->verifyIsInstanceObject(arg, rt->builtin_types.string, Runtime::SUB1_CTX);

    auto ret = system(getStringDataFast(arg).c_str());
    if (!execution_result_matters) {
        return rt->protectedNothing();
    }

    return Builtin::makeIntegerInstanceObject(ret, rt);
}

// sleep(tm) - sleems for the given amount of seconds
static Object *CF_sleep(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountFunc(args, 1);
    auto arg = args[0];
    rt->verifyIsInstanceObject(arg, rt->builtin_types.real, Runtime::SUB1_CTX);

    ::usleep(getRealValueFast(arg) * 100'000);

    return rt->protectedNothing();
}

// error(msg) - signals an error with the given message
static Object *CF_error(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountFunc(args, 1);
    auto arg = args[0];
    rt->verifyIsInstanceObject(arg, rt->builtin_types.string, Runtime::SUB1_CTX);

    rt->signalError(getStringDataFast(arg), rt->getContext().area);
}

// cotton(str) - executes cotton code given in the string
static Object *CF_cotton(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountFunc(args, 1);
    auto arg = args[0];
    rt->verifyIsInstanceObject(arg, rt->builtin_types.string, Runtime::SUB1_CTX);

    Lexer        lexer(rt->getErrorManager());
    std::string &str = getStringDataFast(arg);

    std::vector<Token> tokens = lexer.process(str);
    for (auto &token : tokens) {
        token.nameid     = rt->nmgr->getId(token.data);
        // yeah this is awful but it works so shut up
        token.begin_pos += rt->getContext().sub_areas[1].first_char + 1;
        token.end_pos   += rt->getContext().sub_areas[1].first_char + 1;
    }

    Parser    parser(rt->getErrorManager());
    StmtNode *program = parser.parse(tokens);

    return rt->execute(program, execution_result_matters);
}

// argc() - returns the number of arguments passed to the current function
static Object *CF_argc(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountFunc(args, 0);

    if (!execution_result_matters) {
        return nullptr;
    }

    auto s = rt->getScope()->prev;
    while (s != nullptr && s->can_access_prev) {
        s = s->prev;
    }
    if (s == nullptr) {
        return Builtin::makeIntegerInstanceObject(0, rt);
    }
    return Builtin::makeIntegerInstanceObject(s->arguments.size(), rt);
}

// argv() - returns an array made of arguments passed to the current functions
static Object *CF_argv(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountFunc(args, 0);

    if (!execution_result_matters) {
        return nullptr;
    }

    auto s = rt->getScope()->prev;
    while (s != nullptr && s->can_access_prev) {
        s = s->prev;
    }
    if (s == nullptr) {
        return Builtin::makeArrayInstanceObject({}, rt);
    }
    return Builtin::makeArrayInstanceObject(s->arguments, rt);
}

// argg(index) - returns a function arguments at position index
static Object *CF_argg(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountFunc(args, 1);
    auto arg = args[0];
    rt->verifyIsInstanceObject(arg, rt->builtin_types.integer, Runtime::SUB1_CTX);

    if (!execution_result_matters) {
        return nullptr;
    }

    int64_t i = getIntegerValue(arg, rt);

    auto s = rt->getScope()->prev;
    while (s != nullptr && s->can_access_prev) {
        s = s->prev;
    }
    if (s == nullptr || i >= s->arguments.size()) {
        return rt->protectedNothing();
    }
    if (i < s->arguments.size()) {
        return s->arguments[i];
    }
    else {
        rt->signalError("Argument index is out of range:" + arg->userRepr(rt), rt->getContext().sub_areas[1]);
    }
}

// typeof(obj) - returns a type object with type of obj
static Object *CF_typeof(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountFunc(args, 1);
    auto arg = args[0];
    rt->verifyIsValidObject(arg, Runtime::SUB1_CTX);

    return rt->getTypeObject(arg->type);
}

// isinsobj(obj, type) - tells whether obj is an instance object of the given type (or any type if nothing is
// given)
static Object *CF_isinsobj(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyMinArgsAmountFunc(args, 1);
    auto arg = args[0];
    rt->verifyIsValidObject(arg, Runtime::SUB1_CTX);

    if (args.size() == 2) {
        auto type = args[1];
        rt->verifyIsTypeObject(type, nullptr, Runtime::SUB2_CTX);

        if (!execution_result_matters) {
            return rt->protectedNothing();
        }

        return rt->protectedBoolean(rt->isInstanceObject(arg, type->type));
    }

    return rt->protectedBoolean(rt->isInstanceObject(arg, nullptr));
}

// istypeobj(obj, type) - tells whether obj is a type object of the given type (or any type if nothing is
// given)
static Object *CF_istypeobj(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyMinArgsAmountFunc(args, 1);
    auto arg = args[0];
    rt->verifyIsValidObject(arg, Runtime::SUB1_CTX);

    if (args.size() == 2) {
        auto type = args[1];
        rt->verifyIsTypeObject(type, nullptr, Runtime::SUB2_CTX);

        if (!execution_result_matters) {
            return rt->protectedNothing();
        }

        return rt->protectedBoolean(rt->isInstanceObject(arg, type->type));
    }

    return rt->protectedBoolean(rt->isInstanceObject(arg, nullptr));
}

// repr(obj) - gives a string representation of obj
static Object *CF_repr(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountFunc(args, 1);
    auto arg = args[0];
    rt->verifyIsValidObject(arg, Runtime::SUB1_CTX);

    rt->verifyHasMethod(arg, MagicMethods::mm__repr__(rt), Runtime::SUB1_CTX);
    return rt->runMethod(MagicMethods::mm__repr__(rt), arg, {arg}, execution_result_matters);
}

// hasfield(obj, str) - tells whether obj has field given in str
static Object *CF_hasfield(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountFunc(args, 2);
    auto obj = args[0];
    auto str = args[1];
    rt->verifyIsInstanceObject(obj, nullptr, Runtime::SUB1_CTX);
    rt->verifyIsInstanceObject(str, rt->builtin_types.string, Runtime::SUB2_CTX);

    if (!execution_result_matters) {
        return rt->protectedNothing();
    }
    return rt->protectedBoolean(obj->instance->hasField(rt->nmgr->getId(getStringDataFast(str)), rt));
}

// hasmethod(obj, str) - tells whether obj has method given in str
static Object *CF_hasmethod(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountFunc(args, 2);
    auto obj = args[0];
    auto str = args[1];
    rt->verifyIsValidObject(obj, Runtime::SUB1_CTX);
    rt->verifyIsInstanceObject(str, rt->builtin_types.string, Runtime::SUB2_CTX);

    if (!execution_result_matters) {
        return rt->protectedNothing();
    }
    return rt->protectedBoolean(obj->type->hasMethod(rt->nmgr->getId(getStringDataFast(str))));
}

// assert(val, str) - raises an error given in str(or "assertion error" is str is absent) if value is not true
static Object *CF_assert(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyMinArgsAmountFunc(args, 1);
    auto val = args[0];
    rt->verifyIsInstanceObject(val, rt->builtin_types.boolean, Runtime::SUB1_CTX);

    std::string message = "Assertion error";

    if (args.size() == 2) {
        auto str = args[1];
        rt->verifyIsInstanceObject(str, rt->builtin_types.string, Runtime::SUB2_CTX);

        message += ": " + getStringDataFast(str);
    }

    if (!getBooleanValueFast(val)) {
        rt->signalError(message, rt->getContext().area);
    }

    if (!execution_result_matters) {
        return rt->protectedNothing();
    }
}

// checkglobal(str) - returns whether global variable with name str was defined
static Object *CF_checkglobal(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountFunc(args, 1);
    auto arg = args[0];
    rt->verifyIsInstanceObject(arg, rt->builtin_types.string, Runtime::SUB1_CTX);

    return rt->protectedBoolean(rt->checkGlobal(rt->nmgr->getId(getStringDataFast(arg))));
}

// getglobal(str) - returns global variable with name str
static Object *CF_getglobal(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountFunc(args, 1);
    auto arg = args[0];
    rt->verifyIsInstanceObject(arg, rt->builtin_types.string, Runtime::SUB1_CTX);

    return rt->getGlobal(rt->nmgr->getId(getStringDataFast(arg)));
}

// setglobal(str, obj) - sets global variable with name str to obj
static Object *CF_setglobal(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountFunc(args, 2);
    auto arg1 = args[0];
    auto arg2 = args[1];
    rt->verifyIsInstanceObject(arg1, rt->builtin_types.string, Runtime::SUB1_CTX);
    rt->verifyIsValidObject(arg2, Runtime::SUB2_CTX);

    rt->setGlobal(rt->nmgr->getId(getStringDataFast(arg1)), arg2);
    return arg2;
}

// removeglobal(str) - removes global variable with name str
static Object *CF_removeglobal(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountFunc(args, 1);
    auto arg1 = args[0];
    rt->verifyIsInstanceObject(arg1, rt->builtin_types.string, Runtime::SUB1_CTX);

    rt->removeGlobal(rt->nmgr->getId(getStringDataFast(arg1)));
    return rt->protectedNothing();
}

// smartrun(file) - runs file if it hasn't been run before; returns the result of running it.
static Object *CF_smartrun(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountFunc(args, 1);
    auto arg1 = args[0];
    rt->verifyIsInstanceObject(arg1, rt->builtin_types.string, Runtime::SUB1_CTX);

    std::error_code ec;
    auto            path = std::filesystem::canonical(std::filesystem::path(getStringDataFast(arg1)), ec);
    if (ec) {
        rt->signalError("Invalid path: " + ec.message(), rt->getContext().sub_areas[1]);
    }
    if (!std::filesystem::is_regular_file(path)) {
        rt->signalError("Not a regular file: " + path.string(), rt->getContext().sub_areas[1]);
    }

    auto id = rt->nmgr->getId("cotton smartrun: " + path.string());
    if (rt->checkGlobal(id)) {
        return rt->getGlobal(id);
    }

    Lexer  lexer(rt->getErrorManager());
    Parser parser(rt->getErrorManager());

    auto tokens = lexer.processFile(path.c_str());
    for (auto &token : tokens) {
        token.nameid = rt->nmgr->getId(token.data);
    }
    auto program = parser.parse(tokens);
    rt->setGlobal(id, rt->protectedNothing());

    rt->newScopeFrame();
    auto res = rt->execute(program, true);
    rt->popScopeFrame();

    rt->verifyIsValidObject(res);
    rt->setGlobal(id, res);
    return res;
}

// dumbrun(file) - runs file and returns the result of running it.
static Object *CF_dumbrun(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountFunc(args, 1);
    auto arg1 = args[0];
    rt->verifyIsInstanceObject(arg1, rt->builtin_types.string, Runtime::SUB1_CTX);

    std::error_code ec;
    auto            path = std::filesystem::canonical(std::filesystem::path(getStringDataFast(arg1)), ec);
    if (ec) {
        rt->signalError("Invalid path: " + ec.message(), rt->getContext().sub_areas[1]);
    }
    if (!std::filesystem::is_regular_file(path) || !std::filesystem::exists(path)) {
        rt->signalError("Not a regular file: " + path.string(), rt->getContext().sub_areas[1]);
    }

    Lexer  lexer(rt->getErrorManager());
    Parser parser(rt->getErrorManager());

    auto tokens = lexer.processFile(path.c_str());
    for (auto &token : tokens) {
        token.nameid = rt->nmgr->getId(token.data);
    }
    auto program = parser.parse(tokens);

    rt->newScopeFrame();
    auto res = rt->execute(program, true);
    rt->popScopeFrame();

    rt->verifyIsValidObject(res);
    return res;
}

//  loadlibrary(file) - loads C++ shared library
static Object *CF_loadlibrary(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    ProfilerCAPTURE();
    rt->verifyExactArgsAmountFunc(args, 1);
    auto arg1 = args[0];
    rt->verifyIsInstanceObject(arg1, rt->builtin_types.string, Runtime::SUB1_CTX);

    std::error_code ec;
    auto            path = std::filesystem::canonical(std::filesystem::path(getStringDataFast(arg1)), ec);
    if (ec) {
        rt->signalError("Invalid path: " + ec.message(), rt->getContext().sub_areas[1]);
    }
    if (!std::filesystem::is_regular_file(path)) {
        rt->signalError("Not a regular file: " + path.string(), rt->getContext().sub_areas[1]);
    }

    auto id = rt->nmgr->getId("cotton loadlibrary: " + path.string());
    if (rt->checkGlobal(id)) {
        return rt->getGlobal(id);
    }
    rt->setGlobal(id, rt->protectedNothing());

    void *lib = dlopen(path.c_str(), RTLD_LAZY | RTLD_DEEPBIND);

    auto llp = reinterpret_cast<LibraryLoadPoint>(dlsym(lib, "library_load_point"));
    if (llp == nullptr) {
        rt->signalError("Failed to load library " + path.string(), rt->getContext().area);
    }
    auto res = llp(rt);

    rt->verifyIsValidObject(res);
    rt->setGlobal(id, res);
    return res;
}

void installBuiltinFunctions(Runtime *rt) {
    ProfilerCAPTURE();
    rt->getScope()->addVariable(rt->nmgr->getId("make"),
                                makeFunctionInstanceObject(true, CF_make, nullptr, rt),
                                rt);
    rt->getScope()->addVariable(rt->nmgr->getId("copy"),
                                makeFunctionInstanceObject(true, CF_copy, nullptr, rt),
                                rt);
    rt->getScope()->addVariable(rt->nmgr->getId("bool"),
                                makeFunctionInstanceObject(true, CF_bool, nullptr, rt),
                                rt);
    rt->getScope()->addVariable(rt->nmgr->getId("char"),
                                makeFunctionInstanceObject(true, CF_char, nullptr, rt),
                                rt);
    rt->getScope()->addVariable(rt->nmgr->getId("int"), makeFunctionInstanceObject(true, CF_int, nullptr, rt), rt);
    rt->getScope()->addVariable(rt->nmgr->getId("real"),
                                makeFunctionInstanceObject(true, CF_real, nullptr, rt),
                                rt);
    rt->getScope()->addVariable(rt->nmgr->getId("string"),
                                makeFunctionInstanceObject(true, CF_string, nullptr, rt),
                                rt);
    rt->getScope()->addVariable(rt->nmgr->getId("printraw"),
                                makeFunctionInstanceObject(true, CF_printraw, nullptr, rt),
                                rt);
    rt->getScope()->addVariable(rt->nmgr->getId("print"),
                                makeFunctionInstanceObject(true, CF_print, nullptr, rt),
                                rt);
    rt->getScope()->addVariable(rt->nmgr->getId("printf"),
                                makeFunctionInstanceObject(true, CF_printf, nullptr, rt),
                                rt);
    rt->getScope()->addVariable(rt->nmgr->getId("println"),
                                makeFunctionInstanceObject(true, CF_println, nullptr, rt),
                                rt);
    rt->getScope()->addVariable(rt->nmgr->getId("readraw"),
                                makeFunctionInstanceObject(true, CF_readraw, nullptr, rt),
                                rt);
    rt->getScope()->addVariable(rt->nmgr->getId("read"),
                                makeFunctionInstanceObject(true, CF_read, nullptr, rt),
                                rt);
    rt->getScope()->addVariable(rt->nmgr->getId("readln"),
                                makeFunctionInstanceObject(true, CF_readln, nullptr, rt),
                                rt);
    rt->getScope()->addVariable(rt->nmgr->getId("exit"),
                                makeFunctionInstanceObject(true, CF_exit, nullptr, rt),
                                rt);
    rt->getScope()->addVariable(rt->nmgr->getId("fork"),
                                makeFunctionInstanceObject(true, CF_fork, nullptr, rt),
                                rt);
    rt->getScope()->addVariable(rt->nmgr->getId("system"),
                                makeFunctionInstanceObject(true, CF_system, nullptr, rt),
                                rt);
    rt->getScope()->addVariable(rt->nmgr->getId("sleep"),
                                makeFunctionInstanceObject(true, CF_sleep, nullptr, rt),
                                rt);
    rt->getScope()->addVariable(rt->nmgr->getId("error"),
                                makeFunctionInstanceObject(true, CF_error, nullptr, rt),
                                rt);
    rt->getScope()->addVariable(rt->nmgr->getId("cotton"),
                                makeFunctionInstanceObject(true, CF_cotton, nullptr, rt),
                                rt);
    rt->getScope()->addVariable(rt->nmgr->getId("argc"),
                                makeFunctionInstanceObject(true, CF_argc, nullptr, rt),
                                rt);
    rt->getScope()->addVariable(rt->nmgr->getId("argv"),
                                makeFunctionInstanceObject(true, CF_argv, nullptr, rt),
                                rt);
    rt->getScope()->addVariable(rt->nmgr->getId("argg"),
                                makeFunctionInstanceObject(true, CF_argg, nullptr, rt),
                                rt);
    rt->getScope()->addVariable(rt->nmgr->getId("typeof"),
                                makeFunctionInstanceObject(true, CF_typeof, nullptr, rt),
                                rt);
    rt->getScope()->addVariable(rt->nmgr->getId("isinsobj"),
                                makeFunctionInstanceObject(true, CF_isinsobj, nullptr, rt),
                                rt);
    rt->getScope()->addVariable(rt->nmgr->getId("istypeobj"),
                                makeFunctionInstanceObject(true, CF_istypeobj, nullptr, rt),
                                rt);
    rt->getScope()->addVariable(rt->nmgr->getId("repr"),
                                makeFunctionInstanceObject(true, CF_repr, nullptr, rt),
                                rt);
    rt->getScope()->addVariable(rt->nmgr->getId("hasfield"),
                                makeFunctionInstanceObject(true, CF_hasfield, nullptr, rt),
                                rt);
    rt->getScope()->addVariable(rt->nmgr->getId("hasmethod"),
                                makeFunctionInstanceObject(true, CF_hasmethod, nullptr, rt),
                                rt);
    rt->getScope()->addVariable(rt->nmgr->getId("assert"),
                                makeFunctionInstanceObject(true, CF_assert, nullptr, rt),
                                rt);
    rt->getScope()->addVariable(rt->nmgr->getId("checkglobal"),
                                makeFunctionInstanceObject(true, CF_checkglobal, nullptr, rt),
                                rt);
    rt->getScope()->addVariable(rt->nmgr->getId("getglobal"),
                                makeFunctionInstanceObject(true, CF_getglobal, nullptr, rt),
                                rt);
    rt->getScope()->addVariable(rt->nmgr->getId("setglobal"),
                                makeFunctionInstanceObject(true, CF_setglobal, nullptr, rt),
                                rt);
    rt->getScope()->addVariable(rt->nmgr->getId("removeglobal"),
                                makeFunctionInstanceObject(true, CF_removeglobal, nullptr, rt),
                                rt);
    rt->getScope()->addVariable(rt->nmgr->getId("smartrun"),
                                makeFunctionInstanceObject(true, CF_smartrun, nullptr, rt),
                                rt);
    rt->getScope()->addVariable(rt->nmgr->getId("dumbrun"),
                                makeFunctionInstanceObject(true, CF_dumbrun, nullptr, rt),
                                rt);
    rt->getScope()->addVariable(rt->nmgr->getId("loadlibrary"),
                                makeFunctionInstanceObject(true, CF_loadlibrary, nullptr, rt),
                                rt);
}
}    // namespace Cotton::Builtin
