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

#include <chrono>
#include <cotton_lib/api.h>
#include <cstring>
using namespace std::chrono;
using namespace Cotton;
using namespace Cotton::Builtin;

void emergency_error_exit() {
    exit(1);
}

// TODO: add --profiler flag

int main(int argc, char *argv[]) {
    bool  print_execution_time = false;
    char *file                 = NULL;

    for (int i = 1; i < argc; i++) {
        char *arg = argv[i];

        if (strcmp(arg, "--time") == 0) {
            print_execution_time = true;
            continue;
        }

        if (file != NULL) {
            fprintf(stderr, "Error: unexpected argument: %s\n", arg);
            exit(1);
        }

        file = arg;
    }
#ifndef DEFAULT_SOURCE_FILENAME
    if (file == NULL) {
        fprintf(stderr, "Expected a source file\n");
        exit(1);
    }
#endif

#ifdef DEFAULT_SOURCE_FILENAME
    if (file == NULL) {
        file = DEFAULT_SOURCE_FILENAME;
    }
#endif 

    ErrorManager em(emergency_error_exit);
    Lexer        lx(&em);
    Parser       pr(&em);

    auto tokens = lx.processFile(file);
    for (auto &token : tokens) {
        token.nameid = NameId(&token).id;
    }
    auto program = pr.parse(tokens);

    GCDefaultStrategy gcst;
    Runtime           rt(&gcst, &em);

    auto begin_time = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    rt.execute(program, false);
    auto end_time = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

    if (print_execution_time) {
        printf("TIME: %.3fsec\n", (end_time - begin_time) / 1000.0);
    }

    delete program;
}
