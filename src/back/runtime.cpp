#include "runtime.h"
#include "../builtin/api.h"
#include "../errors.h"
#include "../front/parser.h"
#include "gc.h"
#include "instance.h"
#include "nameid.h"
#include "scope.h"
#include "stack.h"
#include "type.h"

namespace Cotton {
Runtime::Runtime(size_t stack_size, GCStrategy *gc_strategy, ErrorManager *error_manager) {
    this->scope         = new Scope(NULL, true);
    this->stack         = new Stack(stack_size);
    this->gc            = new GC(gc_strategy);
    this->error_manager = error_manager;
    this->current_token = NULL;

    this->nothing_type   = new Builtin::NothingType(this);
    this->function_type  = new Builtin::FunctionType(this);
    this->boolean_type   = new Builtin::BooleanType(this);
    this->integer_type   = new Builtin::IntegerType(this);
    this->real_type      = new Builtin::RealType(this);
    this->character_type = new Builtin::CharacterType(this);
    this->string_type    = new Builtin::StringType(this);

    Builtin::installBuiltinFunctions(this);
}

void Runtime::newFrame(bool can_access_prev_scope) {
    auto scope  = new Scope(this->scope, can_access_prev_scope);
    this->scope = scope;
    this->stack->newFrame();
}

void Runtime::popFrame() {
    auto scope  = this->scope->prev;
    this->scope = scope;
    delete scope;
    this->stack->popFrame();
}

Object *Runtime::make(Type *type, ObjectOptions object_opt) {
    if (type == NULL) {
        this->signalError("Failed to make an object of NULL type");
    }
    Object *obj = NULL;
    if (object_opt == Runtime::INSTANCE_OBJECT) {
        obj = type->create(this);
    }
    else {
        obj = new Object(false, false, NULL, type, this);
    }
    if (obj == NULL) {
        this->signalError("Failed to make an object of type " + type->shortRepr());
    }
    if (this->isInstanceObject(obj) && obj->type->hasMethod(MagicMethods::__make__(), this)) {
        this->runMethod(MagicMethods::__make__(), obj, {});
    }
    return obj;
}

Object *Runtime::copy(Object *obj) {
    if (!this->isTypeObject(obj)) {
        this->signalError("Failed to copy non type object " + obj->shortRepr());
    }
    return obj->type->copy(obj, this);
}

Object *Runtime::runOperator(OperatorNode::OperatorId id, Object *obj, const std::vector<Object *> &args) {
    if (!this->isInstanceObject(obj)) {
        this->signalError("Failed to run operator " + std::to_string(id) + " on " + obj->shortRepr());
    }

    auto op = obj->type->getOperator(id, this);
    return op->operator()(obj, args, this);
}

Object *Runtime::runMethod(int64_t id, Object *obj, const std::vector<Object *> &args) {
    if (!this->isInstanceObject(obj)) {
        this->signalError("Failed to run method " + NameId::shortRepr(id) + " on " + obj->shortRepr());
    }

    auto method = obj->type->getMethod(id, this);
    return runMethod(OperatorNode::CALL, method, args);
}

bool Runtime::isInstanceObject(Object *obj) {
    return obj != NULL && obj->instance != NULL && obj->type != NULL;
}

bool Runtime::isInstanceObject(Object *obj, Type *type) {
    return obj != NULL && obj->instance != NULL && obj->type == type;
}

bool Runtime::isTypeObject(Object *obj) {
    return obj != NULL && obj->type != NULL;
}

void Runtime::highlight(Token *token) {
    this->current_token = token;
}

void Runtime::signalError(const std::string &message, bool include_token) {
    if (include_token && this->current_token != NULL) {
        this->error_manager->signalError(message, *this->current_token);
    }
    else {
        this->error_manager->signalError(message);
    }
}

uint8_t Runtime::ExecutionResult::CONTINUE    = 1;
uint8_t Runtime::ExecutionResult::BREAK       = 1;
uint8_t Runtime::ExecutionResult::RETURN      = 1;
uint8_t Runtime::ExecutionResult::DIRECT_PASS = 1;

Runtime::ExecutionResult::ExecutionResult(uint8_t flags, Object *result, Object *caller) {
    this->flags  = flags;
    this->result = result;
    this->caller = caller;
}

Runtime::ExecutionResult Runtime::execute(ExprNode *node) {
    if (node == NULL) {
        this->signalError("Failed to execute NULL AST node");
    }

    switch (node->id) {
    case ExprNode::FUNCTION_DEFINITION : {
        return this->execute(node->func_def);
    }
    case ExprNode::TYPE_DEFINITION : {
        return this->execute(node->type_def);
    }
    case ExprNode::OPERATOR : {
        return this->execute(node->op);
    }
    case ExprNode::ATOM : {
        return this->execute(node->atom);
    }
    case ExprNode::PARENTHESES_EXPRESSION : {
        return this->execute(node->par_expr);
    }
    default : this->signalError("Failed to execute unknown AST node");
    }
}

Runtime::ExecutionResult Runtime::execute(FuncDefNode *node) {
    if (node == NULL) {
        this->signalError("Failed to execute NULL AST node");
    }
    this->highlight(node->function_token);
    auto func = Builtin::makeFunctionInstanceObject(false, NULL, node, this);
    if (node->name != NULL) {
        this->scope->addVariable(node->name->nameid, func, this);
    }
    return ExecutionResult(0, func);
}

Runtime::ExecutionResult Runtime::execute(TypeDefNode *node) {
    if (node == NULL) {
        this->signalError("Failed to execute NULL AST node");
    }
    this->highlight(node->type_token);
    auto type             = new Builtin::UserDefinedType(this);
    type->nameid          = node->name->nameid;
    type->instance_fields = node->fields;

    for (auto method : node->methods) {
        this->highlight(method->function_token);
        auto f = Builtin::makeFunctionInstanceObject(false, NULL, method, this);
        type->addMethod(method->name->nameid, f, this);
    }

    this->highlight(node->type_token);
    auto res = this->make(type, Runtime::TYPE_OBJECT);

    return ExecutionResult(0, res);
}

static std::vector<Runtime::ExecutionResult> getList(ExprNode *expr, Runtime *rt) {
    std::vector<Runtime::ExecutionResult> res;
    while (expr != NULL) {
        if (expr->id == ExprNode::OPERATOR && expr->op->id == OperatorNode::COMMA) {
            res.push_back(rt->execute(expr->op->second));
            expr = expr->op->first;
        }
        else {
            res.push_back(rt->execute(expr));
            break;
        }
    }
    std::reverse(res.begin(), res.end());
    return res;
}

Runtime::ExecutionResult Runtime::execute(OperatorNode *node) {
    if (node == NULL) {
        this->signalError("Failed to execute NULL AST node");
    }

    auto            caller = this->execute(node->first);
    Object         *self   = caller.result;
    ExecutionResult other(0, NULL);

    this->highlight(node->op);
    switch (node->id) {
    case OperatorNode::DOT : {
        if (node->second == NULL || node->second->id != ExprNode::ATOM
            || node->second->atom->id != AtomNode::IDENTIFIER)
        {
            this->signalError("Selector at the right-side is illegal");
        }
        int64_t selector = node->second->atom->ident->nameid;
        if (!this->isInstanceObject(self)) {
            this->signalError("Left-side object " + self->shortRepr() + " must be an instance object");
        }

        auto res = self->instance->selectField(selector, this);
        return ExecutionResult(0, res, self);
    }
    case OperatorNode::AT : {
        return ExecutionResult(ExecutionResult::DIRECT_PASS, self);
    }
    case OperatorNode::COMMA : {
        return ExecutionResult(0, self);
    }
    case OperatorNode::ASSIGN : {
        other = this->execute(node->second);
        this->highlight(node->op);
        if (other.flags & ExecutionResult::DIRECT_PASS) {
            self->instance = other.result->instance;
            self->type     = other.result->type;
        }
        else {
            self = this->copy(other.result);
        }
        return ExecutionResult(0, self);
    }
    case OperatorNode::PLUS_ASSIGN : {
        other = this->execute(node->second);
        this->highlight(node->op);
        self = this->copy(this->runOperator(OperatorNode::PLUS, self, {other.result}));
        return ExecutionResult(0, self);
    }
    case OperatorNode::MINUS_ASSIGN : {
        other = this->execute(node->second);
        this->highlight(node->op);
        self = this->copy(this->runOperator(OperatorNode::MINUS, self, {other.result}));
        return ExecutionResult(0, self);
    }
    case OperatorNode::MULT_ASSIGN : {
        other = this->execute(node->second);
        this->highlight(node->op);
        self = this->copy(this->runOperator(OperatorNode::MULT, self, {other.result}));
        return ExecutionResult(0, self);
    }
    case OperatorNode::DIV_ASSIGN : {
        other = this->execute(node->second);
        this->highlight(node->op);
        self = this->copy(this->runOperator(OperatorNode::DIV, self, {other.result}));
        return ExecutionResult(0, self);
    }
    case OperatorNode::REM_ASSIGN : {
        other = this->execute(node->second);
        this->highlight(node->op);
        self = this->copy(this->runOperator(OperatorNode::REM_ASSIGN, self, {other.result}));
        return ExecutionResult(0, self);
    }
    }

    std::vector<Object *> args;
    if (node->id == OperatorNode::CALL && caller.caller != NULL) {
        args.push_back(caller.caller);
    }

    for (auto r : getList(node->second, this)) {
        if (r.flags & ExecutionResult::DIRECT_PASS) {
            args.push_back(r.result);
        }
        else {
            args.push_back(this->copy(r.result));
        }
    }

    auto res = this->runOperator(node->id, self, args);
    return ExecutionResult(0, res);
}

Runtime::ExecutionResult Runtime::execute(AtomNode *node) {
    if (node == NULL) {
        this->signalError("Failed to execute NULL AST node");
    }
    this->highlight(node->token);
    switch (node->id) {
    case AtomNode::BOOLEAN : {
        return ExecutionResult(0, Builtin::makeBooleanInstanceObject(node->bool_value, this));
    }
    case AtomNode::CHARACTER : {
        return ExecutionResult(0, Builtin::makeCharacterInstanceObject(node->char_value, this));
    }
    case AtomNode::INTEGER : {
        return ExecutionResult(0, Builtin::makeIntegerInstanceObject(node->int_value, this));
    }
    case AtomNode::REAL : {
        return ExecutionResult(0, Builtin::makeRealInstanceObject(node->real_value, this));
    }
    case AtomNode::STRING : {
        return ExecutionResult(0, Builtin::makeStringInstanceObject(node->string_value, this));
    }
    case AtomNode::NOTHING : {
        return ExecutionResult(0, Builtin::makeNothingInstanceObject(this));
    }
    case AtomNode::IDENTIFIER : {
        if (!this->scope->queryVariable(node->token->nameid, this)) {
            this->scope->addVariable(node->token->nameid, Builtin::makeNothingInstanceObject(this), this);
        }
        return ExecutionResult(0, this->scope->getVariable(node->token->nameid, this));
    }
    default : this->signalError("Unknown atom");
    }
}

Runtime::ExecutionResult Runtime::execute(ParExprNode *node) {
    if (node == NULL) {
        this->signalError("Failed to execute NULL AST node");
    }
    return this->execute(node->expr);
}

Runtime::ExecutionResult Runtime::execute(StmtNode *node) {
    if (node == NULL) {
        this->signalError("Failed to execute NULL AST node");
    }

    switch (node->id) {
    case StmtNode::WHILE : {
        return this->execute(node->while_stmt);
    }
    case StmtNode::FOR : {
        return this->execute(node->for_stmt);
    }
    case StmtNode::IF : {
        return this->execute(node->if_stmt);
    }
    case StmtNode::CONTINUE : {
        return ExecutionResult(ExecutionResult::CONTINUE, NULL);    // NULL -> nothing?
    }
    case StmtNode::BREAK : {
        return ExecutionResult(ExecutionResult::BREAK, NULL);       // NULL -> nothing?
    }
    case StmtNode::RETURN : {
        return this->execute(node->return_stmt);
    }
    case StmtNode::BLOCK : {
        return this->execute(node->block_stmt);
    }
    case StmtNode::EXPR : {
        return this->execute(node->expr);
    }
    default : {
        this->signalError("Unknown node");
    }
    }
}

Runtime::ExecutionResult Runtime::execute(WhileStmtNode *node) {
    if (node == NULL) {
        this->signalError("Failed to execute NULL AST node");
    }
    while (true) {
        this->highlight(node->while_token);

        if (node->cond != NULL) {
            auto cond = this->execute(node->cond).result;
            if (!this->isInstanceObject(cond, this->boolean_type)) {
                this->signalError("Condition " + cond->shortRepr() + " is not Boolean");
            }

            if (!Builtin::getBooleanValue(cond, this)) {
                break;
            }
        }

        if (node->body != NULL) {
            auto body = this->execute(node->body);
            if (body.flags & ExecutionResult::BREAK) {
                break;
            }
            if (body.flags & ExecutionResult::CONTINUE) {
                continue;
            }
            if (body.flags & ExecutionResult::RETURN) {
                if (body.flags & ExecutionResult::DIRECT_PASS) {
                    return ExecutionResult(0, body.result);
                }
                return ExecutionResult(0, this->copy(body.result));
            };
        }
    }
    return ExecutionResult(0, NULL);
}

Runtime::ExecutionResult Runtime::execute(ForStmtNode *node) {
    if (node == NULL) {
        this->signalError("Failed to execute NULL AST node");
    }
    this->highlight(node->for_token);

    if (node->init != NULL) {
        this->execute(node->init);
    }

    while (true) {
        this->highlight(node->for_token);

        if (node->cond != NULL) {
            auto cond = this->execute(node->cond).result;
            if (!this->isInstanceObject(cond, this->boolean_type)) {
                this->signalError("Condition " + cond->shortRepr() + " is not Boolean");
            }

            if (!Builtin::getBooleanValue(cond, this)) {
                break;
            }
        }

        if (node->body != NULL) {
            auto body = this->execute(node->body);
            if (body.flags & ExecutionResult::BREAK) {
                break;
            }
            if (body.flags & ExecutionResult::CONTINUE) {
                continue;
            }
            if (body.flags & ExecutionResult::RETURN) {
                if (body.flags & ExecutionResult::DIRECT_PASS) {
                    return ExecutionResult(0, body.result);
                }
                return ExecutionResult(0, this->copy(body.result));
            };
        }

        if (node->step != NULL) {
            this->execute(node->step);
        }
    }
    return ExecutionResult(0, NULL);
}

Runtime::ExecutionResult Runtime::execute(IfStmtNode *node) {
    if (node == NULL) {
        this->signalError("Failed to execute NULL AST node");
    }
    this->highlight(node->if_token);

    auto cond = this->execute(node->cond).result;
    this->highlight(node->if_token);
    if (!this->isInstanceObject(cond, this->boolean_type)) {
        this->signalError("Condition " + cond->shortRepr() + " is not Boolean");
    }

    if (Builtin::getBooleanValue(cond, this)) {
        this->execute(node->body);
    }
    else if (node->else_body != NULL) {
        this->execute(node->else_body);
    }

    return ExecutionResult(0, NULL);
}

Runtime::ExecutionResult Runtime::execute(ReturnStmtNode *node) {
    if (node == NULL) {
        this->signalError("Failed to execute NULL AST node");
    }
    this->highlight(node->return_token);
    auto res = this->execute(node->value);
    if (res.flags & ExecutionResult::DIRECT_PASS) {
        return ExecutionResult(ExecutionResult::RETURN, res.result);
    }
    return ExecutionResult(ExecutionResult::RETURN, this->copy(res.result));
}

Runtime::ExecutionResult Runtime::execute(BlockStmtNode *node) {
    if (node == NULL) {
        this->signalError("Failed to execute NULL AST node");
    }
    if (!node->is_unscoped) {
        this->newFrame(true);
    }
    for (auto stmt : node->list) {
        auto res = this->execute(stmt);
        if (res.flags & ExecutionResult::CONTINUE) {
            if (!node->is_unscoped) {
                this->popFrame();
            }
            return res;
        }
        if (res.flags & ExecutionResult::BREAK) {
            if (!node->is_unscoped) {
                this->popFrame();
            }
            return res;
        }
        if (res.flags & ExecutionResult::RETURN) {
            if (!node->is_unscoped) {
                this->popFrame();
            }
            if (res.flags & ExecutionResult::DIRECT_PASS) {
                return ExecutionResult(ExecutionResult::RETURN, res.result);
            }
            return ExecutionResult(ExecutionResult::RETURN, this->copy(res.result));
        }
    }
    if (!node->is_unscoped) {
        this->popFrame();
    }
    return ExecutionResult(0, NULL);
}
}    // namespace Cotton
