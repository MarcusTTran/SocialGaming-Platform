cmake_minimum_required(VERSION 3.28.3)
project(fahsa VERSION 0.1 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Global variables accessible at compile time
set(PACKAGE_NAME fahsa)
set(PACKAGE_VERSION 0.1)
set(PACKAGE_STRING "${PACKAGE_NAME} ${PACKAGE_VERSION}")

# You don't have to declare build type for debug mode
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE
        STRING "Build type (default Debug):" FORCE)
endif()

include(cmake/CPM.cmake)

# Downloads SocialGaming file library and a wrapper API for it via GitHub
# Makes them available to our project via the `cpp-tree-sitter` CMake library target.
CPMAddPackage(
  NAME cpp-tree-sitter
  GIT_REPOSITORY https://github.com/nsumner/cpp-tree-sitter.git
  GIT_TAG v0.0.1
)

# Downloads a tree-sitter grammar from github and makes it available as a
# cmake library target.
add_grammar_from_repo(tree-sitter-json                 # Defines the library name for a grammar
  https://github.com/tree-sitter/tree-sitter-json.git  # Repository URL of a tree-sitter grammar
  0.19.0                                               # Version tag for the grammar
)

# Set Flags for the built make call
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fno-rtti -Wall -Wextra -Wconversion")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-rtti -Wall -Wextra -Wconversion \
        -fno-omit-frame-pointer -fsanitize=address,undefined")

# Save outputs to the appropriate subdirectories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")


 # FOR DEMO DELETE LATER
add_subdirectory(tree-sitter)

add_subdirectory(lib)
add_subdirectory(game)

if (BUILD_TESTING)
    enable_testing()
    #add_subdirectory(test)
endif()
