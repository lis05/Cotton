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

namespace Cotton::Builtin {

class CharacterInstance: public Instance {
public:
    uint8_t value;

    CharacterInstance(Runtime *rt);
    ~CharacterInstance();

    Instance   *copy(Runtime *rt);
    size_t      getSize();
    std::string userRepr();
    void        destroy(Runtime *rt);
};

class CharacterType: public Type {
public:
    size_t getInstanceSize();
    CharacterType(Runtime *rt);
    ~CharacterType() = default;
    Object     *create(Runtime *rt);
    Object     *copy(Object *obj, Runtime *rt);
    std::string userRepr();
};

Object *makeCharacterInstanceObject(uint8_t value, Runtime *rt);

void installCharacterMethods(Type *type, Runtime *rt);

uint8_t &getCharacterValue(Object *obj, Runtime *rt);
uint8_t &getCharacterValue(Object *obj, Runtime *rt, const TextArea &ta);
#define getCharacterValueFast(obj) (icast(obj->instance, CharacterInstance)->value)
}    // namespace Cotton::Builtin
