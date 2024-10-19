cmake_minimum_required(VERSION 3.28.3)
project(TreeSitterTry)

# Set the C++ standard to 23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)


include(cmake/CPM.cmake)

CPMAddPackage(
  NAME cpp-tree-sitter
  GIT_REPOSITORY https://github.com/nsumner/cpp-tree-sitter.git
  GIT_TAG v0.0.1
)

add_grammar_from_repo(tree-sitter-json                 # Defines the library name for a grammar
  https://github.com/tree-sitter/tree-sitter-json.git  # Repository URL of a tree-sitter grammar
  0.19.0                                               # Version tag for the grammar
)

# Enable sanitizer flags if using Clang or GNU compilers
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(SANITIZER_FLAGS "-fsanitize=address,undefined")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SANITIZER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${SANITIZER_FLAGS}")
endif()

# Save outputs to the appropriate subdirectories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")

# Add the `game` subdirectory containing the game library configuration
add_subdirectory(game)

# Enable testing by default
option(BUILD_TESTING "Enable testing" ON)
# Enable testing if BUILD_TESTING is enabled
if (BUILD_TESTING)
    enable_testing()
    include(CTest)
    add_subdirectory(test)
endif()

# Add testing subdirectory for parser
add_subdirectory(tree-sitter-test)

# Link `game` with the tree-sitter library
# target_link_libraries(game cpp-tree-sitter tree-sitter-socialgaming tree-sitter-json)




