/*
 Copyright (c) 2024 Ihor Lukianov (lis05)

 Permission is hereby granted, free of charge, to any person obtaining a copy of
 this software and associated documentation files (the "Software"), to deal in
 the Software without restriction, including without limitation the rights to
 use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 the Software, and to permit persons to whom the Software is furnished to do so,
 subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "gc.h"
#include "../profiler.h"
#include "instance.h"
#include "object.h"
#include "runtime.h"
#include "scope.h"
#include "type.h"

namespace Cotton {
GCDefaultStrategy::GCDefaultStrategy() {
    ProfilerCAPTURE();
    this->rt                  = NULL;
    this->num_tracked         = 0;
    this->prev_num_tracked    = NUM_TRACKED_INIT;
    this->sizeof_tracked      = 0;
    this->prev_sizeof_tracked = SIZEOF_TRACKED_INIT;
    this->ops_cnt             = 0;
}

void GCDefaultStrategy::acknowledgeTrack(Object *object) {
    ProfilerCAPTURE();
    if (object == NULL) {
        return;
    }
    this->num_tracked++;
    this->sizeof_tracked += sizeof(Object);
    this->ops_cnt++;
}

void GCDefaultStrategy::acknowledgeTrack(Instance *instance, size_t bytes) {
    ProfilerCAPTURE();
    if (instance == NULL) {
        return;
    }
    this->num_tracked++;
    this->sizeof_tracked += bytes;
    this->ops_cnt++;
}

void GCDefaultStrategy::acknowledgeTrack(Type *type) {
    ProfilerCAPTURE();
    if (type == NULL) {
        return;
    }
    this->num_tracked++;
    this->sizeof_tracked += sizeof(Type);    // it should also count adapters for operators and method, but not rn
    this->ops_cnt++;
}

void GCDefaultStrategy::acknowledgeUntrack(Object *object) {
    ProfilerCAPTURE();
    if (object == NULL) {
        return;
    }
    this->num_tracked--;
    this->sizeof_tracked -= sizeof(Object);
}

void GCDefaultStrategy::acknowledgeUntrack(Instance *instance) {
    ProfilerCAPTURE();
    if (instance == NULL) {
        return;
    }
    this->num_tracked--;
    this->sizeof_tracked -= instance->getSize();
}

void GCDefaultStrategy::acknowledgeUntrack(Type *type) {
    ProfilerCAPTURE();
    if (type == NULL) {
        return;
    }
    this->num_tracked--;
    this->sizeof_tracked -= sizeof(Type);
}

void GCDefaultStrategy::acknowledgeEndOfCycle() {
    ProfilerCAPTURE();
    this->prev_num_tracked = this->rt->gc->tracked_instances.size() + this->rt->gc->tracked_objects.size()
                             + this->rt->gc->tracked_types.size();
    this->prev_sizeof_tracked = 0;
    for (auto &[obj, _] : this->rt->gc->tracked_objects) {
        this->prev_sizeof_tracked += sizeof(obj);
    }
    for (auto &[ins, _] : this->rt->gc->tracked_instances) {
        this->prev_sizeof_tracked += ins->getSize();
    }
    for (auto &[type, _] : this->rt->gc->tracked_types) {
        this->prev_sizeof_tracked += sizeof(type);
    }
    this->num_tracked    = this->prev_num_tracked;
    this->sizeof_tracked = this->prev_sizeof_tracked;
}

void GCDefaultStrategy::acknowledgePing() {
    ProfilerCAPTURE();
    this->checkConditions();
}

void GCDefaultStrategy::checkConditions() {
    ProfilerCAPTURE();
    if (this->sizeof_tracked >= MIN_CYCLE_SIZE
        && ((this->prev_num_tracked < this->num_tracked / NUM_TRACKED_MULT)
            || (this->prev_sizeof_tracked < this->sizeof_tracked / SIZEOF_TRACKED_MULT)
            || (this->ops_cnt >= OPS_MOD)))
    {
        this->ops_cnt %= OPS_MOD;
        this->rt->gc->runCycle();
    }
}

GC::GC(GCStrategy *gc_strategy) {
    ProfilerCAPTURE();
    this->gc_strategy = gc_strategy;
    this->gc_mark     = 1;
    this->enabled     = true;
}

GC::~GC() {
    ProfilerCAPTURE();
    for (auto &[obj, _] : this->tracked_objects) {
        delete obj;
    }
    for (auto &[ins, _] : this->tracked_instances) {
        delete ins;
    }
    for (auto &[type, _] : this->tracked_types) {
        delete type;
    }
}

void GC::track(Object *object) {
    ProfilerCAPTURE();
    if (this->tracked_objects.find(object) != this->tracked_objects.end()) {
        return;
    }
    this->tracked_objects[object] = true;
    this->gc_strategy->acknowledgeTrack(object);
}

void GC::track(Instance *instance, size_t bytes) {
    ProfilerCAPTURE();
    if (this->tracked_instances.find(instance) != this->tracked_instances.end()) {
        return;
    }
    this->tracked_instances[instance] = true;
    this->gc_strategy->acknowledgeTrack(instance, bytes);
}

void GC::track(Type *type) {
    ProfilerCAPTURE();
    if (this->tracked_types.find(type) != this->tracked_types.end()) {
        return;
    }
    this->tracked_types[type] = true;
    this->gc_strategy->acknowledgeTrack(type);
}

void GC::untrack(Object *object) {
    ProfilerCAPTURE();
    auto it = this->tracked_objects.find(object);
    if (it == this->tracked_objects.end()) {
        return;
    }
    this->tracked_objects.erase(object);
}

void GC::untrack(Instance *instance) {
    ProfilerCAPTURE();
    auto it = this->tracked_instances.find(instance);
    if (it == this->tracked_instances.end()) {
        return;
    }
    this->tracked_instances.erase(instance);
}

void GC::untrack(Type *type) {
    ProfilerCAPTURE();
    auto it = this->tracked_types.find(type);
    if (it == this->tracked_types.end()) {
        return;
    }
    this->tracked_types.erase(type);
}

void GC::hold(Object *object) {
    ProfilerCAPTURE();
    this->held_objects[object] = true;
}

void GC::release(Object *object) {
    ProfilerCAPTURE();
    this->held_objects.erase(object);
}

void GC::ping() {
    ProfilerCAPTURE();
    this->gc_strategy->acknowledgePing();
}

static void mark(Instance *ins, Runtime *rt);
static void mark(Type *type, Runtime *rt);

static void mark(Object *obj, Runtime *rt) {
    ProfilerCAPTURE();
    if (obj == NULL) {
        return;
    }
    if (obj->gc_mark == rt->gc->gc_mark) {
        return;
    }

    obj->gc_mark = rt->gc->gc_mark;
    for (auto &o : obj->getGCReachable()) {
        mark(o, rt);
    }
    mark(obj->instance, rt);
    mark(obj->type, rt);
}

static void mark(Instance *ins, Runtime *rt) {
    ProfilerCAPTURE();
    if (ins == NULL) {
        return;
    }
    if (ins->gc_mark == rt->gc->gc_mark) {
        return;
    }

    ins->gc_mark = rt->gc->gc_mark;
    for (auto &o : ins->getGCReachable()) {
        mark(o, rt);
    }
}

static void mark(Type *type, Runtime *rt) {
    ProfilerCAPTURE();
    if (type == NULL) {
        return;
    }
    if (type->gc_mark == rt->gc->gc_mark) {
        return;
    }

    type->gc_mark = rt->gc->gc_mark;
    for (auto &o : type->getGCReachable()) {
        mark(o, rt);
    }
}

void GC::runCycle() {
    ProfilerCAPTURE();
    if (!this->enabled) {
        return;
    }
    // mark
    auto scope = rt->scope;
    while (scope != NULL) {
        for (auto &[_, obj] : scope->variables) {
            mark(obj, this->rt);
        }
        scope = scope->prev;
    }
    for (auto &[obj, _] : this->held_objects) {
        mark(obj, this->rt);
    }
    // sweep
    std::vector<Object *> deleted_objects;
    for (auto &[obj, _] : this->tracked_objects) {
        if (obj->gc_mark != this->gc_mark) {
            deleted_objects.push_back(obj);
        }
    }
    for (auto &obj : deleted_objects) {
        this->tracked_objects.erase(obj);
        delete obj;
    }
    std::vector<Instance *> deleted_instances;
    for (auto &[ins, _] : this->tracked_instances) {
        if (ins->gc_mark != this->gc_mark) {
            deleted_instances.push_back(ins);
        }
    }
    for (auto &ins : deleted_instances) {
        this->tracked_instances.erase(ins);
        delete ins;
    }
    std::vector<Type *> deleted_types;
    for (auto &[type, _] : this->tracked_types) {
        if (type->gc_mark != this->gc_mark) {
            deleted_types.push_back(type);
        }
    }
    for (auto &type : deleted_types) {
        this->tracked_types.erase(type);
        delete type;
    }

    this->gc_mark = !this->gc_mark;
    this->gc_strategy->acknowledgeEndOfCycle();
    // fprintf(stderr, "GC CYCLE END\n");
}

void GC::enable() {
    ProfilerCAPTURE();
    this->enabled = true;
}

void GC::disable() {
    ProfilerCAPTURE();
    this->enabled = false;
}
}    // namespace Cotton
