#include "../cotton_lib/src/cotton_lib/api.h"
using namespace Cotton;

static Object *enable(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    rt->verifyExactArgsAmountMethod(args, 0);

    rt->gc->enable();
    return rt->protected_nothing;
}

static Object *disable(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    rt->verifyExactArgsAmountMethod(args, 0);

    rt->gc->disable();
    return rt->protected_nothing;
}

static Object *status(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    rt->verifyExactArgsAmountMethod(args, 0);

    return rt->protectedBoolean(rt->gc->enabled);
}

static Object *ping(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    rt->verifyExactArgsAmountMethod(args, 0);

    rt->gc->ping(rt);
    return rt->protected_nothing;
}

static Object *forceping(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    rt->verifyExactArgsAmountMethod(args, 0);

    auto status = rt->gc->enabled;
    rt->gc->enable();
    rt->gc->ping(rt);
    rt->gc->enabled = status;

    return rt->protected_nothing;
}

extern "C" Object *library_load_point(Runtime *rt) {
    auto record = Builtin::makeRecordType(rt->nmgr->getId("GC"), rt);
    record->addMethod(rt->nmgr->getId("enable"), Builtin::makeFunctionInstanceObject(true, enable, NULL, rt));
    record->addMethod(rt->nmgr->getId("disable"), Builtin::makeFunctionInstanceObject(true, disable, NULL, rt));
    record->addMethod(rt->nmgr->getId("status"), Builtin::makeFunctionInstanceObject(true, status, NULL, rt));
    record->addMethod(rt->nmgr->getId("ping"), Builtin::makeFunctionInstanceObject(true, ping, NULL, rt));
    record->addMethod(rt->nmgr->getId("forceping"),
                      Builtin::makeFunctionInstanceObject(true, forceping, NULL, rt));
    return rt->make(record, Runtime::INSTANCE_OBJECT);
}
