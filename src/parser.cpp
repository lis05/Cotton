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

#include "parser.h"
#include "errors.h"
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

#define passert(value, message) __passert__(value, #value, message, this, __FILE__, __LINE__);

void __passert__(bool               value,
                 const char        *assertion,
                 const std::string &message,
                 Parser            *parser,
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
        parser->signalError(std::string(buf));
    }
}

#define LEFT_TO_RIGHT 1
#define RIGHT_TO_LEFT -1

Parser::Parser(ErrorManager *error_manager) {
    this->error_manager = error_manager;
}

Parser::OperatorInfo Parser::getOperatorInfo(Token *token, bool is_pre_op, int special) {
    passert(token != NULL, "Failed to get operator from a NULL token");
    switch (token->id) {
    case Token::PLUS_PLUS_OP :
        return (is_pre_op) ? OperatorInfo {OperatorNode::PRE_PLUS_PLUS, 2, RIGHT_TO_LEFT}
                           : OperatorInfo {OperatorNode::POST_PLUS_PLUS, 1, LEFT_TO_RIGHT};
    case Token::MINUS_MINUS_OP :
        return (is_pre_op) ? OperatorInfo {OperatorNode::PRE_MINUS_MINUS, 2, RIGHT_TO_LEFT}
                           : OperatorInfo {OperatorNode::PRE_PLUS_PLUS, 1, LEFT_TO_RIGHT};
    case Token::DOT_OP : return OperatorInfo {OperatorNode::DOT, 1, LEFT_TO_RIGHT};
    case Token::PLUS_OP :
        return (is_pre_op) ? OperatorInfo {OperatorNode::PRE_PLUS, 2, RIGHT_TO_LEFT}
                           : OperatorInfo {OperatorNode::PLUS, 5, LEFT_TO_RIGHT};
    case Token::MINUS_OP :
        return (is_pre_op) ? OperatorInfo {OperatorNode::PRE_MINUS, 2, RIGHT_TO_LEFT}
                           : OperatorInfo {OperatorNode::MINUS, 5, LEFT_TO_RIGHT};
    case Token::NOT_OP              : return OperatorInfo {OperatorNode::NOT, 2, RIGHT_TO_LEFT};
    case Token::INVERSE_OP          : return OperatorInfo {OperatorNode::INVERSE, 2, RIGHT_TO_LEFT};
    case Token::MULT_OP             : return OperatorInfo {OperatorNode::MULT, 3, LEFT_TO_RIGHT};
    case Token::DIV_OP              : return OperatorInfo {OperatorNode::DIV, 3, LEFT_TO_RIGHT};
    case Token::REMAINDER_OP        : return OperatorInfo {OperatorNode::REM, 3, LEFT_TO_RIGHT};
    case Token::RIGHT_SHIFT_OP      : return OperatorInfo {OperatorNode::RIGHT_SHIFT, 4, LEFT_TO_RIGHT};
    case Token::LEFT_SHIFT_OP       : return OperatorInfo {OperatorNode::LEFT_SHIFT, 4, LEFT_TO_RIGHT};
    case Token::LESS_OP             : return OperatorInfo {OperatorNode::LESS, 6, LEFT_TO_RIGHT};
    case Token::LESS_EQUAL_OP       : return OperatorInfo {OperatorNode::LESS_EQUAL, 6, LEFT_TO_RIGHT};
    case Token::GREATER_OP          : return OperatorInfo {OperatorNode::GREATER, 6, LEFT_TO_RIGHT};
    case Token::GREATER_EQUAL_OP    : return OperatorInfo {OperatorNode::GREATER_EQUAL, 6, LEFT_TO_RIGHT};
    case Token::EQUAL_OP            : return OperatorInfo {OperatorNode::EQUAL, 7, LEFT_TO_RIGHT};
    case Token::NOT_EQUAL_OP        : return OperatorInfo {OperatorNode::NOT_EQUAL, 7, LEFT_TO_RIGHT};
    case Token::BITAND_OP           : return OperatorInfo {OperatorNode::BITAND, 8, LEFT_TO_RIGHT};
    case Token::BITXOR_OP           : return OperatorInfo {OperatorNode::BITXOR, 9, LEFT_TO_RIGHT};
    case Token::BITOR_OP            : return OperatorInfo {OperatorNode::BITOR, 10, LEFT_TO_RIGHT};
    case Token::AND_OP              : return OperatorInfo {OperatorNode::AND, 11, LEFT_TO_RIGHT};
    case Token::OR_OP               : return OperatorInfo {OperatorNode::OR, 12, LEFT_TO_RIGHT};
    case Token::ASSIGN_OP           : return OperatorInfo {OperatorNode::ASSIGN, 13, RIGHT_TO_LEFT};
    case Token::ASSIGN_PLUS_OP      : return OperatorInfo {OperatorNode::PLUS_ASSIGN, 13, RIGHT_TO_LEFT};
    case Token::ASSIGN_MINUS_OP     : return OperatorInfo {OperatorNode::MINUS_ASSIGN, 13, RIGHT_TO_LEFT};
    case Token::ASSIGN_MULT_OP      : return OperatorInfo {OperatorNode::MULT_ASSIGN, 13, RIGHT_TO_LEFT};
    case Token::ASSIGN_DIV_OP       : return OperatorInfo {OperatorNode::DIV_ASSIGN, 13, RIGHT_TO_LEFT};
    case Token::ASSIGN_REMAINDER_OP : return OperatorInfo {OperatorNode::REM_ASSIGN, 13, RIGHT_TO_LEFT};
    case Token::COMMA               : return OperatorInfo {OperatorNode::COMMA, 14, LEFT_TO_RIGHT};
    }

    if (special == 1) {
        return OperatorInfo {OperatorNode::CALL, 1, LEFT_TO_RIGHT};
    }
    if (special == 2) {
        return OperatorInfo {OperatorNode::INDEX, 1, LEFT_TO_RIGHT};
    }
    this->highlight(token);
    this->signalError("Not an operator");
    return {};
}

void Parser::saveState() {
    this->saved_states.push_back(this->state);
}

void Parser::restoreState() {
    if (this->saved_states.empty()) {
        signalError("Failed to restore parser's state");
    }
    else {
        this->state = this->saved_states.back(), this->saved_states.pop_back();
    }
}

void Parser::highlight(Token *token) {
    this->state.token = token;
}

void Parser::highlightNext() {
    if (this->hasNext()) {
        this->highlight(&*this->next_token);
    }
    else if (this->next_token != this->tokens.begin()) {
        this->highlight(&*prev(this->next_token));
    }
}

void Parser::signalError(const std::string &message) {
    this->errors_stack.push_back({this->state, message});

    if (!this->state.report_errors) {
        return;
    }

    std::vector<std::pair<Token *, std::string>> errors;
    for (auto it = this->errors_stack.rbegin(); it != this->errors_stack.rend(); it++) {
        errors.push_back({it->first.token, it->second});
    }

    this->error_manager->signalError(errors);
}

bool Parser::canPassThrough(OperatorNode::OperatorId operator_id) {
    return false;
}

bool Parser::hasNext() {
    return this->next_token != this->tokens.end();
}

Token::TokenId Parser::checkNext() {
    return this->next_token->id;
}

bool Parser::consume(Token::TokenId id) {
    if (this->next_token->id != id) {
        return false;
    }
    this->highlight(&*this->next_token);
    this->next_token++;
    return true;
}

Token::TokenId Parser::consume() {
    if (this->hasNext()) {
        this->highlight(&*this->next_token);
    }
    return (this->next_token++)->id;
}

bool Parser::hasPrev() {
    return this->next_token != this->tokens.begin();
}

void Parser::rollback() {
    this->next_token--;
    if (this->next_token != this->tokens.begin()) {
        this->highlight(&*prev(this->next_token));
    }
}

bool Parser::ParsingResult::verify(Parser *p, ResultId id, const std::string error_message) {
    if (this->success && this->id == id) {
        return true;
    }
    p->errors_stack.push_back({p->state, this->error_message});
    p->signalError(error_message);
    return false;
}

static void skipSemicolons(Parser *parser, bool single = false) {
    while (parser->hasNext() && parser->consume(Token::SEMICOLON)) {
        if (single) {
            break;
        }
        continue;
    }
}

Parser::ParsingResult Parser::parseExpr() {
    // temp
    if (this->hasNext()) {
        if (this->next_token->id == Token::IDENTIFIER && this->next_token->data == "E") {
            this->consume();
            return ParsingResult((ExprNode *)NULL);
        }
    }
    return ParsingResult("");
}

Parser::ParsingResult Parser::parseStmt() {
    // skip all semicolons
    skipSemicolons(this);

    if (!this->hasNext()) {
        this->highlightNext();
        this->signalError("Expected a statement");
        return ParsingResult("");
    }

    if (this->consume(Token::WHILE_KW)) {
        this->saveState();
        auto cond = this->parseExpr();
        this->restoreState();
        this->highlightNext();

        if (!cond.verify(this, ParsingResult::EXPR, "While loop must have its condition")) {
            return ParsingResult("");
        }

        this->saveState();
        auto body = this->parseStmt();
        this->restoreState();
        this->highlightNext();

        if (!body.verify(this, ParsingResult::STMT, "While loop must have its body")) {
            return ParsingResult("");
        }

        auto while_stmt = new WhileStmtNode(cond.expr, body.stmt);
        auto stmt       = new StmtNode(while_stmt);
        return ParsingResult(stmt);
    }
    else if (this->consume(Token::FOR_KW)) {
        this->saveState();
        auto init = this->parseExpr();
        this->restoreState();
        this->highlightNext();

        if (!init.verify(this, ParsingResult::EXPR, "For loop must have its initialization")) {
            return ParsingResult("");
        }
        if (!this->consume(Token::SEMICOLON)) {
            this->signalError("For loop's initialization must end with a semicolon.");
            return ParsingResult("");
        }

        this->saveState();
        auto cond = this->parseExpr();
        this->restoreState();
        this->highlightNext();

        if (!cond.verify(this, ParsingResult::EXPR, "For loop must have its condition")) {
            return ParsingResult("");
        }
        if (!this->consume(Token::SEMICOLON)) {
            this->signalError("For loop's condition must end with a semicolon.");
            return ParsingResult("");
        }

        this->saveState();
        auto step = this->parseExpr();
        this->restoreState();
        this->highlightNext();

        if (!step.verify(this, ParsingResult::EXPR, "For loop must have its step")) {
            return ParsingResult("");
        }

        this->saveState();
        auto body = this->parseStmt();
        this->restoreState();
        this->highlightNext();

        if (!body.verify(this, ParsingResult::STMT, "For loop must have its body")) {
            return ParsingResult("");
        }

        auto for_stmt = new ForStmtNode(init.expr, cond.expr, step.expr, body.stmt);
        auto stmt     = new StmtNode(for_stmt);
        return ParsingResult(stmt);
    }
    else if (this->consume(Token::IF_KW)) {
        this->saveState();
        auto cond = this->parseExpr();
        this->restoreState();
        this->highlightNext();

        if (!cond.verify(this, ParsingResult::EXPR, "If statement must have its condition")) {
            return ParsingResult("");
        }

        this->saveState();
        auto body = this->parseStmt();
        this->restoreState();
        this->highlightNext();

        if (!body.verify(this, ParsingResult::STMT, "If statement must have its body")) {
            return ParsingResult("");
        }

        if (!this->consume(Token::ELSE_KW)) {
            auto if_stmt = new IfStmtNode(cond.expr, body.stmt, NULL);
            auto stmt    = new StmtNode(if_stmt);
            return ParsingResult(stmt);
        }

        this->saveState();
        auto else_body = this->parseStmt();
        this->restoreState();
        this->highlightNext();

        if (!else_body.verify(this, ParsingResult::STMT, "If-else statement must have its else body")) {
            return ParsingResult("");
        }

        auto if_stmt = new IfStmtNode(cond.expr, body.stmt, else_body.stmt);
        auto stmt    = new StmtNode(if_stmt);
        return ParsingResult(stmt);
    }

    // temp
    if (this->hasNext()) {
        if (this->next_token->id == Token::IDENTIFIER && this->next_token->data == "S") {
            this->consume();
            return ParsingResult((StmtNode *)NULL);
        }
    }

    return ParsingResult("");
}

StmtNode *Parser::parse(const std::vector<Token> &tokens) {
    this->tokens                  = tokens;
    this->next_token              = this->tokens.begin();
    this->state.cur_associativity = LEFT_TO_RIGHT;
    this->state.cur_priority      = 0;
    this->state.report_errors     = true;
    this->state.token             = NULL;
    this->error_manager->setErrorCharPos(-1);
    this->error_manager->setErrorMessage("");

    if (!tokens.empty()) {
        this->highlight((Token *)&(tokens[0]));
    }

    std::vector<StmtNode *> statements;
    while (this->hasNext()) {
        this->saveState();
        this->state.report_errors = false;
        ParsingResult res         = this->parseStmt();
        this->restoreState();

        res.verify(this, ParsingResult::STMT, "Failed to parse a statement");

        statements.push_back(res.stmt);
    }
    auto block = new BlockStmtNode(false, statements);
    auto res   = new StmtNode(block);
    return res;
}

Parser::ParsingResult::ParsingResult(const std::string &error_message) {
    this->success       = false;
    this->error_message = error_message;
}

Parser::ParsingResult::ParsingResult(ResultId id) {
    this->success = true;
    this->id      = id;
}

Parser::ParsingResult::ParsingResult(ExprNode *expr) {
    this->success = true;
    this->id      = EXPR;
    this->expr    = expr;
}

Parser::ParsingResult::ParsingResult(FuncDefNode *func_def) {
    this->success = true;
    this->id      = FUNC_DEC;
    this->expr    = expr;
}

Parser::ParsingResult::ParsingResult(RecordDefNode *record_def) {
    this->success = true;
    this->id      = RECORD_DEF;
    this->expr    = expr;
}

Parser::ParsingResult::ParsingResult(OperatorNode *op) {
    this->success = true;
    this->id      = OPERATOR;
    this->expr    = expr;
}

Parser::ParsingResult::ParsingResult(AtomNode *atom) {
    this->success = true;
    this->id      = ATOM;
    this->expr    = expr;
}

Parser::ParsingResult::ParsingResult(ParExprNode *par_expr) {
    this->success = true;
    this->id      = PAR_EXPR;
    this->expr    = expr;
}

Parser::ParsingResult::ParsingResult(IdentListNode *ident_list) {
    this->success = true;
    this->id      = IDENT_LIST;
    this->expr    = expr;
}

Parser::ParsingResult::ParsingResult(MethodDefNode *method_def) {
    this->success = true;
    this->id      = METHOD_DEF;
    this->expr    = expr;
}

Parser::ParsingResult::ParsingResult(StmtNode *stmt) {
    this->success = true;
    this->id      = STMT;
    this->stmt    = stmt;
}

Parser::ParsingResult::ParsingResult(WhileStmtNode *while_stmt) {
    this->success    = true;
    this->id         = WHILE_STMT;
    this->while_stmt = while_stmt;
}

Parser::ParsingResult::ParsingResult(ForStmtNode *for_stmt) {
    this->success  = true;
    this->id       = FOR_STMT;
    this->for_stmt = for_stmt;
}

Parser::ParsingResult::ParsingResult(IfStmtNode *if_stmt) {
    this->success = true;
    this->id      = IF_STMT;
    this->if_stmt = if_stmt;
}

Parser::ParsingResult::ParsingResult(ReturnStmtNode *return_stmt) {
    this->success     = true;
    this->id          = RETURN_STMT;
    this->return_stmt = return_stmt;
}

Parser::ParsingResult::ParsingResult(BlockStmtNode *block_stmt) {
    this->success    = true;
    this->id         = BLOCK_STMT;
    this->block_stmt = block_stmt;
}

}    // namespace Cotton
