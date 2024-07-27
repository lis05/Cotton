#include "object.h"
#include "../profiler.h"
#include "gc.h"
#include "instance.h"
#include "nameid.h"
#include "runtime.h"
#include "type.h"
#include <cstdint>

namespace Cotton {
int64_t Object::total_objects = 0;

Object::Object(bool is_instance, Instance *instance, Type *type, Runtime *rt) {
    ProfilerCAPTURE();
    this->is_instance = is_instance;
    this->instance    = instance;
    this->type        = type;
    this->gc_mark     = !rt->gc->gc_mark;
    this->id          = ++total_objects;
    this->can_modify  = true;
    this->single_use  = false;
    rt->gc->track(this);
}

Object::~Object() {
    ProfilerCAPTURE();
    this->instance = NULL;
    this->type     = NULL;
    total_objects--;
}

std::vector<Object *> Object::getGCReachable() {
    ProfilerCAPTURE();
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

std::string Object::userRepr(Runtime *rt) {
    ProfilerCAPTURE();
    if (this == NULL) {
        return std::string("Object(NULL)");
    }
    if (this->instance == NULL) {
        return this->type->userRepr(rt);
    }
    return this->instance->userRepr(rt);
}

void Object::assignTo(Object *obj, Runtime *rt) {
    ProfilerCAPTURE();
    if (!this->can_modify) {
        rt->signalError("Cannot assign to " + this->userRepr(rt), rt->getContext().area);
    }
    auto id          = this->id;
    auto single_use  = this->single_use;
    *this            = *obj;
    this->single_use = single_use;
    this->id         = id;
}

void Object::assignToCopyOf(Object *obj, Runtime *rt) {
    ProfilerCAPTURE();
    if (!this->can_modify) {
        rt->signalError("Cannot assign to " + this->userRepr(rt), rt->getContext().area);
    }
    auto id          = this->id;
    auto single_use  = this->single_use;
    *this            = *rt->copy(obj);
    this->single_use = single_use;
    this->id         = id;
}

void Object::spreadSingleUse() {
    ProfilerCAPTURE();
    this->single_use = true;
    if (this->instance != NULL) {
        this->instance->spreadSingleUse();
    }
}

void Object::spreadMultiUse() {
    ProfilerCAPTURE();
    this->single_use = false;
    if (this->instance != NULL) {
        this->instance->spreadMultiUse();
    }
}

std::ostream &operator<<(std::ostream &stream, Object *obj) {
    ProfilerCAPTURE();
    if (obj == NULL) {
        stream << "{NULL}";
        return stream;
    }
    stream << "{" << (void *)obj << ", ";
    stream << (obj->is_instance ? "I" : "T");
    stream << (obj->gc_mark ? "1" : "0");
    stream << (obj->gc_mark ? "W" : "R");
    stream << ", ins: " << obj->instance << ", type: " << obj->type << "}";
    return stream;
}

}    // namespace Cotton
