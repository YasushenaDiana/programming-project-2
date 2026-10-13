#include "data.h"
#include <fstream>
#include <random>
#include <format>

using namespace std;

string fileName(size_t n)
{
    return format("data_{}.txt", n);
}

void generate(size_t n, mt19937 &gen)
{
    const double minValue = -1000.0;
    const double maxValue = 1000.0;
    uniform_real_distribution<double> dist(minValue, maxValue);
    ofstream f(fileName(n));
    f << n << '\n';
    for (size_t i = 0; i < n; ++i)
        f << dist(gen) << '\n';
}

void prepareFiles(const vector<size_t> &sizes)
{
    mt19937 gen{random_device()()};
    for (auto n : sizes)
    {
        ifstream test(fileName(n));
        if (!test)
            generate(n, gen);
    }
}

Data load(size_t n)
{
    ifstream f(fileName(n));
    size_t count = 0;
    f >> count;
    Data data(count);
    for (auto &x : data)
        f >> x;
    return data;
}

vector<Data> loadAll(const vector<size_t> &sizes)
{
    vector<Data> sets;
    for (auto n : sizes)
        sets.push_back(load(n));
    return sets;
}