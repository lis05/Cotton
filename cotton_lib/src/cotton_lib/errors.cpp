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
#include "front/parser.h"

static void printColor(FILE *fd, std::string color = "reset") {
#ifndef NOCOLOROUTPUT
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
#endif
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

void ErrorManager::signalError(const std::string &message, int64_t char_pos, bool eee) {
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
        if (eee) {
            this->emergency_error_exit();
        }
        else {
            return;
        }
    }

    if (char_pos == -1) {
        printColor(stderr, "red");
        fprintf(stderr, "Error has occurred in file %s: %s.\n", this->error_filename.c_str(), message.c_str());
        printColor(stderr, "reset");
        if (eee) {
            this->emergency_error_exit();
        }
        else {
            return;
        }
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
        if (eee) {
            this->emergency_error_exit();
        }
        else {
            return;
        }
    };

    static char buf[256];
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

    if (eee) {
        this->emergency_error_exit();
    }
    else {
        return;
    }
}

void ErrorManager::signalError(const std::string &message, const Token &token, bool eee) {
    eassert(token.begin_pos != -1 && token.end_pos != -1,
            std::string("Invalid token. Original message: ") + message,
            this);
    auto &filename = token.filename;
    if (filename.empty()) {
        printColor(stderr, "red");
        fprintf(stderr,
                "Error has occurred at positions %ld..%ld: %s.\n",
                token.begin_pos,
                token.end_pos,
                message.c_str());
        printColor(stderr, "reset");

        if (eee) {
            this->emergency_error_exit();
        }
        else {
            return;
        }
    }

    FILE *fd = fopen(filename.c_str(), "r");
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
    fprintf(stderr, "Error has occurred in file %s\n", filename.c_str());
    printColor(stderr, "reset");
    if (fseek(fd, last_line_position, SEEK_SET) == -1) {
        printColor(stderr, "red");
        fprintf(stderr, "Error reading the file. Original error message: %s\n", message.c_str());
        printColor(stderr, "reset");
        if (eee) {
            this->emergency_error_exit();
        }
        else {
            return;
        }
    };

    static char buf[256];
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
    for (int64_t i = 0; i < std::min((int64_t)100, token.end_pos - token.begin_pos); i++) {
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

    if (eee) {
        this->emergency_error_exit();
    }
    else {
        return;
    }
}

void ErrorManager::signalError(bool eee) {
    this->signalError(this->error_message, this->error_char_pos, eee);
}

void ErrorManager::signalError(const std::string &message, bool eee) {
    this->signalError(message, this->error_char_pos, eee);
}

void ErrorManager::signalError(const std::vector<std::pair<Token *, std::string>> errors, bool eee) {
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
            this->signalError(message, *token, false);
        }
        else {
            this->setErrorCharPos(-1);
            this->signalError(message, false);
        }
    }

    if (eee) {
        this->emergency_error_exit();
    }
    else {
        return;
    }
}

void ErrorManager::signalErrorWithContext(const std::vector<std::pair<std::string, Token *>> messages, bool eee) {
    fprintf(stderr, "A runtime error has occurred. Here is the context in which the error occurred:\n");

    size_t cnt = 0;
    for (auto &[message, token] : messages) {
        if (message.empty()) {
            continue;    // ignore meaningless errors
        }
        cnt++;
        if (cnt == messages.size()) {
            break;
        }
        if (token != NULL) {
            this->signalError(message, *token, false);
        }
        else {
            this->setErrorCharPos(-1);
            this->signalError(message, false);
        }
    }

    if (!messages.empty()) {
        auto &[error, token] = messages.back();

        fprintf(stderr, "Here is the error:\n");
        if (token != NULL) {
            this->signalError(error, *token, false);
        }
        else {
            this->setErrorCharPos(-1);
            this->signalError(error, false);
        }
    }

    if (eee) {
        this->emergency_error_exit();
    }
    else {
        return;
    }
}

static bool intersection(int64_t l, int64_t r, int64_t L, int64_t R) {
    return std::max(l, L) <= std::min(r, R);
}

void ErrorManager::signalError(const std::string &message, const TextArea &ta, bool eee) {
    if (ta.filename == NULL || ta.filename->empty()) {
        printColor(stderr, "red");
        fprintf(stderr,
                "Error has occurred at positions %ld..%ld: %s.\n",
                ta.first_char,
                ta.last_char,
                message.c_str());
        printColor(stderr, "reset");

        if (eee) {
            this->emergency_error_exit();
        }
        else {
            return;
        }
    }
    auto filename = *ta.filename;

    FILE *fd = fopen(filename.c_str(), "r");
    if (fd == NULL) {
        printColor(stderr, "red");
        perror("Failed to open file where the error has occurred");
        printColor(stderr, "reset");
        this->setErrorFilename("");
        this->signalError(message);    // will print the original message
    }

    int64_t                                                          lines              = 1;
    int64_t                                                          column             = 1;
    int64_t                                                          last_line_position = 0;
    std::string                                                      cur_line;
    std::vector<std::pair<std::pair<int64_t, int64_t>, std::string>> error_lines;
    for (int64_t pos = 0;; pos++) {
        char c = fgetc(fd);
        if (c == -1 || ferror(fd)) {
            int64_t string_begin = pos - cur_line.size();
            int64_t string_end   = pos - 1;
            if (intersection(string_begin, string_end, ta.first_char, ta.last_char - 1)) {
                error_lines.push_back({
                    {string_begin, lines},
                    cur_line
                });
            }
            break;
        }
        if (c == '\n') {
            int64_t string_begin = pos - cur_line.size();
            int64_t string_end   = pos - 1;
            if (intersection(string_begin, string_end, ta.first_char, ta.last_char - 1)) {
                error_lines.push_back({
                    {string_begin, lines},
                    cur_line
                });
            }
            last_line_position = pos + 1;
            lines++;
            column   = 1;
            cur_line = "";
        }
        else {
            column++;
            cur_line += c;
        }
    }

    printColor(stderr, "red");
    fprintf(stderr, "Error has occurred in file %s\n", filename.c_str());
    printColor(stderr, "reset");
    if (fseek(fd, last_line_position, SEEK_SET) == -1) {
        printColor(stderr, "red");
        fprintf(stderr, "Error reading the file. Original error message: %s\n", message.c_str());
        printColor(stderr, "reset");
        if (eee) {
            this->emergency_error_exit();
        }
        else {
            return;
        }
    };

    if (error_lines.size() == 0) {
        printColor(stderr, "red");
        fprintf(stderr, "at positions %ld..%ld", ta.first_char, ta.last_char);
        printColor(stderr, "reset");
        if (eee) {
            this->emergency_error_exit();
        }
        else {
            return;
        }
    }

    if (error_lines.size() == 1) {
        static char buf[256];
        int64_t     prefix = sprintf(buf,
                                 "%ld:%ld..%ld | ",
                                 error_lines[0].first.second,
                                 ta.first_char - error_lines[0].first.first + 1,
                                 ta.last_char - 1 - error_lines[0].first.first + 1);
        fprintf(stderr, "%s%s\n", buf, error_lines[0].second.c_str());
        int64_t spaces = prefix + ta.first_char - error_lines[0].first.first;
        for (int64_t i = 0; i < spaces; i++) {
            fprintf(stderr, " ");
        }
        printColor(stderr, "red");
        for (int64_t i = 0; i < ta.last_char - ta.first_char; i++) {
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

        if (eee) {
            this->emergency_error_exit();
        }
        else {
            return;
        }
    }

    // first line
    static char buf_first[256];
    int64_t     prefix_first = sprintf(buf_first,
                                   "%ld:%ld..%ld",
                                   error_lines.front().first.second,
                                   ta.first_char - error_lines[0].first.first + 1,
                                   error_lines.front().second.size());
    static char buf_last[256];
    int64_t     prefix_last = sprintf(buf_last,
                                  "%ld:%ld..%ld",
                                  error_lines.back().first.second,
                                  1l,
                                  ta.last_char - 1 - error_lines.back().first.first + 1);
    static char buf_mid[256];
    int64_t     biggest_mid
    = (error_lines.size() > 2) ? sprintf(buf_mid, "%ld:entire line", error_lines.back().first.second) : 0;

    int64_t prefix = std::max(std::max(prefix_first, prefix_last), biggest_mid);

    int64_t spaces = prefix + 3;
    for (int64_t i = 0; i < spaces + ta.first_char - error_lines.front().first.first; i++) {
        fprintf(stderr, " ");
    }
    printColor(stderr, "red");
    for (int64_t i = 0; i < error_lines.front().second.size() - (ta.first_char - error_lines.front().first.first);
         i++)
    {
        fprintf(stderr, "v");
    }
    fprintf(stderr, " --- error message is in the last line");
    printColor(stderr, "reset");
    fprintf(stderr, "\n");

    fprintf(stderr, "%s", buf_first);
    for (int64_t i = prefix - prefix_first; i > 0; i--) {
        fprintf(stderr, " ");
    }
    fprintf(stderr, " | ");
    fprintf(stderr, "%s\n", error_lines.front().second.c_str());

    for (int64_t i = 1; i < error_lines.size() - 1; i++) {
        int64_t prefix_mid = sprintf(buf_mid, "%ld:entire line", error_lines[i].first.second);
        fprintf(stderr, "%s", buf_mid);
        for (int64_t i = prefix - prefix_mid; i > 0; i--) {
            fprintf(stderr, " ");
        }
        fprintf(stderr, " | ");
        fprintf(stderr, "%s\n", error_lines[i].second.c_str());
    }

    fprintf(stderr, "%s", buf_last);
    for (int64_t i = prefix - prefix_last; i > 0; i--) {
        fprintf(stderr, " ");
    }
    fprintf(stderr, " | ");
    fprintf(stderr, "%s\n", error_lines.back().second.c_str());

    spaces = prefix + 3;
    for (int64_t i = 0; i < spaces; i++) {
        fprintf(stderr, " ");
    }
    printColor(stderr, "red");
    for (int64_t i = 0; i < ta.last_char - error_lines.back().first.first; i++) {
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

    if (eee) {
        this->emergency_error_exit();
    }
    else {
        return;
    }
    /*

        static char buf[256];
        int64_t prefix = sprintf(buf, "%ld:%ld..%ld | ", lines, column, column + token.end_pos - token.begin_pos -
       1);

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
        for (int64_t i = 0; i < std::min((int64_t)100, token.end_pos - token.begin_pos); i++) {
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

        if (eee) {
            this->emergency_error_exit();
        }
        else {
            return;
        }*/
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
