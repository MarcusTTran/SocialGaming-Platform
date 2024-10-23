# Specify the include and source directories
set(GAME_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
set(GAME_SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

# Specify the header and source files
file(GLOB GAME_HEADERS "${GAME_INCLUDE_DIR}/*.h")
file(GLOB GAME_SOURCES "${GAME_SRC_DIR}/*.cpp")

include_directories(${GAME_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/lib/networking/include)

# Create the parser library
add_library(game_parser ${GAME_SOURCES} ${GAME_HEADERS})

# Include directories for the game project
target_include_directories(
    game_parser PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
    ${CMAKE_SOURCE_DIR}/lib/networking/include
)

target_link_libraries(game_parser
    tree-sitter-json
    cpp-tree-sitter
    networking  
)

set_target_properties(game_parser PROPERTIES
    PUBLIC_HEADER "include/GameParser.h"
)

# Specify the source file for the main executable
set(MAIN_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp
)

# Create the main executable
add_executable(main ${MAIN_SOURCES})

# Link the main executable with the game_parser library
target_link_libraries(main 
    game_parser
)

# Ensure the include directory is available for the main executable
target_include_directories(
    main PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
)






