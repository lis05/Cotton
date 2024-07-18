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

    this->scope         = new Scope(NULL, NULL, false);
    this->scope->master = this->scope;
    this->gc            = new GC(gc_strategy);
    this->gc->rt        = this;
    this->error_manager = error_manager;
    this->newContext();

    this->function_type  = new Builtin::FunctionType(this);
    this->nothing_type   = new Builtin::NothingType(this);
    this->boolean_type   = new Builtin::BooleanType(this);
    this->integer_type   = new Builtin::IntegerType(this);
    this->real_type      = new Builtin::RealType(this);
    this->character_type = new Builtin::CharacterType(this);
    this->string_type    = new Builtin::StringType(this);
    this->array_type     = new Builtin::ArrayType(this);

    assert(this->function_type->id == FUNCTION_TYPE_ID);
    assert(this->nothing_type->id == NOTHING_TYPE_ID);
    assert(this->boolean_type->id == BOOLEAN_TYPE_ID);
    assert(this->integer_type->id == INTEGER_TYPE_ID);
    assert(this->real_type->id == REAL_TYPE_ID);
    assert(this->character_type->id == CHARACTER_TYPE_ID);
    assert(this->string_type->id == STRING_TYPE_ID);
    assert(this->array_type->id == ARRAY_TYPE_ID);

    auto nothing_obj        = this->make(this->nothing_type, Runtime::TYPE_OBJECT);
    nothing_obj->can_modify = false;
    this->scope->addVariable(NameId("Nothing").id, nothing_obj, this);
    this->registerTypeObject(this->nothing_type, nothing_obj);

    auto function_obj        = this->make(this->function_type, Runtime::TYPE_OBJECT);
    function_obj->can_modify = false;
    this->scope->addVariable(NameId("Function").id, function_obj, this);
    this->registerTypeObject(this->function_type, function_obj);

    auto boolean_obj        = this->make(this->boolean_type, Runtime::TYPE_OBJECT);
    boolean_obj->can_modify = false;
    this->scope->addVariable(NameId("Boolean").id, boolean_obj, this);
    this->registerTypeObject(this->boolean_type, boolean_obj);

    auto integer_obj        = this->make(this->integer_type, Runtime::TYPE_OBJECT);
    integer_obj->can_modify = false;
    this->scope->addVariable(NameId("Integer").id, integer_obj, this);
    this->registerTypeObject(this->integer_type, integer_obj);

    auto real_obj        = this->make(this->real_type, Runtime::TYPE_OBJECT);
    real_obj->can_modify = false;
    this->scope->addVariable(NameId("Real").id, real_obj, this);
    this->registerTypeObject(this->real_type, real_obj);

    auto character_obj        = this->make(this->character_type, Runtime::TYPE_OBJECT);
    character_obj->can_modify = false;
    this->scope->addVariable(NameId("Character").id, character_obj, this);
    this->registerTypeObject(this->character_type, character_obj);

    auto string_obj        = this->make(this->string_type, Runtime::TYPE_OBJECT);
    string_obj->can_modify = false;
    this->scope->addVariable(NameId("String").id, string_obj, this);
    this->registerTypeObject(this->string_type, string_obj);

    auto array_obj        = this->make(this->array_type, Runtime::TYPE_OBJECT);
    array_obj->can_modify = false;
    this->scope->addVariable(NameId("Array").id, array_obj, this);
    this->registerTypeObject(this->array_type, array_obj);

    Builtin::installBooleanMethods(this->boolean_type, this);
    Builtin::installCharacterMethods(this->character_type, this);
    Builtin::installFunctionMethods(this->function_type, this);
    Builtin::installIntegerMethods(this->integer_type, this);
    Builtin::installRealMethods(this->real_type, this);
    Builtin::installNothingMethods(this->nothing_type, this);
    Builtin::installStringMethods(this->string_type, this);
    Builtin::installArrayMethods(this->array_type, this);

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

void Runtime::registerTypeObject(Type *type, Object *obj) {
    this->type_objects[type] = obj;
}

Object *Runtime::getTypeObject(Type *type) {
    auto it = this->type_objects.find(type);
    if (it != this->type_objects.end()) {
        return it->second;
    }
    return this->protected_nothing;
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

Object *Runtime::protectedBoolean(bool val) {
    return val ? this->protected_true : this->protected_false;
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
        this->signalError("Failed to make an object of NULL type", this->getContext().area);
    }
    Object *obj = NULL;
    if (object_opt == Runtime::INSTANCE_OBJECT) {
        obj = type->create(this);
    }
    else {
        obj = newObject(false, NULL, type, this);
    }

    if (obj == NULL) {
        this->signalError("Failed to make an object of type " + type->userRepr(), this->getContext().area);
    }
    obj->spreadSingleUse();
    return obj;
}

Object *Runtime::copy(Object *obj) {
    ProfilerCAPTURE();
    if (!this->isValidObject(obj)) {
        this->signalError("Failed to copy non type object " + obj->userRepr(), this->getContext().area);
    }
    if (obj->single_use) {
        return obj;
    }
    auto res = obj->type->copy(obj, this);
    return res;
}

Object *Runtime::runOperator(OperatorNode::OperatorId id, Object *obj, bool execution_result_matters) {
    ProfilerCAPTURE();
    this->verifyIsValidObject(obj, this->getContext().sub_areas[1]);

    UnaryOperatorAdapter op;

    switch (id) {
    case OperatorNode::POST_PLUS_PLUS :
        op = obj->type->postinc_op;
        if (op == NULL) {
            this->signalError(obj->userRepr() + " doesn't support that operator", this->getContext().area);
        }
        return op(obj, this, execution_result_matters);
    case OperatorNode::POST_MINUS_MINUS :
        op = obj->type->postdec_op;
        if (op == NULL) {
            this->signalError(obj->userRepr() + " doesn't support that operator", this->getContext().area);
        }
        return op(obj, this, execution_result_matters);
    case OperatorNode::PRE_PLUS_PLUS :
        op = obj->type->preinc_op;
        if (op == NULL) {
            this->signalError(obj->userRepr() + " doesn't support that operator", this->getContext().area);
        }
        return op(obj, this, execution_result_matters);
    case OperatorNode::PRE_MINUS_MINUS :
        op = obj->type->predec_op;
        if (op == NULL) {
            this->signalError(obj->userRepr() + " doesn't support that operator", this->getContext().area);
        }
        return op(obj, this, execution_result_matters);
    case OperatorNode::PRE_PLUS :
        op = obj->type->positive_op;
        if (op == NULL) {
            this->signalError(obj->userRepr() + " doesn't support that operator", this->getContext().area);
        }
        return op(obj, this, execution_result_matters);
    case OperatorNode::PRE_MINUS :
        op = obj->type->negative_op;
        if (op == NULL) {
            this->signalError(obj->userRepr() + " doesn't support that operator", this->getContext().area);
        }
        return op(obj, this, execution_result_matters);
    case OperatorNode::NOT :
        op = obj->type->not_op;
        if (op == NULL) {
            this->signalError(obj->userRepr() + " doesn't support that operator", this->getContext().area);
        }
        return op(obj, this, execution_result_matters);
    case OperatorNode::INVERSE :
        op = obj->type->inverse_op;
        if (op == NULL) {
            this->signalError(obj->userRepr() + " doesn't support that operator", this->getContext().area);
        }
        return op(obj, this, execution_result_matters);
    default : this->signalError(obj->userRepr() + " doesn't support that operator", this->getContext().area);
    }
}

Object *
Runtime::runOperator(OperatorNode::OperatorId id, Object *obj, Object *arg, bool execution_result_matters) {
    ProfilerCAPTURE();
    this->verifyIsValidObject(obj, this->getContext().sub_areas[1]);
    this->verifyIsValidObject(arg, this->getContext().sub_areas[2]);

    BinaryOperatorAdapter op;

    switch (id) {
    case OperatorNode::MULT :
        op = obj->type->mult_op;
        if (op == NULL) {
            this->signalError("Left argument " + obj->userRepr() + " doesn't support that operator",
                              this->getContext().area);
        }
        return op(obj, arg, this, execution_result_matters);
    case OperatorNode::DIV :
        op = obj->type->div_op;
        if (op == NULL) {
            this->signalError("Left argument " + obj->userRepr() + " doesn't support that operator",
                              this->getContext().area);
        }

        return op(obj, arg, this, execution_result_matters);
    case OperatorNode::REM :
        op = obj->type->rem_op;
        if (op == NULL) {
            this->signalError("Left argument " + obj->userRepr() + " doesn't support that operator",
                              this->getContext().area);
        }

        return op(obj, arg, this, execution_result_matters);
    case OperatorNode::RIGHT_SHIFT :
        op = obj->type->rshift_op;
        if (op == NULL) {
            this->signalError("Left argument " + obj->userRepr() + " doesn't support that operator",
                              this->getContext().area);
        }

        return op(obj, arg, this, execution_result_matters);
    case OperatorNode::LEFT_SHIFT :
        op = obj->type->lshift_op;
        if (op == NULL) {
            this->signalError("Left argument " + obj->userRepr() + " doesn't support that operator",
                              this->getContext().area);
        }

        return op(obj, arg, this, execution_result_matters);
    case OperatorNode::PLUS :
        op = obj->type->add_op;
        if (op == NULL) {
            this->signalError("Left argument " + obj->userRepr() + " doesn't support that operator",
                              this->getContext().area);
        }

        return op(obj, arg, this, execution_result_matters);
    case OperatorNode::MINUS :
        op = obj->type->sub_op;
        if (op == NULL) {
            this->signalError("Left argument " + obj->userRepr() + " doesn't support that operator",
                              this->getContext().area);
        }

        return op(obj, arg, this, execution_result_matters);
    case OperatorNode::LESS :
        op = obj->type->lt_op;
        if (op == NULL) {
            this->signalError("Left argument " + obj->userRepr() + " doesn't support that operator",
                              this->getContext().area);
        }

        return op(obj, arg, this, execution_result_matters);
    case OperatorNode::LESS_EQUAL :
        op = obj->type->leq_op;
        if (op == NULL) {
            this->signalError("Left argument " + obj->userRepr() + " doesn't support that operator",
                              this->getContext().area);
        }

        return op(obj, arg, this, execution_result_matters);
    case OperatorNode::GREATER :
        op = obj->type->gt_op;
        if (op == NULL) {
            this->signalError("Left argument " + obj->userRepr() + " doesn't support that operator",
                              this->getContext().area);
        }

        return op(obj, arg, this, execution_result_matters);
    case OperatorNode::GREATER_EQUAL :
        op = obj->type->geq_op;
        if (op == NULL) {
            this->signalError("Left argument " + obj->userRepr() + " doesn't support that operator",
                              this->getContext().area);
        }

        return op(obj, arg, this, execution_result_matters);
    case OperatorNode::EQUAL :
        op = obj->type->eq_op;
        if (op == NULL) {
            this->signalError("Left argument " + obj->userRepr() + " doesn't support that operator",
                              this->getContext().area);
        }

        return op(obj, arg, this, execution_result_matters);
    case OperatorNode::NOT_EQUAL :
        op = obj->type->neq_op;
        if (op == NULL) {
            this->signalError("Left argument " + obj->userRepr() + " doesn't support that operator",
                              this->getContext().area);
        }

        return op(obj, arg, this, execution_result_matters);
    case OperatorNode::BITAND :
        op = obj->type->bitand_op;
        if (op == NULL) {
            this->signalError("Left argument " + obj->userRepr() + " doesn't support that operator",
                              this->getContext().area);
        }

        return op(obj, arg, this, execution_result_matters);
    case OperatorNode::BITXOR :
        op = obj->type->bitxor_op;
        if (op == NULL) {
            this->signalError("Left argument " + obj->userRepr() + " doesn't support that operator",
                              this->getContext().area);
        }

        return op(obj, arg, this, execution_result_matters);
    case OperatorNode::BITOR :
        op = obj->type->bitor_op;
        if (op == NULL) {
            this->signalError("Left argument " + obj->userRepr() + " doesn't support that operator",
                              this->getContext().area);
        }

        return op(obj, arg, this, execution_result_matters);
    case OperatorNode::AND :
        op = obj->type->and_op;
        if (op == NULL) {
            this->signalError("Left argument " + obj->userRepr() + " doesn't support that operator",
                              this->getContext().area);
        }

        return op(obj, arg, this, execution_result_matters);
    case OperatorNode::OR :
        op = obj->type->or_op;
        if (op == NULL) {
            this->signalError("Left argument " + obj->userRepr() + " doesn't support that operator",
                              this->getContext().area);
        }

        return op(obj, arg, this, execution_result_matters);
    default :
        this->signalError("Left argument " + obj->userRepr() + " doesn't support that operator",
                          this->getContext().area);
    }
}

Object *Runtime::runOperator(OperatorNode::OperatorId     id,
                             Object                      *obj,
                             const std::vector<Object *> &args,
                             bool                         execution_result_matters) {
    ProfilerCAPTURE();
    this->verifyIsValidObject(obj, this->getContext().sub_areas[1]);

    if (id == OperatorNode::CALL) {
        auto op = obj->type->call_op;
        if (op == NULL) {
            this->signalError("Left argument " + obj->userRepr() + " doesn't support that operator",
                              this->getContext().area);
        }

        return op(obj, args, this, execution_result_matters);
    }
    else if (id == OperatorNode::INDEX) {
        auto op = obj->type->index_op;
        if (op == NULL) {
            this->signalError("Left argument " + obj->userRepr() + " doesn't support that operator",
                              this->getContext().area);
        }

        return op(obj, args, this, execution_result_matters);
    }
    else {
        this->signalError("Left argument " + obj->userRepr() + " doesn't support that operator",
                          this->getContext().area);
    }
}

Object *
Runtime::runMethod(int64_t id, Object *obj, const std::vector<Object *> &args, bool execution_result_matters) {
    auto method = obj->type->getMethod(id, this);
    return this->runOperator(OperatorNode::CALL, method, args, execution_result_matters);
}

void Runtime::newContext() {
    this->error_contexts.push_back({});
}

void Runtime::popContext() {
    this->error_contexts.pop_back();
}

Runtime::ErrorContext &Runtime::getContext() {
    return this->error_contexts.back();
}

void Runtime::signalError(const std::string &message, const TextArea &ta) {
    ProfilerCAPTURE();
    TextArea prev;
    for (auto c : this->error_contexts) {
        if (c.area.first_char < c.area.last_char) {
            if (c.area.first_char == prev.first_char && c.area.last_char == prev.last_char) {
                continue;
            }
            if (c.area.first_char == ta.first_char && c.area.last_char == ta.last_char) {
                continue;
            }
            this->error_manager->signalError("Error occurred here", c.area, false);
            prev = c.area;
        }
    }
    this->error_manager->signalError(message, ta);
}

Object *Runtime::execute(ExprNode *node, bool execution_result_matters) {
    ProfilerCAPTURE();
    if (node == NULL) {
        this->signalError("Failed to execute NULL AST node", this->getContext().area);
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
    default : this->signalError("Failed to execute unknown AST node", this->getContext().area);
    }
}

Object *Runtime::execute(FuncDefNode *node, bool execution_result_matters) {
    ProfilerCAPTURE();
    if (node == NULL) {
        this->signalError("Failed to execute unknown AST node", this->getContext().area);
    }
    this->newContext();
    this->getContext().area = node->text_area;
    // highlight(this, node->function_token);
    auto func               = Builtin::makeFunctionInstanceObject(false, NULL, node, this);
    if (node->name != NULL) {
        this->scope->addVariable(node->name->nameid, func, this);
    }
    this->popContext();
    setExecFlagNONE(this);
    return func;
}

Object *Runtime::execute(TypeDefNode *node, bool execution_result_matters) {
    ProfilerCAPTURE();
    if (node == NULL) {
        this->signalError("Failed to execute unknown AST node", this->getContext().area);
    }
    this->newContext();
    this->getContext().area = node->text_area;
    auto type               = new Builtin::UserDefinedType(this);
    type->nameid            = node->name->nameid;
    type->instance_fields   = node->fields;

    for (auto method : node->methods) {
        this->newContext();
        this->getContext().area = method->text_area;
        auto f                  = Builtin::makeFunctionInstanceObject(false, NULL, method, this);
        type->addMethod(method->name->nameid, f);
        this->popContext();
    }

    auto res = this->make(type, Runtime::TYPE_OBJECT);
    this->scope->addVariable(type->nameid, res, this);
    this->popContext();

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
                rt->newContext();
                rt->getContext().area = expr->op->first->text_area;
                res.push_back(rt->copy(r));
                rt->popContext();
            }
            expr = expr->op->second;
        }
        else {
            auto r = rt->execute(expr, true);
            if (isExecFlagDIRECT_PASS(rt)) {
                res.push_back(r);
            }
            else {
                rt->newContext();
                rt->getContext().area = expr->text_area;
                res.push_back(rt->copy(r));
                rt->popContext();
            }
            break;
        }
    }
    return res;
}

static void getList_addToContext(ExprNode *expr, Runtime *rt) {
    ProfilerCAPTURE();
    while (expr != NULL) {
        if (expr->id == ExprNode::OPERATOR && expr->op->id == OperatorNode::COMMA) {
            rt->getContext().sub_areas.push_back(expr->op->first->text_area);
            expr = expr->op->second;
        }
        else {
            rt->getContext().sub_areas.push_back(expr->text_area);
            break;
        }
    }
}

Object *Runtime::execute(OperatorNode *node, bool execution_result_matters) {
    ProfilerCAPTURE();
    if (node == NULL) {
        this->signalError("Failed to execute unknown AST node", this->getContext().area);
    }

    this->newContext();
    this->getContext().area = node->text_area;

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
        this->popContext();
        return res;
    }
    else if (node->id == OperatorNode::CALL || node->id == OperatorNode::INDEX) {
        if (node->id == OperatorNode::CALL && node->first->id == ExprNode::OPERATOR
            && node->first->op->id == OperatorNode::DOT)
        {
            auto    caller   = this->execute(node->first->op->first, true);
            Object *selected = NULL;

            auto dot = node->first->op;
            if (dot->second == NULL || dot->second->id != ExprNode::ATOM
                || dot->second->atom->id != AtomNode::IDENTIFIER)
            {
                this->signalError("Selector is illegal", dot->second->text_area);
            }
            int64_t selector = dot->second->atom->ident->nameid;
            if (!this->isInstanceObject(caller, NULL)) {
                this->signalError(caller->userRepr() + " must be an instance object", dot->first->text_area);
            }
            if (caller->instance->hasField(selector, this)) {
                selected = caller->instance->selectField(selector, this);
            }
            else if (caller->type->hasMethod(selector)) {
                selected = caller->type->getMethod(selector, this);
            }
            else {
                this->signalError("Invalid selector", dot->second->text_area);
            }

            std::vector<Object *> args;
            auto                  list = getList(node->second, this);
            args.reserve(1 + list.size());
            args.push_back(caller);

            for (auto r : list) {
                args.push_back(r);
            }

            this->newContext();
            this->getContext().area = node->text_area;
            this->getContext().sub_areas.push_back(dot->text_area);    // caller
            getList_addToContext(node->second, this);
            auto res = this->runOperator(node->id, selected, args, execution_result_matters);
            this->popContext();

            setExecFlagNONE(this);
            this->popContext();
            return res;
        }
        else {
            Object               *self = this->execute(node->first, true);
            std::vector<Object *> args;
            auto                  list = getList(node->second, this);
            args.reserve(list.size());

            for (auto r : list) {
                args.push_back(r);
            }

            this->newContext();
            this->getContext().area = node->text_area;
            this->getContext().sub_areas.push_back(node->first->text_area);    // caller
            getList_addToContext(node->second, this);
            auto res = this->runOperator(node->id, self, args, execution_result_matters);
            this->popContext();

            setExecFlagNONE(this);
            this->popContext();
            return res;
        }
    }

    Object *self  = this->execute(node->first, true);
    Object *other = NULL;

    switch (node->id) {
    case OperatorNode::DOT : {
        if (node->second == NULL || node->second->id != ExprNode::ATOM
            || node->second->atom->id != AtomNode::IDENTIFIER)
        {
            this->signalError("Selector is illegal", node->second->text_area);
        }
        int64_t selector = node->second->atom->ident->nameid;
        if (!isInstanceObject(self)) {
            this->signalError(self->userRepr() + " must be an instance object", node->first->text_area);
        }
        if (self->instance->hasField(selector, this)) {
            auto res = self->instance->selectField(selector, this);

            setExecFlagNONE(this);
            this->popContext();
            return res;
        }
        else if (self->type->hasMethod(selector)) {
            auto res = self->type->getMethod(selector, this);

            setExecFlagNONE(this);
            this->popContext();
            return res;
        }
        else {
            this->signalError("Invalid selector", node->second->text_area);
        }
    }
    case OperatorNode::AT : {
        setExecFlagDIRECT_PASS(this);
        this->popContext();
        return self;
    }
    case OperatorNode::ASSIGN : {
        other = this->execute(node->second, true);
        if (isExecFlagDIRECT_PASS(this)) {
            self->assignTo(other, this);
        }
        else {
            self->assignToCopyOf(other, this);
        }

        setExecFlagNONE(this);
        this->popContext();
        return self;
    }
    case OperatorNode::PLUS_ASSIGN : {
        other = this->execute(node->second, true);
        this->getContext().sub_areas.push_back(node->first->text_area);
        this->getContext().sub_areas.push_back(node->second->text_area);
        self->assignToCopyOf(this->runOperator(OperatorNode::PLUS, self, other, true), this);

        setExecFlagNONE(this);
        this->popContext();
        return self;
    }
    case OperatorNode::MINUS_ASSIGN : {
        other = this->execute(node->second, true);
        this->getContext().sub_areas.push_back(node->first->text_area);
        this->getContext().sub_areas.push_back(node->second->text_area);
        self->assignToCopyOf(this->runOperator(OperatorNode::MINUS, self, other, true), this);

        setExecFlagNONE(this);
        this->popContext();
        return self;
    }
    case OperatorNode::MULT_ASSIGN : {
        other = this->execute(node->second, true);
        this->getContext().sub_areas.push_back(node->first->text_area);
        this->getContext().sub_areas.push_back(node->second->text_area);
        self->assignToCopyOf(this->runOperator(OperatorNode::MULT, self, other, true), this);

        setExecFlagNONE(this);
        this->popContext();
        return self;
    }
    case OperatorNode::DIV_ASSIGN : {
        other = this->execute(node->second, true);
        this->getContext().sub_areas.push_back(node->first->text_area);
        this->getContext().sub_areas.push_back(node->second->text_area);
        self->assignToCopyOf(this->runOperator(OperatorNode::DIV, self, other, true), this);

        setExecFlagNONE(this);
        this->popContext();
        return self;
    }
    case OperatorNode::REM_ASSIGN : {
        other = this->execute(node->second, true);
        this->getContext().sub_areas.push_back(node->first->text_area);
        this->getContext().sub_areas.push_back(node->second->text_area);
        self->assignToCopyOf(this->runOperator(OperatorNode::REM, self, other, true), this);

        setExecFlagNONE(this);
        this->popContext();
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
        this->getContext().sub_areas.push_back(node->first->text_area);
        auto res = this->runOperator(node->id, self, execution_result_matters);
        setExecFlagNONE(this);
        this->popContext();
        return res;
    }

    auto arg = this->execute(node->second, true);
    this->getContext().sub_areas.push_back(node->first->text_area);
    this->getContext().sub_areas.push_back(node->second->text_area);
    auto res = this->runOperator(node->id, self, arg, execution_result_matters);
    setExecFlagNONE(this);
    this->popContext();
    return res;
}

Object *Runtime::execute(AtomNode *node, bool execution_result_matters) {
    ProfilerCAPTURE();
    if (node == NULL) {
        this->signalError("Failed to execute unknown AST node", this->getContext().area);
    }
    this->newContext();
    this->getContext().area = node->text_area;
    __gnu_pbds::cc_hash_table<int64_t, Object *>::point_iterator it;

    switch (node->id) {
    case AtomNode::BOOLEAN : {
        if (!execution_result_matters) {
            setExecFlagNONE(this);
            this->popContext();
            return NULL;
        }
        if (node->lit_obj != NULL) {
            setExecFlagNONE(this);
            this->popContext();
            return node->lit_obj;
        }
        it = this->readonly_literals.find(node->token->nameid);
        if (it != this->readonly_literals.end()) {
            setExecFlagNONE(this);
            this->popContext();
            return it->second;
        }
        auto lit = Builtin::makeBooleanInstanceObject(node->bool_value, this);
        this->gc->hold(lit);
        lit->can_modify                              = false;
        this->readonly_literals[node->token->nameid] = lit;

        setExecFlagNONE(this);
        this->popContext();
        return node->lit_obj = lit;
    }
    case AtomNode::CHARACTER : {
        if (!execution_result_matters) {
            setExecFlagNONE(this);
            this->popContext();
            return NULL;
        }
        if (node->lit_obj != NULL) {
            setExecFlagNONE(this);
            this->popContext();
            return node->lit_obj;
        }
        it = this->readonly_literals.find(node->token->nameid);
        if (it != this->readonly_literals.end()) {
            setExecFlagNONE(this);
            this->popContext();
            return node->lit_obj = it->second;
            ;
        }
        auto lit = Builtin::makeCharacterInstanceObject(node->char_value, this);
        this->gc->hold(lit);
        lit->can_modify                              = false;
        this->readonly_literals[node->token->nameid] = lit;

        setExecFlagNONE(this);
        this->popContext();
        return node->lit_obj = lit;
    }
    case AtomNode::INTEGER : {
        if (!execution_result_matters) {
            setExecFlagNONE(this);
            this->popContext();
            return NULL;
        }
        if (node->lit_obj != NULL) {
            setExecFlagNONE(this);
            this->popContext();
            return node->lit_obj;
        }
        it = this->readonly_literals.find(node->token->nameid);
        if (it != this->readonly_literals.end()) {
            setExecFlagNONE(this);
            this->popContext();
            return node->lit_obj = it->second;
            ;
        }
        auto lit = Builtin::makeIntegerInstanceObject(node->int_value, this);
        this->gc->hold(lit);
        lit->can_modify                              = false;
        this->readonly_literals[node->token->nameid] = lit;

        setExecFlagNONE(this);
        this->popContext();
        return node->lit_obj = lit;
    }
    case AtomNode::REAL : {
        if (!execution_result_matters) {
            setExecFlagNONE(this);

            this->popContext();
            return NULL;
        }
        if (node->lit_obj != NULL) {
            setExecFlagNONE(this);
            this->popContext();
            return node->lit_obj;
        }
        it = this->readonly_literals.find(node->token->nameid);
        if (it != this->readonly_literals.end()) {
            setExecFlagNONE(this);
            this->popContext();
            return node->lit_obj = it->second;
            ;
        }
        auto lit = Builtin::makeRealInstanceObject(node->real_value, this);
        this->gc->hold(lit);
        lit->can_modify                              = false;
        this->readonly_literals[node->token->nameid] = lit;

        setExecFlagNONE(this);
        this->popContext();
        return node->lit_obj = lit;
    }
    case AtomNode::STRING : {
        if (!execution_result_matters) {
            setExecFlagNONE(this);
            this->popContext();
            return NULL;
        }
        if (node->lit_obj != NULL) {
            setExecFlagNONE(this);
            this->popContext();
            return node->lit_obj;
        }
        it = this->readonly_literals.find(node->token->nameid);
        if (it != this->readonly_literals.end()) {
            setExecFlagNONE(this);
            this->popContext();
            return node->lit_obj = it->second;
            ;
        }
        auto lit = Builtin::makeStringInstanceObject(node->string_value, this);
        this->gc->hold(lit);
        lit->can_modify                              = false;
        this->readonly_literals[node->token->nameid] = lit;

        setExecFlagNONE(this);
        this->popContext();
        return node->lit_obj = lit;
    }
    case AtomNode::NOTHING : {
        if (!execution_result_matters) {
            setExecFlagNONE(this);
            this->popContext();
            return NULL;
        }
        if (node->lit_obj != NULL) {
            setExecFlagNONE(this);
            this->popContext();
            return node->lit_obj;
        }
        it = this->readonly_literals.find(node->token->nameid);
        if (it != this->readonly_literals.end()) {
            setExecFlagNONE(this);
            this->popContext();
            return node->lit_obj = it->second;
            ;
        }
        auto lit = Builtin::makeNothingInstanceObject(this);
        this->gc->hold(lit);
        lit->can_modify                              = false;
        this->readonly_literals[node->token->nameid] = lit;

        setExecFlagNONE(this);
        this->popContext();
        return node->lit_obj = lit;
    }
    case AtomNode::IDENTIFIER : {
        setExecFlagNONE(this);
        auto res = this->scope->getVariable(node->token->nameid, this);
        this->popContext();
        return res;
    }
    default : this->signalError("Unknown atom", this->getContext().area);
    }
}

Object *Runtime::execute(ParExprNode *node, bool execution_result_matters) {
    ProfilerCAPTURE();
    if (node == NULL) {
        this->signalError("Failed to execute unknown AST node", this->getContext().area);
    }
    return this->execute(node->expr, execution_result_matters);
}

Object *Runtime::execute(StmtNode *node, bool execution_result_matters) {
    ProfilerCAPTURE();
    if (node == NULL) {
        this->signalError("Failed to execute unknown AST node", this->getContext().area);
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
        this->signalError("Unknown node", node->text_area);
    }
    }
}

Object *Runtime::execute(WhileStmtNode *node, bool execution_result_matters) {
    ProfilerCAPTURE();
    if (node == NULL) {
        this->signalError("Failed to execute unknown AST node", this->getContext().area);
    }
    this->newContext();
    while (true) {
        this->getContext().area = node->text_area;
        this->newFrame();

        if (node->cond != NULL) {
            this->getContext().area = node->cond->text_area;
            auto cond               = this->execute(node->cond, true);

            if (!Builtin::getBooleanValue(cond, this)) {
                this->popFrame();
                break;
            }
        }

        if (node->body != NULL) {
            this->getContext().area = node->body->text_area;
            auto body               = this->execute(node->body, execution_result_matters);
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
                    this->popContext();
                    return body;
                }
                setExecFlagRETURN(this);
                this->popFrame();
                this->popContext();
                return (execution_result_matters) ? this->copy(body) : NULL;
            };
        }
        this->popFrame();
    }
    setExecFlagNONE(this);
    this->popContext();
    return this->protected_nothing;
}

Object *Runtime::execute(ForStmtNode *node, bool execution_result_matters) {
    ProfilerCAPTURE();
    if (node == NULL) {
        this->signalError("Failed to execute unknown AST node", this->getContext().area);
    }

    if (node->init != NULL) {
        this->execute(node->init, false);
    }
    this->newContext();

    while (true) {
        this->getContext().area = node->text_area;
        this->newFrame();

        if (node->cond != NULL) {
            this->getContext().area = node->cond->text_area;
            auto cond               = this->execute(node->cond, true);

            if (!Builtin::getBooleanValue(cond, this)) {
                this->popFrame();
                break;
            }
        }

        if (node->body != NULL) {
            this->getContext().area = node->body->text_area;
            auto body               = this->execute(node->body, execution_result_matters);
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
                    this->popContext();
                    return body;
                }
                setExecFlagRETURN(this);
                this->popFrame();
                this->popContext();
                return (execution_result_matters) ? this->copy(body) : NULL;
            };
        }

        if (node->step != NULL) {
            this->getContext().area = node->step->text_area;
            this->execute(node->step, false);
        }
        this->popFrame();
    }
    setExecFlagNONE(this);
    this->popContext();
    return this->protected_nothing;
}

Object *Runtime::execute(IfStmtNode *node, bool execution_result_matters) {
    ProfilerCAPTURE();
    if (node == NULL) {
        this->signalError("Failed to execute unknown AST node", this->getContext().area);
    }

    this->newContext();

    this->getContext().area = node->cond->text_area;
    auto cond               = this->execute(node->cond, true);

    if (Builtin::getBooleanValue(cond, this)) {
        this->popContext();
        return this->execute(node->body, execution_result_matters);
    }
    else if (node->else_body != NULL) {
        this->popContext();
        return this->execute(node->else_body, execution_result_matters);
    }
    setExecFlagNONE(this);
    return this->protected_nothing;
}

Object *Runtime::execute(ReturnStmtNode *node, bool execution_result_matters) {
    ProfilerCAPTURE();
    if (node == NULL) {
        this->signalError("Failed to execute unknown AST node", this->getContext().area);
    }

    this->newContext();
    this->getContext().area = node->text_area;

    auto res = this->execute(node->value, execution_result_matters);
    if (isExecFlagDIRECT_PASS(this)) {
        setExecFlagRETURN(this);
        this->popContext();
        return res;
    }
    setExecFlagRETURN(this);
    this->popContext();
    return (execution_result_matters) ? this->copy(res) : NULL;
}

Object *Runtime::execute(BlockStmtNode *node, bool execution_result_matters) {
    ProfilerCAPTURE();
    if (node == NULL) {
        this->signalError("Failed to execute unknown AST node", this->getContext().area);
    }
    this->newContext();
    this->getContext().area = node->text_area;
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
            this->popContext();
            return res;
        }
    }
    if (!node->is_unscoped) {
        this->popFrame();
    }
    setExecFlagNONE(this);
    this->popContext();
    return (res != NULL) ? res : this->protected_nothing;
}

bool Runtime::isValidObject(Object *obj) {
    return obj != NULL && obj->type != NULL;
}

bool Runtime::isTypeObject(Object *obj, Type *type) {
    if (type == NULL) {
        return obj != NULL && obj->type != NULL && obj->instance == NULL;
    }
    return obj != NULL && obj->type == type && obj->instance == NULL;
}

bool Runtime::isInstanceObject(Object *obj, Type *type) {
    if (type == NULL) {
        return obj != NULL && obj->type != NULL && obj->instance != NULL;
    }
    return obj != NULL && obj->type == type && obj->instance != NULL;
}

bool Runtime::isOfType(Object *obj, BuiltinTypes type) {
    if (obj == NULL) {
        return false;
    }
    switch (type) {
    case NOTHING_TYPE_ID   : return obj->type == this->nothing_type;
    case BOOLEAN_TYPE_ID   : return obj->type == this->boolean_type;
    case FUNCTION_TYPE_ID  : return obj->type == this->function_type;
    case INTEGER_TYPE_ID   : return obj->type == this->integer_type;
    case REAL_TYPE_ID      : return obj->type == this->real_type;
    case CHARACTER_TYPE_ID : return obj->type == this->character_type;
    case STRING_TYPE_ID    : return obj->type == this->string_type;
    case ARRAY_TYPE_ID     : return obj->type == this->array_type;
    default                : return false;
    }
}

bool Runtime::isOfType(Object *obj, Type *type) {
    return obj != NULL && obj->type == type;
}

void Runtime::verifyIsValidObject(Object *obj) {
    if (!this->isValidObject(obj)) {
        this->signalError("Not a valid object: " + obj->userRepr(), this->getContext().area);
    }
}

void Runtime::verifyIsValidObject(Object *obj, const TextArea &ta) {
    if (!this->isValidObject(obj)) {
        this->signalError("Not a valid object: " + obj->userRepr(), ta);
    }
}

void Runtime::verifyIsTypeObject(Object *obj, Type *type) {
    if (!this->isTypeObject(obj, type)) {
        if (type == NULL) {
            this->signalError("Not a type object: " + obj->userRepr(), this->getContext().area);
        }
        else {
            this->signalError("Not a type object of type " + type->userRepr() + ": " + obj->userRepr(),
                              this->getContext().area);
        }
    }
}

void Runtime::verifyIsTypeObject(Object *obj, Type *type, const TextArea &ta) {
    if (!this->isTypeObject(obj, type)) {
        if (type == NULL) {
            this->signalError("Not a type object: " + obj->userRepr(), ta);
        }
        else {
            this->signalError("Not a type object of type " + type->userRepr() + ": " + obj->userRepr(), ta);
        }
    }
}

void Runtime::verifyIsInstanceObject(Object *obj, Type *type) {
    if (!this->isInstanceObject(obj, type)) {
        if (type == NULL) {
            this->signalError("Not an instance object: " + obj->userRepr(), this->getContext().area);
        }
        else {
            this->signalError("Not an instance object of type " + type->userRepr() + ": " + obj->userRepr(),
                              this->getContext().area);
        }
    }
}

void Runtime::verifyIsInstanceObject(Object *obj, Type *type, const TextArea &ta) {
    if (!this->isInstanceObject(obj, type)) {
        if (type == NULL) {
            this->signalError("Not an instance object: " + obj->userRepr(), ta);
        }
        else {
            this->signalError("Not an instance object of type " + type->userRepr() + ": " + obj->userRepr(), ta);
        }
    }
}

void Runtime::verifyIsOfType(Object *obj, Runtime::BuiltinTypes type) {
    this->verifyIsValidObject(obj);
    switch (type) {
    case NOTHING_TYPE_ID :
        if (obj->type != this->nothing_type) {
            this->signalError(obj->userRepr() + " is not of type " + this->nothing_type->userRepr(),
                              this->getContext().area);
        }
        break;
    case BOOLEAN_TYPE_ID :
        if (obj->type != this->boolean_type) {
            this->signalError(obj->userRepr() + " is not of type " + this->nothing_type->userRepr(),
                              this->getContext().area);
        }
        break;
    case FUNCTION_TYPE_ID :
        if (obj->type != this->function_type) {
            this->signalError(obj->userRepr() + " is not of type " + this->nothing_type->userRepr(),
                              this->getContext().area);
        }
        break;
    case INTEGER_TYPE_ID :
        if (obj->type != this->integer_type) {
            this->signalError(obj->userRepr() + " is not of type " + this->nothing_type->userRepr(),
                              this->getContext().area);
        }
        break;
    case REAL_TYPE_ID :
        if (obj->type != this->real_type) {
            this->signalError(obj->userRepr() + " is not of type " + this->nothing_type->userRepr(),
                              this->getContext().area);
        }
        break;
    case CHARACTER_TYPE_ID :
        if (obj->type != this->character_type) {
            this->signalError(obj->userRepr() + " is not of type " + this->nothing_type->userRepr(),
                              this->getContext().area);
        }
        break;
    case STRING_TYPE_ID :
        if (obj->type != this->string_type) {
            this->signalError(obj->userRepr() + " is not of type " + this->nothing_type->userRepr(),
                              this->getContext().area);
        }
        break;
    case ARRAY_TYPE_ID :
        if (obj->type != this->array_type) {
            this->signalError(obj->userRepr() + " is not of type " + this->nothing_type->userRepr(),
                              this->getContext().area);
        }
        break;
    default :
        this->signalError(obj->userRepr() + " is not of type ?UNKNOWN_TYPE?", this->getContext().area);
        break;
        ;
    }
}

void Runtime::verifyIsOfType(Object *obj, Runtime::BuiltinTypes type, const TextArea &ta) {
    this->verifyIsValidObject(obj, ta);
    switch (type) {
    case NOTHING_TYPE_ID :
        if (obj->type != this->nothing_type) {
            this->signalError(obj->userRepr() + " is not of type " + this->nothing_type->userRepr(), ta);
        }
        break;
    case BOOLEAN_TYPE_ID :
        if (obj->type != this->boolean_type) {
            this->signalError(obj->userRepr() + " is not of type " + this->nothing_type->userRepr(), ta);
        }
        break;
    case FUNCTION_TYPE_ID :
        if (obj->type != this->function_type) {
            this->signalError(obj->userRepr() + " is not of type " + this->nothing_type->userRepr(), ta);
        }
        break;
    case INTEGER_TYPE_ID :
        if (obj->type != this->integer_type) {
            this->signalError(obj->userRepr() + " is not of type " + this->nothing_type->userRepr(), ta);
        }
        break;
    case REAL_TYPE_ID :
        if (obj->type != this->real_type) {
            this->signalError(obj->userRepr() + " is not of type " + this->nothing_type->userRepr(), ta);
        }
        break;
    case CHARACTER_TYPE_ID :
        if (obj->type != this->character_type) {
            this->signalError(obj->userRepr() + " is not of type " + this->nothing_type->userRepr(), ta);
        }
        break;
    case STRING_TYPE_ID :
        if (obj->type != this->string_type) {
            this->signalError(obj->userRepr() + " is not of type " + this->nothing_type->userRepr(), ta);
        }
        break;
    case ARRAY_TYPE_ID :
        if (obj->type != this->array_type) {
            this->signalError(obj->userRepr() + " is not of type " + this->nothing_type->userRepr(), ta);
        }
        break;
    default :
        this->signalError(obj->userRepr() + " is not of type ?UNKNOWN_TYPE?", ta);
        break;
        ;
    }
}

void Runtime::verifyIsOfType(Object *obj, Type *type) {
    if (!this->isOfType(obj, type)) {
        this->signalError(obj->userRepr() + " is not of type " + type->userRepr(), this->getContext().area);
    }
}

void Runtime::verifyIsOfType(Object *obj, Type *type, const TextArea &ta) {
    if (!this->isOfType(obj, type)) {
        this->signalError(obj->userRepr() + " is not of type " + type->userRepr(), ta);
    }
}

void Runtime::verifyMinArgsAmountFunc(const std::vector<Object *> &args, int64_t amount) {
    if (args.size() < amount) {
        this->signalError("Expected at least " + std::to_string(amount) + " arguments, got "
                          + std::to_string(args.size()),
                          this->getContext().area);
    }
}

void Runtime::verifyMinArgsAmountFunc(const std::vector<Object *> &args, int64_t amount, const TextArea &ta) {
    if (args.size() < amount) {
        this->signalError("Expected at least " + std::to_string(amount) + " arguments, got "
                          + std::to_string(args.size()),
                          ta);
    }
}

void Runtime::verifyExactArgsAmountFunc(const std::vector<Object *> &args, int64_t amount) {
    if (args.size() != amount) {
        this->signalError("Expected exactly " + std::to_string(amount) + " arguments, got "
                          + std::to_string(args.size()),
                          this->getContext().area);
    }
}

void Runtime::verifyExactArgsAmountFunc(const std::vector<Object *> &args, int64_t amount, const TextArea &ta) {
    if (args.size() != amount) {
        this->signalError("Expected exactly " + std::to_string(amount) + " arguments, got "
                          + std::to_string(args.size()),
                          ta);
    }
}

void Runtime::verifyMinArgsAmountMethod(const std::vector<Object *> &args, int64_t amount) {
    if (args.size() < amount + 1) {
        this->signalError("Expected at least " + std::to_string(amount) + " arguments, got "
                          + std::to_string((int64_t)args.size() - 1),
                          (args.empty())
                          ? this->getContext().area
                          : TextArea(this->getContext().sub_areas.front(), this->getContext().sub_areas.back()));
    }
}

void Runtime::verifyMinArgsAmountMethod(const std::vector<Object *> &args, int64_t amount, const TextArea &ta) {
    if (args.size() < amount + 1) {
        this->signalError("Expected at least " + std::to_string(amount) + " arguments, got "
                          + std::to_string((int64_t)args.size() - 1),
                          ta);
    }
}

void Runtime::verifyExactArgsAmountMethod(const std::vector<Object *> &args, int64_t amount) {
    if (args.size() != amount + 1) {
        this->signalError("Expected exactly " + std::to_string(amount) + " arguments, got "
                          + std::to_string((int64_t)args.size() - 1),
                          this->getContext().area);
    }
}

void Runtime::verifyExactArgsAmountMethod(const std::vector<Object *> &args, int64_t amount, const TextArea &ta) {
    if (args.size() != amount + 1) {
        this->signalError("Expected exactly " + std::to_string(amount) + " arguments, got "
                          + std::to_string((int64_t)args.size() - 1),
                          ta);
    }
}

void Runtime::verifyHasMethod(Object *obj, int64_t id) {
    this->verifyIsInstanceObject(obj, NULL);
    if (!obj->type->hasMethod(id)) {
        this->signalError(obj->userRepr() + " doesn't have method " + NameId::userRepr(id),
                          this->getContext().area);
    }
}

void Runtime::verifyHasMethod(Object *obj, int64_t id, const TextArea &ta) {
    this->verifyIsValidObject(obj, ta);
    if (!obj->type->hasMethod(id)) {
        this->signalError(obj->userRepr() + " doesn't have method " + NameId::userRepr(id), ta);
    }
}

namespace MagicMethods {

    int64_t mm__make__() {
        static NameId res("__make__");
        return res.id;
    }

    int64_t mm__copy__() {
        static NameId res("__copy__");
        return res.id;
    }

    int64_t mm__bool__() {
        static NameId res("__bool__");
        return res.id;
    }

    int64_t mm__char__() {
        static NameId res("__char__");
        return res.id;
    }

    int64_t mm__int__() {
        static NameId res("__int__");
        return res.id;
    }

    int64_t mm__real__() {
        static NameId res("__real__");
        return res.id;
    }

    int64_t mm__string__() {
        static NameId res("__string__");
        return res.id;
    }

    int64_t mm__repr__() {
        static NameId res("__repr__");
        return res.id;
    }

    int64_t mm__read__() {
        static NameId res("__read__");
        return res.id;
    }
}    // namespace MagicMethods

}    // namespace Cotton
