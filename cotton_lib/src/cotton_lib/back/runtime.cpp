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
Runtime::Runtime(GCStrategy *gc_strategy, ErrorManager *error_manager, NamesManager *nmgr)
    : nmgr(nmgr) {
    ProfilerCAPTURE();

    this->scope         = new Scope(nullptr, nullptr, false);
    this->scope->master = this->scope;
    this->gc            = new GC(gc_strategy);
    this->gc->rt        = this;
    this->error_manager = error_manager;
    this->newContext();

    this->builtin_types.function       = new Builtin::FunctionType(this);
    this->builtin_types.nothing        = new Builtin::NothingType(this);
    this->builtin_types.boolean        = new Builtin::BooleanType(this);
    this->builtin_types.integer        = new Builtin::IntegerType(this);
    this->builtin_types.real           = new Builtin::RealType(this);
    this->builtin_types.character      = new Builtin::CharacterType(this);
    this->builtin_types.string         = new Builtin::StringType(this);
    this->builtin_types.array          = new Builtin::ArrayType(this);
    this->builtin_types.array_iterator = new Builtin::ArrayIteratorType(this);

    auto nothing_obj        = this->make(this->builtin_types.nothing, Runtime::TYPE_OBJECT);
    nothing_obj->can_modify = false;
    this->scope->addVariable(this->nmgr->getId("Nothing"), nothing_obj, this);
    this->registerTypeObject(this->builtin_types.nothing, nothing_obj);

    auto function_obj        = this->make(this->builtin_types.function, Runtime::TYPE_OBJECT);
    function_obj->can_modify = false;
    this->scope->addVariable(this->nmgr->getId("Function"), function_obj, this);
    this->registerTypeObject(this->builtin_types.function, function_obj);

    auto boolean_obj        = this->make(this->builtin_types.boolean, Runtime::TYPE_OBJECT);
    boolean_obj->can_modify = false;
    this->scope->addVariable(this->nmgr->getId("Boolean"), boolean_obj, this);
    this->registerTypeObject(this->builtin_types.boolean, boolean_obj);

    auto integer_obj        = this->make(this->builtin_types.integer, Runtime::TYPE_OBJECT);
    integer_obj->can_modify = false;
    this->scope->addVariable(this->nmgr->getId("Integer"), integer_obj, this);
    this->registerTypeObject(this->builtin_types.integer, integer_obj);

    auto real_obj        = this->make(this->builtin_types.real, Runtime::TYPE_OBJECT);
    real_obj->can_modify = false;
    this->scope->addVariable(this->nmgr->getId("Real"), real_obj, this);
    this->registerTypeObject(this->builtin_types.real, real_obj);

    auto character_obj        = this->make(this->builtin_types.character, Runtime::TYPE_OBJECT);
    character_obj->can_modify = false;
    this->scope->addVariable(this->nmgr->getId("Character"), character_obj, this);
    this->registerTypeObject(this->builtin_types.character, character_obj);

    auto string_obj        = this->make(this->builtin_types.string, Runtime::TYPE_OBJECT);
    string_obj->can_modify = false;
    this->scope->addVariable(this->nmgr->getId("String"), string_obj, this);
    this->registerTypeObject(this->builtin_types.string, string_obj);

    auto array_obj        = this->make(this->builtin_types.array, Runtime::TYPE_OBJECT);
    array_obj->can_modify = false;
    this->scope->addVariable(this->nmgr->getId("Array"), array_obj, this);
    this->registerTypeObject(this->builtin_types.array, array_obj);

    auto array_iterator_obj        = this->make(this->builtin_types.array_iterator, Runtime::TYPE_OBJECT);
    array_iterator_obj->can_modify = false;
    this->scope->addVariable(this->nmgr->getId("ArrayIterator"), array_obj, this);
    this->registerTypeObject(this->builtin_types.array_iterator, array_obj);

    Builtin::installBooleanMethods(this->builtin_types.boolean, this);
    Builtin::installCharacterMethods(this->builtin_types.character, this);
    Builtin::installFunctionMethods(this->builtin_types.function, this);
    Builtin::installIntegerMethods(this->builtin_types.integer, this);
    Builtin::installRealMethods(this->builtin_types.real, this);
    Builtin::installNothingMethods(this->builtin_types.nothing, this);
    Builtin::installStringMethods(this->builtin_types.string, this);
    Builtin::installArrayMethods(this->builtin_types.array, this);
    Builtin::installArrayIteratorMethods(this->builtin_types.array_iterator, this);

    Builtin::installBuiltinFunctions(this);

    this->protected_nothing             = this->make(this->builtin_types.nothing, Runtime::INSTANCE_OBJECT);
    this->protected_nothing->can_modify = false;
    this->gc->hold(this->protected_nothing);
    this->protected_nothing->spreadMultiUse();

    this->protected_true = this->make(this->builtin_types.boolean, Runtime::INSTANCE_OBJECT);
    Builtin::getBooleanValue(this->protected_true, this) = true;
    this->protected_true->can_modify                     = false;
    this->gc->hold(this->protected_true);
    this->protected_true->spreadMultiUse();

    this->protected_false = this->make(this->builtin_types.boolean, Runtime::INSTANCE_OBJECT);
    Builtin::getBooleanValue(this->protected_false, this) = false;
    this->protected_false->can_modify                     = false;
    this->gc->hold(this->protected_false);
    this->protected_true->spreadMultiUse();
}

bool Runtime::checkGlobal(NameId id) {
    auto it = this->globals.find(id);
    return it != this->globals.end();
}

Object *Runtime::getGlobal(NameId id) {
    ProfilerCAPTURE();
    auto it = this->globals.find(id);
    if (it == this->globals.end()) {
        this->signalError("Global not found: " + this->nmgr->getString(id), this->getContext().area);
    }
    return it->second;
}

void Runtime::setGlobal(NameId id, Object *obj) {
    ProfilerCAPTURE();
    this->globals[id] = obj;
}

void Runtime::removeGlobal(NameId id) {
    ProfilerCAPTURE();
    this->globals.erase(id);
}

void Runtime::registerTypeObject(Type *type, Object *obj) {
    ProfilerCAPTURE();
    this->registered_type_objects[type] = obj;
}

Object *Runtime::getTypeObject(Type *type) {
    ProfilerCAPTURE();
    auto it = this->registered_type_objects.find(type);
    if (it != this->registered_type_objects.end()) {
        return it->second;
    }
    return this->protected_nothing;
}

Object *Runtime::protectedNothing() {
    return this->protected_nothing;
}

Object *Runtime::protectedBoolean(bool val) {
    ProfilerCAPTURE();
    return val ? this->protected_true : this->protected_false;
}

void Runtime::newScopeFrame(bool can_access_prev_scope) {
    ProfilerCAPTURE();
    auto scope  = new Scope(this->scope, this->scope->master, can_access_prev_scope);
    this->scope = scope;
}

void Runtime::popScopeFrame() {
    ProfilerCAPTURE();
    auto scope = this->scope->prev;
    delete this->scope;
    this->scope = scope;
}

Object *Runtime::make(Type *type, ObjectOptions object_opt) {
    ProfilerCAPTURE();
    if (type == nullptr) {
        this->signalError("Failed to make an object of nullptr type", this->getContext().area);
    }
    Object *obj = nullptr;
    if (object_opt == Runtime::INSTANCE_OBJECT) {
        obj = type->create(this);
    }
    else {
        obj = new Object(false, nullptr, type, this);
    }

    if (obj == nullptr) {
        this->signalError("Failed to make an object of type " + type->userRepr(this), this->getContext().area);
    }
    obj->spreadSingleUse();
    return obj;
}

Object *Runtime::copy(Object *obj) {
    ProfilerCAPTURE();
    if (!this->isValidObject(obj)) {
        this->signalError("Failed to copy non type object " + obj->userRepr(this), this->getContext().area);
    }
    if (obj->single_use) {
        return obj;
    }
    auto res = obj->type->copy(obj, this);
    return res;
}

Object *Runtime::runOperator(OperatorNode::OperatorId id, Object *obj, bool execution_result_matters) {
    ProfilerCAPTURE();

    this->verifyIsValidObject(obj, Runtime::SUB0_CTX);

    UnaryOperatorAdapter op;

    switch (id) {
    case OperatorNode::POST_PLUS_PLUS :
        op = obj->type->postinc_op;
        if (op == nullptr) {
            this->signalError(obj->userRepr(this) + " doesn't support that operator", this->getContext().area);
        }
        return op(obj, this, execution_result_matters);
    case OperatorNode::POST_MINUS_MINUS :
        op = obj->type->postdec_op;
        if (op == nullptr) {
            this->signalError(obj->userRepr(this) + " doesn't support that operator", this->getContext().area);
        }
        return op(obj, this, execution_result_matters);
    case OperatorNode::PRE_PLUS_PLUS :
        op = obj->type->preinc_op;
        if (op == nullptr) {
            this->signalError(obj->userRepr(this) + " doesn't support that operator", this->getContext().area);
        }
        return op(obj, this, execution_result_matters);
    case OperatorNode::PRE_MINUS_MINUS :
        op = obj->type->predec_op;
        if (op == nullptr) {
            this->signalError(obj->userRepr(this) + " doesn't support that operator", this->getContext().area);
        }
        return op(obj, this, execution_result_matters);
    case OperatorNode::PRE_PLUS :
        op = obj->type->positive_op;
        if (op == nullptr) {
            this->signalError(obj->userRepr(this) + " doesn't support that operator", this->getContext().area);
        }
        return op(obj, this, execution_result_matters);
    case OperatorNode::PRE_MINUS :
        op = obj->type->negative_op;
        if (op == nullptr) {
            this->signalError(obj->userRepr(this) + " doesn't support that operator", this->getContext().area);
        }
        return op(obj, this, execution_result_matters);
    case OperatorNode::NOT :
        op = obj->type->not_op;
        if (op == nullptr) {
            this->signalError(obj->userRepr(this) + " doesn't support that operator", this->getContext().area);
        }
        return op(obj, this, execution_result_matters);
    case OperatorNode::INVERSE :
        op = obj->type->inverse_op;
        if (op == nullptr) {
            this->signalError(obj->userRepr(this) + " doesn't support that operator", this->getContext().area);
        }
        return op(obj, this, execution_result_matters);
    default : this->signalError(obj->userRepr(this) + " doesn't support that operator", this->getContext().area);
    }
}

Object *
Runtime::runOperator(OperatorNode::OperatorId id, Object *obj, Object *arg, bool execution_result_matters) {
    ProfilerCAPTURE();
    this->verifyIsValidObject(obj, Runtime::SUB0_CTX);
    this->verifyIsValidObject(arg, Runtime::SUB1_CTX);

    BinaryOperatorAdapter op;

    switch (id) {
    case OperatorNode::MULT :
        op = obj->type->mult_op;
        if (op == nullptr) {
            this->signalError("Left argument " + obj->userRepr(this) + " doesn't support that operator",
                              this->getContext().area);
        }
        return op(obj, arg, this, execution_result_matters);
    case OperatorNode::DIV :
        op = obj->type->div_op;
        if (op == nullptr) {
            this->signalError("Left argument " + obj->userRepr(this) + " doesn't support that operator",
                              this->getContext().area);
        }

        return op(obj, arg, this, execution_result_matters);
    case OperatorNode::REM :
        op = obj->type->rem_op;
        if (op == nullptr) {
            this->signalError("Left argument " + obj->userRepr(this) + " doesn't support that operator",
                              this->getContext().area);
        }

        return op(obj, arg, this, execution_result_matters);
    case OperatorNode::RIGHT_SHIFT :
        op = obj->type->rshift_op;
        if (op == nullptr) {
            this->signalError("Left argument " + obj->userRepr(this) + " doesn't support that operator",
                              this->getContext().area);
        }

        return op(obj, arg, this, execution_result_matters);
    case OperatorNode::LEFT_SHIFT :
        op = obj->type->lshift_op;
        if (op == nullptr) {
            this->signalError("Left argument " + obj->userRepr(this) + " doesn't support that operator",
                              this->getContext().area);
        }

        return op(obj, arg, this, execution_result_matters);
    case OperatorNode::PLUS :
        op = obj->type->add_op;
        if (op == nullptr) {
            this->signalError("Left argument " + obj->userRepr(this) + " doesn't support that operator",
                              this->getContext().area);
        }

        return op(obj, arg, this, execution_result_matters);
    case OperatorNode::MINUS :
        op = obj->type->sub_op;
        if (op == nullptr) {
            this->signalError("Left argument " + obj->userRepr(this) + " doesn't support that operator",
                              this->getContext().area);
        }

        return op(obj, arg, this, execution_result_matters);
    case OperatorNode::LESS :
        op = obj->type->lt_op;
        if (op == nullptr) {
            this->signalError("Left argument " + obj->userRepr(this) + " doesn't support that operator",
                              this->getContext().area);
        }

        return op(obj, arg, this, execution_result_matters);
    case OperatorNode::LESS_EQUAL :
        op = obj->type->leq_op;
        if (op == nullptr) {
            this->signalError("Left argument " + obj->userRepr(this) + " doesn't support that operator",
                              this->getContext().area);
        }

        return op(obj, arg, this, execution_result_matters);
    case OperatorNode::GREATER :
        op = obj->type->gt_op;
        if (op == nullptr) {
            this->signalError("Left argument " + obj->userRepr(this) + " doesn't support that operator",
                              this->getContext().area);
        }

        return op(obj, arg, this, execution_result_matters);
    case OperatorNode::GREATER_EQUAL :
        op = obj->type->geq_op;
        if (op == nullptr) {
            this->signalError("Left argument " + obj->userRepr(this) + " doesn't support that operator",
                              this->getContext().area);
        }

        return op(obj, arg, this, execution_result_matters);
    case OperatorNode::EQUAL :
        op = obj->type->eq_op;
        if (op == nullptr) {
            this->signalError("Left argument " + obj->userRepr(this) + " doesn't support that operator",
                              this->getContext().area);
        }

        return op(obj, arg, this, execution_result_matters);
    case OperatorNode::NOT_EQUAL :
        op = obj->type->neq_op;
        if (op == nullptr) {
            this->signalError("Left argument " + obj->userRepr(this) + " doesn't support that operator",
                              this->getContext().area);
        }

        return op(obj, arg, this, execution_result_matters);
    case OperatorNode::BITAND :
        op = obj->type->bitand_op;
        if (op == nullptr) {
            this->signalError("Left argument " + obj->userRepr(this) + " doesn't support that operator",
                              this->getContext().area);
        }

        return op(obj, arg, this, execution_result_matters);
    case OperatorNode::BITXOR :
        op = obj->type->bitxor_op;
        if (op == nullptr) {
            this->signalError("Left argument " + obj->userRepr(this) + " doesn't support that operator",
                              this->getContext().area);
        }

        return op(obj, arg, this, execution_result_matters);
    case OperatorNode::BITOR :
        op = obj->type->bitor_op;
        if (op == nullptr) {
            this->signalError("Left argument " + obj->userRepr(this) + " doesn't support that operator",
                              this->getContext().area);
        }

        return op(obj, arg, this, execution_result_matters);
    case OperatorNode::AND :
        op = obj->type->and_op;
        if (op == nullptr) {
            this->signalError("Left argument " + obj->userRepr(this) + " doesn't support that operator",
                              this->getContext().area);
        }

        return op(obj, arg, this, execution_result_matters);
    case OperatorNode::OR :
        op = obj->type->or_op;
        if (op == nullptr) {
            this->signalError("Left argument " + obj->userRepr(this) + " doesn't support that operator",
                              this->getContext().area);
        }

        return op(obj, arg, this, execution_result_matters);
    default :
        this->signalError("Left argument " + obj->userRepr(this) + " doesn't support that operator",
                          this->getContext().area);
    }
}

Object *Runtime::runOperator(OperatorNode::OperatorId     id,
                             Object                      *obj,
                             const std::vector<Object *> &args,
                             bool                         execution_result_matters) {
    ProfilerCAPTURE();
    this->verifyIsValidObject(obj, Runtime::SUB1_CTX);

    if (id == OperatorNode::CALL) {
        auto op = obj->type->call_op;
        if (op == nullptr) {
            this->signalError("Left argument " + obj->userRepr(this) + " doesn't support that operator",
                              this->getContext().area);
        }

        return op(obj, args, this, execution_result_matters);
    }
    else if (id == OperatorNode::INDEX) {
        auto op = obj->type->index_op;
        if (op == nullptr) {
            this->signalError("Left argument " + obj->userRepr(this) + " doesn't support that operator",
                              this->getContext().area);
        }

        return op(obj, args, this, execution_result_matters);
    }
    else {
        this->signalError("Left argument " + obj->userRepr(this) + " doesn't support that operator",
                          this->getContext().area);
    }
}

Object *
Runtime::runMethod(NameId id, Object *obj, const std::vector<Object *> &args, bool execution_result_matters) {
    ProfilerCAPTURE();
    auto method = obj->type->getMethod(id, this);
    return this->runOperator(OperatorNode::CALL, method, args, execution_result_matters);
}

void Runtime::newContext() {
    ProfilerCAPTURE();
    this->error_contexts.push_back({});
}

void Runtime::popContext() {
    ProfilerCAPTURE();
    this->error_contexts.pop_back();
}

Runtime::ErrorContext &Runtime::getContext() {
    ProfilerCAPTURE();
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
    if (node == nullptr) {
        this->signalError("Failed to execute nullptr AST node", this->getContext().area);
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
    if (node == nullptr) {
        this->signalError("Failed to execute unknown AST node", this->getContext().area);
    }
    this->newContext();
    this->getContext().area = node->text_area;
    // highlight(this, node->function_token);
    auto func               = Builtin::makeFunctionInstanceObject(false, nullptr, node, this);
    if (node->name != nullptr) {
        this->scope->addVariable(node->name->nameid, func, this);
    }
    this->popContext();
    this->clearExecFlags();
    return func;
}

Object *Runtime::execute(TypeDefNode *node, bool execution_result_matters) {
    ProfilerCAPTURE();
    if (node == nullptr) {
        this->signalError("Failed to execute unknown AST node", this->getContext().area);
    }
    this->newContext();
    this->getContext().area = node->text_area;
    auto type               = new Builtin::RecordType(this);
    type->nameid            = node->name->nameid;
    for (auto f : node->fields) {
        type->instance_fields.push_back(this->nmgr->getId(f->data));
    }

    for (auto method : node->methods) {
        this->newContext();
        this->getContext().area = method->text_area;
        auto f                  = Builtin::makeFunctionInstanceObject(false, nullptr, method, this);
        type->addMethod(method->name->nameid, f);
        this->popContext();
    }

    auto res = this->make(type, Runtime::TYPE_OBJECT);
    this->scope->master->addVariable(type->nameid, res, this);
    this->popContext();

    this->clearExecFlags();
    return res;
}

static std::vector<Object *> getList(ExprNode *expr, Runtime *rt) {
    ProfilerCAPTURE();
    std::vector<Object *> res;
    while (expr != nullptr) {
        if (expr->id == ExprNode::OPERATOR && expr->op->id == OperatorNode::COMMA) {
            auto r = rt->execute(expr->op->first, true);
            if (rt->isExecFlagDIRECT_PASS()) {
                res.push_back(r);
            }
            else {
                rt->newContext();
                rt->getContext().area = expr->op->first->text_area;
                res.push_back(rt->copy(r));
                rt->popContext();
            }
            rt->getGC()->hold(res.back());
            expr = expr->op->second;
        }
        else {
            auto r = rt->execute(expr, true);
            if (rt->isExecFlagDIRECT_PASS()) {
                res.push_back(r);
            }
            else {
                rt->newContext();
                rt->getContext().area = expr->text_area;
                res.push_back(rt->copy(r));
                rt->popContext();
            }
            rt->getGC()->hold(res.back());
            break;
        }
    }
    return res;
}

static void getList_addToContext(ExprNode *expr, Runtime *rt) {
    ProfilerCAPTURE();
    while (expr != nullptr) {
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
    if (node == nullptr) {
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
        auto res = this->execute(node->first, execution_result_matters);
        this->gc->hold(res);
        auto expr = node->second;
        while (expr != nullptr) {
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
        this->gc->release(res);
        return res;
    }
    else if (node->id == OperatorNode::CALL || node->id == OperatorNode::INDEX) {
        if (node->id == OperatorNode::CALL && node->first->id == ExprNode::OPERATOR
            && node->first->op->id == OperatorNode::DOT)
        {
            auto caller = this->execute(node->first->op->first, true);
            this->gc->hold(caller);
            Object *selected = nullptr;

            auto dot = node->first->op;
            if (dot->second == nullptr || dot->second->id != ExprNode::ATOM
                || dot->second->atom->id != AtomNode::IDENTIFIER)
            {
                this->signalError("Selector is illegal", dot->second->text_area);
            }
            NameId selector = dot->second->atom->ident->nameid;
            if (!this->isInstanceObject(caller, nullptr)) {
                this->signalError(caller->userRepr(this) + " must be an instance object", dot->first->text_area);
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

            this->clearExecFlags();
            this->popContext();

            for (auto &item : list) {
                this->gc->release(item);
            }
            this->gc->release(caller);
            return res;
        }
        else {
            Object *self = this->execute(node->first, true);
            this->gc->hold(self);
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

            this->clearExecFlags();
            this->popContext();
            for (auto &item : list) {
                this->gc->release(item);
            }
            this->gc->release(self);
            return res;
        }
    }

    Object *self = this->execute(node->first, true);
    this->gc->hold(self);
    Object *other = nullptr;

    switch (node->id) {
    case OperatorNode::DOT : {
        if (node->second == nullptr || node->second->id != ExprNode::ATOM
            || node->second->atom->id != AtomNode::IDENTIFIER)
        {
            this->signalError("Selector is illegal", node->second->text_area);
        }
        NameId selector = node->second->atom->ident->nameid;
        if (!isInstanceObject(self)) {
            this->signalError(self->userRepr(this) + " must be an instance object", node->first->text_area);
        }
        if (self->instance->hasField(selector, this)) {
            auto res = self->instance->selectField(selector, this);

            this->clearExecFlags();
            this->popContext();
            this->gc->release(self);
            return res;
        }
        else if (self->type->hasMethod(selector)) {
            auto res = self->type->getMethod(selector, this);

            this->clearExecFlags();
            this->popContext();
            this->gc->release(self);
            return res;
        }
        else {
            this->signalError("Invalid selector", node->second->text_area);
        }
    }
    case OperatorNode::AT : {
        this->clearExecFlags();
        this->setExecFlagDIRECT_PASS();
        this->popContext();
        this->gc->release(self);
        return self;
    }
    case OperatorNode::ASSIGN : {
        other = this->execute(node->second, true);
        if (this->isExecFlagDIRECT_PASS()) {
            self->assignTo(other, this);
        }
        else {
            self->assignToCopyOf(other, this);
        }

        this->clearExecFlags();
        this->popContext();
        this->gc->release(self);
        return self;
    }
    case OperatorNode::PLUS_ASSIGN : {
        other = this->execute(node->second, true);
        this->getContext().sub_areas.push_back(node->first->text_area);
        this->getContext().sub_areas.push_back(node->second->text_area);
        self->assignToCopyOf(this->runOperator(OperatorNode::PLUS, self, other, true), this);

        this->clearExecFlags();
        this->popContext();
        this->gc->release(self);
        return self;
    }
    case OperatorNode::MINUS_ASSIGN : {
        other = this->execute(node->second, true);
        this->getContext().sub_areas.push_back(node->first->text_area);
        this->getContext().sub_areas.push_back(node->second->text_area);
        self->assignToCopyOf(this->runOperator(OperatorNode::MINUS, self, other, true), this);

        this->clearExecFlags();
        this->popContext();
        this->gc->release(self);
        return self;
    }
    case OperatorNode::MULT_ASSIGN : {
        other = this->execute(node->second, true);
        this->getContext().sub_areas.push_back(node->first->text_area);
        this->getContext().sub_areas.push_back(node->second->text_area);
        self->assignToCopyOf(this->runOperator(OperatorNode::MULT, self, other, true), this);

        this->clearExecFlags();
        this->popContext();
        this->gc->release(self);
        return self;
    }
    case OperatorNode::DIV_ASSIGN : {
        other = this->execute(node->second, true);
        this->getContext().sub_areas.push_back(node->first->text_area);
        this->getContext().sub_areas.push_back(node->second->text_area);
        self->assignToCopyOf(this->runOperator(OperatorNode::DIV, self, other, true), this);

        this->clearExecFlags();
        this->popContext();
        this->gc->release(self);
        return self;
    }
    case OperatorNode::REM_ASSIGN : {
        other = this->execute(node->second, true);
        this->getContext().sub_areas.push_back(node->first->text_area);
        this->getContext().sub_areas.push_back(node->second->text_area);
        self->assignToCopyOf(this->runOperator(OperatorNode::REM, self, other, true), this);

        this->clearExecFlags();
        this->popContext();
        this->gc->release(self);
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
        this->clearExecFlags();
        this->popContext();
        this->gc->release(self);
        return res;
    }

    auto arg = this->execute(node->second, true);
    this->getContext().sub_areas.push_back(node->first->text_area);
    this->getContext().sub_areas.push_back(node->second->text_area);
    auto res = this->runOperator(node->id, self, arg, execution_result_matters);
    this->clearExecFlags();
    this->popContext();
    this->gc->release(self);
    return res;
}

Object *Runtime::execute(AtomNode *node, bool execution_result_matters) {
    ProfilerCAPTURE();
    if (node == nullptr) {
        this->signalError("Failed to execute unknown AST node", this->getContext().area);
    }
    this->newContext();
    this->getContext().area = node->text_area;
    HashTable<NameId, Object *>::point_iterator it;

    switch (node->id) {
    case AtomNode::BOOLEAN : {
        if (!execution_result_matters) {
            this->clearExecFlags();
            this->popContext();
            return nullptr;
        }
        if (node->lit_obj != nullptr) {
            this->clearExecFlags();
            this->popContext();
            return node->lit_obj;
        }
        it = this->readonly_literals.find(node->token->nameid);
        if (it != this->readonly_literals.end()) {
            this->clearExecFlags();
            this->popContext();
            return it->second;
        }
        auto lit = Builtin::makeBooleanInstanceObject(node->bool_value, this);
        this->gc->hold(lit);
        lit->can_modify                              = false;
        this->readonly_literals[node->token->nameid] = lit;
        lit->spreadMultiUse();

        this->clearExecFlags();
        this->popContext();
        return node->lit_obj = lit;
    }
    case AtomNode::CHARACTER : {
        if (!execution_result_matters) {
            this->clearExecFlags();
            this->popContext();
            return nullptr;
        }
        if (node->lit_obj != nullptr) {
            this->clearExecFlags();
            this->popContext();
            return node->lit_obj;
        }
        it = this->readonly_literals.find(node->token->nameid);
        if (it != this->readonly_literals.end()) {
            this->clearExecFlags();
            this->popContext();
            return node->lit_obj = it->second;
            ;
        }
        auto lit = Builtin::makeCharacterInstanceObject(node->char_value, this);
        this->gc->hold(lit);
        lit->can_modify                              = false;
        this->readonly_literals[node->token->nameid] = lit;
        lit->spreadMultiUse();

        this->clearExecFlags();
        this->popContext();
        return node->lit_obj = lit;
    }
    case AtomNode::INTEGER : {
        if (!execution_result_matters) {
            this->clearExecFlags();
            this->popContext();
            return nullptr;
        }
        if (node->lit_obj != nullptr) {
            this->clearExecFlags();
            this->popContext();
            return node->lit_obj;
        }
        it = this->readonly_literals.find(node->token->nameid);
        if (it != this->readonly_literals.end()) {
            this->clearExecFlags();
            this->popContext();
            return node->lit_obj = it->second;
            ;
        }
        auto lit = Builtin::makeIntegerInstanceObject(node->int_value, this);
        this->gc->hold(lit);
        lit->can_modify                              = false;
        this->readonly_literals[node->token->nameid] = lit;
        lit->spreadMultiUse();

        this->clearExecFlags();
        this->popContext();
        return node->lit_obj = lit;
    }
    case AtomNode::REAL : {
        if (!execution_result_matters) {
            this->clearExecFlags();

            this->popContext();
            return nullptr;
        }
        if (node->lit_obj != nullptr) {
            this->clearExecFlags();
            this->popContext();
            return node->lit_obj;
        }
        it = this->readonly_literals.find(node->token->nameid);
        if (it != this->readonly_literals.end()) {
            this->clearExecFlags();
            this->popContext();
            return node->lit_obj = it->second;
        }
        auto lit = Builtin::makeRealInstanceObject(node->real_value, this);
        this->gc->hold(lit);
        lit->can_modify                              = false;
        this->readonly_literals[node->token->nameid] = lit;
        lit->spreadMultiUse();

        this->clearExecFlags();
        this->popContext();
        return node->lit_obj = lit;
    }
    case AtomNode::STRING : {
        if (!execution_result_matters) {
            this->clearExecFlags();
            this->popContext();
            return nullptr;
        }
        if (node->lit_obj != nullptr) {
            this->clearExecFlags();
            this->popContext();
            return node->lit_obj;
        }
        it = this->readonly_literals.find(node->token->nameid);
        if (it != this->readonly_literals.end()) {
            this->clearExecFlags();
            this->popContext();
            return node->lit_obj = it->second;
            ;
        }
        auto lit = Builtin::makeStringInstanceObject(node->string_value, this);
        this->gc->hold(lit);
        lit->can_modify                              = false;
        this->readonly_literals[node->token->nameid] = lit;
        lit->spreadMultiUse();

        this->clearExecFlags();
        this->popContext();
        return node->lit_obj = lit;
    }
    case AtomNode::NOTHING : {
        if (!execution_result_matters) {
            this->clearExecFlags();
            this->popContext();
            return nullptr;
        }
        if (node->lit_obj != nullptr) {
            this->clearExecFlags();
            this->popContext();
            return node->lit_obj;
        }
        it = this->readonly_literals.find(node->token->nameid);
        if (it != this->readonly_literals.end()) {
            this->clearExecFlags();
            this->popContext();
            return node->lit_obj = it->second;
            ;
        }
        auto lit = Builtin::makeNothingInstanceObject(this);
        this->gc->hold(lit);
        lit->can_modify                              = false;
        this->readonly_literals[node->token->nameid] = lit;
        lit->spreadMultiUse();

        this->clearExecFlags();
        this->popContext();
        return node->lit_obj = lit;
    }
    case AtomNode::IDENTIFIER : {
        this->clearExecFlags();
        auto res = this->scope->getVariable(node->token->nameid, this);
        this->popContext();
        return res;
    }
    default : this->signalError("Unknown atom", this->getContext().area);
    }
}

Object *Runtime::execute(ParExprNode *node, bool execution_result_matters) {
    ProfilerCAPTURE();
    if (node == nullptr) {
        this->signalError("Failed to execute unknown AST node", this->getContext().area);
    }
    return this->execute(node->expr, execution_result_matters);
}

Object *Runtime::execute(StmtNode *node, bool execution_result_matters) {
    ProfilerCAPTURE();
    if (node == nullptr) {
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
    case StmtNode::FOR_IN : {
        return this->execute(node->for_in_stmt, execution_result_matters);
    }
    case StmtNode::IF : {
        return this->execute(node->if_stmt, execution_result_matters);
    }
    case StmtNode::CONTINUE : {
        this->clearExecFlags();
        this->setExecFlagCONTINUE();
        return this->protected_nothing;
    }
    case StmtNode::BREAK : {
        this->clearExecFlags();
        this->setExecFlagBREAK();
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
    if (node == nullptr) {
        this->signalError("Failed to execute unknown AST node", this->getContext().area);
    }
    this->newContext();
    while (true) {
        this->getContext().area = node->text_area;
        this->newScopeFrame();

        if (node->cond != nullptr) {
            this->getContext().area = node->cond->text_area;
            auto cond               = this->execute(node->cond, true);

            if (!Builtin::getBooleanValue(cond, this)) {
                this->popScopeFrame();
                break;
            }
        }

        if (node->body != nullptr) {
            this->getContext().area = node->body->text_area;
            auto body               = this->execute(node->body, execution_result_matters);
            if (this->isExecFlagBREAK()) {
                this->popScopeFrame();
                break;
            }
            else if (this->isExecFlagCONTINUE()) {
                this->popScopeFrame();
                continue;
            }
            else if (this->isExecFlagRETURN()) {
                if (this->isExecFlagDIRECT_PASS()) {
                    this->clearExecFlags();
                    this->setExecFlagRETURN();
                    this->popScopeFrame();
                    this->popContext();
                    return body;
                }
                this->clearExecFlags();
                this->setExecFlagRETURN();
                this->popScopeFrame();
                this->popContext();
                return (execution_result_matters) ? this->copy(body) : nullptr;
            };
        }
        this->popScopeFrame();
    }
    this->clearExecFlags();
    this->popContext();
    return this->protected_nothing;
}

Object *Runtime::execute(ForStmtNode *node, bool execution_result_matters) {
    ProfilerCAPTURE();
    if (node == nullptr) {
        this->signalError("Failed to execute unknown AST node", this->getContext().area);
    }

    this->newScopeFrame();
    if (node->init != nullptr) {
        this->execute(node->init, false);
    }
    this->newContext();
    bool first_cycle = true;
    while (true) {
        if (!first_cycle) {
            if (node->step != nullptr) {
                this->getContext().area = node->step->text_area;
                this->execute(node->step, false);
            }
        }
        if (first_cycle) {
            first_cycle = false;
        }
        this->getContext().area = node->text_area;
        this->newScopeFrame();

        if (node->cond != nullptr) {
            this->getContext().area = node->cond->text_area;
            auto cond               = this->execute(node->cond, true);

            if (!Builtin::getBooleanValue(cond, this)) {
                this->popScopeFrame();
                break;
            }
        }

        if (node->body != nullptr) {
            this->getContext().area = node->body->text_area;
            auto body               = this->execute(node->body, execution_result_matters);
            if (this->isExecFlagBREAK()) {
                this->popScopeFrame();
                break;
            }
            else if (this->isExecFlagCONTINUE()) {
                this->popScopeFrame();
                continue;
            }
            else if (this->isExecFlagRETURN()) {
                if (this->isExecFlagDIRECT_PASS()) {
                    this->clearExecFlags();
                    this->setExecFlagRETURN();
                    this->popScopeFrame();
                    this->popScopeFrame();
                    this->popContext();
                    return body;
                }
                this->clearExecFlags();
                this->setExecFlagRETURN();
                this->popScopeFrame();
                this->popScopeFrame();
                this->popContext();
                return (execution_result_matters) ? this->copy(body) : nullptr;
            };
        }
        this->popScopeFrame();
    }
    this->clearExecFlags();
    this->popContext();
    this->popScopeFrame();
    return this->protected_nothing;
}

Object *Runtime::execute(ForInStmtNode *node, bool execution_result_matters) {
    ProfilerCAPTURE();
    if (node == nullptr) {
        this->signalError("Failed to execute unknown AST node", this->getContext().area);
    }

    this->newScopeFrame();
    auto iterable = this->execute(node->iterable, true);
    this->gc->hold(iterable);

    this->newContext();
    this->getContext().area = node->iterable->text_area;
    this->getContext().sub_areas.push_back(node->iterable->text_area);
    this->getContext().sub_areas.push_back(node->iterable->text_area);
    auto iterator     = this->runMethod(this->nmgr->getId("begin_iterator"), iterable, {iterable}, true);
    auto end_iterator = this->runMethod(this->nmgr->getId("end_iterator"), iterable, {iterable}, true);
    this->gc->hold(iterator);
    this->gc->hold(end_iterator);

    bool first_cycle = true;
    while (true) {
        this->getContext().area         = TextArea(*node->placeholder);
        this->getContext().sub_areas[0] = TextArea(*node->placeholder);
        this->getContext().sub_areas[1] = TextArea(*node->placeholder);
        if (!first_cycle) {
            auto new_iterator = this->runMethod(this->nmgr->getId("next_iterator"), iterator, {iterator}, true);
            this->gc->release(iterator);
            this->gc->release(end_iterator);
            this->gc->hold(new_iterator);
            iterator = new_iterator;
        }
        if (first_cycle) {
            first_cycle = false;
        }
        {
            auto res = this->runMethod(this->nmgr->getId("equal"), iterator, {iterator, end_iterator}, true);
            this->verifyIsInstanceObject(res, this->builtin_types.boolean, Runtime::AREA_CTX);
            if (getBooleanValueFast(res)) {
                break;    // womp womp
            }
        }
        auto item = this->runMethod(this->nmgr->getId("get_item"), iterator, {iterator}, true);
        this->scope->addVariable(node->placeholder->nameid, item, this);

        this->getContext().area = node->text_area;
        this->newScopeFrame();

        if (node->body != nullptr) {
            this->getContext().area = node->body->text_area;
            auto body               = this->execute(node->body, execution_result_matters);
            if (this->isExecFlagBREAK()) {
                this->popScopeFrame();
                break;
            }
            else if (this->isExecFlagCONTINUE()) {
                this->popScopeFrame();
                continue;
            }
            else if (this->isExecFlagRETURN()) {
                if (this->isExecFlagDIRECT_PASS()) {
                    this->clearExecFlags();
                    this->setExecFlagRETURN();
                    this->popScopeFrame();
                    this->popScopeFrame();
                    this->popContext();
                    this->gc->release(iterator);
                    this->gc->release(end_iterator);
                    this->gc->release(iterable);
                    return body;
                }
                this->clearExecFlags();
                this->setExecFlagRETURN();
                this->popScopeFrame();
                this->popScopeFrame();
                this->popContext();
                this->gc->release(iterator);
                this->gc->release(end_iterator);
                this->gc->release(iterable);
                return (execution_result_matters) ? this->copy(body) : nullptr;
            };
        }
        this->popScopeFrame();
    }
    this->clearExecFlags();
    this->popContext();
    this->popScopeFrame();
    this->gc->release(iterator);
    this->gc->release(end_iterator);
    this->gc->release(iterable);
    return this->protected_nothing;
}

Object *Runtime::execute(IfStmtNode *node, bool execution_result_matters) {
    ProfilerCAPTURE();
    if (node == nullptr) {
        this->signalError("Failed to execute unknown AST node", this->getContext().area);
    }

    this->newContext();

    this->getContext().area = node->cond->text_area;
    auto cond               = this->execute(node->cond, true);

    if (Builtin::getBooleanValue(cond, this)) {
        this->popContext();
        return this->execute(node->body, execution_result_matters);
    }
    else if (node->else_body != nullptr) {
        this->popContext();
        return this->execute(node->else_body, execution_result_matters);
    }
    this->clearExecFlags();
    this->popContext();
    return this->protected_nothing;
}

Object *Runtime::execute(ReturnStmtNode *node, bool execution_result_matters) {
    ProfilerCAPTURE();
    if (node == nullptr) {
        this->signalError("Failed to execute unknown AST node", this->getContext().area);
    }

    this->newContext();
    this->getContext().area = node->text_area;

    if (node->value == NULL) {
        this->clearExecFlags();
        this->setExecFlagRETURN();
        this->popContext();
        return this->protectedNothing();
    }

    auto res = this->execute(node->value, execution_result_matters);
    if (this->isExecFlagDIRECT_PASS()) {
        this->clearExecFlags();
        this->setExecFlagRETURN();
        this->popContext();
        return res;
    }
    this->clearExecFlags();
    this->setExecFlagRETURN();
    this->popContext();
    return (execution_result_matters) ? this->copy(res) : nullptr;
}

Object *Runtime::execute(BlockStmtNode *node, bool execution_result_matters) {
    ProfilerCAPTURE();
    if (node == nullptr) {
        this->signalError("Failed to execute unknown AST node", this->getContext().area);
    }
    this->newContext();
    this->getContext().area = node->text_area;
    if (!node->is_unscoped) {
        this->newScopeFrame(true);
    }
    Object *res = nullptr;
    for (auto stmt : node->list) {
        if (stmt == nullptr) {
            continue;
        }
        res = this->execute(stmt, execution_result_matters);
        if (!this->isExecFlagNONE()) {
            if (!node->is_unscoped) {
                this->popScopeFrame();
            }
            this->popContext();
            return res;
        }
    }
    if (!node->is_unscoped) {
        this->popScopeFrame();
    }
    this->clearExecFlags();
    this->popContext();
    return (res != nullptr) ? res : this->protected_nothing;
}

bool Runtime::isValidObject(Object *obj) {
    ProfilerCAPTURE();
    return obj != nullptr && obj->type != nullptr;
}

bool Runtime::isTypeObject(Object *obj, Type *type) {
    ProfilerCAPTURE();
    if (type == nullptr) {
        return obj != nullptr && obj->type != nullptr && obj->instance == nullptr;
    }
    return obj != nullptr && obj->type == type && obj->instance == nullptr;
}

bool Runtime::isInstanceObject(Object *obj, Type *type) {
    ProfilerCAPTURE();
    if (type == nullptr) {
        return obj != nullptr && obj->type != nullptr && obj->instance != nullptr;
    }
    return obj != nullptr && obj->type == type && obj->instance != nullptr;
}

bool Runtime::isOfType(Object *obj, Type *type) {
    ProfilerCAPTURE();
    return obj != nullptr && obj->type == type;
}

TextArea &Runtime::getTextArea(ContextId ctx_id) {
    if (ctx_id == Runtime::AREA_CTX) {
        return this->getContext().area;
    }
    return this->getContext().sub_areas[(int)ctx_id];
}

void Runtime::verifyIsValidObject(Object *obj, Runtime::ContextId ctx_id) {
    ProfilerCAPTURE();

    if (!this->isValidObject(obj)) {
        this->signalError("Not a valid object: " + obj->userRepr(this), this->getTextArea(ctx_id));
    }
}

void Runtime::verifyIsTypeObject(Object *obj, Type *type, Runtime::ContextId ctx_id) {
    ProfilerCAPTURE();

    if (!this->isTypeObject(obj, type)) {
        if (type == nullptr) {
            this->signalError("Not a type object: " + obj->userRepr(this), this->getTextArea(ctx_id));
        }
        else {
            this->signalError("Not a type object of type " + type->userRepr(this) + ": " + obj->userRepr(this),
                              this->getTextArea(ctx_id));
        }
    }
}

void Runtime::verifyIsInstanceObject(Object *obj, Type *type, Runtime::ContextId ctx_id) {
    ProfilerCAPTURE();

    if (!this->isInstanceObject(obj, type)) {
        if (type == nullptr) {
            this->signalError("Not an instance object: " + obj->userRepr(this), this->getTextArea(ctx_id));
        }
        else {
            this->signalError("Not an instance object of type " + type->userRepr(this) + ": "
                              + obj->userRepr(this),
                              this->getTextArea(ctx_id));
        }
    }
}

void Runtime::verifyIsOfType(Object *obj, Type *type, Runtime::ContextId ctx_id) {
    ProfilerCAPTURE();

    if (!this->isOfType(obj, type)) {
        this->signalError(obj->userRepr(this) + " is not of type " + type->userRepr(this),
                          this->getTextArea(ctx_id));
    }
}

void Runtime::verifyMinArgsAmountFunc(const std::vector<Object *> &args,
                                      int64_t                      amount,
                                      Runtime::ContextId           ctx_id) {
    ProfilerCAPTURE();

    if (args.size() < amount) {
        this->signalError("Expected at least " + std::to_string(amount) + " arguments, got "
                          + std::to_string(args.size()),
                          this->getTextArea(ctx_id));
    }
}

void Runtime::verifyExactArgsAmountFunc(const std::vector<Object *> &args,
                                        int64_t                      amount,
                                        Runtime::ContextId           ctx_id) {
    ProfilerCAPTURE();

    if (args.size() != amount) {
        this->signalError("Expected exactly " + std::to_string(amount) + " arguments, got "
                          + std::to_string(args.size()),
                          this->getTextArea(ctx_id));
    }
}

void Runtime::verifyMinArgsAmountMethod(const std::vector<Object *> &args,
                                        int64_t                      amount,
                                        Runtime::ContextId           ctx_id) {
    ProfilerCAPTURE();

    if (args.size() < amount + 1) {
        this->signalError("Expected at least " + std::to_string(amount) + " arguments, got "
                          + std::to_string((int64_t)args.size() - 1),
                          this->getTextArea(ctx_id));
    }
}

void Runtime::verifyExactArgsAmountMethod(const std::vector<Object *> &args,
                                          int64_t                      amount,
                                          Runtime::ContextId           ctx_id) {
    ProfilerCAPTURE();

    if (args.size() != amount + 1) {
        this->signalError("Expected exactly " + std::to_string(amount) + " arguments, got "
                          + std::to_string((int64_t)args.size() - 1),
                          this->getTextArea(ctx_id));
    }
}

void Runtime::verifyHasMethod(Object *obj, NameId id, Runtime::ContextId ctx_id) {
    ProfilerCAPTURE();

    this->verifyIsValidObject(obj, ctx_id);
    if (!obj->type->hasMethod(id)) {
        this->signalError(obj->userRepr(this) + " doesn't have method " + this->nmgr->getString(id),
                          this->getTextArea(ctx_id));
    }
}

GC *Runtime::getGC() {
    return this->gc;
}

ErrorManager *Runtime::getErrorManager() {
    return this->error_manager;
}

Scope *Runtime::getScope() {
    return this->scope;
}

void Runtime::clearExecFlags() {
    this->execution_flags = ExecutionFlags::NONE;
}

void Runtime::setExecFlagCONTINUE() {
    this->execution_flags |= ExecutionFlags::CONTINUE;
}

void Runtime::setExecFlagBREAK() {
    this->execution_flags |= ExecutionFlags::BREAK;
}

void Runtime::setExecFlagRETURN() {
    this->execution_flags |= ExecutionFlags::RETURN;
}

void Runtime::setExecFlagDIRECT_PASS() {
    this->execution_flags |= ExecutionFlags::DIRECT_PASS;
}

bool Runtime::isExecFlagNONE() {
    return this->execution_flags == ExecutionFlags::NONE;
}

bool Runtime::isExecFlagCONTINUE() {
    return this->execution_flags & ExecutionFlags::CONTINUE;
}

bool Runtime::isExecFlagBREAK() {
    return this->execution_flags & ExecutionFlags::BREAK;
}

bool Runtime::isExecFlagRETURN() {
    return this->execution_flags & ExecutionFlags::RETURN;
}

bool Runtime::isExecFlagDIRECT_PASS() {
    return this->execution_flags & ExecutionFlags::DIRECT_PASS;
}

namespace MagicMethods {
    NameId mm__make__(Runtime *rt) {
        ProfilerCAPTURE();
        return rt->nmgr->getId("__make__");
    }

    NameId mm__copy__(Runtime *rt) {
        ProfilerCAPTURE();
        return rt->nmgr->getId("__copy__");
    }

    NameId mm__bool__(Runtime *rt) {
        ProfilerCAPTURE();
        return rt->nmgr->getId("__bool__");
    }

    NameId mm__char__(Runtime *rt) {
        ProfilerCAPTURE();
        return rt->nmgr->getId("__char__");
    }

    NameId mm__int__(Runtime *rt) {
        ProfilerCAPTURE();
        return rt->nmgr->getId("__int__");
    }

    NameId mm__real__(Runtime *rt) {
        ProfilerCAPTURE();
        return rt->nmgr->getId("__real__");
    }

    NameId mm__string__(Runtime *rt) {
        ProfilerCAPTURE();
        return rt->nmgr->getId("__string__");
    }

    NameId mm__repr__(Runtime *rt) {
        ProfilerCAPTURE();
        return rt->nmgr->getId("__repr__");
    }

    NameId mm__read__(Runtime *rt) {
        ProfilerCAPTURE();
        return rt->nmgr->getId("__read__");
    }

    NameId mm__get_iterator__(Runtime *rt) {
        ProfilerCAPTURE();
        return rt->nmgr->getId("__get_iterator__");
    }

    NameId mm__deref_iterator__(Runtime *rt) {
        ProfilerCAPTURE();
        return rt->nmgr->getId("__deref_iterator__");
    }

    NameId mm__next_iterator__(Runtime *rt) {
        ProfilerCAPTURE();
        return rt->nmgr->getId("__next_iterator__");
    }

    NameId mm__is_last_iterator__(Runtime *rt) {
        ProfilerCAPTURE();
        return rt->nmgr->getId("__is_last_iterator__");
    }
}    // namespace MagicMethods

}    // namespace Cotton
