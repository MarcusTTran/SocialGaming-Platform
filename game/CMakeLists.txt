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
    ${fmt_SOURCE_DIR}/include  
)

# Check if tree-sitter-socialgaming target exists
if (NOT TARGET tree-sitter-socialgaming)
    add_library(tree-sitter-socialgaming INTERFACE)
    target_include_directories(tree-sitter-socialgaming INTERFACE ${tree-sitter-socialgaming_SOURCE_DIR}/src)
    target_sources(tree-sitter-socialgaming INTERFACE ${tree-sitter-socialgaming_SOURCE_DIR}/src/parser.c)
endif()

# Link with required libraries
target_link_libraries(game_parser
    tree-sitter-json
    cpp-tree-sitter
    tree-sitter-socialgaming  
    networking
    fmt 
)


# Properties for the game_parser library
set_target_properties(game_parser PROPERTIES
    PUBLIC_HEADER "include/GameParser.h"
)

# Specify the source files for the main executable
set(MAIN_SOURCES
    ${GAME_SRC_DIR}/main.cpp
    ${GAME_SRC_DIR}/GameParser.cpp
    ${GAME_SRC_DIR}/ParserRulesAPI.cpp
    ${GAME_SRC_DIR}/RulesRunner.cpp
    #${GAME_SRC_DIR}/testOPmain.cpp
)

# Main executable
add_executable(main ${MAIN_SOURCES})

# Link the main executable with the game_parser library and other dependencies
target_link_libraries(main 
    game_parser
    cpp-tree-sitter
    tree-sitter-socialgaming
    fmt
)

# Ensure the include directory is available for the main executable
target_include_directories(
    main PUBLIC
    ${GAME_INCLUDE_DIR}
)

# Set output directory for the main executable
set_target_properties(main PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
