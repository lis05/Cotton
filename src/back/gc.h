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

#pragma once
#include <ext/pb_ds/assoc_container.hpp>

namespace Cotton {
class Object;
class Instance;
class Type;
class Runtime;
class GC;

class GCStrategy {
public:
    virtual void acknowledgeTrack(Object *object, GC *gc, Runtime *rt)                   = 0;
    virtual void acknowledgeTrack(Instance *instance, size_t bytes, GC *gc, Runtime *rt) = 0;
    virtual void acknowledgeTrack(Type *type, GC *gc, Runtime *rt)                       = 0;
    virtual void acknowledgeUntrack(Object *object, GC *gc, Runtime *rt)                 = 0;
    virtual void acknowledgeUntrack(Instance *instance, GC *gc, Runtime *rt)             = 0;
    virtual void acknowledgeUntrack(Type *type, GC *gc, Runtime *rt)                     = 0;
    virtual void acknowledgeEndOfCycle(GC *gc, Runtime *rt)                              = 0;
    virtual void acknowledgePing(GC *gc, Runtime *rt)                                    = 0;
};

class GCDefaultStrategy: public GCStrategy {
public:
    const int NUM_TRACKED_INIT = 10'000;              // prev_num_tracked is set to this at initialization
    const int NUM_TRACKED_MULT = 6;                   // cycle runs when prev_num_tracked < num_tracked / 2;
    int64_t   num_tracked, prev_num_tracked;

    const int SIZEOF_TRACKED_INIT = 80'000;           // prev_num_tracked is set to this at initialization
    const int SIZEOF_TRACKED_MULT = 6;                // cycle runs when prev_sizeof_tracked < num_tracked / 2;
    int64_t   sizeof_tracked, prev_sizeof_tracked;    // in bytes

    const int OPS_MOD = 100'000;                       // cycle runs when opc_cnt % OPS_MOD == 0
    int64_t   ops_cnt;                                // number of tracks and untrackes modulo OPS_MOD

    GCDefaultStrategy();
    ~GCDefaultStrategy() = default;

    void acknowledgeTrack(Object *object, GC *gc, Runtime *rt);
    void acknowledgeTrack(Instance *instance, size_t bytes, GC *gc, Runtime *rt);
    void acknowledgeTrack(Type *type, GC *gc, Runtime *rt);
    void acknowledgeUntrack(Object *object, GC *gc, Runtime *rt);
    void acknowledgeUntrack(Instance *instance, GC *gc, Runtime *rt);
    void acknowledgeUntrack(Type *type, GC *gc, Runtime *rt);
    void acknowledgeEndOfCycle(GC *gc, Runtime *rt);
    void acknowledgePing(GC *gc, Runtime *rt);

    void checkConditions(GC *gc, Runtime *rt);
};

class GC {
public:
    __gnu_pbds::gp_hash_table<Object *, bool> tracked_objects;
    __gnu_pbds::gp_hash_table<Object *, bool> held_objects;

    __gnu_pbds::gp_hash_table<Instance *, bool> tracked_instances;
    __gnu_pbds::gp_hash_table<Type *, bool>     tracked_types;

    bool        gc_mark : 1;    // for the next sweep
    GCStrategy *gc_strategy;
    bool        enabled;

    GC(GCStrategy *gc_strategy);
    ~GC();

    void track(Object *object, Runtime *rt);
    void track(Instance *instance, size_t bytes, Runtime *rt);
    void track(Type *type, Runtime *rt);

    void untrack(Object *object, Runtime *rt);
    void untrack(Instance *instance, Runtime *rt);
    void untrack(Type *type, Runtime *rt);

    void hold(Object *object, Runtime *rt);    // even if object can't be reached, it will still be preserved, as
                                               // well as items reachable from it
    void release(Object *object, Runtime *rt);

    void ping(Runtime *rt);

    void runCycle(Runtime *rt);

    void enable(Runtime *rt);
    void disable(Runtime *rt);
};
}    // namespace Cotton
