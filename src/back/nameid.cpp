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
__gnu_pbds::cc_hash_table<std::string, int64_t> NameId::map;
__gnu_pbds::cc_hash_table<int64_t, std::string> NameId::reverse_map;

NameId::NameId() {
    ProfilerCAPTURE();
    this->id    = -1;
    this->token = NULL;
    this->str   = NULL;
}

NameId::NameId(Token *token) {
    ProfilerCAPTURE();
    if (token == NULL) {
        this->id = -1;
    }
    else {
        if (NameId::map.find(token->data) != NameId::map.end()) {
            this->id = NameId::map[token->data];
        }
        else {
            this->id                      = NameId::map.size();
            NameId::map[token->data]      = this->id;
            NameId::reverse_map[this->id] = token->data;
        }
    }
    this->token = token;
    this->str   = &this->token->data;
}

NameId::NameId(std::string *str) {
    ProfilerCAPTURE();
    if (str == NULL) {
        this->id = -1;
    }
    else {
        if (NameId::map.find(*str) != NameId::map.end()) {
            this->id = NameId::map[*str];
        }
        else {
            this->id                      = NameId::map.size();
            NameId::map[*str]             = this->id;
            NameId::reverse_map[this->id] = *str;
        }
    }
    this->token = NULL;
    this->str   = str;
}

NameId::NameId(std::string str) {
    ProfilerCAPTURE();
    if (NameId::map.find(str) != NameId::map.end()) {
        this->id = NameId::map[str];
    }
    else {
        this->id                      = NameId::map.size();
        NameId::map[str]              = this->id;
        NameId::reverse_map[this->id] = str;
    }

    this->token   = NULL;
    this->val_str = str;
}

NameId::~NameId() {
    ProfilerCAPTURE();
    this->id    = -1;
    this->token = NULL;
}

std::string NameId::fromId(int64_t id) {
    ProfilerCAPTURE();
    auto it = NameId::reverse_map.find(id);
    if (it != reverse_map.end()) {
        return it->second;
    }
    return "invalid name id";
}

std::string NameId::shortRepr(int64_t id) {
    ProfilerCAPTURE();
    return fromId(id);
    // return "NameId(id = " + std::to_string(id) + ", str = \'" + NameId::fromId(id) + "\')";
}

};    // namespace Cotton
