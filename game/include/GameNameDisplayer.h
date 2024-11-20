#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <algorithm>
#include <sstream>
#include <map>


std::string gameNameDisplayer();
std::map<int,std::string>& getConfigMap();
std::string gameNameExtraction(std::string configFolder);