#include "profiler.h"
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <iostream>

namespace Cotton::Profiler {

std::unordered_map<const char *, int> captures;


void capture(const char *name) {
    captures[name]++;
}

void printResult() {
    std::vector<std::pair<int, const char *>> res;
    for (auto [name, num]: captures) res.push_back({num, name});
    std::sort(res.begin(), res.end(), std::less<std::pair<int, const char*>>());

    for (auto [num, name]: res) {
        std::cout << num << ": " << name << "\n";
    }
}

}    // namespace Cotton::Profiler
