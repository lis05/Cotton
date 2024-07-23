add_library(cotton_lib SHARED
src/cotton_lib/api.h
src/cotton_lib/errors.h 
src/cotton_lib/errors.cpp

src/cotton_lib/front/api.h
src/cotton_lib/front/lexer.h
src/cotton_lib/front/lexer.cpp
src/cotton_lib/front/parser.h
src/cotton_lib/front/parser.cpp

src/cotton_lib/back/api.h
src/cotton_lib/back/gc.h
src/cotton_lib/back/gc.cpp
src/cotton_lib/back/instance.h
src/cotton_lib/back/instance.cpp
src/cotton_lib/back/nameid.h
src/cotton_lib/back/nameid.cpp
src/cotton_lib/back/object.h
src/cotton_lib/back/object.cpp
src/cotton_lib/back/runtime.h
src/cotton_lib/back/runtime.cpp
src/cotton_lib/back/scope.h
src/cotton_lib/back/scope.cpp
src/cotton_lib/back/type.h
src/cotton_lib/back/type.cpp

src/cotton_lib/back/mempool.h
src/cotton_lib/back/mempool.cpp

src/cotton_lib/builtin/api.h

src/cotton_lib/builtin/types/api.h
src/cotton_lib/builtin/types/array.h
src/cotton_lib/builtin/types/array.cpp
src/cotton_lib/builtin/types/boolean.h
src/cotton_lib/builtin/types/boolean.cpp
src/cotton_lib/builtin/types/character.h
src/cotton_lib/builtin/types/character.cpp
src/cotton_lib/builtin/types/function.h
src/cotton_lib/builtin/types/function.cpp
src/cotton_lib/builtin/types/integer.h
src/cotton_lib/builtin/types/integer.cpp
src/cotton_lib/builtin/types/nothing.h
src/cotton_lib/builtin/types/nothing.cpp
src/cotton_lib/builtin/types/real.h
src/cotton_lib/builtin/types/real.cpp
src/cotton_lib/builtin/types/string.h
src/cotton_lib/builtin/types/string.cpp
src/cotton_lib/builtin/types/user_defined.h
src/cotton_lib/builtin/types/user_defined.cpp

src/cotton_lib/builtin/functions/api.h
src/cotton_lib/builtin/functions/functions.h
src/cotton_lib/builtin/functions/functions.cpp

src/cotton_lib/profiler.h
src/cotton_lib/profiler.cpp
)

target_include_directories(cotton_lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

