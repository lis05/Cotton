#include "errors.h"

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
            fprintf(stderr, "Error has occurred at position %ld: %s.\n", char_pos, message.c_str());
        }
        else {
            fprintf(stderr, "Error has occurred: %s.\n", message.c_str());
        }
        this->emergency_error_exit();
    }

    if (char_pos == -1) {
        fprintf(stderr, "Error has occurred in file %s: %s.\n", this->error_filename.c_str(), message.c_str());
        this->emergency_error_exit();
    }

    FILE *fd = fopen(this->error_filename.c_str(), "r");
    if (fd == NULL) {
        perror("Failed to open file where the error has occured");
        this->setErrorFilename("");
        this->signalError(message);
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

    fprintf(stderr, "Error has occured in file %s\n", this->error_filename.c_str());
    if (fseek(fd, last_line_position, SEEK_SET) == -1) {
        this->emergency_error_exit();
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
    fprintf(stderr, "^\n");
    for (int64_t i = 0; i < spaces; i++) {
        fprintf(stderr, " ");
    }
    fprintf(stderr, "|\n");
    for (int64_t i = 0; i < spaces; i++) {
        fprintf(stderr, " ");
    }
    fprintf(stderr, "+-- %s.\n", message.c_str());

    this->emergency_error_exit();
}

void ErrorManager::signalError() {
    this->signalError(this->error_message, this->error_char_pos);
}

void ErrorManager::signalError(const std::string &message) {
    this->signalError(message, this->error_char_pos);
}

void __assert__(bool               value,
                const char        *assertion,
                const std::string &message,
                ErrorManager      *error_manager,
                const char        *filename,
                int                line) {
    if (!value) {
        static char buf[1024];
        sprintf(buf, "Assertion failed: %s. Happened in %s, line %d: %s\n", message.c_str(), filename, line, assertion);
        error_manager->signalError(std::string(buf));
    }
}

}    // namespace Cotton
