cmake_minimum_required(VERSION 3.28.3)
project(TreeSitterTry)

# Set the C++ standard to 23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(cmake/CPM.cmake)

# Add cpp-tree-sitter
CPMAddPackage(
  NAME cpp-tree-sitter
  GIT_REPOSITORY https://github.com/nsumner/cpp-tree-sitter.git
  GIT_TAG main # version 0.0.3
)

# Ensure cpp-tree-sitter uses C++23
if (cpp-tree-sitter_ADDED)
  set_target_properties(cpp-tree-sitter PROPERTIES 
  CXX_STANDARD 23 
  CXX_STANDARD_REQUIRED YES
  )
endif()

# Add tree-sitter-socialgaming
CPMAddPackage(
  NAME tree-sitter-socialgaming
  GIT_REPOSITORY https://github.com/nsumner/tree-sitter-socialgaming.git
  GIT_TAG main  
)

CPMAddPackage(
    NAME fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 8.1.1  # Specify a compatible version, adjust if necessary
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

# Add your other subdirectories as before
add_subdirectory(lib/networking)
add_subdirectory(game)
add_subdirectory(server)

# Link `game_parser` with tree-sitter libraries
target_link_libraries(game_parser
    tree-sitter-json
    cpp-tree-sitter
    tree-sitter-socialgaming
    networking
)

# Set rpath to include the Boost library path
set(CMAKE_BUILD_RPATH "/usr/local/boost_1_82_0/lib")
