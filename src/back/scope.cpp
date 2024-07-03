#include "scope.h"
#include "nameid.h"
#include "runtime.h"

namespace Cotton {
Scope::Scope(Scope *prev, bool can_access_prev) {
    this->prev            = prev;
    this->can_access_prev = can_access_prev;
}

Scope::~Scope() {
    this->prev            = NULL;
    this->can_access_prev = false;
    this->variables.clear();
}

void Scope::addVariable(int64_t id, Object *obj, Runtime *rt) {
    this->variables[id] = obj;
}

Object *Scope::getVariable(int64_t id, Runtime *rt) {
    Scope *s = this;
    while (s != NULL) {
        auto it = s->variables.find(id);
        if (it != s->variables.end()) {
            return it->second;
        }
        if (s->can_access_prev) {
            s = s->prev;
        }
        else {
            break;
        }
    }
    rt->signalError("Failed to find variable " + NameId::shortRepr(id));
}

bool Scope::queryVariable(int64_t id, Runtime *rt) {
    return this->getVariable(id, rt) != NULL;
}

}    // namespace Cotton
