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

/**
 *  @brief An abstract class representing a garbage collector strategy
 */
class GCStrategy {
public:
    /**
        @brief Acknowledges track of an Object. This function is called by the gc.
        \param object The object that got tracked. Assumed to be valid.
      */
    virtual void acknowledgeTrack(Object *object) = 0;

    /**
        @brief Acknowledges track of an Instance. This function is called by the gc.
        \param instance The Instance that got tracked. Assumed to be valid.
        \param bytes Size of the instance in bytes. It is added to the total size counter.
      */
    virtual void acknowledgeTrack(Instance *instance, size_t bytes) = 0;

    /**
        @brief Acknowledges track of a Type. This function is called by the gc.
        \param type The type that got tracked. Assumed to be valid.
      */
    virtual void acknowledgeTrack(Type *type) = 0;

    /**
        @brief Acknowledges untrack of an Object. This function is called by the gc.
        \param object The object that got untracked. Assumed to be valid.
      */
    virtual void acknowledgeUntrack(Object *object) = 0;

    /**
        @brief Acknowledges untrack of an Instance. This function is called by the gc.
        \param instance The instance that got untracked. Assumed to be valid.
      */
    virtual void acknowledgeUntrack(Instance *instance) = 0;

    /**
        @brief Acknowledges untrack of a Type. This function is called by the gc.
        \param type The type that got untracked. Assumed to be valid.
      */
    virtual void acknowledgeUntrack(Type *type) = 0;

    /**
        @brief Acknowledges the end of a gc cycle. This function is called by the gc.
        \param rt The Runtime. Assumed to be valid.
      */
    virtual void acknowledgeEndOfCycle(Runtime *rt) = 0;

    /**
        @brief Acknowledges a gc ping event. This function is called by the gc.
        \param rt The Runtime. Assumed to be valid.
      */
    virtual void acknowledgePing(Runtime *rt) = 0;
};

/**
    @brief The default gc strategy.
 */
class GCDefaultStrategy: public GCStrategy {
private:
    const int NUM_TRACKED_INIT = 10'000;    // prev_num_tracked is set to this at initialization
    const int NUM_TRACKED_MULT = 6;         // cycle runs when prev_num_tracked < num_tracked / 2;
    int64_t   num_tracked, prev_num_tracked;

    const int MIN_CYCLE_SIZE      = 80'000;
    const int SIZEOF_TRACKED_INIT = 80'000;           // prev_num_tracked is set to this at initialization
    const int SIZEOF_TRACKED_MULT = 6;                // cycle runs when prev_sizeof_tracked < num_tracked / 2;
    int64_t   sizeof_tracked, prev_sizeof_tracked;    // in bytes

    const int OPS_MOD = 100'000;                      // cycle runs when opc_cnt % OPS_MOD == 0
    int64_t   ops_cnt;                                // number of tracks and untrackes modulo OPS_MOD

public:
    /**
     * @brief Construct a new GCDefaultStrategy object
     *
     */
    GCDefaultStrategy();

    /**
     * @brief Destroy the GCDefaultStrategy object
     *
     */
    ~GCDefaultStrategy() = default;

    /**
        @brief Acknowledges track of an Object. This function is called by the gc.
        \param object The object that got tracked. Assumed to be valid.
      */
    void acknowledgeTrack(Object *object);

    /**
        @brief Acknowledges track of an Instance. This function is called by the gc.
        \param instance The Instance that got tracked. Assumed to be valid.
        \param bytes Size of the instance in bytes. It is added to the total size counter.
      */
    void acknowledgeTrack(Instance *instance, size_t bytes);

    /**
        @brief Acknowledges track of a Type. This function is called by the gc.
        \param type The type that got tracked. Assumed to be valid.
      */
    void acknowledgeTrack(Type *type);

    /**
        @brief Acknowledges untrack of an Object. This function is called by the gc.
        \param object The object that got untracked. Assumed to be valid.
      */
    void acknowledgeUntrack(Object *object);

    /**
        @brief Acknowledges untrack of an Instance. This function is called by the gc.
        \param instance The instance that got untracked. Assumed to be valid.
      */
    void acknowledgeUntrack(Instance *instance);

    /**
        @brief Acknowledges untrack of a Type. This function is called by the gc.
        \param type The type that got untracked. Assumed to be valid.
      */
    void acknowledgeUntrack(Type *type);

    /**
        @brief Acknowledges the end of a gc cycle. This function is called by the gc.
        \param rt The Runtime. Assumed to be valid.
      */
    void acknowledgeEndOfCycle(Runtime *rt);
    /**
        @brief Acknowledges a gc ping event. This function is called by the gc.
        \param rt The Runtime. Assumed to be valid.
      */
    void acknowledgePing(Runtime *rt);

    /***
     * @brief Checks if conditions for pinging the gc have been met. This function is called by the gc.
     *
     * @param rt The Runtime. Assumed to be valid.
     * @return * void
     */
    void checkConditions(Runtime *rt);
};

/**
 * @brief Garbage collector class.
 *
 */
class GC {
public:
    Runtime                                     *rt;
    __gnu_pbds::gp_hash_table<Object *, bool>    tracked_objects;
    __gnu_pbds::gp_hash_table<Object *, int64_t> held_objects;

    __gnu_pbds::gp_hash_table<Instance *, bool> tracked_instances;
    __gnu_pbds::gp_hash_table<Type *, bool>     tracked_types;

    bool        gc_mark : 1;
    GCStrategy *gc_strategy;
    bool        enabled;

    /**
     * @brief Construct a new GC object
     *
     * @param gc_strategy Garbage collection strategy. Assumed to be valid.
     */
    GC(GCStrategy *gc_strategy);

    /**
     * @brief Destroy the GC object
     *
     */
    ~GC();

    /**
     * @brief Tracks the object;
     *
     * @param object Assumed to be valid.
     */
    void track(Object *object);

    /**
     * @brief Tracks the instance;
     *
     * @param instance Assumed to be valid.
     * @param bytes The size of the instance in bytes.
     */
    void track(Instance *instance, size_t bytes);

    /**
     * @brief Tracks the type;
     *
     * @param type Assumed to be valid.
     */
    void track(Type *type);

    /**
     * @brief Untracks the object;
     *
     * @param object Assumed to be valid.
     */
    void untrack(Object *object);

    /**
     * @brief Untracks the instance;
     *
     * @param instance Assumed to be valid.
     */
    void untrack(Instance *instance);

    /**
     * @brief Unracks the type;
     *
     * @param type Assumed to be valid.
     */
    void untrack(Type *type);

    /**
     * @brief Holds the given object. The gc will consider it as reachable even if it is not.
     *
     * @param object Assumed to be valid.
     */
    void hold(Object *object);

    /**
     * @brief Releases the held object.
     *
     * @param object Assumed to be valid.
     */
    void release(Object *object);

    /**
     * @brief Pings the gc. If the strategy's conditions are met, a gc cycle will be ran.
     *
     * @param rt The Runtime. Assumed to be valid.
     */
    void ping(Runtime *rt);

    /**
     * @brief Runs a gc cycle.
     *
     * @param rt The Runtime. Assumed to be valid.
     */
    void runCycle(Runtime *rt);

    /**
     * @brief Enables the gc.
     *
     */
    void enable();

    /**
     * @brief Disables the gc. Future ping calls will not run a gc cycle.z
     *
     */
    void disable();
};
}    // namespace Cotton
