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

#include "../../profiler.h"
#include "api.h"
#include "record.h"

namespace Cotton::Builtin {

RecordInstance::RecordInstance(Runtime *rt)
    : Instance(rt, sizeof(RecordInstance)) {
    ProfilerCAPTURE();
}

RecordInstance::~RecordInstance() {
    ProfilerCAPTURE();
}

Object *RecordInstance::selectField(NameId id, Runtime *rt) {
    ProfilerCAPTURE();
    auto it = this->fields.find(id);
    if (it != this->fields.end()) {
        return it->second;
    }
    rt->signalError(this->userRepr(rt) + "doesn't have field " + rt->nmgr->getString(id), rt->getContext().area);
}

bool RecordInstance::hasField(NameId id, Runtime *rt) {
    ProfilerCAPTURE();
    return this->fields.find(id) != this->fields.end();
}

void RecordInstance::addField(NameId id, Object *obj, Runtime *rt) {
    ProfilerCAPTURE();
    this->fields[id] = obj;
}

Instance *RecordInstance::copy(Runtime *rt) {
    ProfilerCAPTURE();
    return this;    // because record is a complex data type
}

size_t RecordInstance::getSize() {
    ProfilerCAPTURE();
    return sizeof(RecordInstance);
}

size_t RecordType::getInstanceSize() {
    ProfilerCAPTURE();
    return sizeof(RecordInstance);
}

std::string RecordInstance::userRepr(Runtime *rt) {
    ProfilerCAPTURE();
    if (this == nullptr) {
        return "Record(nullptr)";
    }
    return rt->nmgr->getString(this->nameid);
}

std::vector<Object *> RecordInstance::getGCReachable() {
    ProfilerCAPTURE();
    std::vector<Object *> res;
    for (auto field : this->fields) {
        res.push_back(field.second);
    }
    return res;
}

RecordType::RecordType(Runtime *rt)
    : Type(rt) {
    ProfilerCAPTURE();
}

Object *RecordType::create(Runtime *rt) {
    ProfilerCAPTURE();
    auto ins = new RecordInstance(rt);
    ins->nameid = this->nameid;
    for (auto f : this->instance_fields) {
        ins->addField(f, makeNothingInstanceObject(rt), rt);
    }
    Object *obj = new Object(true, ins, this, rt);

    return obj;
}

std::string RecordType::userRepr(Runtime *rt) {
    ProfilerCAPTURE();
    if (this == nullptr) {
        return "nullptr";
    }
    return rt->nmgr->getString(this->nameid);
}

// TODO: == and !=

Object *RecordType::copy(Object *obj, Runtime *rt) {
    ProfilerCAPTURE();
    rt->verifyIsValidObject(obj);
    if (obj->instance == nullptr) {
        return new Object(false, nullptr, this, rt);
    }
    auto ins = obj->instance->copy(rt);
    auto res = new Object(true, ins, this, rt);
    return res;
}

Object *makeRecordInstanceObject(Runtime *rt) {
    ProfilerCAPTURE();
    auto res = rt->make(rt->builtin_types.nothing, rt->INSTANCE_OBJECT);
    return res;
}

RecordType *makeRecordType(NameId nameid, Runtime *rt) {
    ProfilerCAPTURE();
    auto res    = new RecordType(rt);
    res->nameid = nameid;
    return res;
}
}    // namespace Cotton::Builtin
