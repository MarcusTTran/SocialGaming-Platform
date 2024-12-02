// #include <iostream>
// #include <map>
// #include <vector>
// #include <memory>
// #include <string>
// #include <unordered_map>
// #include <variant>
// #include "GameConfiguration.h"
// #include "optionsDisplay.h"

// std::unique_ptr<ConfigurationOptions> createConfigurationOptions(const GameConfiguration* object)
// {

//     for(auto& i:object->getSetup()){
//         std::cout<<i.kind.value();
//          if (i.kind.value() == "enum")
//     {
//         return std::make_unique<EnumConfigurationOptions>(i);
//     }
//     // else if (i.kind == "boolean")
//     // {
//     //     return std::make_unique<BooleanConfigurationOptions>(i);
//     // }
//     else if (i.kind.value() == "integer")
//     {
//         return std::make_unique<IntegerConfigurationOptions>(i);
//     }

//     throw std::invalid_argument("Unsupported kind");
//     }
   
// }
