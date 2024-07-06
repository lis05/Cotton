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

Object::Object(bool is_instance, Instance *instance, Type *type, Runtime *rt, bool can_assign) {
    ProfilerCAPTURE();
    this->rt          = rt;
    this->is_instance = is_instance;
    this->instance    = instance;
    this->type        = type;
    this->gc_mark     = !rt->gc->gc_mark;
    this->id          = ++total_objects;
    this->can_assign  = can_assign;
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

std::string Object::shortRepr() {
    ProfilerCAPTURE();
    if (this == NULL) {
        return std::string("Object(NULL)");
    }
    std::string res  = "Object(id = " + std::to_string(this->id) + ", flags = ";
    res             += (this->is_instance) ? "I" : "T";
    res             += (this->gc_mark) ? "1" : "0";
    res             += (this->can_assign) ? "W" : "R";
    res             += ", instance = " + ((this->instance == NULL) ? "NULL" : this->instance->shortRepr());
    res             += ", type = " + ((this->type == NULL) ? "NULL" : this->type->shortRepr());
    res             += ")";
    return res;
}

void Object::assignTo(Object *obj) {
    ProfilerCAPTURE();
    if (!this->can_assign) {
        rt->signalError("Cannot assign to " + this->shortRepr());
    }
    *this = *obj;
}

void Object::assignToCopyOf(Object *obj) {
    ProfilerCAPTURE();
    if (!this->can_assign) {
        rt->signalError("Cannot assign to " + this->shortRepr());
    }
    if (obj->instance == NULL) {
        this->instance = NULL;
    }
    else {
        this->instance = obj->instance->copy();
    }
    this->type = obj->type;
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
