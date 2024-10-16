cmake_minimum_required(VERSION 3.28.3)
project(UserInteractionTest VERSION 0.1)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(user_interaction_test
    src/user_interaction.cpp
    src/user_interaction_test.cpp
)
