#include "runtime.h"
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
}

void Runtime::signalSubError(const std::string &message) {
    std::cout << "err: " << message << std::endl;    // temp
    this->error_messages.push_back({message, this->current_token});
}

void Runtime::signalError(const std::string &message) {
    std::cout << "err: " << message << std::endl;    // temp
    this->error_messages.push_back({message, this->current_token});

    // TODO
    this->error_manager->signalError("Complete error, exiting");
}

Object *Runtime::make(Type *type, ObjectOptions object_opt) {
    if (!this->validate(type)) {
        return NULL;
    }
    Object *obj = NULL;
    if (object_opt == Runtime::INSTANCE_OBJECT) {
        obj = type->create(this);
    }
    else {
        obj = new Object(false, false, NULL, type, this);
    }
    if (obj == NULL) {
        this->signalSubError("Failed to make an object");
        return NULL;
    }
    // TODO: call __make__
    return obj;
}

Object *Runtime::runOperator(OperatorNode::OperatorId id, Object *obj, std::vector<Object *> args) {
    if (!this->validate(obj)) {
        return NULL;
    }
    if (!this->validate(obj->type)) {
        return NULL;
    }
    auto op = obj->type->getOperator(id);
    if (op == NULL) {
        this->signalSubError("Failed to run operator");
        return NULL;
    }
    return obj->type->getOperator(id)->operator()(obj, args, this);
}

Object *Runtime::runMethod(int64_t id, Object *obj, std::vector<Object *> args) {
    if (!this->validate(obj)) {
        return NULL;
    }
    if (!this->validate(obj->type)) {
        return NULL;
    }
    auto method = obj->type->getMethod(id);
    return this->runOperator(OperatorNode::CALL, method, args);
}

Runtime::ExecutionResult Runtime::execute(StmtNode *stmt) {
    return ExecutionResult(false, NULL, false);
}

bool Runtime::validate(Object *obj) {
    if (obj == NULL) {
        this->signalSubError("Invalid object (NULL)");
        return false;
    }
    return true;
}

bool Runtime::validate(Instance *ins) {
    if (ins == NULL) {
        this->signalSubError("Invalid instance (NULL)");
        return false;
    }
    return true;
}

bool Runtime::validate(Type *type) {
    if (type == NULL) {
        this->signalSubError("Invalid type (NULL)");
        return false;
    }
    return true;
}

Runtime::ExecutionResult::ExecutionResult(bool success, Object *res, bool directly_passed) {
    this->success         = success;
    this->res             = res;
    this->directly_passed = directly_passed;
}
}    // namespace Cotton
