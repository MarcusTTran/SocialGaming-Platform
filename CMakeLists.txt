cmake_minimum_required(VERSION 3.28.3)
project(TreeSitterTry)

# Set the C++ standard to 23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the necessary subdirectories for external dependencies
add_subdirectory(cpp-tree-sitter)
add_subdirectory(tree-sitter-socialgaming)

# Include directories for the external dependencies
include_directories(${CMAKE_SOURCE_DIR}/cpp-tree-sitter/include)
include_directories(${CMAKE_SOURCE_DIR}/tree-sitter-socialgaming/src)

# Include directory for the game's header files
include_directories(${CMAKE_SOURCE_DIR}/game/include)
#include_directories(${CMAKE_SOURCE_DIR}/tree-sitter/lib/include)

# Enable sanitizer flags if using Clang or GNU compilers
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(SANITIZER_FLAGS "-fsanitize=address,undefined")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SANITIZER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${SANITIZER_FLAGS}")
endif()

add_grammar_from_repo(tree-sitter-json                 # Defines the library name for a grammar
  https://github.com/tree-sitter/tree-sitter-json.git  # Repository URL of a tree-sitter grammar
  0.19.0                                               # Version tag for the grammar
)

# Add the `game` subdirectory containing the game library configuration
add_subdirectory(game)

# Link `game` with the tree-sitter library
target_link_libraries(game cpp-tree-sitter tree-sitter-socialgaming tree-sitter-json)

# Create the main executable, including only the necessary source files
add_executable(main game/src/main.cpp)


