#include "object.h"
#include "gc.h"
#include "instance.h"
#include "runtime.h"
#include "stack.h"
#include "type.h"
#include <cstdint>

namespace Cotton {
Object::Object(bool is_instance, bool on_stack, Instance *instance, Type *type, Runtime *rt) {
    this->is_instance = is_instance;
    this->on_stack    = on_stack;
    this->instance    = instance;
    this->type        = type;
    this->gc_mark     = !rt->gc->gc_mark;
}

Object::~Object() {
    this->instance = NULL;
    this->type     = NULL;
}

std::vector<Object *> Object::getGCReachable() {
    std::vector<Object *> res;
    if (this->instance != NULL) {
        for (auto &elem : this->instance->getGCReachable()) {
            res.push_back(elem);
        }
    }
    if (this->type != NULL) {
        for (auto &elem : this->type->getGCReachable()) {
            res.push_back(elem);
        }
    }
    return res;
}

Object *Object::copy(Runtime *rt) {
    Instance *ins  = NULL;
    Type     *type = this->type;
    if (this->instance != NULL) {
        ins = this->instance->copy(rt);
    }
    if (this->on_stack) {
        auto res = rt->stack->allocAndInitObject(this->is_instance, ins, type, rt);
        if (res != NULL) {
            return res;
        }
    }
    return new Object(this->is_instance, false, ins, type, rt);
}

std::ostream &operator<<(std::ostream &stream, Object *obj) {
    if (obj == NULL) {
        stream << "{NULL}";
        return stream;
    }
    stream << "{" << (void*)obj << ", ";
    stream << (obj->is_instance ? "I" : "X");
    stream << (obj->on_stack ? "S" : "H");
    stream << (obj->gc_mark ? "1" : "0");
    stream << ", ins: " << obj->instance << " "  << (obj->instance->on_stack ? "S" : "H") <<  ", type: " << obj->type << "}";
    return stream;
}
}    // namespace Cotton
