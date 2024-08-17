#include "../cotton_lib/src/cotton_lib/api.h"
#include <chrono>
#include <random>
using namespace Cotton;

static Object *randint64(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    rt->verifyExactArgsAmountMethod(args, 2);
    auto low = args[1];
    auto high = args[2];
    rt->verifyIsInstanceObject(low, rt->builtin_types.integer, Runtime::SUB1_CTX);
    rt->verifyIsInstanceObject(high, rt->builtin_types.integer, Runtime::SUB2_CTX);
    static std::mt19937_64 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    auto res = std::uniform_int_distribution<int64_t>(getIntegerValueFast(low), getIntegerValueFast(high))(rng);
    return Builtin::makeIntegerInstanceObject(res, rt);
}

extern "C" Object *library_load_point(Runtime *rt) {
    auto record = Builtin::makeRecordType(rt->nmgr->getId("__mt19997_64"), rt);
    record->addMethod(rt->nmgr->getId("randint64"), Builtin::makeFunctionInstanceObject(true, randint64, nullptr, rt));
    return rt->make(record, Runtime::INSTANCE_OBJECT);
}
