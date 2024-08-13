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

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

namespace Cotton {
class ErrorManager;

class Token {
public:
    enum TokenId {
        OPEN_BRACKET,
        CLOSE_BRACKET,
        TYPE_KW,
        OPEN_CURLY_BRACKET,
        CLOSE_CURLY_BRACKET,
        SEMICOLON,
        METHOD_KW,
        FUNCTION_KW,
        COMMA,
        PLUS_PLUS_OP,
        MINUS_MINUS_OP,
        OPEN_SQUARE_BRACKET,
        CLOSE_SQUARE_BRACKET,
        DOT_OP,
        AT_OP,
        PLUS_OP,
        MINUS_OP,
        NOT_OP,
        INVERSE_OP,
        MULT_OP,
        DIV_OP,
        REMAINDER_OP,
        RIGHT_SHIFT_OP,
        LEFT_SHIFT_OP,
        LESS_OP,
        LESS_EQUAL_OP,
        GREATER_OP,
        GREATER_EQUAL_OP,
        EQUAL_OP,
        NOT_EQUAL_OP,
        BITAND_OP,
        BITXOR_OP,
        BITOR_OP,
        AND_OP,
        OR_OP,
        ASSIGN_OP,
        ASSIGN_PLUS_OP,
        ASSIGN_MINUS_OP,
        ASSIGN_MULT_OP,
        ASSIGN_DIV_OP,
        ASSIGN_REMAINDER_OP,
        BOOLEAN_LIT,
        NOTHING_LIT,
        WHILE_KW,
        FOR_KW,
        IF_KW,
        ELSE_KW,
        CONTINUE_KW,
        BREAK_KW,
        RETURN_KW,
        UNSCOPED_KW,
        STRING_LIT,
        CHAR_LIT,
        INT_LIT,
        REAL_LIT,
        IDENTIFIER,
    };

    TokenId     id;
    std::string data;

    int64_t nameid;

    bool        bool_value;
    int64_t     int_value;
    char        char_value;
    double      real_value;
    std::string string_value;

    int64_t     begin_pos, end_pos;
    std::string filename;

    Token();
    ~Token() = default;

    Token(const std::string &data, int64_t begin_pos, int64_t end_pos, const std::string &filename);
    void identify(ErrorManager *error_manager);
};

std::ostream &operator<<(std::ostream &out, const Token &token);

class ErrorManager;

class Lexer {
private:
    void               faze1FillComments(std::string &input);
    std::vector<Token> faze2FormConnectedTokens(const std::string &input);
    std::vector<Token> faze3SplitByOperatorsAndSeparators(const std::vector<Token> &tokens);
    std::vector<Token> faze4FindRealNumbers(const std::vector<Token> &tokens);
    void               faze5IdentifyTokens(std::vector<Token> &tokens);

public:
    std::vector<Token> process(std::string &input);
    std::vector<Token> processFile(const std::string &filename);

    Lexer()  = delete;
    ~Lexer() = default;

    Lexer(ErrorManager *error_manager);
    ErrorManager *error_manager;
};
}    // namespace Cotton
