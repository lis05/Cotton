#include "scope.h"
#include "../profiler.h"
#include "nameid.h"
#include "runtime.h"

namespace Cotton {
Scope::Scope(Scope *prev, Scope *master, bool can_access_prev) {
    ProfilerCAPTURE();
    this->prev             = prev;
    this->master           = master;
    this->can_access_prev  = can_access_prev;
    this->is_function_call = false;
}

Scope::~Scope() {
    ProfilerCAPTURE();
    this->prev            = nullptr;
    this->master          = nullptr;
    this->can_access_prev = false;
    this->variables.clear();
    this->arguments.clear();
}

Scope *Scope::getPrev() {
    ProfilerCAPTURE();
    return this->prev;
}

Scope *Scope::getMaster() {
    ProfilerCAPTURE();
    return this->master;
}

std::vector<Object *> &Scope::getArguments() {
    ProfilerCAPTURE();
    return this->arguments;
}

bool Scope::canAccessPrev() {
    ProfilerCAPTURE();
    return this->can_access_prev;
}

bool Scope::isFunctionCall() {
    ProfilerCAPTURE();
    return this->is_function_call;
}

void Scope::setCanAccessPrev(bool value) {
    ProfilerCAPTURE();
    this->can_access_prev = value;
}

void Scope::setIsFunctionCall(bool value) {
    ProfilerCAPTURE();
    this->is_function_call = value;
}

void Scope::addVariable(NameId id, Object *obj, Runtime *rt) {
    ProfilerCAPTURE();
    this->variables[id] = obj;
    obj->spreadMultiUse();
}

Object *Scope::getVariable(NameId id, Runtime *rt) {
    ProfilerCAPTURE();
    Scope *s = this;
    while (s != nullptr) {
        auto it = s->variables.find(id);
        if (it != s->variables.end()) {
            return it->second;
        }
        if (s->can_access_prev) {
            s = s->prev;
        }
        else {
            if (s == s->master) {
                break;
            }
            else {
                s = s->master;
            }
        }
    }

    rt->signalError("Failed to find variable " + rt->nmgr->getString(id), rt->getContext().area);
}

void Scope::removeVariable(NameId id, Runtime *rt) {
    ProfilerCAPTURE();
    this->variables.erase(id);
}

bool Scope::queryVariable(NameId id, Runtime *rt) {
    ProfilerCAPTURE();
    Scope *s = this;
    while (s != nullptr) {
        auto it = s->variables.find(id);
        if (it != s->variables.end()) {
            return true;
        }
        if (s->can_access_prev) {
            s = s->prev;
        }
        else {
            if (s == s->master) {
                break;
            }
            else {
                s = s->master;
            }
        }
    }
    return false;
}

}    // namespace Cotton
