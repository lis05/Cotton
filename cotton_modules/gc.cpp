#include "../cotton_lib/src/cotton_lib/api.h"
using namespace Cotton;

static Object *enable(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    rt->verifyExactArgsAmountMethod(args, 0);

    rt->getGC()->enable();
    return rt->protectedNothing();
}

static Object *disable(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    rt->verifyExactArgsAmountMethod(args, 0);

    rt->getGC()->disable();
    return rt->protectedNothing();
}

static Object *status(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    rt->verifyExactArgsAmountMethod(args, 0);

    return rt->protectedBoolean(rt->getGC()->enabled);
}

static Object *ping(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    rt->verifyExactArgsAmountMethod(args, 0);

    rt->getGC()->ping(rt);
    return rt->protectedNothing();
}

static Object *forceping(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    rt->verifyExactArgsAmountMethod(args, 0);

    auto status = rt->getGC()->enabled;
    rt->getGC()->enable();
    rt->getGC()->ping(rt);
    rt->getGC()->enabled = status;

    return rt->protectedNothing();
}

extern "C" Object *library_load_point(Runtime *rt) {
    auto record = Builtin::makeRecordType(rt->nmgr->getId("GC"), rt);
    record->addMethod(rt->nmgr->getId("enable"), Builtin::makeFunctionInstanceObject(true, enable, nullptr, rt));
    record->addMethod(rt->nmgr->getId("disable"), Builtin::makeFunctionInstanceObject(true, disable, nullptr, rt));
    record->addMethod(rt->nmgr->getId("status"), Builtin::makeFunctionInstanceObject(true, status, nullptr, rt));
    record->addMethod(rt->nmgr->getId("ping"), Builtin::makeFunctionInstanceObject(true, ping, nullptr, rt));
    record->addMethod(rt->nmgr->getId("forceping"),
                      Builtin::makeFunctionInstanceObject(true, forceping, nullptr, rt));
    return rt->make(record, Runtime::INSTANCE_OBJECT);
}
