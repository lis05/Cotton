#include "object.h"
#include "gc.h"
#include "instance.h"
#include "nameid.h"
#include "runtime.h"
#include "stack.h"
#include "type.h"
#include <cstdint>

namespace Cotton {
int64_t Object::total_objects = 0;

Object::Object(bool is_instance, bool on_stack, Instance *instance, Type *type, Runtime *rt) {
    this->is_instance = is_instance;
    this->on_stack    = on_stack;
    this->instance    = instance;
    this->type        = type;
    this->gc_mark     = !rt->gc->gc_mark;
    this->id          = ++total_objects;

    if (!on_stack) {
        rt->gc->track(this, rt);
    }
}

Object::~Object() {
    this->instance = NULL;
    this->type     = NULL;
    total_objects--;
}

std::vector<Object *> Object::getGCReachable(Runtime *rt) {
    std::vector<Object *> res;
    if (this->instance != NULL) {
        for (auto &elem : this->instance->getGCReachable(rt)) {
            res.push_back(elem);
        }
    }
    if (this->type != NULL) {
        for (auto &elem : this->type->getGCReachable(rt)) {
            res.push_back(elem);
        }
    }
    return res;
}

std::string Object::shortRepr() {
    if (this == NULL) {
        return std::string("Object(NULL)");
    }
    std::string res  = "Object(id = " + std::to_string(this->id) + ", flags = ";
    res             += (this->is_instance) ? "I" : "T";
    res             += (this->on_stack) ? "S" : "H";
    res             += (this->gc_mark) ? "1" : "0";
    res             += ", instance = " + ((this->instance == NULL) ? "NULL" : this->instance->shortRepr());
    res             += ", type = " + ((this->type == NULL) ? "NULL" : this->type->shortRepr());
    res             += ")";
    return res;
}

std::ostream &operator<<(std::ostream &stream, Object *obj) {
    if (obj == NULL) {
        stream << "{NULL}";
        return stream;
    }
    stream << "{" << (void *)obj << ", ";
    stream << (obj->is_instance ? "I" : "T");
    stream << (obj->on_stack ? "S" : "H");
    stream << (obj->gc_mark ? "1" : "0");
    stream << ", ins: " << obj->instance << " " << (obj->instance->on_stack ? "S" : "H") << ", type: " << obj->type
           << "}";
    return stream;
}

}    // namespace Cotton
