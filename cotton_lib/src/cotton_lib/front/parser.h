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

#include "lexer.h"
#include <cstdint>
#include <string>
#include <vector>

namespace Cotton {
class Token;
class ErrorManager;

class ExprNode;
class FuncDefNode;
class TypeDefNode;
class OperatorNode;
class AtomNode;
class ParExprNode;
class IdentListNode;
class StmtNode;
class WhileStmtNode;
class ForStmtNode;
class IfStmtNode;
class ReturnStmtNode;
class BlockStmtNode;

class TextArea {
public:
    int64_t first_char, last_char;
    std::string filename;

    TextArea();
    TextArea(const Token &token);
    TextArea(const TextArea &first, const TextArea &last);
};

class ExprNode {
public:
    TextArea text_area;

    enum ExprNodeId { FUNCTION_DEFINITION, TYPE_DEFINITION, OPERATOR, ATOM, PARENTHESES_EXPRESSION } id;

    union {
        FuncDefNode  *func_def;
        TypeDefNode  *type_def;
        OperatorNode *op;
        AtomNode     *atom;
        ParExprNode  *par_expr;
    };

    ExprNode() = delete;
    ~ExprNode();

    ExprNode(FuncDefNode *func_def, TextArea text_area);
    ExprNode(TypeDefNode *type_def, TextArea text_area);
    ExprNode(OperatorNode *op, TextArea text_area);
    ExprNode(AtomNode *atom, TextArea text_area);
    ExprNode(ParExprNode *par_expr, TextArea text_area);

    void print(int indent = 0, int step = 2);
};

class FuncDefNode {
public:
    TextArea       text_area;
    Token         *name;      // NULL means not present
    IdentListNode *params;    // NULL means not present
    StmtNode      *body;

    FuncDefNode() = delete;
    ~FuncDefNode();

    FuncDefNode(Token *name, IdentListNode *params, StmtNode *body, TextArea text_area);

    void print(int indent = 0, int step = 2);
};

class TypeDefNode {
public:
    TextArea                   text_area;
    Token                     *name;
    std::vector<Token *>       fields;
    std::vector<FuncDefNode *> methods;

    TypeDefNode() = delete;
    ~TypeDefNode();

    TypeDefNode(Token                            *name,
                const std::vector<Token *>       &fields,
                const std::vector<FuncDefNode *> &methods,
                TextArea                          text_area);

    void print(int indent = 0, int step = 2);
};

class OperatorNode {
public:
    TextArea text_area;

    enum OperatorId {
        POST_PLUS_PLUS,
        POST_MINUS_MINUS,
        CALL,
        INDEX,
        DOT,
        AT,
        PRE_PLUS_PLUS,
        PRE_MINUS_MINUS,
        PRE_PLUS,
        PRE_MINUS,
        NOT,
        INVERSE,
        MULT,
        DIV,
        REM,
        RIGHT_SHIFT,
        LEFT_SHIFT,
        PLUS,
        MINUS,
        LESS,
        LESS_EQUAL,
        GREATER,
        GREATER_EQUAL,
        EQUAL,
        NOT_EQUAL,
        BITAND,
        BITXOR,
        BITOR,
        AND,
        OR,
        ASSIGN,
        PLUS_ASSIGN,
        MINUS_ASSIGN,
        MULT_ASSIGN,
        DIV_ASSIGN,
        REM_ASSIGN,
        COMMA,
        TOTAL_OPERATORS          // not a method
    } id;

    ExprNode *first, *second;    // if second is NULL then it's not present, and the operator is unary
    Token    *op;

    OperatorNode() = delete;
    ~OperatorNode();

    OperatorNode(OperatorId id, ExprNode *first, ExprNode *second, Token *op, TextArea text_area);

    void print(int indent = 0, int step = 2);
};

class Object;

class AtomNode {
public:
    TextArea text_area;

    enum AtomId { BOOLEAN, CHARACTER, INTEGER, REAL, STRING, IDENTIFIER, NOTHING } id;

    bool        bool_value;
    char        char_value;
    int64_t     int_value;
    double      real_value;
    std::string string_value;
    Token      *ident;

    Object *lit_obj;
    Token  *token;

    AtomNode() = delete;
    ~AtomNode();

    AtomNode(Token *token, TextArea text_area);    // takes value from the token

    void print(int indent = 0, int step = 2);
};

class ParExprNode {
public:
    TextArea  text_area;
    ExprNode *expr;

    ParExprNode() = delete;
    ~ParExprNode();

    ParExprNode(ExprNode *expr, TextArea text_area);

    void print(int indent = 0, int step = 2);
};

class IdentListNode {
public:
    TextArea             text_area;
    std::vector<Token *> list;

    IdentListNode() = delete;
    ~IdentListNode();

    IdentListNode(const std::vector<Token *> &list, TextArea text_area);

    void print(int indent = 0, int step = 2);
};

class StmtNode {
public:
    TextArea text_area;

    enum StmtId { WHILE, FOR, IF, CONTINUE, BREAK, RETURN, BLOCK, EXPR } id;

    union {
        WhileStmtNode  *while_stmt;
        ForStmtNode    *for_stmt;
        IfStmtNode     *if_stmt;
        ReturnStmtNode *return_stmt;
        BlockStmtNode  *block_stmt;
        ExprNode       *expr;
    };

    StmtNode() = delete;
    ~StmtNode();

    StmtNode(WhileStmtNode *while_stmt, TextArea text_area);
    StmtNode(ForStmtNode *for_stmt, TextArea text_area);
    StmtNode(IfStmtNode *if_stmt, TextArea text_area);
    StmtNode(StmtId id, TextArea text_area);    // for continue and brake only
    StmtNode(ReturnStmtNode *return_stmt, TextArea text_area);
    StmtNode(BlockStmtNode *block_stmt, TextArea text_area);
    StmtNode(ExprNode *expr, TextArea text_area);

    void print(int indent = 0, int step = 2);
};

class WhileStmtNode {
public:
    TextArea  text_area;
    ExprNode *cond;
    StmtNode *body;

    WhileStmtNode() = delete;
    ~WhileStmtNode();

    WhileStmtNode(ExprNode *cond, StmtNode *body, TextArea text_area);

    void print(int indent = 0, int step = 2);
};

class ForStmtNode {
public:
    TextArea text_area;

    ExprNode *init, *cond, *step;
    StmtNode *body;

    ForStmtNode() = delete;
    ~ForStmtNode();

    ForStmtNode(ExprNode *init, ExprNode *cond, ExprNode *step, StmtNode *body, TextArea text_area);

    void print(int indent = 0, int step = 2);
};

class IfStmtNode {
public:
    TextArea text_area;

    ExprNode *cond;
    StmtNode *body;
    StmtNode *else_body;    // if NULL then not present

    IfStmtNode() = delete;
    ~IfStmtNode();

    IfStmtNode(ExprNode *cond, StmtNode *body, StmtNode *else_body, TextArea text_area);

    void print(int indent = 0, int step = 2);
};

class ReturnStmtNode {
public:
    TextArea text_area;

    ExprNode *value;

    ReturnStmtNode() = delete;
    ~ReturnStmtNode();

    ReturnStmtNode(ExprNode *value, TextArea text_area);

    void print(int indent = 0, int step = 2);
};

class BlockStmtNode {
public:
    TextArea text_area;

    bool                    is_unscoped;
    std::vector<StmtNode *> list;

    BlockStmtNode() = delete;
    ~BlockStmtNode();

    BlockStmtNode(bool is_unscoped, const std::vector<StmtNode *> list, TextArea text_area);

    void print(int indent = 0, int step = 2);
};

class Parser {
public:
    Parser()  = delete;
    ~Parser() = default;

    Parser(ErrorManager *error_manager);

    ErrorManager *error_manager;

    std::vector<Token>           tokens;
    std::vector<Token>::iterator next_token;

    class OperatorInfo {
    public:
        OperatorNode::OperatorId id;
        int                      priority;
        int                      associativity;
    };

    OperatorInfo getOperatorInfo(Token *token, bool is_pre_op, int special = 0);
    void         goThrough(OperatorInfo op_info);

    class ParserState {
    public:
        int    cur_priority;
        int    cur_associativity;
        bool   report_errors;
        Token *token;    // for errors
    };

    ParserState                                      state;
    std::vector<ParserState>                         saved_states;
    // errors signaled but ignored due to !this->report_errors
    std::vector<std::pair<ParserState, std::string>> errors_stack;
    void                                             saveState();
    void                                             restoreState();
    void                                             highlight(Token *token);    // for errors
    void                                             highlightNext();            // for errors
    // is espected to signal an error. however, if the current state forbids that, the errors and the current state
    // are stored in errors stack, and function returns
    void                                             signalError(const std::string &message);

    bool canPassThrough(OperatorInfo op_info);

    bool           hasNext();      // returns false if tokens have ended
    Token::TokenId checkNext();    // doesn't consume
    bool           consume(Token::TokenId id);
    Token::TokenId consume();
    bool           hasPrev();
    void           rollback();    // goes one token back

    class ParsingResult {
    public:
        bool        success;
        std::string error_message;
        ParserState state;

        enum ResultId {
            EXPR,
            FUNC_DEC,
            TYPE_DEF,
            OPERATOR,
            ATOM,
            PAR_EXPR,
            IDENT_LIST,
            METHOD_DEF,
            STMT,
            WHILE_STMT,
            FOR_STMT,
            IF_STMT,
            CONTINUE_STMT,
            BREAK_STMT,
            RETURN_STMT,
            BLOCK_STMT
        } id;

        union {
            ExprNode       *expr;
            FuncDefNode    *func_def;
            TypeDefNode    *type_def;
            OperatorNode   *op;
            AtomNode       *atom;
            ParExprNode    *par_expr;
            IdentListNode  *ident_list;
            FuncDefNode    *method_def;
            StmtNode       *stmt;
            WhileStmtNode  *while_stmt;
            ForStmtNode    *for_stmt;
            IfStmtNode     *if_stmt;
            ReturnStmtNode *return_stmt;
            BlockStmtNode  *block_stmt;
        };

        ParsingResult(const std::string &error_message, Parser *p);    // means failure
        ParsingResult(ResultId id, Parser *p);
        ParsingResult(ExprNode *expr, Parser *p);
        ParsingResult(FuncDefNode *func_def, Parser *p);
        ParsingResult(TypeDefNode *type_def, Parser *p);
        ParsingResult(OperatorNode *op, Parser *p);
        ParsingResult(AtomNode *atom, Parser *p);
        ParsingResult(ParExprNode *par_expr, Parser *p);
        ParsingResult(IdentListNode *ident_list, Parser *p);
        ParsingResult(StmtNode *stmt, Parser *p);
        ParsingResult(WhileStmtNode *while_stmt, Parser *p);
        ParsingResult(ForStmtNode *for_stmt, Parser *p);
        ParsingResult(IfStmtNode *if_stmt, Parser *p);
        ParsingResult(ReturnStmtNode *return_stmt, Parser *p);
        ParsingResult(BlockStmtNode *block_stmt, Parser *p);

        // returns !success || this->id != id
        // if returns false, also adds error_message and the current state to the errors stack
        bool verify(Parser *p, ResultId id, const std::string error_message, bool highlight_next = true);
    };

public:
    ParsingResult parseExpr();
    ParsingResult parseStmt();

public:
    StmtNode *parse(const std::vector<Token> &tokens);
};
}    // namespace Cotton
