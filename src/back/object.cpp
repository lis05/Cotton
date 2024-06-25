#include "object.h"
#include "instance.h"
#include "runtime.h"
#include "type.h"
#include <cstdint>

namespace Cotton {
Object::Object(bool is_instance, bool on_stack, Instance *instance, Type *type) {
    this->is_instance = is_instance;
    this->on_stack    = on_stack;
    this->instance    = instance;
    this->type        = type;
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
}    // namespace Cotton
