#include "experiments.h"

using namespace std;

int threadsCount()
{
    return max(1, int(thread::hardware_concurrency()));
}

vector<int> kValues()
{
    const int maxK = 256;
    const int stepFactor = 2;
    int threads = threadsCount();
    vector<int> ks;
    for (int k = 1; k <= threads && k <= maxK; ++k)
        ks.push_back(k);
    for (int k = ks.back() * stepFactor; k <= maxK; k *= stepFactor)
        ks.push_back(k);
    return ks;
}

void printHeader(const string &first, const vector<Data> &sets)
{
    cout << format("  {:<12}", first);
    for (const auto &data : sets)
        cout << format("{:>16}", format("n = {}", data.size()));
    cout << '\n';
}

void printRow(const string &name, const vector<double> &times)
{
    cout << format("  {:<12}", name);
    for (double time : times)
        cout << format("{:>16.4f}", time);
    cout << '\n';
}

void printBestK(const vector<int> &ks, const vector<vector<double>> &times, const vector<Data> &sets)
{
    int threads = threadsCount();
    for (size_t i = 0; i < sets.size(); ++i)
    {
        size_t best = 0;
        for (size_t j = 1; j < ks.size(); ++j)
            if (times[j][i] < times[best][i])
                best = j;
        cout << format("  n = {}: best K = {} ({:.4f} ms), hardware threads = {}, K / threads = {:.2f}\n",
                       sets[i].size(), ks[best], times[best][i], threads, double(ks[best]) / threads);
    }
}

void compareOps(const vector<Data> &sets, const vector<double> &fast, const vector<double> &slow)
{
    cout << "\n===== Fast vs slow operation (no policy) =====\n";
    cout << format("  {:>10}{:>16}{:>16}{:>12}\n", "n", "fast, ms", "slow, ms", "slow/fast");
    for (size_t i = 0; i < sets.size(); ++i)
        cout << format("  {:>10}{:>16.4f}{:>16.4f}{:>12.1f}\n", sets[i].size(), fast[i], slow[i], slow[i] / fast[i]);
}