
#include <unordered_map>
#include <string>
#include <variant>
#include <vector>

using PairOfPairs = std::pair<std::pair<std::string, std::string>, std::pair<std::string, std::string>>;
using DataType = std::variant< std::string, bool, PairOfPairs,  std::vector<PairOfPairs>, std::unordered_map<std::string, std::string> >;

