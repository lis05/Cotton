#include "../cotton_lib/src/cotton_lib/api.h"
#include <cstring>
#include <filesystem>
using namespace Cotton;

static Object *recursive(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    rt->verifyExactArgsAmountMethod(args, 1);
    auto                  self = args[0];
    auto                  arg  = args[1];
    std::vector<Object *> res;
    rt->verifyIsInstanceObject(arg, rt->builtin_types.string, MethodArgCtx(0));
    for (auto const &dir_entry : std::filesystem::recursive_directory_iterator(getStringDataFast(arg))) {
        res.push_back(Builtin::makeStringInstanceObject(dir_entry.path().string(), rt));
    }

    return Builtin::makeArrayInstanceObject(res, rt);
}

extern "C" Object *library_load_point(Runtime *rt) {
    auto record = Builtin::makeRecordType(rt->nmgr->getId("Glob"), rt);
    record->addMethod(rt->nmgr->getId("recursive"), Builtin::makeFunctionInstanceObject(true, recursive, nullptr, rt));
    return rt->make(record, Runtime::INSTANCE_OBJECT);
}
