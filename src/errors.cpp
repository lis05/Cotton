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

#include "errors.h"
#include "lexer.h"

static void printColor(FILE *fd, std::string color = "reset") {
    std::string col = "\033[0m";

    if (color == "blue") {
        col = "\033[0;34m";
    }
    else if (color == "green") {
        col = "\033[0;32m";
    }
    else if (color == "cyan") {
        col = "\033[0;36m";
    }
    else if (color == "red") {
        col = "\033[0;31m";
    }
    else if (color == "magenta") {
        col = "\033[0;35m";
    }
    else if (color == "yellow") {
        col = "\033[0;33m";
    }

    fprintf(fd, "%s", col.c_str());
}

namespace Cotton {
ErrorManager::ErrorManager(void (*emergency_error_exit)()) {
    this->emergency_error_exit = emergency_error_exit;
    this->clearError();
}

void ErrorManager::setErrorFilename(const std::string &filename) {
    this->error_filename = filename;
}

void ErrorManager::setErrorCharPos(int64_t char_pos) {
    this->error_char_pos = char_pos;
}

void ErrorManager::setErrorMessage(const std::string &message) {
    this->error_message = message;
}

void ErrorManager::clearError() {
    this->error_filename = "";
    this->error_char_pos = -1;
    this->error_message  = "";
}

void ErrorManager::signalError(const std::string &message, int64_t char_pos) {
    if (this->error_filename.empty()) {
        if (char_pos != -1) {
            printColor(stderr, "red");
            fprintf(stderr, "Error has occurred at position %ld: %s.\n", char_pos, message.c_str());
            printColor(stderr, "reset");
        }
        else {
            printColor(stderr, "red");
            fprintf(stderr, "Error has occurred: %s.\n", message.c_str());
            printColor(stderr, "reset");
        }
        this->emergency_error_exit();
        return;
    }

    if (char_pos == -1) {
        printColor(stderr, "red");
        fprintf(stderr, "Error has occurred in file %s: %s.\n", this->error_filename.c_str(), message.c_str());
        printColor(stderr, "reset");
        this->emergency_error_exit();
        return;
    }

    FILE *fd = fopen(this->error_filename.c_str(), "r");
    if (fd == NULL) {
        printColor(stderr, "red");
        perror("Failed to open file where the error has occurred");
        printColor(stderr, "reset");
        this->setErrorFilename("");
        this->signalError(message);    // will print the original message
    }

    int64_t lines              = 1;
    int64_t column             = 1;
    int64_t last_line_position = 0;
    for (int64_t pos = 0; pos < char_pos; pos++) {
        char c = fgetc(fd);
        if (c == -1 || ferror(fd)) {
            break;
        }
        if (c == '\n') {
            last_line_position = pos + 1;
            lines++;
            column = 1;
        }
        else {
            column++;
        }
    }

    printColor(stderr, "red");
    fprintf(stderr, "Error has occurred in file %s\n", this->error_filename.c_str());
    printColor(stderr, "reset");
    if (fseek(fd, last_line_position, SEEK_SET) == -1) {
        printColor(stderr, "red");
        fprintf(stderr, "Error reading the file. Original error message: %s\n", message.c_str());
        printColor(stderr, "reset");
        this->emergency_error_exit();
        return;
    };

    static char buf[64];
    int64_t     prefix = sprintf(buf, "%ld:%ld | ", lines, column);

    fprintf(stderr, "%s", buf);
    int64_t pos;
    for (pos = last_line_position;; pos++) {
        {
            char c = fgetc(fd);
            if (c == '\n' || c == -1 || ferror(fd)) {
                fprintf(stderr, "\n");
                break;
            }
            fprintf(stderr, "%c", c);
        }
    }

    int64_t spaces = std::min(pos, char_pos) - last_line_position + prefix;
    for (int64_t i = 0; i < spaces; i++) {
        fprintf(stderr, " ");
    }
    printColor(stderr, "red");
    fprintf(stderr, "^\n");
    for (int64_t i = 0; i < spaces; i++) {
        fprintf(stderr, " ");
    }
    fprintf(stderr, "|\n");
    for (int64_t i = 0; i < spaces; i++) {
        fprintf(stderr, " ");
    }
    fprintf(stderr, "+-- %s.\n", message.c_str());
    printColor(stderr, "reset");

    this->emergency_error_exit();
    return;
}

void ErrorManager::signalError(const std::string &message, const Token &token) {
    assert(token.begin_pos != -1 && token.end_pos != -1,
           std::string("Invalid token. Original message: ") + message,
           this);
    if (this->error_filename.empty()) {
        printColor(stderr, "red");
        fprintf(stderr,
                "Error has occurred at positions %ld..%ld: %s.\n",
                token.begin_pos,
                token.end_pos,
                message.c_str());
        printColor(stderr, "reset");

        this->emergency_error_exit();
        return;
    }

    FILE *fd = fopen(this->error_filename.c_str(), "r");
    if (fd == NULL) {
        printColor(stderr, "red");
        perror("Failed to open file where the error has occurred");
        printColor(stderr, "reset");
        this->setErrorFilename("");
        this->signalError(message);    // will print the original message
    }

    int64_t lines              = 1;
    int64_t column             = 1;
    int64_t last_line_position = 0;
    for (int64_t pos = 0; pos < token.begin_pos; pos++) {
        char c = fgetc(fd);
        if (c == -1 || ferror(fd)) {
            break;
        }
        if (c == '\n') {
            last_line_position = pos + 1;
            lines++;
            column = 1;
        }
        else {
            column++;
        }
    }

    printColor(stderr, "red");
    fprintf(stderr, "Error has occurred in file %s\n", this->error_filename.c_str());
    printColor(stderr, "reset");
    if (fseek(fd, last_line_position, SEEK_SET) == -1) {
        printColor(stderr, "red");
        fprintf(stderr, "Error reading the file. Original error message: %s\n", message.c_str());
        printColor(stderr, "reset");
        this->emergency_error_exit();
        return;
    };

    static char buf[64];
    int64_t prefix = sprintf(buf, "%ld:%ld..%ld | ", lines, column, column + token.end_pos - token.begin_pos - 1);

    fprintf(stderr, "%s", buf);
    int64_t pos;
    for (pos = last_line_position;; pos++) {
        {
            char c = fgetc(fd);
            if (c == '\n' || c == -1 || ferror(fd)) {
                fprintf(stderr, "\n");
                break;
            }
            fprintf(stderr, "%c", c);
        }
    }

    int64_t spaces = std::min(pos, token.begin_pos) - last_line_position + prefix;
    for (int64_t i = 0; i < spaces; i++) {
        fprintf(stderr, " ");
    }
    printColor(stderr, "red");
    for (int64_t i = 0; i < token.end_pos - token.begin_pos; i++) {
        fprintf(stderr, "^");
    }
    fprintf(stderr, "\n");
    for (int64_t i = 0; i < spaces; i++) {
        fprintf(stderr, " ");
    }
    fprintf(stderr, "|\n");
    for (int64_t i = 0; i < spaces; i++) {
        fprintf(stderr, " ");
    }
    fprintf(stderr, "+-- %s.\n", message.c_str());
    printColor(stderr, "reset");

    this->emergency_error_exit();
    return;
}

void ErrorManager::signalError() {
    this->signalError(this->error_message, this->error_char_pos);
}

void ErrorManager::signalError(const std::string &message) {
    this->signalError(message, this->error_char_pos);
}

static void foo() {}

void ErrorManager::signalError(const std::vector<std::pair<Token *, std::string>> errors) {
    // a stupid solution, but why not
    void (*f)()                = this->emergency_error_exit;
    this->emergency_error_exit = foo;

    size_t cnt = 0;
    for (auto &[token, message] : errors) {
        if (message.empty()) {
            continue;    // ignore meaningless errors
        }
        if (cnt != 0) {
            fprintf(stderr, "The mentioned error happened because of the following error:\n");
        }
        cnt++;
        if (token != NULL) {
            this->signalError(message, *token);
        }
        else {
            this->setErrorCharPos(-1);
            this->signalError(message);
        }
    }

    this->emergency_error_exit = f;
    this->emergency_error_exit();
    return;
}

void __assert__(bool               value,
                const char        *assertion,
                const std::string &message,
                ErrorManager      *error_manager,
                const char        *filename,
                int                line) {
    if (!value) {
        static char buf[1024];
        sprintf(buf,
                "Assertion failed: %s. Happened in %s, line %d: %s\n",
                message.c_str(),
                filename,
                line,
                assertion);
        error_manager->signalError(std::string(buf));
    }
}

}    // namespace Cotton
