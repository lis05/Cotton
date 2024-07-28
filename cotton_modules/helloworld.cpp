#include "../cotton_lib/src/cotton_lib/api.h"
using namespace Cotton;

extern "C" Object* library_load_point(Runtime *rt) {
    return Builtin::makeStringInstanceObject("hello world", rt);
}