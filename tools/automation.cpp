#include <iostream>
#include <vector>
#include <c++/11/bits/algorithmfwd.h>
#include <random>


void shuffleList(std::vector<std::string>& list){

    //randome device used to seed mt19937, use mt due to performance issues from just using random_device.
    std::random_device rd;
    std::mt19937 g(rd());

   std::shuffle (list.begin(),list.end(),g);

}