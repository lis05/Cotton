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
#include "../../back/api.h"
#include "../../front/api.h"
#include <ext/pb_ds/assoc_container.hpp>

namespace Cotton::Builtin {
class UserDefinedInstance: public Instance {
public:
    __gnu_pbds::cc_hash_table<int64_t, Object *> fields;
    int64_t                                      nameid;
    UserDefinedInstance(Runtime *rt);
    ~UserDefinedInstance();
    Object *selectField(int64_t id, Runtime *rt);
    bool    hasField(int64_t id, Runtime *rt);
    void    addField(int64_t id, Object *obj, Runtime *rt);

    Instance             *copy(Runtime *rt);
    size_t                getSize();
    std::string           userRepr(Runtime *rt);
    std::vector<Object *> getGCReachable();
};

class UserDefinedType: public Type {
public:
    int64_t              nameid;
    std::vector<Token *> instance_fields;

    size_t getInstanceSize();
    UserDefinedType(Runtime *rt);
    ~UserDefinedType() = default;
    Object     *create(Runtime *rt);
    std::string userRepr(Runtime *rt);
    Object     *copy(Object *obj, Runtime *rt);
};
}    // namespace Cotton::Builtin
