cmake_minimum_required(VERSION 3.28.3)
project(fahsa VERSION 0.1 LANGUAGES C CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Global variables accessible at compile time
set(PACKAGE_NAME fahsa)
set(PACKAGE_VERSION 0.1)
set(PACKAGE_STRING "${PACKAGE_NAME} ${PACKAGE_VERSION}")

# Set default build type if not set
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type (default Debug):" FORCE)
endif()

# Set Flags for the built make call
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fno-rtti -Wall -Wextra -Wconversion")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-rtti -Wall -Wextra -Wconversion \
        -fno-omit-frame-pointer -fsanitize=address,undefined")

# Save outputs to the appropriate subdirectories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")

# Add the cpp-tree-sitter subdirectory
add_subdirectory(cpp-tree-sitter)

# Add the `lib/web-socket-networking` subdirectory
add_subdirectory(lib/web-socket-networking)

# Add the `game` subdirectory
add_subdirectory(game)

# Enable testing if BUILD_TESTING is enabled
if (BUILD_TESTING)
    enable_testing()
    # Uncomment the next line if you have tests to include
    # add_subdirectory(test)
endif()

