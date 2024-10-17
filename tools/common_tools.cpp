#include <algorithm>
#include <string>
#include <vector>
#include <concepts>
#include <map>
#include <span>

void removeWhiteSpace(std::string &s){
    s.erase(std::remove(s.begin(), s.end(), ' '), s.end());
}

//Ensure that the span can use the at method
template <typename T>
concept AtMethod = requires(T span, const auto& key){
    { span.at(key) };
};

//sorts in ascending order
template <typename T>
void sortList(std::span <T> list){
    std::ranges::sort(list, std::less{});
}

//sorts in ascending order using a key
template <AtMethod T>
void sortList(std::span <T> list, const auto &key){
    std::ranges::sort(list, std::less{}, [&key](const T& mapElement) {
        return mapElement.at(key);
    });
}