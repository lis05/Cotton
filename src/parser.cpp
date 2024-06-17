#include "parser.h"
#include "lexer.h"

namespace Cotton {
ExprNode::~ExprNode() {
    switch (this->id) {
    case FUNCTION_DEFINITION    : delete this->func_def; break;
    case RECORD_DEFINITION      : delete this->record_def; break;
    case OPERATOR               : delete this->op; break;
    case ATOM                   : delete this->atom; break;
    case PARENTHESES_EXPRESSION : delete this->par_expr; break;
    }

    this->func_def   = NULL;
    this->record_def = NULL;
    this->op         = NULL;
    this->atom       = NULL;
    this->par_expr   = NULL;
}

ExprNode::ExprNode(FuncDefNode *func_def) {
    this->record_def = NULL;
    this->op         = NULL;
    this->atom       = NULL;
    this->par_expr   = NULL;

    this->id       = FUNCTION_DEFINITION;
    this->func_def = func_def;
}

ExprNode::ExprNode(RecordDefNode *record_def) {
    this->func_def = NULL;
    this->op       = NULL;
    this->atom     = NULL;
    this->par_expr = NULL;

    this->id         = RECORD_DEFINITION;
    this->record_def = record_def;
}

ExprNode::ExprNode(OperatorNode *op) {
    this->func_def   = NULL;
    this->record_def = NULL;
    this->atom       = NULL;
    this->par_expr   = NULL;

    this->id = OPERATOR;
    this->op = op;
}

ExprNode::ExprNode(AtomNode *atom) {
    this->func_def   = NULL;
    this->record_def = NULL;
    this->op         = NULL;
    this->par_expr   = NULL;

    this->id   = ATOM;
    this->atom = atom;
}

ExprNode::ExprNode(ParExprNode *par_expr) {
    this->func_def   = NULL;
    this->record_def = NULL;
    this->op         = NULL;
    this->atom       = NULL;

    this->id       = PARENTHESES_EXPRESSION;
    this->par_expr = par_expr;
}

FuncDefNode::~FuncDefNode() {
    delete this->params;
    delete this->body;

    this->name   = NULL;
    this->params = NULL;
    this->body   = NULL;
}

FuncDefNode::FuncDefNode(Token *name, IdentListNode *params, StmtNode *body) {
    this->name   = name;
    this->params = params;
    this->body   = body;
}

RecordDefNode::~RecordDefNode() {
    for (auto method : this->methods) {
        delete method;
    }

    this->name = NULL;
    this->fields.clear();
    this->methods.clear();
}

RecordDefNode::RecordDefNode(Token                              *name,
                             const std::vector<Token *>         &fields,
                             const std::vector<MethodDefNode *> &methods) {
    this->name    = name;
    this->fields  = fields;
    this->methods = methods;
}

OperatorNode::~OperatorNode() {
    delete first;
    delete second;

    this->first  = NULL;
    this->second = NULL;
    this->op     = NULL;
}

OperatorNode::OperatorNode(OperatorId id, ExprNode *first, ExprNode *second, Token *op) {
    this->id     = id;
    this->first  = first;
    this->second = second;
    this->op     = op;
}

AtomNode::~AtomNode() {
    if (this->id == IDENTIFIER) {
        delete this->ident;
    }

    this->ident = NULL;
    this->token = NULL;
}

AtomNode::AtomNode(Token *token) {
    this->token = token;
    switch (token->id) {
    case Token::BOOLEAN_LIT : {
        this->id         = BOOLEAN;
        this->bool_value = token->bool_value;
        break;
    }
    case Token::CHAR_LIT : {
        this->id         = CHARACTER;
        this->char_value = token->char_value;
        break;
    }
    case Token::INT_LIT : {
        this->id        = INTEGER;
        this->int_value = token->int_value;
        break;
    }
    case Token::REAL_LIT : {
        this->id         = REAL;
        this->real_value = token->real_value;
        break;
    }
    case Token::STRING_LIT : {
        this->id           = STRING;
        this->string_value = token->string_value;
        break;
    }
    case Token::IDENTIFIER : {
        this->id    = IDENTIFIER;
        this->ident = token;
        break;
    }
    };
}

ParExprNode::~ParExprNode() {
    delete this->expr;
}

ParExprNode::ParExprNode(ExprNode *expr) {
    this->expr = expr;
}

IdentListNode::~IdentListNode() {
    this->list.clear();
}

IdentListNode::IdentListNode(const std::vector<Token *> &list) {
    this->list = list;
}

MethodDefNode::~MethodDefNode() {
    delete this->params;
    delete this->body;

    this->name   = NULL;
    this->params = NULL;
    this->body   = NULL;
}

MethodDefNode::MethodDefNode(Token *name, IdentListNode *params, StmtNode *body) {
    this->name   = name;
    this->params = params;
    this->body   = body;
}

StmtNode::~StmtNode() {
    switch (this->id) {
    case WHILE  : delete this->while_stmt; break;
    case FOR    : delete this->for_stmt; break;
    case IF     : delete this->if_stmt; break;
    case RETURN : delete this->return_stmt; break;
    case BLOCK  : delete this->block_stmt; break;
    case EXPR   : delete this->expr;
    }

    this->while_stmt  = NULL;
    this->for_stmt    = NULL;
    this->if_stmt     = NULL;
    this->return_stmt = NULL;
    this->block_stmt  = NULL;
    this->expr        = NULL;
}

StmtNode::StmtNode(WhileStmtNode *while_stmt) {
    this->for_stmt    = NULL;
    this->if_stmt     = NULL;
    this->return_stmt = NULL;
    this->block_stmt  = NULL;
    this->expr        = NULL;

    this->id         = WHILE;
    this->while_stmt = while_stmt;
}

StmtNode::StmtNode(ForStmtNode *for_stmt) {
    this->while_stmt  = NULL;
    this->if_stmt     = NULL;
    this->return_stmt = NULL;
    this->block_stmt  = NULL;
    this->expr        = NULL;

    this->id       = FOR;
    this->for_stmt = for_stmt;
}

StmtNode::StmtNode(IfStmtNode *if_stmt) {
    this->while_stmt  = NULL;
    this->for_stmt    = NULL;
    this->return_stmt = NULL;
    this->block_stmt  = NULL;
    this->expr        = NULL;

    this->id      = IF;
    this->if_stmt = if_stmt;
}

StmtNode::StmtNode(StmtId id) {
    this->while_stmt  = NULL;
    this->for_stmt    = NULL;
    this->if_stmt     = NULL;
    this->return_stmt = NULL;
    this->block_stmt  = NULL;
    this->expr        = NULL;

    this->id = id;
}

StmtNode::StmtNode(ReturnStmtNode *return_stmt) {
    this->while_stmt = NULL;
    this->for_stmt   = NULL;
    this->if_stmt    = NULL;
    this->block_stmt = NULL;
    this->expr       = NULL;

    this->id          = RETURN;
    this->return_stmt = return_stmt;
}

StmtNode::StmtNode(BlockStmtNode *block_stmt) {
    this->while_stmt  = NULL;
    this->for_stmt    = NULL;
    this->if_stmt     = NULL;
    this->return_stmt = NULL;
    this->expr        = NULL;

    this->id         = BLOCK;
    this->block_stmt = block_stmt;
}

StmtNode::StmtNode(ExprNode *expr) {
    this->while_stmt  = NULL;
    this->for_stmt    = NULL;
    this->if_stmt     = NULL;
    this->return_stmt = NULL;
    this->block_stmt  = NULL;

    this->id   = EXPR;
    this->expr = expr;
}

WhileStmtNode::~WhileStmtNode() {
    delete this->cond;
    delete this->body;

    this->cond = NULL;
    this->body = NULL;
}

WhileStmtNode::WhileStmtNode(ExprNode *cond, StmtNode *body) {
    this->cond = cond;
    this->body = body;
}

ForStmtNode::~ForStmtNode() {
    delete this->init;
    delete this->cond;
    delete this->step;
    delete this->body;

    this->init = NULL;
    this->cond = NULL;
    this->step = NULL;
    this->body = NULL;
}

ForStmtNode::ForStmtNode(ExprNode *init, ExprNode *cond, ExprNode *step, StmtNode *body) {
    this->init = init;
    this->cond = cond;
    this->step = step;
    this->body = body;
}

IfStmtNode::~IfStmtNode() {
    delete this->cond;
    delete this->body;
    delete this->else_body;

    this->cond      = NULL;
    this->body      = NULL;
    this->else_body = NULL;
}

IfStmtNode::IfStmtNode(ExprNode *cond, StmtNode *body, StmtNode *else_body) {
    this->cond      = cond;
    this->body      = body;
    this->else_body = else_body;
}

ReturnStmtNode::~ReturnStmtNode() {
    delete this->value;

    this->value = NULL;
}

ReturnStmtNode::ReturnStmtNode(ExprNode *value) {
    this->value = value;
}

BlockStmtNode::~BlockStmtNode() {
    for (auto elem : this->list) {
        delete elem;
    }

    this->list.clear();
}

BlockStmtNode::BlockStmtNode(bool is_scoped, const std::vector<StmtNode *> list) {
    this->is_scoped = is_scoped;
    this->list      = list;
}
}    // namespace Cotton
