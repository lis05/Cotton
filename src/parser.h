#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace Cotton {
class Token;
class ErrorManager;

class ExprNode;
class FuncDefNode;
class RecordDefNode;
class OperatorNode;
class AtomNode;
class ParExprNode;
class IdentListNode;
class MethodDefNode;
class StmtNode;
class WhileStmtNode;
class ForStmtNode;
class IfStmtNode;
class ReturnStmtNode;
class BlockStmtNode;

class ExprNode {
public:
    enum ExprNodeId { FUNCTION_DEFINITION, RECORD_DEFINITION, OPERATOR, ATOM, PARENTHESES_EXPRESSION } id;

    union {
        FuncDefNode   *func_def;
        RecordDefNode *record_def;
        OperatorNode  *op;
        AtomNode      *atom;
        ParExprNode   *par_expr;
    };

    ExprNode() = delete;
    ~ExprNode();

    ExprNode(FuncDefNode *func_def);
    ExprNode(RecordDefNode *record_def);
    ExprNode(OperatorNode *op);
    ExprNode(AtomNode *atom);
    ExprNode(ParExprNode *par_expr);
};

class FuncDefNode {
public:
    Token         *name;      // NULL means not present
    IdentListNode *params;    // NULL means not present
    StmtNode      *body;

    FuncDefNode() = delete;
    ~FuncDefNode();

    FuncDefNode(Token *name, IdentListNode *params, StmtNode *body);
};

class RecordDefNode {
public:
    Token                       *name;
    std::vector<Token *>         fields;
    std::vector<MethodDefNode *> methods;

    RecordDefNode() = delete;
    ~RecordDefNode();

    RecordDefNode(Token *name, const std::vector<Token *> &fields, const std::vector<MethodDefNode *> &methods);
};

class OperatorNode {
public:
    enum OperatorId {
        POST_PLUS_PLUS,
        POST_MINUS_MINUS,
        CALL,
        INDEX,
        DOT,
        PREF_PLUS_PLUS,
        PREF_MINUS_MINUS,
        PREF_PLUS,
        PREF_MINUS,
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
        COMMA
    } id;

    ExprNode *first, *second;    // if second is NULL then it's not present, and the operator is unary
    Token    *op;                // for error handling

    OperatorNode() = delete;
    ~OperatorNode();

    OperatorNode(OperatorId id, ExprNode *first, ExprNode *second, Token *op);
};

class AtomNode {
public:
    enum AtomId { BOOLEAN, CHARACTER, INTEGER, REAL, STRING, IDENTIFIER, NOTHING } id;

    bool        bool_value;
    char        char_value;
    int64_t     int_value;
    double      real_value;
    std::string string_value;
    Token      *ident;

    Token *token;    // for error handling

    AtomNode() = delete;
    ~AtomNode();

    AtomNode(Token *token);    // takes value from the token
};

class ParExprNode {
public:
    ExprNode *expr;

    ParExprNode() = delete;
    ~ParExprNode();

    ParExprNode(ExprNode *expr);
};

class IdentListNode {
public:
    std::vector<Token *> list;

    IdentListNode() = delete;
    ~IdentListNode();

    IdentListNode(const std::vector<Token *> &list);
};

class MethodDefNode {
public:
    Token         *name;
    IdentListNode *params;    // NULL means not present
    StmtNode      *body;

    MethodDefNode() = delete;
    ~MethodDefNode();

    MethodDefNode(Token *name, IdentListNode *params, StmtNode *body);
};

class StmtNode {
public:
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

    StmtNode(WhileStmtNode *while_stmt);
    StmtNode(ForStmtNode *for_stmt);
    StmtNode(IfStmtNode *if_stmt);
    StmtNode(StmtId id);    // for continue and brake only
    StmtNode(ReturnStmtNode *return_stmt);
    StmtNode(BlockStmtNode *block_stmt);
    StmtNode(ExprNode *expr);
};

class WhileStmtNode {
public:
    ExprNode *cond;
    StmtNode *body;

    WhileStmtNode() = delete;
    ~WhileStmtNode();

    WhileStmtNode(ExprNode *cond, StmtNode *body);
};

class ForStmtNode {
public:
    ExprNode *init, *cond, *step;
    StmtNode *body;

    ForStmtNode() = delete;
    ~ForStmtNode();

    ForStmtNode(ExprNode *init, ExprNode *cond, ExprNode *step, StmtNode *body);
};

class IfStmtNode {
public:
    ExprNode *cond;
    StmtNode *body;
    StmtNode *else_body;    // if NULL then not present

    IfStmtNode() = delete;
    ~IfStmtNode();

    IfStmtNode(ExprNode *cond, StmtNode *body, StmtNode *else_body);
};

class ReturnStmtNode {
public:
    ExprNode *value;

    ReturnStmtNode() = delete;
    ~ReturnStmtNode();

    ReturnStmtNode(ExprNode *value);
};

class BlockStmtNode {
public:
    bool                    is_scoped;
    std::vector<StmtNode *> list;

    BlockStmtNode() = delete;
    ~BlockStmtNode();

    BlockStmtNode(bool is_scoped, const std::vector<StmtNode *> list);
};

}    // namespace Cotton
