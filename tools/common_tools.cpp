#include <algorithm>
#include <string>
#include <vector>

std::string removeWhiteSpace(std::string s){
    s.erase(std::remove(s.begin(), s.end(), ' '), s.end());
    return s;
}

//sorts in ascending order
template <typename MapType>
void sortList(std::vector <MapType>&list){
    std::ranges::sort(list, std::less{});
}

//sorts in ascending order using a key
template <typename MapType>
void sortList(std::vector <MapType>&list, const auto &key){
    std::ranges::sort(list, std::less{}, [&key](const MapType& mapElement) {
        return mapElement.at(key);
    });
}