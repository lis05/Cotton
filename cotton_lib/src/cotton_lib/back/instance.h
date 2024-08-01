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
#include "../util.h"
#include "nameid.h"

namespace Cotton {

class Runtime;
class Object;

/**
 * @brief Class representing an instance of a type.
 *
 */
class Instance {
public:
    /// @brief The total number of instances created.
    static int64_t total_instances;
    /// @brief id of the instance
    int64_t        id;
    /// @brief gc mark of the instance
    bool           gc_mark : 1;

    /**
     * @brief Construct a new Instance object.
     *
     * @param rt The runtime. Must be valid.
     * @param bytes Size of instance in bytes.
     */
    Instance(Runtime *rt, size_t bytes);

    /// @brief Destroy the Instance object
    virtual ~Instance() = default;

    /**
     * @brief Selects the field given by id from the instance.
     *
     * @param id id of the field. Must be valid.
     * @param rt The runtime. Must be valid.
     * @return The selected field.
     */
    virtual Object *selectField(NameId id, Runtime *rt);

    /**
     * @brief Returns whether the instance has the field given by id. B
     *
     * @param id id of the field.
     * @param rt The runtime. Must be valid.
     * @return `true` if the instance has the field, `false` otherwise.
     */
    virtual bool hasField(NameId id, Runtime *rt);

    /**
     * @brief Adds the field to the instance.
     *
     * @param id id of the field.
     * @param rt The runtime. Must be valid.
     */
    virtual void addField(NameId id, Object *obj, Runtime *rt);

    /**
     * @brief Returns a list of all objects that are reachable from the instance.
     *
     * @return List of valid objects.
     */
    virtual std::vector<Object *> getGCReachable();

    /**
     * @brief Returns a copy of the instance.
     *
     * @param rt The runtime. Must be valid.
     * @return A valid copy of the instance.
     */
    virtual Instance *copy(Runtime *rt) = 0;

    /**
     * @brief Returns the size of the instance.
     *
     * @return Size of the instance in bytes.
     */
    virtual size_t getSize() = 0;

    /**
     * @brief Returns string representation of the instance.
     *
     * @param rt The runtime. Must be valid.
     * @return String representation.
     */
    virtual std::string userRepr(Runtime *rt) = 0;

    /// @brief Spreads single use mark to all of the internals of the instance.
    virtual void spreadSingleUse();

    /// @brief Spreads multi use mark to all of the internals of the instance.
    virtual void spreadMultiUse();
};

}    // namespace Cotton
