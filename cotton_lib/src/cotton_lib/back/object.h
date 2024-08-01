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

#include <ostream>
#include <vector>

namespace Cotton {

class Instance;
class Type;
class Runtime;

/// @brief Class representing a Cotton object.
class Object {
private:
    static int64_t total_objects;

public:
    /// @brief Id of the object
    int64_t id;

    /// @brief `true` if object holds an instance (then it's called instance object), `false` if not (then it's
    /// called type object)
    bool is_instance : 1;

    /// @brief Mark used by the gc in order to determine if the object is reachable
    bool gc_mark : 1;

    /// @brief if `false`, then assignment to this object will not be possible
    bool can_modify : 1;

    /// @brief if object is expected to be used only once, therefore some changes in its behavior (like copying)
    /// can be made to speed things up
    bool single_use : 1;

    /// @brief Instance of the object. May be nullptr.
    Instance *instance;

    /// @brief Type of the object. Must be valid (non-nullptr).
    Type *type;

    /**
     * @brief Construct a new Object object
     *
     * @param is_instance if `true`, creates an instance object, otherwise creates a type object
     * @param instance The instance to initialize the object with. Must be valid if `is_instance = true`
     * @param type The type to initialize the object with. Must be valid.
     * @param rt The runtime. Must be valid.
     */
    Object(bool is_instance, Instance *instance, Type *type, Runtime *rt);

    /// @brief Destroy the Object object. Should not be called outside of the garbage collector.
    ~Object();

    /**
     * @brief Returns a list of all objects that are reachable from this object.
     *
     * @return List of valid objects.
     */
    std::vector<Object *> getGCReachable();

    /**
     * @brief Returns string representation of the object.
     *
     * @param rt The runtime. Must be valid.
     * @return String representation.
     */
    std::string userRepr(Runtime *rt);

    /**
     * @brief Assigns this object to `obj` without making a copy of it.
     *
     * @param obj The object to assign to. Mast be valid.
     * @param rt The runtime. Must be valid.
     */
    void assignTo(Object *obj, Runtime *rt);

    /**
     * @brief Assigns this object to a copy of `obj`.
     *
     * @param obj The object a copy of which to assign to. Mast be valid.
     * @param rt The runtime. Must be valid.
     */
    void assignToCopyOf(Object *obj, Runtime *rt);

    /// @brief Spreads single use mark to all of the internals of the object.
    virtual void spreadSingleUse();

    /// @brief Spreads multi use mark to all of the internals of the object.
    virtual void spreadMultiUse();
};

/// @brief Outputs a string representation of the object. Don't use this, use Object::userRepr instead.
/// @param stream Ostream.
/// @param obj The object.
/// @return Ostream.
std::ostream &operator<<(std::ostream &stream, Object *obj);

};    // namespace Cotton
