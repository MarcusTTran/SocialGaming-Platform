
cmake_minimum_required(VERSION 3.28.3)
project(fahsa VERSION 0.1 LANGUAGES CXX)

# These will be used _________ {where?}
set(PACKAGE_NAME fahsa)
set(PACKAGE_VERSION 0.1)
set(PACKAGE_STRING "${PACKAGE_NAME} ${PACKAGE_VERSION}")

# You don't have to declare build type for debug mode
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE
        STRING "Build type (default Debug):" FORCE)
endif()

# _____________ {meaning of flags?}
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fno-rtti -Wall -Wextra -Wconversion")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-rtti -Wall -Wextra -Wconversion \
        -fno-omit-frame-pointer -fsanitize=address,undefined")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")

#add_subdirectory(lib)

add_executable(fahsa main.cpp)

if (BUILD_TESTING)
    enable_testing()
    #add_subdirectory(test)
endif()
