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
    this->object_allocator = new PoolAllocator(1024);
    for (auto &a : this->array_of_allocators) {
        a = NULL;
    }

    this->scope         = new Scope(NULL, NULL, true);
    this->scope->master = this->scope;
    this->gc            = new GC(gc_strategy);
    this->gc->rt        = this;
    this->error_manager = error_manager;
    this->current_token = NULL;

    this->nothing_type   = new Builtin::NothingType(this);
    this->boolean_type   = new Builtin::BooleanType(this);
    this->function_type  = new Builtin::FunctionType(this);
    this->integer_type   = new Builtin::IntegerType(this);
    this->real_type      = new Builtin::RealType(this);
    this->character_type = new Builtin::CharacterType(this);
    this->string_type    = new Builtin::StringType(this);
    this->array_type    = new Builtin::ArrayType(this);

    assert(this->nothing_type->id == NOTHING_TYPE_ID);
    assert(this->boolean_type->id == BOOLEAN_TYPE_ID);
    assert(this->function_type->id == FUNCTION_TYPE_ID);
    assert(this->integer_type->id == INTEGER_TYPE_ID);
    assert(this->real_type->id == REAL_TYPE_ID);
    assert(this->character_type->id == CHARACTER_TYPE_ID);
    assert(this->string_type->id == STRING_TYPE_ID);
    assert(this->array_type->id == ARRAY_TYPE_ID);

    this->scope->addVariable(NameId("Nothing").id, this->make(this->nothing_type, Runtime::TYPE_OBJECT), this);
    this->scope->addVariable(NameId("Function").id, this->make(this->function_type, Runtime::TYPE_OBJECT), this);
    this->scope->addVariable(NameId("Boolean").id, this->make(this->boolean_type, Runtime::TYPE_OBJECT), this);
    this->scope->addVariable(NameId("Integer").id, this->make(this->integer_type, Runtime::TYPE_OBJECT), this);
    this->scope->addVariable(NameId("Real").id, this->make(this->real_type, Runtime::TYPE_OBJECT), this);
    this->scope->addVariable(NameId("Character").id, this->make(this->character_type, Runtime::TYPE_OBJECT), this);
    this->scope->addVariable(NameId("String").id, this->make(this->string_type, Runtime::TYPE_OBJECT), this);
    this->scope->addVariable(NameId("Array").id, this->make(this->array_type, Runtime::TYPE_OBJECT), this);

    Builtin::installBuiltinFunctions(this);

    this->protected_nothing             = this->make(this->nothing_type, Runtime::INSTANCE_OBJECT);
    this->protected_nothing->can_modify = false;
    this->gc->hold(this->protected_nothing);

    this->protected_true = this->make(this->boolean_type, Runtime::INSTANCE_OBJECT);
    Builtin::getBooleanValue(this->protected_true, this) = true;
    this->protected_true->can_modify                     = false;
    this->gc->hold(this->protected_true);

    this->protected_false = this->make(this->boolean_type, Runtime::INSTANCE_OBJECT);
    Builtin::getBooleanValue(this->protected_false, this) = false;
    this->protected_false->can_modify                     = false;
    this->gc->hold(this->protected_false);
}

PoolAllocator *Runtime::getAllocator(size_t size) {
    if (size < 4096) {
        if (this->array_of_allocators[size] != NULL) {
            return this->array_of_allocators[size];
        }
        return this->array_of_allocators[size] = new PoolAllocator(1024);
    }
    auto it = this->allocators.find(size);
    if (it != this->allocators.end()) {
        return it->second;
    }
    return this->allocators[size] = new PoolAllocator(1024);
}

void *Runtime::alloc(size_t size) {
    return this->getAllocator(size)->allocate(size);
}

void Runtime::dealloc(void *ptr, size_t size) {
    return this->getAllocator(size)->deallocate(ptr, size);
}

void Runtime::destroy(Object *obj) {
    if (obj != NULL) {
        this->object_allocator->deallocate(obj, sizeof(obj));
    }
}

void Runtime::destroy(Instance *ins) {
    if (ins != NULL) {
        ins->destroy(this);
    }
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
        obj = type->create(this);
    }
    else {
        obj = newObject(false, NULL, type, this);
    }

    if (obj == NULL) {
        this->signalError("Failed to make an object of type " + type->userRepr());
    }
    obj->spreadSingleUse();
    return obj;
}

Object *Runtime::copy(Object *obj) {
    ProfilerCAPTURE();
    if (!isTypeObject(obj)) {
        this->signalError("Failed to copy non type object " + obj->userRepr());
    }
    if (obj->single_use) {
        return obj;
    }
    auto res = obj->type->copy(obj, this);
    return res;
}

Object *Runtime::runOperator(OperatorNode::OperatorId id, Object *obj, bool execution_result_matters) {
    ProfilerCAPTURE();
    if (!isInstanceObject(obj)) {
        this->signalError("Failed to run operator " + std::to_string(id) + " on " + obj->userRepr());
    }

    UnaryOperatorAdapter op;

    switch (id) {
    case OperatorNode::POST_PLUS_PLUS :
        op = obj->type->postinc_op;
        if (op == NULL) {
            this->signalError(obj->userRepr() + " doesn't support that operator");
        }
        return op(obj, this, execution_result_matters);
    case OperatorNode::POST_MINUS_MINUS :
        op = obj->type->postdec_op;
        if (op == NULL) {
            this->signalError(obj->userRepr() + " doesn't support that operator");
        }
        return op(obj, this, execution_result_matters);
    case OperatorNode::PRE_PLUS_PLUS :
        op = obj->type->preinc_op;
        if (op == NULL) {
            this->signalError(obj->userRepr() + " doesn't support that operator");
        }
        return op(obj, this, execution_result_matters);
    case OperatorNode::PRE_MINUS_MINUS :
        op = obj->type->predec_op;
        if (op == NULL) {
            this->signalError(obj->userRepr() + " doesn't support that operator");
        }
        return op(obj, this, execution_result_matters);
    case OperatorNode::PRE_PLUS :
        op = obj->type->positive_op;
        if (op == NULL) {
            this->signalError(obj->userRepr() + " doesn't support that operator");
        }
        return op(obj, this, execution_result_matters);
    case OperatorNode::PRE_MINUS :
        op = obj->type->negative_op;
        if (op == NULL) {
            this->signalError(obj->userRepr() + " doesn't support that operator");
        }
        return op(obj, this, execution_result_matters);
    case OperatorNode::NOT :
        op = obj->type->not_op;
        if (op == NULL) {
            this->signalError(obj->userRepr() + " doesn't support that operator");
        }
        return op(obj, this, execution_result_matters);
    case OperatorNode::INVERSE :
        op = obj->type->inverse_op;
        if (op == NULL) {
            this->signalError(obj->userRepr() + " doesn't support that operator");
        }
        return op(obj, this, execution_result_matters);
    default : this->signalError(obj->userRepr() + " doesn't support that operator");
    }
}

Object *
Runtime::runOperator(OperatorNode::OperatorId id, Object *obj, Object *arg, bool execution_result_matters) {
    ProfilerCAPTURE();
    if (!isInstanceObject(obj)) {
        this->signalError("Failed to run operator " + std::to_string(id) + " on " + obj->userRepr());
    }

    BinaryOperatorAdapter op;

    switch (id) {
    case OperatorNode::MULT :
        op = obj->type->mult_op;
        if (op == NULL) {
            this->signalError(obj->userRepr() + " doesn't support that operator");
        }
        return op(obj, arg, this, execution_result_matters);
    case OperatorNode::DIV :
        op = obj->type->div_op;
        if (op == NULL) {
            this->signalError(obj->userRepr() + " doesn't support that operator");
        }

        return op(obj, arg, this, execution_result_matters);
    case OperatorNode::REM :
        op = obj->type->rem_op;
        if (op == NULL) {
            this->signalError(obj->userRepr() + " doesn't support that operator");
        }

        return op(obj, arg, this, execution_result_matters);
    case OperatorNode::RIGHT_SHIFT :
        op = obj->type->rshift_op;
        if (op == NULL) {
            this->signalError(obj->userRepr() + " doesn't support that operator");
        }

        return op(obj, arg, this, execution_result_matters);
    case OperatorNode::LEFT_SHIFT :
        op = obj->type->lshift_op;
        if (op == NULL) {
            this->signalError(obj->userRepr() + " doesn't support that operator");
        }

        return op(obj, arg, this, execution_result_matters);
    case OperatorNode::PLUS :
        op = obj->type->add_op;
        if (op == NULL) {
            this->signalError(obj->userRepr() + " doesn't support that operator");
        }

        return op(obj, arg, this, execution_result_matters);
    case OperatorNode::MINUS :
        op = obj->type->sub_op;
        if (op == NULL) {
            this->signalError(obj->userRepr() + " doesn't support that operator");
        }

        return op(obj, arg, this, execution_result_matters);
    case OperatorNode::LESS :
        op = obj->type->lt_op;
        if (op == NULL) {
            this->signalError(obj->userRepr() + " doesn't support that operator");
        }

        return op(obj, arg, this, execution_result_matters);
    case OperatorNode::LESS_EQUAL :
        op = obj->type->leq_op;
        if (op == NULL) {
            this->signalError(obj->userRepr() + " doesn't support that operator");
        }

        return op(obj, arg, this, execution_result_matters);
    case OperatorNode::GREATER :
        op = obj->type->gt_op;
        if (op == NULL) {
            this->signalError(obj->userRepr() + " doesn't support that operator");
        }

        return op(obj, arg, this, execution_result_matters);
    case OperatorNode::GREATER_EQUAL :
        op = obj->type->geq_op;
        if (op == NULL) {
            this->signalError(obj->userRepr() + " doesn't support that operator");
        }

        return op(obj, arg, this, execution_result_matters);
    case OperatorNode::EQUAL :
        op = obj->type->eq_op;
        if (op == NULL) {
            this->signalError(obj->userRepr() + " doesn't support that operator");
        }

        return op(obj, arg, this, execution_result_matters);
    case OperatorNode::NOT_EQUAL :
        op = obj->type->neq_op;
        if (op == NULL) {
            this->signalError(obj->userRepr() + " doesn't support that operator");
        }

        return op(obj, arg, this, execution_result_matters);
    case OperatorNode::BITAND :
        op = obj->type->bitand_op;
        if (op == NULL) {
            this->signalError(obj->userRepr() + " doesn't support that operator");
        }

        return op(obj, arg, this, execution_result_matters);
    case OperatorNode::BITXOR :
        op = obj->type->bitxor_op;
        if (op == NULL) {
            this->signalError(obj->userRepr() + " doesn't support that operator");
        }

        return op(obj, arg, this, execution_result_matters);
    case OperatorNode::BITOR :
        op = obj->type->bitor_op;
        if (op == NULL) {
            this->signalError(obj->userRepr() + " doesn't support that operator");
        }

        return op(obj, arg, this, execution_result_matters);
    case OperatorNode::AND :
        op = obj->type->and_op;
        if (op == NULL) {
            this->signalError(obj->userRepr() + " doesn't support that operator");
        }

        return op(obj, arg, this, execution_result_matters);
    case OperatorNode::OR :
        op = obj->type->or_op;
        if (op == NULL) {
            this->signalError(obj->userRepr() + " doesn't support that operator");
        }

        return op(obj, arg, this, execution_result_matters);
    default : this->signalError(obj->userRepr() + " doesn't support that operator");
    }
}

Object *Runtime::runOperator(OperatorNode::OperatorId     id,
                             Object                      *obj,
                             const std::vector<Object *> &args,
                             bool                         execution_result_matters) {
    ProfilerCAPTURE();
    if (!isInstanceObject(obj)) {
        this->signalError("Failed to run operator " + std::to_string(id) + " on " + obj->userRepr());
    }

    if (id == OperatorNode::CALL) {
        auto op = obj->type->call_op;
        if (op == NULL) {
            this->signalError(obj->userRepr() + " doesn't support that operator");
        }

        return op(obj, args, this, execution_result_matters);
    }
    else if (id == OperatorNode::INDEX) {
        auto op = obj->type->index_op;
        if (op == NULL) {
            this->signalError(obj->userRepr() + " doesn't support that operator");
        }

        return op(obj, args, this, execution_result_matters);
    }
    this->signalError(obj->userRepr() + " doesn't support that operator");
}

Object *
Runtime::runMethod(int64_t id, Object *obj, const std::vector<Object *> &args, bool execution_result_matters) {
    ProfilerCAPTURE();
    if (!isInstanceObject(obj)) {
        this->signalError("Failed to run method " + NameId::userRepr(id) + " on " + obj->userRepr());
    }

    auto method = obj->type->getMethod(id, this);
    return this->runOperator(OperatorNode::CALL, method, args, execution_result_matters);
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

Object *Runtime::execute(ExprNode *node, bool execution_result_matters) {
    ProfilerCAPTURE();
    if (node == NULL) {
        this->signalError("Failed to execute NULL AST node");
    }

    switch (node->id) {
    case ExprNode::FUNCTION_DEFINITION : {
        return this->execute(node->func_def, execution_result_matters);
    }
    case ExprNode::TYPE_DEFINITION : {
        return this->execute(node->type_def, execution_result_matters);
    }
    case ExprNode::OPERATOR : {
        return this->execute(node->op, execution_result_matters);
    }
    case ExprNode::ATOM : {
        return this->execute(node->atom, execution_result_matters);
    }
    case ExprNode::PARENTHESES_EXPRESSION : {
        return this->execute(node->par_expr, execution_result_matters);
    }
    default : this->signalError("Failed to execute unknown AST node");
    }
}

Object *Runtime::execute(FuncDefNode *node, bool execution_result_matters) {
    ProfilerCAPTURE();
    if (node == NULL) {
        this->signalError("Failed to execute NULL AST node");
    }
    highlight(this, node->function_token);
    auto func = Builtin::makeFunctionInstanceObject(false, NULL, node, this);
    if (node->name != NULL) {
        this->scope->addVariable(node->name->nameid, func, this);
    }
    setExecFlagNONE(this);
    return func;
}

Object *Runtime::execute(TypeDefNode *node, bool execution_result_matters) {
    ProfilerCAPTURE();
    if (node == NULL) {
        this->signalError("Failed to execute NULL AST node");
    }
    highlight(this, node->type_token);
    auto type             = new Builtin::UserDefinedType(this);
    type->nameid          = node->name->nameid;
    type->instance_fields = node->fields;

    for (auto method : node->methods) {
        highlight(this, method->function_token);
        auto f = Builtin::makeFunctionInstanceObject(false, NULL, method, this);
        type->addMethod(method->name->nameid, f);
    }

    highlight(this, node->type_token);
    auto res = this->make(type, Runtime::TYPE_OBJECT);
    this->scope->addVariable(type->nameid, res, this);

    setExecFlagNONE(this);
    return res;
}

static std::vector<Object *> getList(ExprNode *expr, Runtime *rt) {
    ProfilerCAPTURE();
    std::vector<Object *> res;
    while (expr != NULL) {
        if (expr->id == ExprNode::OPERATOR && expr->op->id == OperatorNode::COMMA) {
            auto r = rt->execute(expr->op->first, true);
            if (isExecFlagDIRECT_PASS(rt)) {
                res.push_back(r);
            }
            else {
                res.push_back(rt->copy(r));
            }
            expr = expr->op->second;
        }
        else {
            auto r = rt->execute(expr, true);
            if (isExecFlagDIRECT_PASS(rt)) {
                res.push_back(r);
            }
            else {
                res.push_back(rt->copy(r));
            }
            break;
        }
    }
    return res;
}

Object *Runtime::execute(OperatorNode *node, bool execution_result_matters) {
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
        auto res  = this->execute(node->first, execution_result_matters);
        auto expr = node->second;
        while (expr != NULL) {
            if (expr->id == ExprNode::OPERATOR && expr->op->id == OperatorNode::COMMA) {
                auto r = this->execute(expr->op->first, false);
                expr   = expr->op->second;
            }
            else {
                auto r = this->execute(expr, false);
                break;
            }
        }
        return res;
    }
    else if (node->id == OperatorNode::CALL || node->id == OperatorNode::INDEX) {
        if (node->id == OperatorNode::CALL && node->first->id == ExprNode::OPERATOR && node->first->op->id == OperatorNode::DOT) {
            auto    caller   = this->execute(node->first->op->first, true);
            Object *selected = NULL;

            auto dot = node->first->op;
            if (dot->second == NULL || dot->second->id != ExprNode::ATOM
                || dot->second->atom->id != AtomNode::IDENTIFIER)
            {
                this->signalError("Selector at the right-side is illegal");
            }
            int64_t selector = dot->second->atom->ident->nameid;
            if (!isInstanceObject(caller)) {
                this->signalError("Left-side object " + caller->userRepr() + " must be an instance object");
            }
            if (caller->instance->hasField(selector, this)) {
                selected = caller->instance->selectField(selector, this);
            }
            else if (caller->type->hasMethod(selector)) {
                selected = caller->type->getMethod(selector, this);
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

            auto res = this->runOperator(node->id, selected, args, execution_result_matters);
            setExecFlagNONE(this);
            return res;
        }
        else {
            Object *self = this->execute(node->first, true);
            highlight(this, node->op);
            std::vector<Object *> args;
            auto                  list = getList(node->second, this);
            args.reserve(list.size());

            for (auto r : list) {
                args.push_back(r);
            }

            auto res = this->runOperator(node->id, self, args, execution_result_matters);
            setExecFlagNONE(this);
            return res;
        }
    }

    Object *self  = this->execute(node->first, true);
    Object *other = NULL;

    highlight(this, node->op);
    switch (node->id) {
    case OperatorNode::DOT : {
        if (node->second == NULL || node->second->id != ExprNode::ATOM
            || node->second->atom->id != AtomNode::IDENTIFIER)
        {
            this->signalError("Selector at the right-side is illegal");
        }
        int64_t selector = node->second->atom->ident->nameid;
        if (!isInstanceObject(self)) {
            this->signalError("Left-side object " + self->userRepr() + " must be an instance object");
        }
        if (self->instance->hasField(selector, this)) {
            auto res = self->instance->selectField(selector, this);

            setExecFlagNONE(this);
            return res;
        }
        else if (self->type->hasMethod(selector)) {
            auto res = self->type->getMethod(selector, this);

            setExecFlagNONE(this);
            return res;
        }
        else {
            this->signalError("Invalid selector");
        }
    }
    case OperatorNode::AT : {
        setExecFlagDIRECT_PASS(this);
        return self;
    }
    case OperatorNode::ASSIGN : {
        other = this->execute(node->second, true);
        highlight(this, node->op);
        if (isExecFlagDIRECT_PASS(this)) {
            self->assignTo(other, this);
        }
        else {
            self->assignToCopyOf(other, this);
        }

        setExecFlagNONE(this);
        return self;
    }
    case OperatorNode::PLUS_ASSIGN : {
        other = this->execute(node->second, true);
        highlight(this, node->op);
        self->assignToCopyOf(this->runOperator(OperatorNode::PLUS, self, other, true), this);

        setExecFlagNONE(this);
        return self;
    }
    case OperatorNode::MINUS_ASSIGN : {
        other = this->execute(node->second, true);
        highlight(this, node->op);
        self->assignToCopyOf(this->runOperator(OperatorNode::MINUS, self, other, true), this);

        setExecFlagNONE(this);
        return self;
    }
    case OperatorNode::MULT_ASSIGN : {
        other = this->execute(node->second, true);
        highlight(this, node->op);
        self->assignToCopyOf(this->runOperator(OperatorNode::MULT, self, other, true), this);

        setExecFlagNONE(this);
        return self;
    }
    case OperatorNode::DIV_ASSIGN : {
        other = this->execute(node->second, true);
        highlight(this, node->op);
        self->assignToCopyOf(this->runOperator(OperatorNode::DIV, self, other, true), this);

        setExecFlagNONE(this);
        return self;
    }
    case OperatorNode::REM_ASSIGN : {
        other = this->execute(node->second, true);
        highlight(this, node->op);
        self->assignToCopyOf(this->runOperator(OperatorNode::REM, self, other, true), this);

        setExecFlagNONE(this);
        return self;
    }
    }

    switch (node->id) {
    case OperatorNode::POST_PLUS_PLUS :
    case OperatorNode::POST_MINUS_MINUS :
    case OperatorNode::PRE_PLUS_PLUS :
    case OperatorNode::PRE_MINUS_MINUS :
    case OperatorNode::PRE_PLUS :
    case OperatorNode::PRE_MINUS :
    case OperatorNode::NOT :
    case OperatorNode::INVERSE :
        auto res = this->runOperator(node->id, self, execution_result_matters);
        setExecFlagNONE(this);
        return res;
    }

    auto arg = this->execute(node->second, true);
    auto res = this->runOperator(node->id, self, arg, execution_result_matters);
    setExecFlagNONE(this);
    return res;
}

Object *Runtime::execute(AtomNode *node, bool execution_result_matters) {
    ProfilerCAPTURE();
    if (node == NULL) {
        this->signalError("Failed to execute NULL AST node");
    }
    highlight(this, node->token);
    __gnu_pbds::cc_hash_table<int64_t, Object *>::point_iterator it;

    switch (node->id) {
    case AtomNode::BOOLEAN : {
        if (!execution_result_matters) {
            setExecFlagNONE(this);
            return NULL;
        }
        if (node->lit_obj != NULL) {
            setExecFlagNONE(this);
            return node->lit_obj;
        }
        it = this->readonly_literals.find(node->token->nameid);
        if (it != this->readonly_literals.end()) {
            setExecFlagNONE(this);
            return it->second;
        }
        auto lit = Builtin::makeBooleanInstanceObject(node->bool_value, this);
        this->gc->hold(lit);
        lit->can_modify                              = false;
        this->readonly_literals[node->token->nameid] = lit;

        setExecFlagNONE(this);
        return node->lit_obj = lit;
    }
    case AtomNode::CHARACTER : {
        if (!execution_result_matters) {
            setExecFlagNONE(this);
            return NULL;
        }
        if (node->lit_obj != NULL) {
            setExecFlagNONE(this);
            return node->lit_obj;
        }
        it = this->readonly_literals.find(node->token->nameid);
        if (it != this->readonly_literals.end()) {
            setExecFlagNONE(this);
            return node->lit_obj = it->second;
            ;
        }
        auto lit = Builtin::makeCharacterInstanceObject(node->char_value, this);
        this->gc->hold(lit);
        lit->can_modify                              = false;
        this->readonly_literals[node->token->nameid] = lit;

        setExecFlagNONE(this);
        return node->lit_obj = lit;
    }
    case AtomNode::INTEGER : {
        if (!execution_result_matters) {
            setExecFlagNONE(this);
            return NULL;
        }
        if (node->lit_obj != NULL) {
            setExecFlagNONE(this);
            return node->lit_obj;
        }
        it = this->readonly_literals.find(node->token->nameid);
        if (it != this->readonly_literals.end()) {
            setExecFlagNONE(this);
            return node->lit_obj = it->second;
            ;
        }
        auto lit = Builtin::makeIntegerInstanceObject(node->int_value, this);
        this->gc->hold(lit);
        lit->can_modify                              = false;
        this->readonly_literals[node->token->nameid] = lit;

        setExecFlagNONE(this);
        return node->lit_obj = lit;
    }
    case AtomNode::REAL : {
        if (!execution_result_matters) {
            setExecFlagNONE(this);
            return NULL;
        }
        if (node->lit_obj != NULL) {
            setExecFlagNONE(this);
            return node->lit_obj;
        }
        it = this->readonly_literals.find(node->token->nameid);
        if (it != this->readonly_literals.end()) {
            setExecFlagNONE(this);
            return node->lit_obj = it->second;
            ;
        }
        auto lit = Builtin::makeRealInstanceObject(node->real_value, this);
        this->gc->hold(lit);
        lit->can_modify                              = false;
        this->readonly_literals[node->token->nameid] = lit;

        setExecFlagNONE(this);
        return node->lit_obj = lit;
    }
    case AtomNode::STRING : {
        if (!execution_result_matters) {
            setExecFlagNONE(this);
            return NULL;
        }
        if (node->lit_obj != NULL) {
            setExecFlagNONE(this);
            return node->lit_obj;
        }
        it = this->readonly_literals.find(node->token->nameid);
        if (it != this->readonly_literals.end()) {
            setExecFlagNONE(this);
            return node->lit_obj = it->second;
            ;
        }
        auto lit = Builtin::makeStringInstanceObject(node->string_value, this);
        this->gc->hold(lit);
        lit->can_modify                              = false;
        this->readonly_literals[node->token->nameid] = lit;

        setExecFlagNONE(this);
        return node->lit_obj = lit;
    }
    case AtomNode::NOTHING : {
        if (!execution_result_matters) {
            setExecFlagNONE(this);
            return NULL;
        }
        if (node->lit_obj != NULL) {
            setExecFlagNONE(this);
            return node->lit_obj;
        }
        it = this->readonly_literals.find(node->token->nameid);
        if (it != this->readonly_literals.end()) {
            setExecFlagNONE(this);
            return node->lit_obj = it->second;
            ;
        }
        auto lit = Builtin::makeNothingInstanceObject(this);
        this->gc->hold(lit);
        lit->can_modify                              = false;
        this->readonly_literals[node->token->nameid] = lit;

        setExecFlagNONE(this);
        return node->lit_obj = lit;
    }
    case AtomNode::IDENTIFIER : {
        setExecFlagNONE(this);
        return this->scope->getVariable(node->token->nameid, this);
    }
    default : this->signalError("Unknown atom");
    }
}

Object *Runtime::execute(ParExprNode *node, bool execution_result_matters) {
    ProfilerCAPTURE();
    if (node == NULL) {
        this->signalError("Failed to execute NULL AST node");
    }
    return this->execute(node->expr, execution_result_matters);
}

Object *Runtime::execute(StmtNode *node, bool execution_result_matters) {
    ProfilerCAPTURE();
    if (node == NULL) {
        this->signalError("Failed to execute NULL AST node");
    }
    this->gc->ping(this);
    switch (node->id) {
    case StmtNode::WHILE : {
        return this->execute(node->while_stmt, execution_result_matters);
    }
    case StmtNode::FOR : {
        return this->execute(node->for_stmt, execution_result_matters);
    }
    case StmtNode::IF : {
        return this->execute(node->if_stmt, execution_result_matters);
    }
    case StmtNode::CONTINUE : {
        setExecFlagCONTINUE(this);
        return this->protected_nothing;
    }
    case StmtNode::BREAK : {
        setExecFlagBREAK(this);
        return this->protected_nothing;
    }
    case StmtNode::RETURN : {
        return this->execute(node->return_stmt, execution_result_matters);
    }
    case StmtNode::BLOCK : {
        return this->execute(node->block_stmt, execution_result_matters);
    }
    case StmtNode::EXPR : {
        return this->execute(node->expr, execution_result_matters);
    }
    default : {
        this->signalError("Unknown node");
    }
    }
}

Object *Runtime::execute(WhileStmtNode *node, bool execution_result_matters) {
    ProfilerCAPTURE();
    if (node == NULL) {
        this->signalError("Failed to execute NULL AST node");
    }
    while (true) {
        this->newFrame();
        highlight(this, node->while_token);

        if (node->cond != NULL) {
            auto cond = this->execute(node->cond, true);

            if (!Builtin::getBooleanValue(cond, this)) {
                this->popFrame();
                break;
            }
        }

        if (node->body != NULL) {
            auto body = this->execute(node->body, execution_result_matters);
            if (isExecFlagBREAK(this)) {
                this->popFrame();
                break;
            }
            else if (isExecFlagCONTINUE(this)) {
                continue;
            }
            else if (isExecFlagRETURN(this)) {
                if (isExecFlagDIRECT_PASS(this)) {
                    setExecFlagRETURN(this);
                    this->popFrame();
                    return body;
                }
                setExecFlagRETURN(this);
                this->popFrame();
                return (execution_result_matters) ? this->copy(body) : NULL;
            };
        }
        this->popFrame();
    }
    setExecFlagNONE(this);
    return this->protected_nothing;
}

Object *Runtime::execute(ForStmtNode *node, bool execution_result_matters) {
    ProfilerCAPTURE();
    if (node == NULL) {
        this->signalError("Failed to execute NULL AST node");
    }
    highlight(this, node->for_token);

    if (node->init != NULL) {
        this->execute(node->init, false);
    }

    while (true) {
        this->newFrame();
        highlight(this, node->for_token);

        if (node->cond != NULL) {
            auto cond = this->execute(node->cond, true);

            if (!Builtin::getBooleanValue(cond, this)) {
                this->popFrame();
                break;
            }
        }

        if (node->body != NULL) {
            auto body = this->execute(node->body, execution_result_matters);
            if (isExecFlagBREAK(this)) {
                this->popFrame();
                break;
            }
            else if (isExecFlagCONTINUE(this)) {
                continue;
            }
            else if (isExecFlagRETURN(this)) {
                if (isExecFlagDIRECT_PASS(this)) {
                    setExecFlagRETURN(this);
                    this->popFrame();
                    return body;
                }
                setExecFlagRETURN(this);
                this->popFrame();
                return (execution_result_matters) ? this->copy(body) : NULL;
            };
        }

        if (node->step != NULL) {
            this->execute(node->step, false);
        }
        this->popFrame();
    }
    setExecFlagNONE(this);
    return this->protected_nothing;
}

Object *Runtime::execute(IfStmtNode *node, bool execution_result_matters) {
    ProfilerCAPTURE();
    if (node == NULL) {
        this->signalError("Failed to execute NULL AST node");
    }
    highlight(this, node->if_token);

    auto cond = this->execute(node->cond, true);
    highlight(this, node->if_token);

    if (Builtin::getBooleanValue(cond, this)) {
        return this->execute(node->body, execution_result_matters);
    }
    else if (node->else_body != NULL) {
        return this->execute(node->else_body, execution_result_matters);
    }
    setExecFlagNONE(this);
    return this->protected_nothing;
}

Object *Runtime::execute(ReturnStmtNode *node, bool execution_result_matters) {
    ProfilerCAPTURE();
    if (node == NULL) {
        this->signalError("Failed to execute NULL AST node");
    }
    highlight(this, node->return_token);
    auto res = this->execute(node->value, execution_result_matters);
    if (isExecFlagDIRECT_PASS(this)) {
        setExecFlagRETURN(this);
        return res;
    }
    setExecFlagRETURN(this);
    return (execution_result_matters) ? this->copy(res) : NULL;
}

Object *Runtime::execute(BlockStmtNode *node, bool execution_result_matters) {
    ProfilerCAPTURE();
    if (node == NULL) {
        this->signalError("Failed to execute NULL AST node");
    }
    if (!node->is_unscoped) {
        this->newFrame(true);
    }
    Object *res = NULL;
    for (auto stmt : node->list) {
        if (stmt == NULL) {
            continue;
        }
        res = this->execute(stmt, execution_result_matters);
        if (!isExecFlagNONE(this)) {
            if (!node->is_unscoped) {
                this->popFrame();
            }
            return res;
        }
    }
    if (!node->is_unscoped) {
        this->popFrame();
    }
    setExecFlagNONE(this);
    return (res != NULL) ? res : this->protected_nothing;
}
}    // namespace Cotton
