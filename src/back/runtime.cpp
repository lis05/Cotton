#include "runtime.h"
#include "../front/parser.h"
#include "gc.h"
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
    this->error_messages.push_back({message, this->current_token});
}

Object *Runtime::make(Type *type) {
    if (type == NULL) {
        this->signalSubError("Failed to make given type because it is absent");
        return NULL;
    }
    auto obj = type->create(this);
    if (obj == NULL) {
        this->signalSubError("Failed to make an object of given type");
        return NULL;
    }
    if (!obj->on_stack) {
        this->gc->track(obj, this);
    }
    if (obj->type->hasMethod(NameIds::__MAKE__())) {
        this->runMethod(NameIds::__MAKE__(), obj, {});
    }
    return obj;
}

Object *Runtime::runOperator(OperatorNode::OperatorId id, Object *obj, std::vector<Object *> args) {
    if (!this->validate(obj)) {
        return NULL;
    }
    if (!this->validate(obj->type)) {
        return NULL;
    }
    return obj->type->getOperator(id)->operator()(obj->type, obj, args, this);
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

Object *Runtime::copy(Object *obj) {
    if (!validate(obj)) {
        return NULL;
    }
    auto cpy = obj->copy(this);
    if (cpy == NULL) {
        this->signalSubError("Failed to copy object");
        return NULL;
    }
    if (cpy->on_stack) {
        this->gc->track(cpy, this);
    }
    return cpy;
}
}    // namespace Cotton
