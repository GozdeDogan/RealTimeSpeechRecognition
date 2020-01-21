#include "timestamp.h"
#include <iostream>

using namespace std;

void _timestamp(const size_t line, const std::string file) {
    static auto then = chrono::high_resolution_clock::now();
    const auto now = chrono::high_resolution_clock::now();

    cout << file << ':' << line << '\t' << (now - then).count() << '\n';

    then = now;
}
