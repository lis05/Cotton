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

#include "../front/lexer.h"
#include "../util.h"
#include <cstdint>

namespace Cotton {

/// @brief Name identifier. It is used when working with string names in order to speed things up.
typedef int64_t NameId;

/// @brief Class for managing string names efficiently using NameIDs
class NamesManager {
private:
    static std::string const       failed;
    HashTable<std::string, NameId> data;
    HashTable<NameId, std::string> data_reversed;

public:
    /**
     * @brief Returns NameId that corresponds to the provided string. If no such id exists - then it registers the
     * string under a new id, and returns it.
     *
     * @param str Queried string.
     * @return `NameId` that corresponds to the provided string.
     */
    NameId getId(const std::string &str);

    /**
     * @brief Returns string associated with the provided id.
     *
     * @param id Requested `NameId`. Assumed to be correct.
     * @return String that corresponds to the provided id.
     */
    std::string const &getString(NameId id);

    /**
     * @brief Returns whether the provided id has a string associated with it.
     *
     * @param id NameId to be checked.
     * @return `true` if there is a string associated with the id, `false` otherwise.
     */
    bool check(NameId id);
};
}    // namespace Cotton
