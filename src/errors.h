#pragma once

#include <cstdint>
#include <string>

#include "lexer.h"

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
