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

#include "front/lexer.h"
#include <cstdint>
#include <string>

namespace Cotton {
class ErrorManager {
private:
    // when an error occurs, this is called. it is expected to NOT return.
    void (*emergency_error_exit)();

    // these can be set without immediately signaling the error
    std::string error_filename;
    int64_t     error_char_pos;
    std::string error_message;

public:
    ErrorManager()  = delete;
    ~ErrorManager() = default;

    ErrorManager(void (*emergency_error_exit)());

    void setErrorFilename(const std::string &filename);
    void setErrorCharPos(int64_t char_pos);
    void setErrorMessage(const std::string &message);
    void clearError();

    void signalError();
    void signalError(const std::string &message);
    void signalError(const std::string &message, int64_t char_pos);

    void signalError(const std::string &message, const Token &token);

    void signalError(const std::vector<std::pair<Token *, std::string>> errors);

    friend void __assert__(bool               value,
                           const char        *assertion,
                           const std::string &message,
                           ErrorManager      *error_manager,
                           const char        *filename,
                           int                line);
};

void __assert__(bool               value,
                const char        *assertion,
                const std::string &message,
                ErrorManager      *error_manager,
                const char        *filename,
                int                line);

#define assert(value, message, error_manager)                                                                     \
    __assert__(value, #value, message, error_manager, __FILE__, __LINE__);

}    // namespace Cotton
