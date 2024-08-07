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

std::string const NamesManager::failed = "[INVALID NAMEID]";

NameId NamesManager::getId(const std::string &str) {
    ProfilerCAPTURE();
    auto it = this->data.find(str);
    if (it != this->data.end()) {
        return it->second;
    }
    auto new_id                 = (NameId)this->data.size() + 1;
    this->data[str]             = new_id;
    this->data_reversed[new_id] = str;
    return new_id;
}

std::string const &NamesManager::getString(NameId id) {
    ProfilerCAPTURE();
    auto it = this->data_reversed.find(id);
    if (it == this->data_reversed.end()) {
        return this->failed;
    }
    return it->second;
}

bool NamesManager::check(NameId id) {
    ProfilerCAPTURE();
    auto it = this->data_reversed.find(id);
    return it != this->data_reversed.end();
}
};    // namespace Cotton
