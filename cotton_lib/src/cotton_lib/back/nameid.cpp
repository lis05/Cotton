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

#include "nameid.h"
#include "../profiler.h"

namespace Cotton {

NameId NameIds::get(Token *token) {
    ProfilerCAPTURE();
    NameId res;
    if (token == NULL) {
        res.id = -1;
    }
    else {
        if (this->map.find(token->data) != this->map.end()) {
            res.id = this->map[token->data];
        }
        else {
            res.id                    = this->map.size();
            this->map[token->data]    = res.id;
            this->reverse_map[res.id] = token->data;
        }
    }
    res.token = token;
    res.str   = &res.token->data;
    return res;
}

NameId NameIds::get(std::string *str) {
    ProfilerCAPTURE();
    NameId res;
    if (str == NULL) {
        res.id = -1;
    }
    else {
        if (this->map.find(*str) != this->map.end()) {
            res.id = this->map[*str];
        }
        else {
            res.id                    = this->map.size();
            this->map[*str]           = res.id;
            this->reverse_map[res.id] = *str;
        }
    }
    res.token = NULL;
    res.str   = str;
    return res;
}

NameId NameIds::get(std::string str) {
    ProfilerCAPTURE();
    NameId res;
    if (this->map.find(str) != this->map.end()) {
        res.id = this->map[str];
    }
    else {
        res.id                    = this->map.size();
        this->map[str]            = res.id;
        this->reverse_map[res.id] = str;
    }

    res.token   = NULL;
    res.val_str = str;
    return res;
}

std::string NameIds::fromId(int64_t id) {
    ProfilerCAPTURE();
    auto it = this->reverse_map.find(id);
    if (it != reverse_map.end()) {
        return it->second;
    }
    return "[INVALID NAMEID]";
}

std::string NameIds::userRepr(int64_t id) {
    ProfilerCAPTURE();
    return this->fromId(id);
    // return "NameId(id = " + std::to_string(id) + ", str = \'" + NameId::fromId(id) + "\')";
}

};    // namespace Cotton
