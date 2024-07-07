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

#include "mempool.h"
#include <ostream>
#include <vector>

namespace Cotton {

class Instance;
class Type;
class Runtime;

class Object {
public:
    static int64_t total_objects;
    int64_t        id;
    bool           is_instance : 1;    // otherwise it's type
    bool           gc_mark     : 1;
    bool           can_modify  : 1;
    bool single_use : 1;    // if object is expected to be used only once (like x + y is going to be used only
                            // once in an expression)

    Instance *instance;
    Type     *type;

    Object(bool is_instance, Instance *instance, Type *type, Runtime *rt);
    ~Object();

    std::vector<Object *> getGCReachable();
    // short representation of the object
    std::string           shortRepr();

    void assignTo(Object *obj, Runtime *rt);
    void assignToCopyOf(Object *obj, Runtime *rt);

    // marks object with single use, as well as its insides
    void spreadSingleUse();
    void spreadMultiUse();
};

std::ostream &operator<<(std::ostream &stream, Object *obj);
// namespace MagicMethods

};    // namespace Cotton
