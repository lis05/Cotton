#include "nothing.h"

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

#include "../back/stack.h"
#include "nothing.h"

namespace Cotton::Builtin {

NothingInstance::NothingInstance(Runtime *rt, bool on_stack)
    : Instance(rt, sizeof(NothingInstance), on_stack) {}

NothingInstance::~NothingInstance() {}

Instance *NothingInstance::copy(Runtime *rt) {
    Instance *res = rt->stack->allocAndInitInstance<NothingInstance>(sizeof(NothingInstance), rt);
    if (res != NULL) {
        res->on_stack = true;
        return res;
    }
    res = new (std::nothrow) NothingInstance(rt, false);
    if (res == NULL) {
        return NULL;
    }
    res->on_stack = false;
    return res;
}

size_t NothingInstance::getSize() {
    return sizeof(NothingInstance);
}

size_t NothingType::getInstanceSize() {
    return sizeof(NothingInstance);
}

NothingType::NothingType(Runtime *rt)
    : Type(true, rt) {}

Object *NothingType::create(Runtime *rt) {
    auto ins = createInstance(rt, true, NothingInstance);
    auto obj = createObject(rt, true, ins, this, true);
    return obj;
}
}    // namespace Cotton::Builtin
