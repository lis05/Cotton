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
    this->gc_mark     = !rt->getGC()->gc_mark;
    this->id          = ++total_objects;
    this->can_modify  = true;
    this->single_use  = false;
    rt->getGC()->track(this);
}

Object::~Object() {
    ProfilerCAPTURE();
    this->instance = nullptr;
    this->type     = nullptr;
    total_objects--;
}

std::vector<Object *> Object::getGCReachable() {
    ProfilerCAPTURE();
    std::vector<Object *> res;
    if (this->instance != nullptr) {
        for (auto &elem : this->instance->getGCReachable()) {
            res.push_back(elem);
        }
    }
    if (this->type != nullptr) {
        for (auto &elem : this->type->getGCReachable()) {
            res.push_back(elem);
        }
    }
    return res;
}

std::string Object::userRepr(Runtime *rt) {
    ProfilerCAPTURE();
    if (this == nullptr) {
        return std::string("Object(nullptr)");
    }
    if (this->instance == nullptr) {
        return this->type->userRepr(rt);
    }
    return this->instance->userRepr(rt);
}

void Object::assignTo(Object *obj, Runtime *rt) {
    ProfilerCAPTURE();
    if (!this->can_modify) {
        rt->signalError("Cannot assign to " + this->userRepr(rt), rt->getContext().area);
    }
    auto id  = this->id;
    *this    = *obj;
    this->id = id;
    this->spreadMultiUse();
}

void Object::assignToCopyOf(Object *obj, Runtime *rt) {
    ProfilerCAPTURE();
    if (!this->can_modify) {
        rt->signalError("Cannot assign to " + this->userRepr(rt), rt->getContext().area);
    }
    auto id  = this->id;
    *this    = *rt->copy(obj);
    this->id = id;
    this->spreadMultiUse();
}

void Object::spreadSingleUse() {
    ProfilerCAPTURE();
    this->single_use = true;
    if (this->instance != nullptr) {
        this->instance->spreadSingleUse();
    }
}

void Object::spreadMultiUse() {
    ProfilerCAPTURE();
    this->single_use = false;
    if (this->instance != nullptr) {
        this->instance->spreadMultiUse();
    }
}

std::ostream &operator<<(std::ostream &stream, Object *obj) {
    ProfilerCAPTURE();
    if (obj == nullptr) {
        stream << "{nullptr}";
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
