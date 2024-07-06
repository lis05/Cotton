#include "runtime.h"
#include "../builtin/api.h"
#include "../errors.h"
#include "../front/parser.h"
#include "../profiler.h"
#include "gc.h"
#include "instance.h"
#include "nameid.h"
#include "scope.h"
#include "type.h"

namespace Cotton {
Runtime::Runtime(GCStrategy *gc_strategy, ErrorManager *error_manager) {
    ProfilerCAPTURE();
    this->scope         = new Scope(NULL, NULL, true);
    this->scope->master = this->scope;
    this->gc            = new GC(gc_strategy);
    this->error_manager = error_manager;
    this->current_token = NULL;

    this->gc->rt    = this;
    gc_strategy->rt = this;

    this->nothing_type   = new Builtin::NothingType(this);
    this->function_type  = new Builtin::FunctionType(this);
    this->boolean_type   = new Builtin::BooleanType(this);
    this->integer_type   = new Builtin::IntegerType(this);
    this->real_type      = new Builtin::RealType(this);
    this->character_type = new Builtin::CharacterType(this);
    this->string_type    = new Builtin::StringType(this);

    this->scope->addVariable(NameId("Nothing").id, this->make(this->nothing_type, Runtime::TYPE_OBJECT), this);
    this->scope->addVariable(NameId("Function").id, this->make(this->function_type, Runtime::TYPE_OBJECT), this);
    this->scope->addVariable(NameId("Boolean").id, this->make(this->boolean_type, Runtime::TYPE_OBJECT), this);
    this->scope->addVariable(NameId("Integer").id, this->make(this->integer_type, Runtime::TYPE_OBJECT), this);
    this->scope->addVariable(NameId("Real").id, this->make(this->real_type, Runtime::TYPE_OBJECT), this);
    this->scope->addVariable(NameId("Character").id, this->make(this->character_type, Runtime::TYPE_OBJECT), this);
    this->scope->addVariable(NameId("String").id, this->make(this->string_type, Runtime::TYPE_OBJECT), this);

    Builtin::installBuiltinFunctions(this);

    this->exec_res_default = this->make(this->nothing_type, Runtime::INSTANCE_OBJECT);
    this->gc->hold(this->exec_res_default);
}

void Runtime::newFrame(bool can_access_prev_scope) {
    ProfilerCAPTURE();
    auto scope  = new Scope(this->scope, this->scope->master, can_access_prev_scope);
    this->scope = scope;
}

void Runtime::popFrame() {
    ProfilerCAPTURE();
    auto scope = this->scope->prev;
    delete this->scope;
    this->scope = scope;
}

Object *Runtime::make(Type *type, ObjectOptions object_opt) {
    ProfilerCAPTURE();
    if (type == NULL) {
        this->signalError("Failed to make an object of NULL type");
    }
    Object *obj = NULL;
    if (object_opt == Runtime::INSTANCE_OBJECT) {
        obj = type->create();
    }
    else {
        obj = new Object(false, NULL, type, this);
    }

    if (obj == NULL) {
        this->signalError("Failed to make an object of type " + type->shortRepr());
    }
    if (this->isInstanceObject(obj)) {
        if (obj->type->hasMethod(MagicMethods::__make__())) {
            this->runMethod(MagicMethods::__make__(), obj, {obj});
        }
    }
    return obj;
}

Object *Runtime::copy(Object *obj) {
    ProfilerCAPTURE();
    if (!this->isTypeObject(obj)) {
        this->signalError("Failed to copy non type object " + obj->shortRepr());
    }
    return obj->type->copy(obj);
}

Object *Runtime::runOperator(OperatorNode::OperatorId id, Object *obj, const std::vector<Object *> &args) {
    ProfilerCAPTURE();
    if (!this->isInstanceObject(obj)) {
        this->signalError("Failed to run operator " + std::to_string(id) + " on " + obj->shortRepr());
    }

    auto op = obj->type->getOperator(id);
    return op->operator()(obj, args);
}

Object *Runtime::runMethod(int64_t id, Object *obj, const std::vector<Object *> &args) {
    ProfilerCAPTURE();
    if (!this->isInstanceObject(obj)) {
        this->signalError("Failed to run method " + NameId::shortRepr(id) + " on " + obj->shortRepr());
    }

    auto method = obj->type->getMethod(id);
    return runOperator(OperatorNode::CALL, method, args);
}

Object *Runtime::runIfHasMethodOrNULL(int64_t id, Object *obj, const std::vector<Object *> &args) {
    ProfilerCAPTURE();
    if (!this->isInstanceObject(obj)) {
        this->signalError("Failed to run method " + NameId::shortRepr(id) + " on " + obj->shortRepr());
    }

    auto method = obj->type->getMethodOrNULL(id);
    if (method != NULL) {
        return runOperator(OperatorNode::CALL, method, args);
    }
    return NULL;
}

bool Runtime::isInstanceObject(Object *obj) {
    ProfilerCAPTURE();
    return obj != NULL && obj->instance != NULL && obj->type != NULL;
}

bool Runtime::isInstanceObject(Object *obj, Type *type) {
    ProfilerCAPTURE();
    return obj != NULL && obj->instance != NULL && obj->type == type;
}

bool Runtime::isTypeObject(Object *obj) {
    ProfilerCAPTURE();
    return obj != NULL && obj->type != NULL;
}

void Runtime::highlight(Token *token) {
    ProfilerCAPTURE();
    this->current_token = token;
}

void Runtime::signalError(const std::string &message, bool include_token) {
    ProfilerCAPTURE();
    if (include_token && this->current_token != NULL) {
        this->error_manager->signalError(message, *this->current_token);
    }
    else {
        this->error_manager->signalError(message);
    }
}

uint8_t Runtime::ExecutionResult::CONTINUE    = 1;
uint8_t Runtime::ExecutionResult::BREAK       = 2;
uint8_t Runtime::ExecutionResult::RETURN      = 4;
uint8_t Runtime::ExecutionResult::DIRECT_PASS = 8;

Runtime::ExecutionResult::ExecutionResult(uint8_t flags, Object *result) {
    ProfilerCAPTURE();
    this->flags  = flags;
    this->result = result;
}

Runtime::ExecutionResult Runtime::execute(ExprNode *node) {
    ProfilerCAPTURE();
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
    ProfilerCAPTURE();
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
    ProfilerCAPTURE();
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
        type->addMethod(method->name->nameid, f);
    }

    this->highlight(node->type_token);
    auto res = this->make(type, Runtime::TYPE_OBJECT);
    this->scope->addVariable(type->nameid, res, this);

    return ExecutionResult(0, res);
}

static std::vector<Object *> getList(ExprNode *expr, Runtime *rt) {
    ProfilerCAPTURE();
    std::vector<Object *> res;
    while (expr != NULL) {
        if (expr->id == ExprNode::OPERATOR && expr->op->id == OperatorNode::COMMA) {
            auto r = rt->execute(expr->op->first);

            if (r.flags & Runtime::ExecutionResult::DIRECT_PASS) {
                res.push_back(r.result);
            }
            else {
                res.push_back(rt->copy(r.result));
            }
            expr = expr->op->second;
        }
        else {
            auto r = rt->execute(expr);
            if (r.flags & Runtime::ExecutionResult::DIRECT_PASS) {
                res.push_back(r.result);
            }
            else {
                res.push_back(rt->copy(r.result));
            }
            break;
        }
    }
    return res;
}

Runtime::ExecutionResult Runtime::execute(OperatorNode *node) {
    ProfilerCAPTURE();
    if (node == NULL) {
        this->signalError("Failed to execute NULL AST node");
    }

    if (node->id == OperatorNode::ASSIGN) {
        if (node->first->id == ExprNode::ATOM && node->first->atom->id == AtomNode::IDENTIFIER) {
            if (!this->scope->queryVariable(node->first->atom->ident->nameid, this)) {
                this->scope->addVariable(node->first->atom->ident->nameid,
                                         Builtin::makeNothingInstanceObject(this),
                                         this);
            }
        }
    }
    else if (node->id == OperatorNode::COMMA) {
        ExecutionResult res  = this->execute(node->first);
        auto            expr = node->second;
        while (expr != NULL) {
            if (expr->id == ExprNode::OPERATOR && expr->op->id == OperatorNode::COMMA) {
                auto r = this->execute(expr->op->first);
                expr   = expr->op->second;
            }
            else {
                auto r = this->execute(expr);
                break;
            }
        }
        return res;
    }
    else if (node->id == OperatorNode::CALL) {
        if (node->first->id == ExprNode::OPERATOR && node->first->op->id == OperatorNode::DOT) {
            auto    caller   = this->execute(node->first->op->first).result;
            Object *selected = NULL;

            auto dot = node->first->op;
            if (dot->second == NULL || dot->second->id != ExprNode::ATOM
                || dot->second->atom->id != AtomNode::IDENTIFIER)
            {
                this->signalError("Selector at the right-side is illegal");
            }
            int64_t selector = dot->second->atom->ident->nameid;
            if (!this->isInstanceObject(caller)) {
                this->signalError("Left-side object " + caller->shortRepr() + " must be an instance object");
            }
            if (caller->instance->hasField(selector)) {
                selected = caller->instance->selectField(selector);
            }
            else if (caller->type->hasMethod(selector)) {
                selected = caller->type->getMethod(selector);
            }
            else {
                this->signalError("Invalid selector");
            }

            std::vector<Object *> args;
            auto                  list = getList(node->second, this);
            args.reserve(1 + list.size());
            args.push_back(caller);

            for (auto r : list) {
                args.push_back(r);
            }

            auto res = this->runOperator(node->id, caller, args);
            return ExecutionResult(0, res);
        }
    }

    Object         *self = this->execute(node->first).result;
    ExecutionResult other(0, this->exec_res_default);

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
        if (self->instance->hasField(selector)) {
            auto res = self->instance->selectField(selector);
            return ExecutionResult(0, res);
        }
        else if (self->type->hasMethod(selector)) {
            auto res = self->type->getMethod(selector);
            return ExecutionResult(0, res);
        }
        else {
            this->signalError("Invalid selector");
        }
    }
    case OperatorNode::AT : {
        return ExecutionResult(ExecutionResult::DIRECT_PASS, self);
    }
    case OperatorNode::ASSIGN : {
        other = this->execute(node->second);
        this->highlight(node->op);
        if (other.flags & ExecutionResult::DIRECT_PASS) {
            self->instance = other.result->instance;
            self->type     = other.result->type;
        }
        else {
            self->assignToCopyOf(other.result);
        }
        return ExecutionResult(0, self);
    }
    case OperatorNode::PLUS_ASSIGN : {
        other = this->execute(node->second);
        this->highlight(node->op);
        self->assignToCopyOf(this->runOperator(OperatorNode::PLUS, self, {other.result}));
        return ExecutionResult(0, self);
    }
    case OperatorNode::MINUS_ASSIGN : {
        other = this->execute(node->second);
        this->highlight(node->op);
        self->assignToCopyOf(this->runOperator(OperatorNode::MINUS, self, {other.result}));
        return ExecutionResult(0, self);
    }
    case OperatorNode::MULT_ASSIGN : {
        other = this->execute(node->second);
        this->highlight(node->op);
        self->assignToCopyOf(this->runOperator(OperatorNode::MULT, self, {other.result}));
        return ExecutionResult(0, self);
    }
    case OperatorNode::DIV_ASSIGN : {
        other = this->execute(node->second);
        this->highlight(node->op);
        self->assignToCopyOf(this->runOperator(OperatorNode::DIV, self, {other.result}));
        return ExecutionResult(0, self);
    }
    case OperatorNode::REM_ASSIGN : {
        other = this->execute(node->second);
        this->highlight(node->op);
        self->assignToCopyOf(this->runOperator(OperatorNode::REM, self, {other.result}));
        return ExecutionResult(0, self);
    }
    }

    std::vector<Object *> args;
    auto                  list = getList(node->second, this);
    args.reserve(1 + list.size());

    for (auto r : list) {
        args.push_back(r);
    }

    auto res = this->runOperator(node->id, self, args);
    return ExecutionResult(0, res);
}

Runtime::ExecutionResult Runtime::execute(AtomNode *node) {
    ProfilerCAPTURE();
    if (node == NULL) {
        this->signalError("Failed to execute NULL AST node");
    }
    this->highlight(node->token);
    __gnu_pbds::cc_hash_table<int64_t, Object *>::point_iterator it;

    switch (node->id) {
    case AtomNode::BOOLEAN : {
        if (node->lit_obj != NULL) {
            return ExecutionResult(0, node->lit_obj);
        }
        it = this->readonly_literals.find(node->token->nameid);
        if (it != this->readonly_literals.end()) {
            return ExecutionResult(0, it->second);
        }
        auto lit = Builtin::makeBooleanInstanceObject(node->bool_value, this);
        this->gc->hold(lit);
        lit->can_assign                              = false;
        this->readonly_literals[node->token->nameid] = lit;
        return ExecutionResult(0, node->lit_obj = lit);
    }
    case AtomNode::CHARACTER : {
        if (node->lit_obj != NULL) {
            return ExecutionResult(0, node->lit_obj);
        }
        it = this->readonly_literals.find(node->token->nameid);
        if (it != this->readonly_literals.end()) {
            return ExecutionResult(0, node->lit_obj = it->second);
        }
        auto lit = Builtin::makeCharacterInstanceObject(node->char_value, this);
        this->gc->hold(lit);
        lit->can_assign                              = false;
        this->readonly_literals[node->token->nameid] = lit;
        return ExecutionResult(0, node->lit_obj = lit);
    }
    case AtomNode::INTEGER : {
        if (node->lit_obj != NULL) {
            return ExecutionResult(0, node->lit_obj);
        }
        it = this->readonly_literals.find(node->token->nameid);
        if (it != this->readonly_literals.end()) {
            return ExecutionResult(0, node->lit_obj = it->second);
        }
        auto lit = Builtin::makeIntegerInstanceObject(node->int_value, this);
        this->gc->hold(lit);
        lit->can_assign                              = false;
        this->readonly_literals[node->token->nameid] = lit;
        return ExecutionResult(0, node->lit_obj = lit);
    }
    case AtomNode::REAL : {
        if (node->lit_obj != NULL) {
            return ExecutionResult(0, node->lit_obj);
        }
        it = this->readonly_literals.find(node->token->nameid);
        if (it != this->readonly_literals.end()) {
            return ExecutionResult(0, node->lit_obj = it->second);
        }
        auto lit = Builtin::makeRealInstanceObject(node->real_value, this);
        this->gc->hold(lit);
        lit->can_assign                              = false;
        this->readonly_literals[node->token->nameid] = lit;
        return ExecutionResult(0, node->lit_obj = lit);
    }
    case AtomNode::STRING : {
        if (node->lit_obj != NULL) {
            return ExecutionResult(0, node->lit_obj);
        }
        it = this->readonly_literals.find(node->token->nameid);
        if (it != this->readonly_literals.end()) {
            return ExecutionResult(0, node->lit_obj = it->second);
        }
        auto lit = Builtin::makeStringInstanceObject(node->string_value, this);
        this->gc->hold(lit);
        lit->can_assign                              = false;
        this->readonly_literals[node->token->nameid] = lit;
        return ExecutionResult(0, node->lit_obj = lit);
    }
    case AtomNode::NOTHING : {
        if (node->lit_obj != NULL) {
            return ExecutionResult(0, node->lit_obj);
        }
        it = this->readonly_literals.find(node->token->nameid);
        if (it != this->readonly_literals.end()) {
            return ExecutionResult(0, node->lit_obj = it->second);
        }
        auto lit = Builtin::makeNothingInstanceObject(this);
        this->gc->hold(lit);
        lit->can_assign                              = false;
        this->readonly_literals[node->token->nameid] = lit;
        return ExecutionResult(0, node->lit_obj = lit);
    }
    case AtomNode::IDENTIFIER : {
        return ExecutionResult(0, this->scope->getVariable(node->token->nameid, this));
    }
    default : this->signalError("Unknown atom");
    }
}

Runtime::ExecutionResult Runtime::execute(ParExprNode *node) {
    ProfilerCAPTURE();
    if (node == NULL) {
        this->signalError("Failed to execute NULL AST node");
    }
    return this->execute(node->expr);
}

Runtime::ExecutionResult Runtime::execute(StmtNode *node) {
    ProfilerCAPTURE();
    if (node == NULL) {
        this->signalError("Failed to execute NULL AST node");
    }
    this->gc->ping();
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
    ProfilerCAPTURE();
    if (node == NULL) {
        this->signalError("Failed to execute NULL AST node");
    }
    while (true) {
        this->newFrame();
        this->highlight(node->while_token);

        if (node->cond != NULL) {
            auto cond = this->execute(node->cond).result;

            if (!Builtin::getBooleanValue(cond, this)) {
                this->popFrame();
                break;
            }
        }

        if (node->body != NULL) {
            auto body = this->execute(node->body);
            if (body.flags & ExecutionResult::BREAK) {
                this->popFrame();
                break;
            }
            else if (body.flags & ExecutionResult::CONTINUE) {
                continue;
            }
            else if (body.flags & ExecutionResult::RETURN) {
                if (body.flags & ExecutionResult::DIRECT_PASS) {
                    auto res = ExecutionResult(ExecutionResult::RETURN, body.result);
                    this->popFrame();
                    return res;
                }
                auto res = ExecutionResult(ExecutionResult::RETURN, this->copy(body.result));
                this->popFrame();
                return res;
            };
        }
        this->popFrame();
    }
    return ExecutionResult(0, this->exec_res_default);
}

Runtime::ExecutionResult Runtime::execute(ForStmtNode *node) {
    ProfilerCAPTURE();
    if (node == NULL) {
        this->signalError("Failed to execute NULL AST node");
    }
    this->highlight(node->for_token);

    if (node->init != NULL) {
        this->execute(node->init);
    }

    while (true) {
        this->newFrame();
        this->highlight(node->for_token);

        if (node->cond != NULL) {
            auto cond = this->execute(node->cond).result;

            if (!Builtin::getBooleanValue(cond, this)) {
                this->popFrame();
                break;
            }
        }

        if (node->body != NULL) {
            auto body = this->execute(node->body);
            if (body.flags & ExecutionResult::BREAK) {
                this->popFrame();
                break;
            }
            else if (body.flags & ExecutionResult::CONTINUE) {
                continue;
            }
            else if (body.flags & ExecutionResult::RETURN) {
                if (body.flags & ExecutionResult::DIRECT_PASS) {
                    auto res = ExecutionResult(ExecutionResult::RETURN, body.result);
                    this->popFrame();
                    return res;
                }
                auto res = ExecutionResult(ExecutionResult::RETURN, this->copy(body.result));
                this->popFrame();
                return res;
            };
        }

        if (node->step != NULL) {
            this->execute(node->step);
        }
        this->popFrame();
    }
    return ExecutionResult(0, this->exec_res_default);
}

Runtime::ExecutionResult Runtime::execute(IfStmtNode *node) {
    ProfilerCAPTURE();
    if (node == NULL) {
        this->signalError("Failed to execute NULL AST node");
    }
    this->highlight(node->if_token);

    auto cond = this->execute(node->cond).result;
    this->highlight(node->if_token);

    if (Builtin::getBooleanValue(cond, this)) {
        return this->execute(node->body);
    }
    else if (node->else_body != NULL) {
        return this->execute(node->else_body);
    }
    return ExecutionResult(0, this->exec_res_default);
}

Runtime::ExecutionResult Runtime::execute(ReturnStmtNode *node) {
    ProfilerCAPTURE();
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
    ProfilerCAPTURE();
    if (node == NULL) {
        this->signalError("Failed to execute NULL AST node");
    }
    if (!node->is_unscoped) {
        this->newFrame(true);
    }
    for (auto stmt : node->list) {
        if (stmt == NULL) {
            continue;
        }
        auto res = this->execute(stmt);
        if (res.flags & ExecutionResult::CONTINUE) {
            if (!node->is_unscoped) {
                this->popFrame();
            }
            return res;
        }
        else if (res.flags & ExecutionResult::BREAK) {
            if (!node->is_unscoped) {
                this->popFrame();
            }
            return res;
        }
        else if (res.flags & ExecutionResult::RETURN) {
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
    return ExecutionResult(0, this->exec_res_default);
}
}    // namespace Cotton
