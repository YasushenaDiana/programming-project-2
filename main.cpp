//Diana Yasushenaʼs Compiler: Apple Clang 21.0.0 (CLion, macOS), C++23
#include <iostream>
#include <format>
#include <vector>
#include "data.h"
#include "experiments.h"

using namespace std;

int main()
{
    const vector<size_t> sizes = {10'000, 100'000, 1'000'000};
    const int repeats = 5;

#ifdef NDEBUG
    cout << "Build: Release\n";
#else
    cout << "Build: Debug\n";
#endif
#ifdef __OPTIMIZE__
    cout << "Optimization: on\n";
#else
    cout << "Optimization: off\n";
#endif
    cout << format("Hardware threads: {}\n", threadsCount());
    cout << format("Each time is an average of {} runs\n", repeats);
    cout << "Fast operation: x * 2 + 1\n";
    cout << "Slow operation: sqrt(|sin x| + cos^2 x) + ln(|x| + 1) * atan x\n";

    prepareFiles(sizes);
    vector<Data> sets = loadAll(sizes);

    vector<double> fast = research("Fast", FastOp(), sets, repeats);
    vector<double> slow = research("Slow", SlowOp(), sets, repeats);
    compareOps(sets, fast, slow);
    return 0;
}