#include <chrono>
#include <vector>
#include <unordered_map>
#include <string>
#include <filesystem>

namespace BT {
using Price = std::unordered_map<std::string, double>;
using Target = std::unordered_map<std::string, double>;
using Return = std::vector<double>;
using Path = std::filesystem::path;
}