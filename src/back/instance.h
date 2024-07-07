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
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace Cotton {

class Runtime;
class Object;

class Instance {
public:
    Runtime        *rt;
    static int64_t  total_instances;
    int64_t         id;
    // returns a valid object (non-null)
    virtual Object *selectField(int64_t id);
    virtual bool    hasField(int64_t id);
    virtual void    addField(int64_t id, Object *obj);
    bool            gc_mark : 1;

    Instance(Runtime *rt, size_t bytes);
    virtual ~Instance() = default;

    virtual std::vector<Object *> getGCReachable();
    virtual Instance             *copy()      = 0;
    virtual size_t                getSize()   = 0;    // in bytes
    virtual std::string           shortRepr() = 0;

    virtual void spreadSingleUse();
    virtual void spreadMultiUse();
};

}    // namespace Cotton
