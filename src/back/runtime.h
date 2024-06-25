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

#pragma once

#include <string>
#include <vector>

namespace Cotton {
class Scope;
class Stack;
class GC;
class Object;
class Instance;
class Type;
class ErrorManager;
class Token;

class Runtime {
public:
    Scope *scope;
    void   newScope();
    void   popScope();

    Stack *stack;
    GC    *gc;

    ErrorManager            *error_manager;
    std::vector<std::string> error_messages;
    void                     highlight(Token *token);
    void                     signalError(const std::string &message);

    class State {
    public:
        Token *token;    // used for error logging
    };

    std::vector<State> states;
    void               saveState();
    void               restoreState();

    class ExecutionResult {
    public:
        bool        success;
        Object     *res;
        std::string error_message;

        void verify(Runtime *rt, const std::string error_message);
    };
};
}    // namespace Cotton
