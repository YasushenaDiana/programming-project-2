#pragma once
#include <iostream>
#include <format>
#include <string>
#include <vector>
#include <algorithm>
#include <execution>
#include <thread>
#include <chrono>
#include <cmath>
#include "data.h"

struct FastOp
{
    double operator()(double x) const
    {
        const double mult = 2.0;
        const double add = 1.0;
        return x * mult + add;
    }
};

struct SlowOp
{
    double operator()(double x) const
    {
        const double shift = 1.0;
        return std::sqrt(std::abs(std::sin(x)) + std::cos(x) * std::cos(x)) +
               std::log(std::abs(x) + shift) * std::atan(x);
    }
};

int threadsCount();
std::vector<int> kValues();
void printHeader(const std::string &first, const std::vector<Data> &sets);
void printRow(const std::string &name, const std::vector<double> &times);
void printBestK(const std::vector<int> &ks, const std::vector<std::vector<double>> &times,
                const std::vector<Data> &sets);
void compareOps(const std::vector<Data> &sets, const std::vector<double> &fast, const std::vector<double> &slow);

template <class F>
double timeit(F &&f, int repeats)
{
    double total = 0;
    for (int i = 0; i < repeats; ++i)
    {
        auto start = std::chrono::steady_clock::now();
        f();
        auto finish = std::chrono::steady_clock::now();
        total += std::chrono::duration<double, std::milli>(finish - start).count();
    }
    return total / repeats;
}

template <class Op>
void parallelTransform(const Data &data, Data &result, Op op, int k)
{
    std::vector<Data> parts(k);
    std::size_t n = data.size();
    {
        std::vector<std::jthread> threads;
        for (int i = 0; i < k; ++i)
        {
            std::size_t from = n * i / k;
            std::size_t to = n * (i + 1) / k;
            threads.emplace_back([&data, &parts, op, from, to, i] {
                parts[i].resize(to - from);
                std::transform(data.begin() + from, data.begin() + to, parts[i].begin(), op);
            });
        }
    }
    auto pos = result.begin();
    for (const auto &part : parts)
        pos = std::copy(part.begin(), part.end(), pos);
}

template <class F>
double measure(F &&run, const Data &expected, int repeats, bool &correct)
{
    Data result(expected.size());
    double time = timeit([&] { run(result); }, repeats);
    if (result != expected)
        correct = false;
    return time;
}

template <class Policy, class Op>
double timePolicy(const Policy &policy, const Data &data, Op op, const Data &expected, int repeats, bool &correct)
{
    return measure([&](Data &result) { std::transform(policy, data.begin(), data.end(), result.begin(), op); },
                   expected, repeats, correct);
}

template <class Op>
std::vector<double> libraryTable(const std::vector<Data> &sets, Op op, const std::vector<Data> &expected, int repeats)
{
    bool correct = true;
    std::vector<double> noPolicy, seq, par, unseq, parUnseq;
    for (std::size_t i = 0; i < sets.size(); ++i)
    {
        const Data &data = sets[i];
        noPolicy.push_back(measure([&](Data &result) { std::transform(data.begin(), data.end(), result.begin(), op); },
                                   expected[i], repeats, correct));
        seq.push_back(timePolicy(std::execution::seq, data, op, expected[i], repeats, correct));
        par.push_back(timePolicy(std::execution::par, data, op, expected[i], repeats, correct));
        unseq.push_back(timePolicy(std::execution::unseq, data, op, expected[i], repeats, correct));
        parUnseq.push_back(timePolicy(std::execution::par_unseq, data, op, expected[i], repeats, correct));
    }
    std::cout << "Library transform, time in ms:\n";
    printHeader("policy", sets);
    printRow("no policy", noPolicy);
    printRow("seq", seq);
    printRow("par", par);
    printRow("unseq", unseq);
    printRow("par_unseq", parUnseq);
    std::cout << std::format("  all results correct: {}\n\n", correct);
    return noPolicy;
}

template <class Op>
void ownTable(const std::vector<Data> &sets, Op op, const std::vector<Data> &expected, int repeats)
{
    bool correct = true;
    std::vector<int> ks = kValues();
    std::vector<std::vector<double>> times(ks.size());
    for (std::size_t j = 0; j < ks.size(); ++j)
        for (std::size_t i = 0; i < sets.size(); ++i)
            times[j].push_back(measure([&](Data &result) { parallelTransform(sets[i], result, op, ks[j]); },
                                       expected[i], repeats, correct));
    std::cout << "Own parallel transform, time in ms:\n";
    printHeader("K", sets);
    for (std::size_t j = 0; j < ks.size(); ++j)
        printRow(std::to_string(ks[j]), times[j]);
    std::cout << std::format("  all results correct: {}\n", correct);
    printBestK(ks, times, sets);
}

template <class Op>
std::vector<double> research(const std::string &opName, Op op, const std::vector<Data> &sets, int repeats)
{
    std::vector<Data> expected;
    for (const auto &data : sets)
    {
        Data result(data.size());
        std::transform(data.begin(), data.end(), result.begin(), op);
        expected.push_back(result);
    }
    std::cout << std::format("\n===== {} operation =====\n", opName);
    std::vector<double> noPolicy = libraryTable(sets, op, expected, repeats);
    ownTable(sets, op, expected, repeats);
    return noPolicy;
}