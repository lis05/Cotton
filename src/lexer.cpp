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

#include "lexer.h"
#include "errors.h"
#include <cctype>

namespace Cotton {

Token::Token() {
    this->id        = NOTHING_LIT;
    this->data      = "";
    this->begin_pos = -1;
    this->end_pos   = -1;
}

Token::Token(const std::string &data, int64_t begin_pos, int64_t end_pos) {
    this->id        = NOTHING_LIT;
    this->data      = data;
    this->begin_pos = begin_pos;
    this->end_pos   = end_pos;
}

static char getCharacter(std::string::const_iterator &it, const std::string &str, ErrorManager *error_manager) {
    char res;
    if (*it == '\\') {
        if (it + 1 == str.end()) {
            error_manager->signalError("Expected a character", it + 1 - str.begin());
        }
        switch (it[1]) {
        case 'a'  : res = '\a'; break;
        case 'b'  : res = '\b'; break;
        case 'f'  : res = '\f'; break;
        case 'n'  : res = '\n'; break;
        case 'r'  : res = '\r'; break;
        case 't'  : res = '\t'; break;
        case 'v'  : res = '\v'; break;
        case '\\' : res = '\\'; break;
        case '\"' : res = '\"'; break;
        case '\'' : res = '\''; break;
        default   : error_manager->signalError("Invalid character", it + 1 - str.begin());
        }
        it += 2;
    }
    else {
        res = *it;
        it++;
    }

    return res;
}

static std::string parseString(const std::string &str, ErrorManager *error_manager) {
    assert(!str.empty(), "Empty string", error_manager);
    std::string res;
    // string
    if (str.front() == '\"') {
        auto it = str.begin() + 1;
        while (it != str.end() && *it != '\"') {
            res += getCharacter(it, str, error_manager);
        }
        assert(*it == '\"', "Expected a string end", error_manager);
    }
    else {
        auto it = str.begin() + 1;
        assert(it != str.end() && *it != '\'', "Expected a character", error_manager);
        res += getCharacter(it, str, error_manager);
        assert(*it == '\'', "Expected a character end", error_manager);
    }
    return res;
}

void Token::identify(ErrorManager *error_manager) {
    if (this->data == "(") {
        this->id = OPEN_BRACKET;
    }
    else if (this->data == ")") {
        this->id = CLOSE_BRACKET;
    }
    else if (this->data == "record") {
        this->id = RECORD_KW;
    }
    else if (this->data == "{") {
        this->id = OPEN_CURLY_BRACKET;
    }
    else if (this->data == "}") {
        this->id = CLOSE_CURLY_BRACKET;
    }
    else if (this->data == ";") {
        this->id = SEMICOLON;
    }
    else if (this->data == "method") {
        this->id = METHOD_KW;
    }
    else if (this->data == "function") {
        this->id = FUNCTION_KW;
    }
    else if (this->data == ",") {
        this->id = COMMA;
    }
    else if (this->data == "++") {
        this->id = PLUS_PLUS_OP;
    }
    else if (this->data == "--") {
        this->id = MINUS_MINUS_OP;
    }
    else if (this->data == "[") {
        this->id = OPEN_SQUARE_BRACKET;
    }
    else if (this->data == "]") {
        this->id = CLOSE_SQUARE_BRACKET;
    }
    else if (this->data == ".") {
        this->id = DOT_OP;
    }
    else if (this->data == "ref") {
        this->id = REF_OP;
    }
    else if (this->data == "+") {
        this->id = PLUS_OP;
    }
    else if (this->data == "-") {
        this->id = MINUS_OP;
    }
    else if (this->data == "!") {
        this->id = NOT_OP;
    }
    else if (this->data == "~") {
        this->id = INVERSE_OP;
    }
    else if (this->data == "*") {
        this->id = MULT_OP;
    }
    else if (this->data == "/") {
        this->id = DIV_OP;
    }
    else if (this->data == "%") {
        this->id = REMAINDER_OP;
    }
    else if (this->data == ">>") {
        this->id = RIGHT_SHIFT_OP;
    }
    else if (this->data == "<<") {
        this->id = LEFT_SHIFT_OP;
    }
    else if (this->data == "<") {
        this->id = LESS_OP;
    }
    else if (this->data == "<=") {
        this->id = LESS_EQUAL_OP;
    }
    else if (this->data == ">") {
        this->id = GREATER_OP;
    }
    else if (this->data == ">=") {
        this->id = GREATER_EQUAL_OP;
    }
    else if (this->data == "==") {
        this->id = EQUAL_OP;
    }
    else if (this->data == "!=") {
        this->id = NOT_EQUAL_OP;
    }
    else if (this->data == "&") {
        this->id = BITAND_OP;
    }
    else if (this->data == "^") {
        this->id = BITXOR_OP;
    }
    else if (this->data == "|") {
        this->id = BITOR_OP;
    }
    else if (this->data == "and") {
        this->id = AND_OP;
    }
    else if (this->data == "or") {
        this->id = OR_OP;
    }
    else if (this->data == "=") {
        this->id = ASSIGN_OP;
    }
    else if (this->data == "+=") {
        this->id = ASSIGN_PLUS_OP;
    }
    else if (this->data == "-=") {
        this->id = ASSIGN_MINUS_OP;
    }
    else if (this->data == "*=") {
        this->id = ASSIGN_MULT_OP;
    }
    else if (this->data == "/=") {
        this->id = ASSIGN_DIV_OP;
    }
    else if (this->data == "%=") {
        this->id = ASSIGN_REMAINDER_OP;
    }
    else if (this->data == "true" || this->data == "false") {
        this->id         = BOOLEAN_LIT;
        this->bool_value = this->data == "true";
    }
    else if (this->data == "nothing") {
        this->id = NOTHING_LIT;
    }
    else if (this->data == "while") {
        this->id = WHILE_KW;
    }
    else if (this->data == "for") {
        this->id = FOR_KW;
    }
    else if (this->data == "if") {
        this->id = IF_KW;
    }
    else if (this->data == "else") {
        this->id = ELSE_KW;
    }
    else if (this->data == "continue") {
        this->id = CONTINUE_KW;
    }
    else if (this->data == "break") {
        this->id = BREAK_KW;
    }
    else if (this->data == "return") {
        this->id = RETURN_KW;
    }
    else if (this->data == "block") {
        this->id = BLOCK_KW;
    }
    else if (this->data.size() > 0 && this->data.front() == '\"' && this->data.back() == '\"') {
        this->id           = STRING_LIT;
        this->string_value = parseString(this->data, error_manager);
    }
    else if (this->data.size() > 0 && this->data.front() == '\'' && this->data.back() == '\'') {
        this->id           = CHAR_LIT;
        this->string_value = parseString(this->data, error_manager);
    }
    else {
        // check for int and real
        int64_t int_value;
        double  real_value;
        size_t  pos;

        try {
            int_value = std::stoll(this->data, &pos);
            if (pos == this->data.size()) {
                this->id        = INT_LIT;
                this->int_value = int_value;
                return;
            }
        } catch (...) {
        }

        // not int

        try {
            real_value = std::stod(this->data, &pos);
            if (pos == this->data.size()) {
                this->id         = REAL_LIT;
                this->real_value = real_value;
                return;
            }
        } catch (...) {
        }

        // not real

        this->id = IDENTIFIER;
    }
}

std::ostream &operator<<(std::ostream &out, const Token &token) {
    switch (token.id) {
    case Token::OPEN_BRACKET         : out << "("; break;
    case Token::CLOSE_BRACKET        : out << ")"; break;
    case Token::RECORD_KW            : out << "record"; break;
    case Token::OPEN_CURLY_BRACKET   : out << "{"; break;
    case Token::CLOSE_CURLY_BRACKET  : out << "}"; break;
    case Token::SEMICOLON            : out << ";"; break;
    case Token::METHOD_KW            : out << "method"; break;
    case Token::FUNCTION_KW          : out << "function"; break;
    case Token::COMMA                : out << ","; break;
    case Token::PLUS_PLUS_OP         : out << "++"; break;
    case Token::MINUS_MINUS_OP       : out << "--"; break;
    case Token::OPEN_SQUARE_BRACKET  : out << "["; break;
    case Token::CLOSE_SQUARE_BRACKET : out << "]"; break;
    case Token::DOT_OP               : out << "."; break;
    case Token::REF_OP               : out << "ref"; break;
    case Token::PLUS_OP              : out << "+"; break;
    case Token::MINUS_OP             : out << "-"; break;
    case Token::NOT_OP               : out << "!"; break;
    case Token::INVERSE_OP           : out << "~"; break;
    case Token::MULT_OP              : out << "*"; break;
    case Token::DIV_OP               : out << "/"; break;
    case Token::REMAINDER_OP         : out << "%"; break;
    case Token::RIGHT_SHIFT_OP       : out << ">>"; break;
    case Token::LEFT_SHIFT_OP        : out << "<<"; break;
    case Token::LESS_OP              : out << "<"; break;
    case Token::LESS_EQUAL_OP        : out << ">"; break;
    case Token::GREATER_OP           : out << ">"; break;
    case Token::GREATER_EQUAL_OP     : out << ">="; break;
    case Token::EQUAL_OP             : out << "=="; break;
    case Token::NOT_EQUAL_OP         : out << "!="; break;
    case Token::BITAND_OP            : out << "&"; break;
    case Token::BITXOR_OP            : out << "^"; break;
    case Token::BITOR_OP             : out << "|"; break;
    case Token::AND_OP               : out << "and"; break;
    case Token::OR_OP                : out << "or"; break;
    case Token::ASSIGN_OP            : out << "="; break;
    case Token::ASSIGN_PLUS_OP       : out << "+="; break;
    case Token::ASSIGN_MINUS_OP      : out << "-="; break;
    case Token::ASSIGN_MULT_OP       : out << "*="; break;
    case Token::ASSIGN_DIV_OP        : out << "/="; break;
    case Token::ASSIGN_REMAINDER_OP  : out << "%="; break;
    case Token::BOOLEAN_LIT          : out << (token.bool_value ? "true" : "false"); break;
    case Token::NOTHING_LIT          : out << "nothing"; break;
    case Token::WHILE_KW             : out << "while"; break;
    case Token::FOR_KW               : out << "for"; break;
    case Token::IF_KW                : out << "if"; break;
    case Token::ELSE_KW              : out << "else"; break;
    case Token::CONTINUE_KW          : out << "continue"; break;
    case Token::BREAK_KW             : out << "break"; break;
    case Token::RETURN_KW            : out << "return"; break;
    case Token::BLOCK_KW             : out << "block"; break;
    case Token::STRING_LIT           : out << "\"" << token.string_value << "\""; break;
    case Token::CHAR_LIT             : out << "\'" << token.char_value << "\'"; break;
    case Token::INT_LIT              : out << token.int_value; break;
    case Token::REAL_LIT             : out << token.real_value; break;
    case Token::IDENTIFIER           : out << "id:" << token.data; break;
    }

    return out;
}

static void skipString(Lexer &lexer, std::string::iterator &it, std::string &str) {
    auto string_begin = it;
    bool found_end    = false;
    it++;
    while (it != str.end()) {
        if (*it == '\"') {
            // it may be \", so we have to check for that
            int  cnt = 0;
            auto it2 = it - 1;
            while (it2 != string_begin && *it2 == '\\') {
                cnt++;
                it2--;
            }

            if (cnt % 2 == 0) {
                // string end
                found_end = true;
                break;
            }
        }

        it++;
    }
    if (!found_end) {
        lexer.error_manager->signalError("Expected the end of the string", it - str.begin());
    }
    it++;
}

static void skipCharacter(Lexer &lexer, std::string::iterator &it, std::string &str) {
    it++;

    if (it == str.end()) {
        lexer.error_manager->signalError("Expected a character", it - str.begin());
    }

    if (*it == '\'') {
        lexer.error_manager->signalError("Character cannot be empty", it - str.begin());
    }

    if (*it == '\\') {
        it++;
    }

    if (it == str.end()) {
        lexer.error_manager->signalError("Expected the end of the character", it - str.begin());
    }

    it++;

    if (*it != '\'') {
        lexer.error_manager->signalError("Expected the end of the character", it - str.begin());
    }

    it++;
}

void Lexer::faze1FillComments(std::string &input) {
    auto it = input.begin();
    while (it != input.end()) {
        if (*it == '\"') {
            skipString(*this, it, input);
        }
        else if (*it == '\'') {
            skipCharacter(*this, it, input);
        }
        // now we're safe, and can remove comments
        else if (*it == '/' && it + 1 != input.end() && it[1] == '*') {
            // everything until */ is a comment
            while (it != input.end()) {
                if (*it == '*' && it + 1 != input.end() && it[1] == '/') {
                    *it    = ' ';
                    it[1]  = ' ';
                    it    += 2;
                    break;
                }
                *it = ' ';
                it++;
            }
        }
        else if (*it == '/' && it + 1 != input.end() && it[1] == '/') {
            // everything until new line is a comment
            while (it != input.end()) {
                if (*it == '\n') {
                    *it = ' ';
                    it++;
                    break;
                }
                *it = ' ';
                it++;
            }
        }

        else {
            it++;
        }
    }
}

std::vector<Token> Lexer::faze2FormConnectedTokens(const std::string &input) {
    std::string        buf;
    std::vector<Token> tokens;
    auto               it = input.begin();

    while (it != input.end()) {
        if (*it == '"') {
            if (!buf.empty()) {
                tokens.push_back(Token(buf, it - input.begin() - buf.size(), it - input.begin()));
                buf.clear();
            }
            auto string_begin = it;
            it++;
            // since the string has passed the first faze, it has it's ending
            while (*it != '"') {
                getCharacter(it, input, this->error_manager);
            }
            this->error_manager->setErrorCharPos(it - input.begin());
            assert(*it == '\"', "String doesn't end with a double quote", this->error_manager);
            it++;
            auto string_end = it;

            it = string_begin;
            while (it != string_end) {
                buf += *it;
                it++;
            }

            tokens.push_back(Token(buf, it - input.begin() - buf.size(), it - input.begin()));
            buf.clear();
        }
        else if (*it == '\'') {
            if (!buf.empty()) {
                tokens.push_back(Token(buf, it - input.begin() - buf.size(), it - input.begin()));
                buf.clear();
            }
            auto char_begin = it;
            it++;
            // since the character has passed the first faze, it has it's ending
            getCharacter(it, input, this->error_manager);
            this->error_manager->setErrorCharPos(it - input.begin());
            assert(*it == '\'', "Character doesn't end with a single quote", this->error_manager);
            it++;
            auto char_end = it;

            it = char_begin;
            while (it != char_end) {
                buf += *it;
                it++;
            }

            tokens.push_back(Token(buf, it - input.begin() - buf.size(), it - input.begin()));
            buf.clear();
        }
        else if (isspace(*it)) {
            if (!buf.empty()) {
                tokens.push_back(Token(buf, it - input.begin() - buf.size(), it - input.begin()));
                buf.clear();
            }
            it++;
        }
        else {
            buf += *it;
            it++;
        }
    }
    if (!buf.empty()) {
        tokens.push_back(Token(buf, it - input.begin() - buf.size(), it - input.begin()));
        buf.clear();
    }
    return tokens;
}

// no dot
static std::vector<std::string> sortedSplitters
= {"(",  ")",  "{", "}",  ";", "++", "--", "[", "]", "+=", "-=", "!=", "~", "*=", "/=", "%=", ">>",
   "<<", "<=", "<", ">=", ">", "==", "!",  "&", "^", "|",  "=",  "+",  "-", "*",  "/",  "%",  ","};

static void splitByOperator(const Token                 &token,
                            std::string::const_iterator &split_end,
                            std::string::const_iterator &it,
                            std::vector<Token>          &res,
                            const std::string           &splitter) {
    if (it != split_end) {
        Token t(token.data.substr(split_end - token.data.begin(), it - split_end),
                split_end - token.data.begin() + token.begin_pos,
                it - token.data.begin() + token.begin_pos);
        res.push_back(t);
        split_end = it;
    }
    it += splitter.size();
    {
        Token t(token.data.substr(split_end - token.data.begin(), it - split_end),
                split_end - token.data.begin() + token.begin_pos,
                it - token.data.begin() + token.begin_pos);
        res.push_back(t);
        split_end = it;
    }
}

std::vector<Token> Lexer::faze3SplitByOperatorsAndSeparators(const std::vector<Token> &tokens) {
    std::vector<Token> res;
    for (auto &token : tokens) {
        auto it        = token.data.begin();
        auto split_end = it;
        while (it != token.data.end()) {
            bool f = false;
            for (auto &splitter : sortedSplitters) {
                if (token.data.substr(it - token.data.begin(), splitter.size()) == splitter) {
                    f = true;
                    splitByOperator(token, split_end, it, res, splitter);
                    break;
                }
            }
            if (!f) {
                it++;
            }
        }

        if (it != split_end) {
            Token t(token.data.substr(split_end - token.data.begin(), it - split_end),
                    split_end - token.data.begin() + token.begin_pos,
                    it - token.data.begin() + token.begin_pos);
            res.push_back(t);
            split_end = it;
        }
    }
    return res;
}

std::vector<Token> Lexer::faze4FindRealNumbers(const std::vector<Token> &tokens) {
    std::vector<Token> res;
    for (auto &token : tokens) {
        if (token.data.find(".") != token.data.npos) {
            try {
                size_t ptr;
                std::stod(token.data, &ptr);
                if (ptr == token.data.size()) {
                    res.push_back(token);
                    continue;
                }
            } catch (...) {
            }

            // not real, splitting by dot operator
            auto        it        = token.data.begin();
            auto        split_end = it;
            bool        f         = false;
            std::string splitter  = ".";
            while (it != token.data.end()) {
                if (token.data.substr(it - token.data.begin(), splitter.size()) == splitter) {
                    f = true;
                    splitByOperator(token, split_end, it, res, splitter);
                }
                else {
                    it++;
                }
            }

            if (it != split_end) {
                Token t(token.data.substr(split_end - token.data.begin(), it - split_end),
                        split_end - token.data.begin() + token.begin_pos,
                        it - token.data.begin() + token.begin_pos);
                res.push_back(t);
                split_end = it;
            }
        }
        else {
            res.push_back(token);
        }
    }
    return res;
}

void Lexer::faze5IdentifyTokens(std::vector<Token> &tokens) {
    for (auto &token : tokens) {
        token.identify(this->error_manager);
    }
}

Lexer::Lexer(ErrorManager *error_manager) {
    this->error_manager = error_manager;
}

std::vector<Token> Lexer::process(std::string &input) {
    this->faze1FillComments(input);
    std::vector<Token> tokens = this->faze2FormConnectedTokens(input);
    tokens                    = this->faze3SplitByOperatorsAndSeparators(tokens);
    tokens                    = this->faze4FindRealNumbers(tokens);
    this->faze5IdentifyTokens(tokens);
    if (!tokens.empty()) {
        tokens.push_back(Token("", tokens.back().end_pos, tokens.back().end_pos + 1));
    }
    return tokens;
}

std::vector<Token> Lexer::processFile(const std::string &filename) {
    this->error_manager->setErrorFilename(filename);

    FILE *fd = fopen(filename.c_str(), "r");
    if (fd == NULL) {
        this->error_manager->signalError("Could not open file", 0);
    }

    std::string data;
    while (true) {
        char c = fgetc(fd);
        if (c == -1 || ferror(fd)) {
            break;
        }
        data += c;
    }

    fclose(fd);
    return this->process(data);
}
}    // namespace Cotton
