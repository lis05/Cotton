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
#include "../errors.h"
#include "lexer.h"

namespace Cotton {

TextArea::TextArea() {
    this->first_char = INT64_MAX;
    this->last_char  = INT64_MIN;
    this->filename   = nullptr;
}

TextArea::TextArea(Token &token) {
    this->first_char = token.begin_pos;
    this->last_char  = token.end_pos;
    this->filename   = &token.filename;
}

TextArea::TextArea(const TextArea &first, const TextArea &last) {
    this->first_char = std::min(first.first_char, last.first_char);
    this->last_char  = std::max(first.last_char, last.last_char);
    this->filename   = (first.filename == nullptr || first.filename->empty()) ? last.filename : first.filename;
}

ExprNode::~ExprNode() {
    switch (this->id) {
    case FUNCTION_DEFINITION    : delete this->func_def; break;
    case TYPE_DEFINITION        : delete this->type_def; break;
    case OPERATOR               : delete this->op; break;
    case ATOM                   : delete this->atom; break;
    case PARENTHESES_EXPRESSION : delete this->par_expr; break;
    }

    this->func_def = nullptr;
    this->type_def = nullptr;
    this->op       = nullptr;
    this->atom     = nullptr;
    this->par_expr = nullptr;
}

ExprNode::ExprNode(FuncDefNode *func_def, TextArea text_area) {
    this->text_area = text_area;
    this->type_def  = nullptr;
    this->op        = nullptr;
    this->atom      = nullptr;
    this->par_expr  = nullptr;

    this->id       = FUNCTION_DEFINITION;
    this->func_def = func_def;
}

ExprNode::ExprNode(TypeDefNode *type_def, TextArea text_area) {
    this->text_area = text_area;
    this->func_def  = nullptr;
    this->op        = nullptr;
    this->atom      = nullptr;
    this->par_expr  = nullptr;

    this->id       = TYPE_DEFINITION;
    this->type_def = type_def;
}

ExprNode::ExprNode(OperatorNode *op, TextArea text_area) {
    this->text_area = text_area;
    this->func_def  = nullptr;
    this->type_def  = nullptr;
    this->atom      = nullptr;
    this->par_expr  = nullptr;

    this->id = OPERATOR;
    this->op = op;
}

ExprNode::ExprNode(AtomNode *atom, TextArea text_area) {
    this->text_area = text_area;
    this->func_def  = nullptr;
    this->type_def  = nullptr;
    this->op        = nullptr;
    this->par_expr  = nullptr;

    this->id   = ATOM;
    this->atom = atom;
}

ExprNode::ExprNode(ParExprNode *par_expr, TextArea text_area) {
    this->text_area = text_area;
    this->func_def  = nullptr;
    this->type_def  = nullptr;
    this->op        = nullptr;
    this->atom      = nullptr;

    this->id       = PARENTHESES_EXPRESSION;
    this->par_expr = par_expr;
}

static void printPrefix(int indent, int step, bool init = true) {
    int cnt = 0;
    for (int i = 0; i < indent; i++) {
        for (int ii = 0; ii < step; ii++) {
            printf(" ");
        }
        if (init || (!init && i < indent - 1)) {
            if (i % 2 == 0) {
                printf("|");
            }
            else {
                printf("?");
            }
        }
    }
}

void ExprNode::print(int indent, int step) {
    printPrefix(indent, step, false);
    if (this == nullptr) {
        printf("nullptr\n");
        return;
    }
    printf("expression:\n");
    if (this->id == ExprNode::FUNCTION_DEFINITION) {
        this->func_def->print(indent + 1, step);
    }
    else if (this->id == ExprNode::TYPE_DEFINITION) {
        this->type_def->print(indent + 1, step);
    }
    else if (this->id == ExprNode::OPERATOR) {
        this->op->print(indent + 1, step);
    }
    else if (this->id == ExprNode::ATOM) {
        this->atom->print(indent + 1, step);
    }
    else if (this->id == ExprNode::PARENTHESES_EXPRESSION) {
        this->par_expr->print(indent + 1, step);
    }
}

FuncDefNode::~FuncDefNode() {
    delete this->params;
    delete this->body;

    this->name   = nullptr;
    this->params = nullptr;
    this->body   = nullptr;
}

FuncDefNode::FuncDefNode(Token *name, IdentListNode *params, StmtNode *body, TextArea text_area) {
    this->text_area = text_area;
    this->name      = name;
    this->params    = params;
    this->body      = body;
}

void FuncDefNode::print(int indent, int step) {
    printPrefix(indent, step, false);
    if (this == nullptr) {
        printf("nullptr\n");
        return;
    }
    printf("function definition:\n");

    printPrefix(indent, step);
    if (this->name == nullptr) {
        printf("name: nullptr token\n");
    }
    else {
        printf("name: %s\n", this->name->data.c_str());
    }

    printPrefix(indent, step);
    printf("params:\n");
    this->params->print(indent + 1, step);

    printPrefix(indent, step);
    printf("body:\n");
    this->body->print(indent + 1, step);
}

TypeDefNode::~TypeDefNode() {
    for (auto method : this->methods) {
        delete method;
    }

    this->name = nullptr;
    this->fields.clear();
    this->methods.clear();
}

TypeDefNode::TypeDefNode(Token                            *name,
                         const std::vector<Token *>       &fields,
                         const std::vector<FuncDefNode *> &methods,
                         TextArea                          text_area) {
    this->text_area = text_area;
    this->name      = name;
    this->fields    = fields;
    this->methods   = methods;
}

void TypeDefNode::print(int indent, int step) {
    printPrefix(indent, step, false);
    if (this == nullptr) {
        printf("nullptr\n");
        return;
    }
    printf("type definition:\n");

    printPrefix(indent, step);
    if (this->name == nullptr) {
        printf("name: nullptr token\n");
    }
    else {
        printf("name: %s\n", this->name->data.c_str());
    }

    printPrefix(indent, step);
    printf("fields:\n");
    int i = 0;
    for (auto field : this->fields) {
        printPrefix(indent, step);
        printf("%d: %s\n", i++, field->data.c_str());
    }

    printPrefix(indent, step);
    printf("methods:\n");
    i = 0;
    for (auto method : this->methods) {
        printPrefix(indent, step);
        printf("%d:\n", i++);
        method->print(indent + 1, step);
    }
}

OperatorNode::~OperatorNode() {
    delete first;
    delete second;

    this->first  = nullptr;
    this->second = nullptr;
    this->op     = nullptr;
}

OperatorNode::OperatorNode(OperatorId id, ExprNode *first, ExprNode *second, Token *op, TextArea text_area) {
    this->text_area = text_area;
    this->id        = id;
    this->first     = first;
    this->second    = second;
    this->op        = op;
}

void OperatorNode::print(int indent, int step) {
    printPrefix(indent, step, false);
    if (this == nullptr) {
        printf("nullptr\n");
        return;
    }
    printf("operator %s\n", this->op->data.c_str());

    printPrefix(indent, step);
    printf("arg1:\n");
    this->first->print(indent + 1, step);

    printPrefix(indent, step);
    printf("arg2:\n");
    this->second->print(indent + 1, step);
}

AtomNode::~AtomNode() {
    this->ident = nullptr;
    this->token = nullptr;
}

AtomNode::AtomNode(Token *token, TextArea text_area) {
    this->text_area = text_area;
    this->lit_obj   = nullptr;
    this->token     = token;
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
    case Token::NOTHING_LIT : {
        this->id = NOTHING;
        break;
    }
    }
}

void AtomNode::print(int indent, int step) {
    printPrefix(indent, step, false);
    if (this == nullptr) {
        printf("nullptr\n");
        return;
    }
    printf("atom");

    if (this->id == BOOLEAN) {
        printf(" boolean: %s\n", this->token->data.c_str());
    }
    else if (this->id == CHARACTER) {
        printf(" character: %s\n", this->token->data.c_str());
    }
    else if (this->id == INTEGER) {
        printf(" integer: %s\n", this->token->data.c_str());
    }
    else if (this->id == REAL) {
        printf(" real: %s\n", this->token->data.c_str());
    }
    else if (this->id == STRING) {
        printf(" string: %s\n", this->token->data.c_str());
    }
    else if (this->id == IDENTIFIER) {
        printf(" identifier: %s\n", this->token->data.c_str());
    }
    else if (this->id == NOTHING) {
        printf(" nothing: %s\n", this->token->data.c_str());
    }
}

ParExprNode::~ParExprNode() {
    delete this->expr;
}

ParExprNode::ParExprNode(ExprNode *expr, TextArea text_area) {
    this->text_area = text_area;
    this->expr      = expr;
}

void ParExprNode::print(int indent, int step) {
    printPrefix(indent, step, false);
    if (this == nullptr) {
        printf("nullptr\n");
        return;
    }
    printf("parentheses expression\n");

    this->expr->print(indent + 1, step);
}

IdentListNode::~IdentListNode() {
    this->list.clear();
}

IdentListNode::IdentListNode(const std::vector<Token *> &list, TextArea text_area) {
    this->text_area = text_area;
    this->list      = list;
}

void IdentListNode::print(int indent, int step) {
    printPrefix(indent, step, false);
    if (this == nullptr) {
        printf("nullptr\n");
        return;
    }
    printf("identifier list:\n");

    int i = 0;
    for (auto ident : this->list) {
        printPrefix(indent, step);
        printf("%d: %s\n", i++, ident->data.c_str());
    }
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

    this->while_stmt  = nullptr;
    this->for_stmt    = nullptr;
    this->if_stmt     = nullptr;
    this->return_stmt = nullptr;
    this->block_stmt  = nullptr;
    this->expr        = nullptr;
}

StmtNode::StmtNode(WhileStmtNode *while_stmt, TextArea text_area) {
    this->text_area   = text_area;
    this->for_stmt    = nullptr;
    this->if_stmt     = nullptr;
    this->return_stmt = nullptr;
    this->block_stmt  = nullptr;
    this->expr        = nullptr;

    this->id         = WHILE;
    this->while_stmt = while_stmt;
}

StmtNode::StmtNode(ForStmtNode *for_stmt, TextArea text_area) {
    this->text_area   = text_area;
    this->while_stmt  = nullptr;
    this->if_stmt     = nullptr;
    this->return_stmt = nullptr;
    this->block_stmt  = nullptr;
    this->expr        = nullptr;

    this->id       = FOR;
    this->for_stmt = for_stmt;
}

StmtNode::StmtNode(IfStmtNode *if_stmt, TextArea text_area) {
    this->text_area   = text_area;
    this->while_stmt  = nullptr;
    this->for_stmt    = nullptr;
    this->return_stmt = nullptr;
    this->block_stmt  = nullptr;
    this->expr        = nullptr;

    this->id      = IF;
    this->if_stmt = if_stmt;
}

StmtNode::StmtNode(StmtId id, TextArea text_area) {
    this->text_area   = text_area;
    this->while_stmt  = nullptr;
    this->for_stmt    = nullptr;
    this->if_stmt     = nullptr;
    this->return_stmt = nullptr;
    this->block_stmt  = nullptr;
    this->expr        = nullptr;

    this->id = id;
}

StmtNode::StmtNode(ReturnStmtNode *return_stmt, TextArea text_area) {
    this->text_area  = text_area;
    this->while_stmt = nullptr;
    this->for_stmt   = nullptr;
    this->if_stmt    = nullptr;
    this->block_stmt = nullptr;
    this->expr       = nullptr;

    this->id          = RETURN;
    this->return_stmt = return_stmt;
}

StmtNode::StmtNode(BlockStmtNode *block_stmt, TextArea text_area) {
    this->text_area   = text_area;
    this->while_stmt  = nullptr;
    this->for_stmt    = nullptr;
    this->if_stmt     = nullptr;
    this->return_stmt = nullptr;
    this->expr        = nullptr;

    this->id         = BLOCK;
    this->block_stmt = block_stmt;
}

StmtNode::StmtNode(ExprNode *expr, TextArea text_area) {
    this->text_area   = text_area;
    this->while_stmt  = nullptr;
    this->for_stmt    = nullptr;
    this->if_stmt     = nullptr;
    this->return_stmt = nullptr;
    this->block_stmt  = nullptr;

    this->id   = EXPR;
    this->expr = expr;
}

void StmtNode::print(int indent, int step) {
    printPrefix(indent, step, false);
    if (this == nullptr) {
        printf("nullptr\n");
        return;
    }
    printf("statement:\n");
    if (this->id == StmtNode::WHILE) {
        this->while_stmt->print(indent + 1, step);
    }
    else if (this->id == StmtNode::FOR) {
        this->for_stmt->print(indent + 1, step);
    }
    else if (this->id == StmtNode::IF) {
        this->if_stmt->print(indent + 1, step);
    }
    else if (this->id == StmtNode::CONTINUE) {
        printPrefix(indent + 1, step, false);
        printf("continue\n");
    }
    else if (this->id == StmtNode::BREAK) {
        printPrefix(indent + 1, step, false);
        printf("break\n");
    }
    else if (this->id == StmtNode::RETURN) {
        this->return_stmt->print(indent + 1, step);
    }
    else if (this->id == StmtNode::BLOCK) {
        this->block_stmt->print(indent + 1, step);
    }
    else if (this->id == StmtNode::EXPR) {
        this->expr->print(indent + 1, step);
    }
}

WhileStmtNode::~WhileStmtNode() {
    delete this->cond;
    delete this->body;

    this->cond = nullptr;
    this->body = nullptr;
}

WhileStmtNode::WhileStmtNode(ExprNode *cond, StmtNode *body, TextArea text_area) {
    this->text_area = text_area;
    this->cond      = cond;
    this->body      = body;
}

void WhileStmtNode::print(int indent, int step) {
    printPrefix(indent, step, false);
    if (this == nullptr) {
        printf("nullptr\n");
        return;
    }
    printf("while:\n");

    printPrefix(indent, step);
    printf("condition:\n");
    this->cond->print(indent + 1, step);

    printPrefix(indent, step);
    printf("body:\n");
    this->body->print(indent + 1, step);
}

ForStmtNode::~ForStmtNode() {
    delete this->init;
    delete this->cond;
    delete this->step;
    delete this->body;

    this->init = nullptr;
    this->cond = nullptr;
    this->step = nullptr;
    this->body = nullptr;
}

ForStmtNode::ForStmtNode(ExprNode *init, ExprNode *cond, ExprNode *step, StmtNode *body, TextArea text_area) {
    this->text_area = text_area;
    this->init      = init;
    this->cond      = cond;
    this->step      = step;
    this->body      = body;
}

void ForStmtNode::print(int indent, int step) {
    printPrefix(indent, step, false);
    if (this == nullptr) {
        printf("nullptr\n");
        return;
    }
    printf("for:\n");

    printPrefix(indent, step);
    printf("initialization:\n");
    this->init->print(indent + 1, step);

    printPrefix(indent, step);
    printf("condition:\n");
    this->cond->print(indent + 1, step);

    printPrefix(indent, step);
    printf("step:\n");
    this->step->print(indent + 1, step);

    printPrefix(indent, step);
    printf("body:\n");
    this->body->print(indent + 1, step);
}

IfStmtNode::~IfStmtNode() {
    delete this->cond;
    delete this->body;
    delete this->else_body;

    this->cond      = nullptr;
    this->body      = nullptr;
    this->else_body = nullptr;
}

IfStmtNode::IfStmtNode(ExprNode *cond, StmtNode *body, StmtNode *else_body, TextArea text_area) {
    this->text_area = text_area;
    this->cond      = cond;
    this->body      = body;
    this->else_body = else_body;
}

void IfStmtNode::print(int indent, int step) {
    printPrefix(indent, step, false);
    if (this == nullptr) {
        printf("nullptr\n");
        return;
    }
    printf("if:\n");

    printPrefix(indent, step);
    printf("condition:\n");
    this->cond->print(indent + 1, step);

    printPrefix(indent, step);
    printf("body:\n");
    this->body->print(indent + 1, step);

    printPrefix(indent, step);
    printf("else-body:\n");
    this->else_body->print(indent + 1, step);
}

ReturnStmtNode::~ReturnStmtNode() {
    delete this->value;

    this->value = nullptr;
}

ReturnStmtNode::ReturnStmtNode(ExprNode *value, TextArea text_area) {
    this->text_area = text_area;
    this->value     = value;
}

void ReturnStmtNode::print(int indent, int step) {
    printPrefix(indent, step, false);
    if (this == nullptr) {
        printf("nullptr\n");
        return;
    }
    printf("return:\n");

    this->value->print(indent + 1, step);
}

BlockStmtNode::~BlockStmtNode() {
    for (auto elem : this->list) {
        delete elem;
    }

    this->list.clear();
}

BlockStmtNode::BlockStmtNode(bool is_unscoped, const std::vector<StmtNode *> list, TextArea text_area) {
    this->text_area   = text_area;
    this->is_unscoped = is_unscoped;
    this->list        = list;
}

void BlockStmtNode::print(int indent, int step) {
    printPrefix(indent, step, false);
    if (this == nullptr) {
        printf("nullptr\n");
        return;
    }
    printf("%sblock:\n", this->is_unscoped ? "unscoped " : "");

    int i = 0;
    for (auto elem : this->list) {
        printPrefix(indent, step);
        printf("%d:\n", i++);
        elem->print(indent + 1, step);
    }
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
    passert(token != nullptr, "Failed to get operator from a nullptr token");
    switch (token->id) {
    case Token::PLUS_PLUS_OP :
        return (is_pre_op) ? OperatorInfo {OperatorNode::PRE_PLUS_PLUS, 2, RIGHT_TO_LEFT}
                           : OperatorInfo {OperatorNode::POST_PLUS_PLUS, 1, LEFT_TO_RIGHT};
    case Token::MINUS_MINUS_OP :
        return (is_pre_op) ? OperatorInfo {OperatorNode::PRE_MINUS_MINUS, 2, RIGHT_TO_LEFT}
                           : OperatorInfo {OperatorNode::POST_MINUS_MINUS, 1, LEFT_TO_RIGHT};
    case Token::DOT_OP : return OperatorInfo {OperatorNode::DOT, 1, LEFT_TO_RIGHT};
    case Token::AT_OP  : return OperatorInfo {OperatorNode::AT, 2, RIGHT_TO_LEFT};
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
    case Token::COMMA               : return OperatorInfo {OperatorNode::COMMA, 14, RIGHT_TO_LEFT};
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

void Parser::goThrough(OperatorInfo op_info) {
    this->state.cur_associativity = op_info.associativity;
    this->state.cur_priority      = op_info.priority;
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
    if (this->next_token != this->tokens.end()) {
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

bool Parser::canPassThrough(OperatorInfo op_info) {
    if (this->state.cur_priority > op_info.priority) {
        // our operator is of higher priority, therefore we can go through
        return true;
    }
    if (this->state.cur_priority == op_info.priority) {
        // our operator is of the same priority, so associativity determines the result
        //
        // ex: associativity = LEFT_TO_RIGHT:  1 + 2 + 3 is (1 + 2) + 3, so no passing through
        //                                           ^              ^
        // but associativity = RIGHT_TO_LEFT: a = b = c is a = (b = c), so we pass through
        //                                          ^             ^
        return op_info.associativity == RIGHT_TO_LEFT;
    }
    // otherwise, our operator is of lower priority, so we may not go through it
    return false;
}

bool Parser::hasNext() {
    return this->next_token != prev(this->tokens.end());
}

Token::TokenId Parser::checkNext() {
    return this->next_token->id;
}

bool Parser::consume(Token::TokenId id) {
    if (this->next_token->id != id) {
        return false;
    }
    this->next_token++;
    this->highlight(&*this->next_token);
    return true;
}

Token::TokenId Parser::consume() {
    auto res = (this->next_token++)->id;
    if (this->next_token != this->tokens.end()) {
        this->highlight(&*this->next_token);
    }
    return res;
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

bool Parser::ParsingResult::verify(Parser *p, ResultId id, const std::string error_message, bool highlight_next) {
    if (this->success && this->id == id) {
        if (highlight_next) {
            p->highlightNext();
        }
        return true;
    }
    p->errors_stack.push_back({this->state, this->error_message});
    p->signalError(error_message);
    return false;
}

static bool skipSemicolons(Parser *parser, bool single = false) {
    bool res = false;
    while (parser->hasNext() && parser->consume(Token::SEMICOLON)) {
        res = true;
        if (single) {
            break;
        }
        continue;
    }
    return res;
}

#define PRE_OP         true
#define POST_OP        false
#define EMPTY_PRIORITY 100

Parser::ParsingResult Parser::parseExpr() {
    TextArea                ta;
    std::vector<ExprNode *> expr_stack;
    while (this->hasNext()) {
        if (this->consume(Token::FUNCTION_KW)) {
            Token *function_token = &*prev(this->next_token);
            Token *name           = nullptr;
            if (this->hasNext() && this->next_token->id == Token::IDENTIFIER) {
                name = &*this->next_token;
                this->consume();
            }

            if (!this->consume(Token::OPEN_BRACKET)) {
                return ParsingResult("Expected an open bracket", this);
            }

            std::vector<Token *> list;
            if (!this->consume(Token::CLOSE_BRACKET)) {
                while (this->hasNext()) {
                    Token *param = &*this->next_token;
                    if (!this->consume(Token::IDENTIFIER)) {
                        return ParsingResult("Expected an identifier", this);
                    }
                    list.push_back(param);

                    if (this->checkNext() == Token::CLOSE_BRACKET) {
                        break;
                    }

                    if (!this->consume(Token::COMMA)) {
                        return ParsingResult("Expected a comma", this);
                    }
                }

                if (!this->consume(Token::CLOSE_BRACKET)) {
                    return ParsingResult("Expected a close bracket", this);
                }
            }

            this->saveState();
            auto body = this->parseStmt();
            this->restoreState();

            if (!body.verify(this, ParsingResult::STMT, "Expected a function body")) {
                return ParsingResult("", this);
            }

            ta              = (list.empty()) ? TextArea() : TextArea(*list.front());
            ta              = (list.empty()) ? TextArea() : TextArea(ta, TextArea(*list.back()));
            auto param_list = new IdentListNode(list, ta);

            ta            = TextArea(TextArea(*function_token), body.stmt->text_area);
            auto func_def = new FuncDefNode(name, param_list, body.stmt, ta);
            auto expr     = new ExprNode(func_def, ta);
            expr_stack.push_back(expr);
        }
        else if (this->consume(Token::TYPE_KW)) {
            Token *type_token = &*prev(this->next_token);
            if (!this->hasNext() || this->checkNext() != Token::IDENTIFIER) {
                return ParsingResult("Expected an identifier", this);
            }
            Token *name = &*this->next_token;
            this->consume();

            if (!this->consume(Token::OPEN_CURLY_BRACKET)) {
                return ParsingResult("Expected an open curly bracket", this);
            }

            std::vector<Token *>       fields;
            std::vector<FuncDefNode *> methods;
            Token                     *last_bracket;
            if (!this->consume(Token::CLOSE_CURLY_BRACKET)) {
                while (this->hasNext()) {
                    if (this->consume(Token::METHOD_KW)) {
                        Token *method_token = &*prev(this->next_token);
                        if (!this->hasNext() || this->checkNext() != Token::IDENTIFIER) {
                            return ParsingResult("Expected an identifier", this);
                        }
                        Token *name = &*this->next_token;
                        this->consume();

                        if (!this->consume(Token::OPEN_BRACKET)) {
                            return ParsingResult("Expected an open bracket", this);
                        }

                        std::vector<Token *> list;
                        Token               *last_bracket;
                        if (!this->consume(Token::CLOSE_BRACKET)) {
                            while (this->hasNext()) {
                                Token *param = &*this->next_token;
                                if (!this->consume(Token::IDENTIFIER)) {
                                    return ParsingResult("Expected an identifier", this);
                                }
                                list.push_back(param);

                                if (this->checkNext() == Token::CLOSE_BRACKET) {
                                    break;
                                }

                                if (!this->consume(Token::COMMA)) {
                                    return ParsingResult("Expected a comma", this);
                                }
                            }

                            last_bracket = &*this->next_token;

                            if (!this->consume(Token::CLOSE_BRACKET)) {
                                return ParsingResult("Expected a close bracket", this);
                            }
                        }

                        this->saveState();
                        auto body = this->parseStmt();
                        this->restoreState();

                        if (!body.verify(this, ParsingResult::STMT, "Expected a method body")) {
                            return ParsingResult("", this);
                        }

                        ta              = (list.empty()) ? TextArea() : TextArea(*list.front());
                        ta              = (list.empty()) ? TextArea() : TextArea(ta, TextArea(*list.back()));
                        auto param_list = new IdentListNode(list, ta);

                        ta              = TextArea(TextArea(*method_token), TextArea(*last_bracket));
                        auto method_def = new FuncDefNode(name, param_list, body.stmt, ta);
                        methods.push_back(method_def);
                    }
                    else if (this->checkNext() == Token::IDENTIFIER) {
                        Token *field = &*this->next_token;
                        this->consume();
                        fields.push_back(field);

                        if (!this->consume(Token::SEMICOLON)) {
                            return ParsingResult("Expected a semicolon", this);
                        }
                    }
                    else if (this->checkNext() == Token::CLOSE_CURLY_BRACKET) {
                        break;
                    }
                    else {
                        return ParsingResult("Expected either an indentifier or a method definition", this);
                    }
                }

                last_bracket = &*this->next_token;

                if (!this->consume(Token::CLOSE_CURLY_BRACKET)) {
                    return ParsingResult("Expected a close curly bracket", this);
                }
            }
            else {
                last_bracket = &*prev(this->next_token);
            }

            ta            = TextArea(TextArea(*type_token), TextArea(*last_bracket));
            auto type_def = new TypeDefNode(name, fields, methods, ta);
            auto expr     = new ExprNode(type_def, ta);
            expr_stack.push_back(expr);
        }
        else {
            // either an operator or an atom

            switch (this->checkNext()) {
            case Token::CLOSE_BRACKET :
            case Token::CLOSE_CURLY_BRACKET :
            case Token::CLOSE_SQUARE_BRACKET :
            case Token::SEMICOLON :
            case Token::FUNCTION_KW :
            case Token::OPEN_CURLY_BRACKET :
            case Token::TYPE_KW :
            case Token::METHOD_KW :
            case Token::WHILE_KW :
            case Token::FOR_KW :
            case Token::IF_KW :
            case Token::ELSE_KW :
            case Token::CONTINUE_KW :
            case Token::BREAK_KW :
            case Token::RETURN_KW :
            case Token::UNSCOPED_KW          : goto END_LOOP;
            }

            switch (this->checkNext()) {
            case Token::PLUS_PLUS_OP :
            case Token::MINUS_MINUS_OP : {
                OperatorInfo op_info;
                if (expr_stack.empty()) {
                    op_info = this->getOperatorInfo(&*this->next_token, PRE_OP);     // ++A
                }
                else {
                    op_info = this->getOperatorInfo(&*this->next_token, POST_OP);    // A++
                }

                if (!this->canPassThrough(op_info)) {
                    goto END_LOOP;
                }

                auto operator_token = &*this->next_token;
                this->consume();

                if (expr_stack.empty()) {
                    // ++A
                    this->saveState();
                    this->goThrough(op_info);
                    auto arg = this->parseExpr();
                    this->restoreState();

                    if (!arg.verify(this, ParsingResult::EXPR, "Expected an expression")) {
                        return ParsingResult("", this);
                    }

                    auto op   = new OperatorNode(op_info.id,
                                               arg.expr,
                                               nullptr,
                                               operator_token,
                                               TextArea(TextArea(*operator_token), arg.expr->text_area));
                    auto expr = new ExprNode(op, op->text_area);
                    expr_stack.push_back(expr);
                }
                else {
                    // A++
                    auto op = new OperatorNode(op_info.id,
                                               expr_stack.back(),
                                               nullptr,
                                               operator_token,
                                               TextArea(TextArea(*operator_token), expr_stack.back()->text_area));
                    expr_stack.pop_back();
                    auto expr = new ExprNode(op, op->text_area);
                    expr_stack.push_back(expr);
                }
                break;
            }
            case Token::OPEN_BRACKET :
            case Token::OPEN_SQUARE_BRACKET : {
                OperatorInfo op_info;
                int          special = this->checkNext() == Token::OPEN_BRACKET ? 1 : 2;
                if (expr_stack.empty()) {
                    // expression in parentheses
                    if (this->consume(Token::OPEN_BRACKET)) {
                        auto operator_token = &*this->next_token;
                        this->saveState();
                        this->state.cur_priority      = EMPTY_PRIORITY;
                        this->state.cur_associativity = LEFT_TO_RIGHT;
                        auto in_expr                  = this->parseExpr();
                        this->restoreState();

                        if (!in_expr.verify(this, ParsingResult::EXPR, "Expected an expression")) {
                            return ParsingResult("", this);
                        }

                        if (!this->consume(Token::CLOSE_BRACKET)) {
                            return ParsingResult("Expected a close bracket", this);
                        }

                        auto par_expr = new ParExprNode(
                        in_expr.expr,
                        TextArea(TextArea(*operator_token),
                                 TextArea(in_expr.expr->text_area, TextArea(*prev(this->next_token)))));
                        auto expr = new ExprNode(par_expr, par_expr->text_area);
                        expr_stack.push_back(expr);
                        break;
                    }
                    else {
                        return ParsingResult("Expected an expression before the operator", this);
                    }
                }
                else {
                    op_info = this->getOperatorInfo(&*this->next_token, POST_OP, special);    // A(A)
                }

                if (!this->canPassThrough(op_info)) {
                    goto END_LOOP;
                }

                auto operator_token = &*this->next_token;
                this->consume();

                if (operator_token->id == Token::OPEN_BRACKET && this->consume(Token::CLOSE_BRACKET)) {
                    auto op = new OperatorNode(
                    op_info.id,
                    expr_stack.back(),
                    nullptr,
                    operator_token,
                    TextArea(TextArea(*operator_token),
                             TextArea(expr_stack.back()->text_area, TextArea(*prev(this->next_token)))));
                    expr_stack.pop_back();
                    auto expr = new ExprNode(op, op->text_area);
                    expr_stack.push_back(expr);
                    break;
                }

                if (operator_token->id == Token::OPEN_SQUARE_BRACKET && this->consume(Token::CLOSE_SQUARE_BRACKET))
                {
                    auto op = new OperatorNode(
                    op_info.id,
                    expr_stack.back(),
                    nullptr,
                    operator_token,
                    TextArea(TextArea(*operator_token),
                             TextArea(expr_stack.back()->text_area, TextArea(*prev(this->next_token)))));
                    expr_stack.pop_back();
                    auto expr = new ExprNode(op, op->text_area);
                    expr_stack.push_back(expr);
                    break;
                }

                this->saveState();
                this->state.cur_priority      = EMPTY_PRIORITY;
                this->state.cur_associativity = LEFT_TO_RIGHT;
                auto arg                      = this->parseExpr();
                this->restoreState();

                if (operator_token->id == Token::OPEN_BRACKET && !this->consume(Token::CLOSE_BRACKET)) {
                    return ParsingResult("Expected a close bracket", this);
                }

                if (operator_token->id == Token::OPEN_SQUARE_BRACKET
                    && !this->consume(Token::CLOSE_SQUARE_BRACKET))
                {
                    return ParsingResult("Expected a close square bracket", this);
                }

                if (!arg.verify(this, ParsingResult::EXPR, "Expected an expression")) {
                    return ParsingResult("", this);
                }

                auto op = new OperatorNode(
                op_info.id,
                expr_stack.back(),
                arg.expr,
                operator_token,
                TextArea(TextArea(TextArea(TextArea(*operator_token), TextArea(*prev(this->next_token))),
                                  expr_stack.back()->text_area),
                         arg.expr->text_area));
                expr_stack.pop_back();
                auto expr = new ExprNode(op, op->text_area);
                expr_stack.push_back(expr);
                break;
            }
            case Token::PLUS_OP :
            case Token::MINUS_OP : {
                OperatorInfo op_info;
                if (expr_stack.empty()) {
                    op_info = this->getOperatorInfo(&*this->next_token, PRE_OP);     // +A
                }
                else {
                    op_info = this->getOperatorInfo(&*this->next_token, POST_OP);    // A+A
                }

                if (!this->canPassThrough(op_info)) {
                    goto END_LOOP;
                }

                auto operator_token = &*this->next_token;
                this->consume();

                if (expr_stack.empty()) {
                    // +A
                    this->saveState();
                    this->goThrough(op_info);
                    auto arg = this->parseExpr();
                    this->restoreState();

                    if (!arg.verify(this, ParsingResult::EXPR, "Expected an expression")) {
                        return ParsingResult("", this);
                    }

                    auto op   = new OperatorNode(op_info.id,
                                               arg.expr,
                                               nullptr,
                                               operator_token,
                                               TextArea(TextArea(*operator_token), arg.expr->text_area));
                    auto expr = new ExprNode(op, op->text_area);
                    expr_stack.push_back(expr);
                }
                else {
                    // A+A
                    this->saveState();
                    this->goThrough(op_info);
                    auto arg = this->parseExpr();
                    this->restoreState();

                    if (!arg.verify(this, ParsingResult::EXPR, "Expected an expression")) {
                        return ParsingResult("", this);
                    }

                    auto op
                    = new OperatorNode(op_info.id,
                                       expr_stack.back(),
                                       arg.expr,
                                       operator_token,
                                       TextArea(TextArea(TextArea(*operator_token), expr_stack.back()->text_area),
                                                arg.expr->text_area));
                    expr_stack.pop_back();
                    auto expr = new ExprNode(op, op->text_area);
                    expr_stack.push_back(expr);
                }
                break;
            }
            case Token::AT_OP :
            case Token::NOT_OP :
            case Token::INVERSE_OP : {
                OperatorInfo op_info;
                op_info = this->getOperatorInfo(&*this->next_token, PRE_OP);    // !A

                if (!this->canPassThrough(op_info)) {
                    goto END_LOOP;
                }

                auto operator_token = &*this->next_token;
                this->consume();

                // !A
                this->saveState();
                this->goThrough(op_info);
                auto arg = this->parseExpr();
                this->restoreState();

                if (!arg.verify(this, ParsingResult::EXPR, "Expected an expression")) {
                    return ParsingResult("", this);
                }

                auto op   = new OperatorNode(op_info.id,
                                           arg.expr,
                                           nullptr,
                                           operator_token,
                                           TextArea(TextArea(*operator_token), arg.expr->text_area));
                auto expr = new ExprNode(op, op->text_area);
                expr_stack.push_back(expr);
                break;
            }
            case Token::DOT_OP :
            case Token::MULT_OP :
            case Token::DIV_OP :
            case Token::REMAINDER_OP :
            case Token::RIGHT_SHIFT_OP :
            case Token::LEFT_SHIFT_OP :
            case Token::LESS_OP :
            case Token::LESS_EQUAL_OP :
            case Token::GREATER_OP :
            case Token::GREATER_EQUAL_OP :
            case Token::EQUAL_OP :
            case Token::NOT_EQUAL_OP :
            case Token::BITAND_OP :
            case Token::BITOR_OP :
            case Token::BITXOR_OP :
            case Token::AND_OP :
            case Token::OR_OP :
            case Token::ASSIGN_OP :
            case Token::ASSIGN_PLUS_OP :
            case Token::ASSIGN_MINUS_OP :
            case Token::ASSIGN_MULT_OP :
            case Token::ASSIGN_DIV_OP :
            case Token::ASSIGN_REMAINDER_OP :
            case Token::COMMA               : {
                OperatorInfo op_info;
                if (expr_stack.empty()) {
                    return ParsingResult("Expected an expression before the operator", this);
                }
                else {
                    op_info = this->getOperatorInfo(&*this->next_token, POST_OP);    // A*A
                }

                if (!this->canPassThrough(op_info)) {
                    goto END_LOOP;
                }

                auto operator_token = &*this->next_token;
                this->consume();

                // A+A
                this->saveState();
                this->goThrough(op_info);
                auto arg = this->parseExpr();
                this->restoreState();

                if (!arg.verify(this, ParsingResult::EXPR, "Expected an expression")) {
                    return ParsingResult("", this);
                }

                auto op = new OperatorNode(
                op_info.id,
                expr_stack.back(),
                arg.expr,
                operator_token,
                TextArea(TextArea(TextArea(*operator_token), expr_stack.back()->text_area), arg.expr->text_area));
                expr_stack.pop_back();
                auto expr = new ExprNode(op, op->text_area);
                expr_stack.push_back(expr);
                break;
            }
            case Token::BOOLEAN_LIT :
            case Token::CHAR_LIT :
            case Token::INT_LIT :
            case Token::REAL_LIT :
            case Token::STRING_LIT :
            case Token::IDENTIFIER :
            case Token::NOTHING_LIT : {
                auto atom_token = &*this->next_token;
                if (!expr_stack.empty()) {
                    return ParsingResult(
                    "Invalid expression. Expected either an operator, a keyword, or a separator",
                    this);
                }
                this->consume();

                auto atom = new AtomNode(atom_token, TextArea(*atom_token));
                auto expr = new ExprNode(atom, atom->text_area);
                expr_stack.push_back(expr);
                break;
            }
            default : return ParsingResult("Invalid expression", this);
            }
        }
    }
END_LOOP:;
    if (expr_stack.size() != 1) {
        this->highlightNext();
        return ParsingResult("Expected an expression", this);
    }

    return ParsingResult(expr_stack[0], this);
}

Parser::ParsingResult Parser::parseStmt() {
    // skip all semicolons
    auto had_semicolons = skipSemicolons(this);

    if (!this->hasNext()) {
        this->highlightNext();
        this->signalError("Expected a statement");
        return ParsingResult("", this);
    }
    if (had_semicolons && this->consume(Token::CLOSE_BRACKET)) {
        this->rollback();
        return ParsingResult((StmtNode *)nullptr, this);
    }
    if (had_semicolons && this->consume(Token::CLOSE_CURLY_BRACKET)) {
        this->rollback();
        return ParsingResult((StmtNode *)nullptr, this);
    }
    if (had_semicolons && this->consume(Token::CLOSE_SQUARE_BRACKET)) {
        this->rollback();
        return ParsingResult((StmtNode *)nullptr, this);
    }

    if (this->consume(Token::WHILE_KW)) {
        Token *while_token = &*prev(this->next_token);
        this->saveState();
        auto cond = this->parseExpr();
        this->restoreState();

        if (!cond.verify(this, ParsingResult::EXPR, "Expected while loop condition")) {
            return ParsingResult("", this);
        }

        this->saveState();
        auto body = this->parseStmt();
        this->restoreState();

        if (!body.verify(this, ParsingResult::STMT, "Expected while loop body")) {
            return ParsingResult("", this);
        }

        auto while_stmt
        = new WhileStmtNode(cond.expr, body.stmt, TextArea(TextArea(*while_token), body.stmt->text_area));
        auto stmt = new StmtNode(while_stmt, while_stmt->text_area);
        return ParsingResult(stmt, this);
    }
    else if (this->consume(Token::FOR_KW)) {
        Token *for_token  = &*prev(this->next_token);
        ParsingResult init((ExprNode *)nullptr, this), cond((ExprNode *)nullptr, this),
        step((ExprNode *)nullptr, this);
        if (!this->consume(Token::SEMICOLON)) {
            this->saveState();
            init = this->parseExpr();
            this->restoreState();

            if (!init.verify(this, ParsingResult::EXPR, "Expected for loop initialization")) {
                return ParsingResult("", this);
            }
            if (!this->consume(Token::SEMICOLON)) {
                this->signalError("For loop's initialization must end with a semicolon.");
                return ParsingResult("", this);
            }
        }
        this->highlightNext();

        if (!this->consume(Token::SEMICOLON)) {
            this->saveState();
            cond = this->parseExpr();
            this->restoreState();

            if (!cond.verify(this, ParsingResult::EXPR, "Expected for loop condition")) {
                return ParsingResult("", this);
            }
            if (!this->consume(Token::SEMICOLON)) {
                this->signalError("For loop's condition must end with a semicolon.");
                return ParsingResult("", this);
            }
        }
        this->highlightNext();

        if (!this->consume(Token::SEMICOLON)) {
            this->saveState();
            step = this->parseExpr();
            this->restoreState();

            if (!step.verify(this, ParsingResult::EXPR, "Expected for loop step")) {
                return ParsingResult("", this);
            }
            if (!this->consume(Token::SEMICOLON)) {
                this->signalError("For loop's step must end with a semicolon.");
                return ParsingResult("", this);
            }
        }
        this->highlightNext();

        this->saveState();
        auto body = this->parseStmt();
        this->restoreState();

        if (!body.verify(this, ParsingResult::STMT, "Expected for loop body")) {
            return ParsingResult("", this);
        }

        auto for_stmt = new ForStmtNode(init.expr,
                                        cond.expr,
                                        step.expr,
                                        body.stmt,
                                        TextArea(TextArea(*for_token), body.stmt->text_area));
        auto stmt     = new StmtNode(for_stmt, for_stmt->text_area);
        return ParsingResult(stmt, this);
    }
    else if (this->consume(Token::IF_KW)) {
        Token *if_token = &*prev(this->next_token);
        this->saveState();
        auto cond = this->parseExpr();
        this->restoreState();

        if (!cond.verify(this, ParsingResult::EXPR, "Expected if statement condition")) {
            return ParsingResult("", this);
        }

        this->saveState();
        auto body = this->parseStmt();
        this->restoreState();

        if (!body.verify(this, ParsingResult::STMT, "Expected if statement body")) {
            return ParsingResult("", this);
        }

        if (!this->consume(Token::ELSE_KW)) {
            auto if_stmt
            = new IfStmtNode(cond.expr, body.stmt, nullptr, TextArea(TextArea(*if_token), body.stmt->text_area));
            auto stmt = new StmtNode(if_stmt, if_stmt->text_area);
            return ParsingResult(stmt, this);
        }

        this->saveState();
        auto else_body = this->parseStmt();
        this->restoreState();

        if (!else_body.verify(this, ParsingResult::STMT, "Expected if statement else body")) {
            return ParsingResult("", this);
        }

        auto if_stmt = new IfStmtNode(cond.expr,
                                      body.stmt,
                                      else_body.stmt,
                                      TextArea(TextArea(*if_token), else_body.stmt->text_area));
        auto stmt    = new StmtNode(if_stmt, if_stmt->text_area);
        return ParsingResult(stmt, this);
    }
    else if (this->consume(Token::CONTINUE_KW)) {
        auto stmt = new StmtNode(StmtNode::CONTINUE, TextArea(*prev(this->next_token)));
        return ParsingResult(stmt, this);
    }
    else if (this->consume(Token::BREAK_KW)) {
        auto stmt = new StmtNode(StmtNode::BREAK, TextArea(*prev(this->next_token)));
        return ParsingResult(stmt, this);
    }
    else if (this->consume(Token::RETURN_KW)) {
        Token *return_token = &*prev(this->next_token);
        if (this->consume(Token::SEMICOLON)) {
            auto return_stmt = new ReturnStmtNode(nullptr, TextArea(*return_token));
            auto stmt        = new StmtNode(return_stmt, return_stmt->text_area);
            return ParsingResult(stmt, this);
        }

        this->saveState();
        auto value = this->parseExpr();
        this->restoreState();

        if (!value.verify(this, ParsingResult::EXPR, "Expected return statement value")) {
            return ParsingResult("", this);
        }

        auto return_stmt
        = new ReturnStmtNode(value.expr, TextArea(TextArea(*return_token), value.expr->text_area));
        auto stmt = new StmtNode(return_stmt, return_stmt->text_area);
        return ParsingResult(stmt, this);
    }
    else if (this->consume(Token::UNSCOPED_KW)) {
        Token *block_token = &*prev(this->next_token);
        if (!this->consume(Token::OPEN_CURLY_BRACKET)) {
            return ParsingResult("Unscoped keyword must be followed by an open curly bracket", this);
        }
        std::vector<StmtNode *> list;
        while (this->hasNext()) {
            if (this->consume(Token::CLOSE_CURLY_BRACKET)) {
                auto block_stmt
                = new BlockStmtNode(true,
                                    list,
                                    TextArea(TextArea(*block_token), TextArea(*prev(this->next_token))));
                auto stmt = new StmtNode(block_stmt, block_stmt->text_area);
                return ParsingResult(stmt, this);
            }

            this->saveState();
            auto stmt = this->parseStmt();
            this->restoreState();

            if (!stmt.verify(this, ParsingResult::STMT, "Expected a statement")) {
                return ParsingResult("", this);
            }

            list.push_back(stmt.stmt);
        }

        return ParsingResult("Block statement must end with a close curly bracket", this);
    }
    else if (this->consume(Token::OPEN_CURLY_BRACKET)) {
        Token                  *block_token = &*prev(this->next_token);
        std::vector<StmtNode *> list;
        while (this->hasNext()) {
            if (this->consume(Token::CLOSE_CURLY_BRACKET)) {
                auto block_stmt
                = new BlockStmtNode(false,
                                    list,
                                    TextArea(TextArea(*block_token), TextArea(*prev(this->next_token))));
                auto stmt = new StmtNode(block_stmt, block_stmt->text_area);
                return ParsingResult(stmt, this);
            }

            this->saveState();
            auto stmt = this->parseStmt();
            this->restoreState();

            if (!stmt.verify(this, ParsingResult::STMT, "Expected a statement")) {
                return ParsingResult("", this);
            }

            list.push_back(stmt.stmt);
        }
        return ParsingResult("Block statement must end with a close curly bracket", this);
    }

    // the only left case is an expression
    this->saveState();
    auto expr = this->parseExpr();
    this->restoreState();

    if (!expr.verify(this, ParsingResult::EXPR, "Unknown statement")) {
        return ParsingResult("", this);
    }
    if (!this->consume(Token::SEMICOLON)) {
        return ParsingResult("Expression statement must end with a semicolon", this);
    }

    auto stmt = new StmtNode(expr.expr, expr.expr->text_area);
    return ParsingResult(stmt, this);
}

StmtNode *Parser::parse(const std::vector<Token> &tokens) {
    this->tokens                  = tokens;
    this->next_token              = this->tokens.begin();
    this->state.cur_associativity = LEFT_TO_RIGHT;
    this->state.cur_priority      = EMPTY_PRIORITY;
    this->state.report_errors     = true;
    this->state.token             = nullptr;
    this->error_manager->setErrorCharPos(-1);
    this->error_manager->setErrorMessage("");

    if (!tokens.empty()) {
        this->highlight((Token *)&(tokens[0]));
    }

    if (tokens.empty()) {
        return new StmtNode(new BlockStmtNode(false, {}, TextArea()), TextArea());
    }

    std::vector<StmtNode *> statements;
    while (this->hasNext()) {
        skipSemicolons(this);
        if (!this->hasNext()) {
            break;
        }

        this->saveState();
        this->state.report_errors = false;
        ParsingResult res         = this->parseStmt();
        this->restoreState();

        res.verify(this, ParsingResult::STMT, "Failed to parse a statement");

        statements.push_back(res.stmt);
    }
    auto ta    = (statements.empty()) ? TextArea() : statements.front()->text_area;
    ta         = (statements.empty()) ? TextArea() : TextArea(ta, statements.back()->text_area);
    auto block = new BlockStmtNode(false, statements, ta);
    auto res   = new StmtNode(block, ta);
    return res;
}

Parser::ParsingResult::ParsingResult(const std::string &error_message, Parser *p) {
    this->success       = false;
    this->error_message = error_message;
    this->state         = p->state;
}

Parser::ParsingResult::ParsingResult(ResultId id, Parser *p) {
    this->success = true;
    this->id      = id;
    this->state   = p->state;
}

Parser::ParsingResult::ParsingResult(ExprNode *expr, Parser *p) {
    this->success = true;
    this->id      = EXPR;
    this->expr    = expr;
    this->state   = p->state;
}

Parser::ParsingResult::ParsingResult(FuncDefNode *func_def, Parser *p) {
    this->success = true;
    this->id      = FUNC_DEC;
    this->expr    = expr;
    this->state   = p->state;
}

Parser::ParsingResult::ParsingResult(TypeDefNode *type_def, Parser *p) {
    this->success = true;
    this->id      = TYPE_DEF;
    this->expr    = expr;
    this->state   = p->state;
}

Parser::ParsingResult::ParsingResult(OperatorNode *op, Parser *p) {
    this->success = true;
    this->id      = OPERATOR;
    this->expr    = expr;
    this->state   = p->state;
}

Parser::ParsingResult::ParsingResult(AtomNode *atom, Parser *p) {
    this->success = true;
    this->id      = ATOM;
    this->expr    = expr;
    this->state   = p->state;
}

Parser::ParsingResult::ParsingResult(ParExprNode *par_expr, Parser *p) {
    this->success = true;
    this->id      = PAR_EXPR;
    this->expr    = expr;
    this->state   = p->state;
}

Parser::ParsingResult::ParsingResult(IdentListNode *ident_list, Parser *p) {
    this->success = true;
    this->id      = IDENT_LIST;
    this->expr    = expr;
    this->state   = p->state;
}

Parser::ParsingResult::ParsingResult(StmtNode *stmt, Parser *p) {
    this->success = true;
    this->id      = STMT;
    this->stmt    = stmt;
    this->state   = p->state;
}

Parser::ParsingResult::ParsingResult(WhileStmtNode *while_stmt, Parser *p) {
    this->success    = true;
    this->id         = WHILE_STMT;
    this->while_stmt = while_stmt;
    this->state      = p->state;
}

Parser::ParsingResult::ParsingResult(ForStmtNode *for_stmt, Parser *p) {
    this->success  = true;
    this->id       = FOR_STMT;
    this->for_stmt = for_stmt;
    this->state    = p->state;
}


Parser::ParsingResult::ParsingResult(IfStmtNode *if_stmt, Parser *p) {
    this->success = true;
    this->id      = IF_STMT;
    this->if_stmt = if_stmt;
    this->state   = p->state;
}

Parser::ParsingResult::ParsingResult(ReturnStmtNode *return_stmt, Parser *p) {
    this->success     = true;
    this->id          = RETURN_STMT;
    this->return_stmt = return_stmt;
    this->state       = p->state;
}

Parser::ParsingResult::ParsingResult(BlockStmtNode *block_stmt, Parser *p) {
    this->success    = true;
    this->id         = BLOCK_STMT;
    this->block_stmt = block_stmt;
    this->state      = p->state;
}

}    // namespace Cotton
