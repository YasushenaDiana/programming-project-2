#pragma once
#include <vector>
#include <string>
#include <cstddef>

using Data = std::vector<double>;

std::string fileName(std::size_t n);
void prepareFiles(const std::vector<std::size_t> &sizes);
Data load(std::size_t n);
std::vector<Data> loadAll(const std::vector<std::size_t> &sizes);