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

    this->nothing_type  = new Builtin::NothingType(this);
    this->function_type = new Builtin::FunctionType(this);
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
    this->runMethod(MagicMethods::__make__(), obj, {});
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

bool Runtime::isTypeObject(Object *obj) {
    return obj != NULL && obj->type != NULL;
}

void Runtime::highlight(Token *token) {
    this->current_token = token;
}

void Runtime::signalError(const std::string &message, bool include_token) {
    if (include_token) {
        this->error_manager->signalError(message, *this->current_token);
    }
    else {
        this->error_manager->signalError(message);
    }
}

Object *Runtime::execute(StmtNode *stmt) {}

}    // namespace Cotton
