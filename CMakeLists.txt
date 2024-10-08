cmake_minimum_required(VERSION 3.19)
project(TreeSitterTry)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(cpp-tree-sitter)

add_subdirectory(tree-sitter-socialgaming)

include_directories(${CMAKE_SOURCE_DIR}/cpp-tree-sitter/include)
include_directories(${CMAKE_SOURCE_DIR}/tree-sitter-socialgaming/src)

add_executable(main main.cpp)

target_link_libraries(main cpp-tree-sitter tree-sitter-socialgaming)
