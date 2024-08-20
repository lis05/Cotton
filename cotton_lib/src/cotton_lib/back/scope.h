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

class Object;
class Runtime;
class GC;

/// @brief Class representing a scope that holds variables and function arguments
class Scope {
    friend class GC;
    friend class Runtime;

private:
    Scope                      *prev, *master;
    HashTable<NameId, Object *> variables;
    std::vector<Object *>       arguments;
    bool                        can_access_prev;
    bool                        is_function_call;

public:
    /**
     * @brief Construct a new Scope object
     *
     * @param prev Previous scope.
     * @param master Master scope. The first scope created. Must be valid.
     * @param can_access_prev If `true`, lookup for variables in the current scope will be able to access the
     * previous scope, and look there recursively as well.
     */
    Scope(Scope *prev, Scope *master, bool can_access_prev);
    ~Scope();

    /// @brief Returns previous scope
    Scope *getPrev();

    /// @brief Returns master scope
    Scope *getMaster();

    /// @brief Returns a list of function arguments.
    std::vector<Object *> &getArguments();

    /// @brief returns `can_access_prev` of the current scope
    bool canAccessPrev();

    /// @brief returns `is_function_call` of the current scope
    bool isFunctionCall();

    /// @brief sets `can_acess_prev` of the current scope to `value`
    void setCanAccessPrev(bool value);

    /// @brief sets `is_function_call` of the current scope to `value`
    void setIsFunctionCall(bool value);

    /**
     * @brief Adds a new variable under the given nameid to the scope.
     *
     * Adds to the current scope.
     *
     * @param id NameId of the variable.
     * @param obj Must be valid.
     * @param rt The runtime. Must be valid.
     */
    void addVariable(NameId id, Object *obj, Runtime *rt);

    /**
     * @brief Returns whether a variable with the given name exists.
     *
     * If the current scope doesn't have the variable and the `can_access_prev` flags is `true`, then lookup will
     * be done in the previous scope recursivelly. If `can_access_prev` if `false`, the final lookup will be done
     * in the master scope.
     *
     * @param id Nameid of the variable.
     * @param rt The runtime. Must be valid.
     * @return `true` if such a variable exists, `false` otherwise.
     */
    bool queryVariable(NameId id, Runtime *rt);

    /**
     * @brief Returns the variable with the given id. Signals an error if no such variable exists.
     *
     * If the current scope doesn't have the variable and the `can_access_prev` flags is `true`, then lookup will
     * be done in the previous scope recursivelly. If `can_access_prev` if `false`, the final lookup will be done
     * in the master scope.
     *
     * @param id Nameid of the variable.
     * @param rt The runtime. Must be valid.
     * @return Object*
     */
    Object *getVariable(NameId id, Runtime *rt);

    /**
     * @brief Removes variable with the given nameid from the current scope.
     *
     * @param id Nameid of the variable.
     * @param rt The runtime. Must be valid.
     */
    void removeVariable(NameId id, Runtime *rt);
};
}    // namespace Cotton
