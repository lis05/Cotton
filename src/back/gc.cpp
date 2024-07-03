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
#include "instance.h"
#include "object.h"
#include "runtime.h"
#include "scope.h"
#include "type.h"

namespace Cotton {
GCDefaultStrategy::GCDefaultStrategy() {
    this->num_tracked         = 0;
    this->prev_num_tracked    = NUM_TRACKED_INIT;
    this->sizeof_tracked      = 0;
    this->prev_sizeof_tracked = SIZEOF_TRACKED_INIT;
    this->ops_cnt             = 0;
}

void GCDefaultStrategy::acknowledgeTrack(Object *object, GC *gc, Runtime *rt) {
    if (object == NULL) {
        return;
    }
    this->num_tracked++;
    this->sizeof_tracked += sizeof(Object);
    this->ops_cnt++;
}

void GCDefaultStrategy::acknowledgeTrack(Instance *instance, size_t bytes, GC *gc, Runtime *rt) {
    if (instance == NULL) {
        return;
    }
    this->num_tracked++;
    this->sizeof_tracked += bytes;
    this->ops_cnt++;
}

void GCDefaultStrategy::acknowledgeTrack(Type *type, GC *gc, Runtime *rt) {
    if (type == NULL) {
        return;
    }
    this->num_tracked++;
    this->sizeof_tracked += sizeof(Type);    // it should also count adapters for operators and method, but not rn
    this->ops_cnt++;
}

void GCDefaultStrategy::acknowledgeUntrack(Object *object, GC *gc, Runtime *rt) {
    if (object == NULL) {
        return;
    }
    this->num_tracked--;
    this->sizeof_tracked -= sizeof(Object);
}

void GCDefaultStrategy::acknowledgeUntrack(Instance *instance, GC *gc, Runtime *rt) {
    if (instance == NULL) {
        return;
    }
    this->num_tracked--;
    this->sizeof_tracked -= instance->getSize();
}

void GCDefaultStrategy::acknowledgeUntrack(Type *type, GC *gc, Runtime *rt) {
    if (type == NULL) {
        return;
    }
    this->num_tracked--;
    this->sizeof_tracked -= sizeof(Type);
}

void GCDefaultStrategy::acknowledgeEndOfCycle(GC *gc, Runtime *rt) {
    this->prev_num_tracked = gc->tracked_instances.size() + gc->tracked_objects.size() + gc->tracked_types.size();
    this->prev_sizeof_tracked = 0;
    for (auto &[obj, _] : gc->tracked_objects) {
        this->prev_sizeof_tracked += sizeof(obj);
    }
    for (auto &[ins, _] : gc->tracked_instances) {
        this->prev_sizeof_tracked += ins->getSize();
    }
    for (auto &[type, _] : gc->tracked_types) {
        this->prev_sizeof_tracked += sizeof(type);
    }
    this->num_tracked    = this->prev_num_tracked;
    this->sizeof_tracked = this->prev_sizeof_tracked;
}

void GCDefaultStrategy::acknowledgePing(GC *gc, Runtime *rt) {
    this->checkConditions(gc, rt);
}

void GCDefaultStrategy::checkConditions(GC *gc, Runtime *rt) {
    if ((this->prev_num_tracked < this->num_tracked / NUM_TRACKED_MULT)
        || (this->prev_sizeof_tracked < this->sizeof_tracked / SIZEOF_TRACKED_MULT) || (this->ops_cnt == OPS_MOD))
    {
        this->ops_cnt %= OPS_MOD;
        gc->runCycle(rt);
    }
}

GC::GC(GCStrategy *gc_strategy) {
    this->gc_strategy = gc_strategy;
    this->gc_mark     = 1;
    this->enabled     = true;
}

GC::~GC() {
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

void GC::track(Object *object, Runtime *rt) {
    if (this->tracked_objects.find(object) != this->tracked_objects.end()) {
        return;
    }
    this->tracked_objects[object] = true;
    this->gc_strategy->acknowledgeTrack(object, this, rt);
}

void GC::track(Instance *instance, size_t bytes, Runtime *rt) {
    if (this->tracked_instances.find(instance) != this->tracked_instances.end()) {
        return;
    }
    this->tracked_instances[instance] = true;
    this->gc_strategy->acknowledgeTrack(instance, bytes, this, rt);
}

void GC::track(Type *type, Runtime *rt) {
    if (this->tracked_types.find(type) != this->tracked_types.end()) {
        return;
    }
    this->tracked_types[type] = true;
    this->gc_strategy->acknowledgeTrack(type, this, rt);
}

void GC::untrack(Object *object, Runtime *rt) {
    auto it = this->tracked_objects.find(object);
    if (it == this->tracked_objects.end()) {
        return;
    }
    this->tracked_objects.erase(object);
}

void GC::untrack(Instance *instance, Runtime *rt) {
    auto it = this->tracked_instances.find(instance);
    if (it == this->tracked_instances.end()) {
        return;
    }
    this->tracked_instances.erase(instance);
}

void GC::untrack(Type *type, Runtime *rt) {
    auto it = this->tracked_types.find(type);
    if (it == this->tracked_types.end()) {
        return;
    }
    this->tracked_types.erase(type);
}

void GC::hold(Object *object, Runtime *rt) {
    this->held_objects[object] = true;
}

void GC::release(Object *object, Runtime *rt) {
    this->held_objects.erase(object);
}

void GC::ping(Runtime *rt) {
    this->gc_strategy->acknowledgePing(this, rt);
}

static void mark(Instance *ins, GC *gc, Runtime *rt);
static void mark(Type *type, GC *gc, Runtime *rt);

static void mark(Object *obj, GC *gc, Runtime *rt) {
    if (obj == NULL) {
        return;
    }
    if (obj->gc_mark == gc->gc_mark) {
        return;
    }

    obj->gc_mark = gc->gc_mark;
    for (auto &o : obj->getGCReachable(rt)) {
        mark(o, gc, rt);
    }
    mark(obj->instance, gc, rt);
    mark(obj->type, gc, rt);
}

static void mark(Instance *ins, GC *gc, Runtime *rt) {
    if (ins == NULL) {
        return;
    }
    if (ins->gc_mark == gc->gc_mark) {
        return;
    }

    ins->gc_mark = gc->gc_mark;
    for (auto &o : ins->getGCReachable(rt)) {
        mark(o, gc, rt);
    }
}

static void mark(Type *type, GC *gc, Runtime *rt) {
    if (type == NULL) {
        return;
    }
    if (type->gc_mark == gc->gc_mark) {
        return;
    }

    type->gc_mark = gc->gc_mark;
    for (auto &o : type->getGCReachable(rt)) {
        mark(o, gc, rt);
    }
}

void GC::runCycle(Runtime *rt) {
    if (!this->enabled) {
        return;
    }
    // mark
    auto scope = rt->scope;
    while (scope != NULL) {
        for (auto &[_, obj] : scope->variables) {
            mark(obj, this, rt);
        }
        scope = scope->prev;
    }
    for (auto &[obj, _] : this->held_objects) {
        mark(obj, this, rt);
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
    this->gc_strategy->acknowledgeEndOfCycle(this, rt);
}

void GC::enable(Runtime *rt) {
    this->enabled = true;
}

void GC::disable(Runtime *rt) {
    this->enabled = false;
}
}    // namespace Cotton
