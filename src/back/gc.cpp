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
    this->num_tracked++;
    this->sizeof_tracked += sizeof(Object);
    this->ops_cnt++;
    this->ops_cnt %= OPS_MOD;
    this->checkConditions(gc, rt);
}

void GCDefaultStrategy::acknowledgeTrack(Instance *instance, GC *gc, Runtime *rt) {
    this->num_tracked++;
    this->sizeof_tracked += instance->getSize();
    this->ops_cnt++;
    this->ops_cnt %= OPS_MOD;
    this->checkConditions(gc, rt);
}

void GCDefaultStrategy::acknowledgeTrack(Type *type, GC *gc, Runtime *rt) {
    this->num_tracked++;
    this->sizeof_tracked += sizeof(Type);    // it should also count adapters for operators and method, but not rn
    this->ops_cnt++;
    this->ops_cnt %= OPS_MOD;
    this->checkConditions(gc, rt);
}

void GCDefaultStrategy::acknowledgeUntrack(Object *object, GC *gc, Runtime *rt) {
    this->num_tracked--;
    this->sizeof_tracked -= sizeof(Object);
    this->ops_cnt        += OPS_MOD - 1;
    this->ops_cnt        %= OPS_MOD;
    this->checkConditions(gc, rt);
}

void GCDefaultStrategy::acknowledgeUntrack(Instance *instance, GC *gc, Runtime *rt) {
    this->num_tracked--;
    this->sizeof_tracked -= instance->getSize();
    this->ops_cnt        += OPS_MOD - 1;
    this->ops_cnt        %= OPS_MOD;
    this->checkConditions(gc, rt);
}

void GCDefaultStrategy::acknowledgeUntrack(Type *type, GC *gc, Runtime *rt) {
    this->num_tracked--;
    this->sizeof_tracked -= sizeof(Type);
    this->ops_cnt        += OPS_MOD - 1;
    this->ops_cnt        %= OPS_MOD;
    this->checkConditions(gc, rt);
}

void GCDefaultStrategy::checkConditions(GC *gc, Runtime *rt) {
    if ((this->prev_num_tracked < this->num_tracked / NUM_TRACKED_MULT)
        || (this->prev_sizeof_tracked < this->sizeof_tracked / SIZEOF_TRACKED_MULT) || (this->ops_cnt == 0))
    {
        gc->runCycle(rt);
    }
}

GC::GC(GCStrategy *gc_strategy) {
    this->gc_strategy = gc_strategy;
    this->gc_mark     = 1;
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

void GC::track(Instance *instance, Runtime *rt) {
    if (this->tracked_instances.find(instance) != this->tracked_instances.end()) {
        return;
    }
    this->tracked_instances[instance] = true;
    this->gc_strategy->acknowledgeTrack(instance, this, rt);
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

static void mark(Instance *ins, GC *gc);
static void mark(Type *type, GC *gc);

static void mark(Object *obj, GC *gc) {
    if (obj == NULL) {
        return;
    }
    if (obj->gc_mark == gc->gc_mark) {
        return;
    }

    obj->gc_mark = gc->gc_mark;
    for (auto &o : obj->getGCReachable()) {
        mark(o, gc);
    }
    mark(obj->instance, gc);
    mark(obj->type, gc);
}

static void mark(Instance *ins, GC *gc) {
    if (ins == NULL) {
        return;
    }
    if (ins->gc_mark == gc->gc_mark) {
        return;
    }

    ins->gc_mark = gc->gc_mark;
    for (auto &o : ins->getGCReachable()) {
        mark(o, gc);
    }
}

static void mark(Type *type, GC *gc) {
    if (type == NULL) {
        return;
    }
    if (type->gc_mark == gc->gc_mark) {
        return;
    }

    type->gc_mark = gc->gc_mark;
    for (auto &o : type->getGCReachable()) {
        mark(o, gc);
    }
}

void GC::runCycle(Runtime *rt) {
    // mark
    auto scope = rt->scope;
    while (scope != NULL) {
        for (auto &[_, obj] : scope->variables) {
            mark(obj, this);
        }
        scope = scope->prev;
    }

    // sweep
    for (auto &[obj, _] : this->tracked_objects) {
        if (obj->gc_mark != this->gc_mark) {
            delete obj;
        }
    }
    this->tracked_objects.erase_if([this](const std::pair<Object *, bool> &item) {
        return item.first->gc_mark != this->gc_mark;
    });
    for (auto &[ins, _] : this->tracked_instances) {
        if (ins->gc_mark != this->gc_mark) {
            delete ins;
        }
    }
    this->tracked_instances.erase_if([this](const std::pair<Instance *, bool> &item) {
        return item.first->gc_mark != this->gc_mark;
    });
    for (auto &[type, _] : this->tracked_types) {
        if (type->gc_mark != this->gc_mark) {
            delete type;
        }
    }
    this->tracked_types.erase_if([this](const std::pair<Type *, bool> &item) {
        return item.first->gc_mark != this->gc_mark;
    });

    this->gc_mark = !this->gc_mark;
}
}    // namespace Cotton
