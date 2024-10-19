# # Include directories for the game project
# include_directories(${CMAKE_CURRENT_LIST_DIR}/include)
# include_directories(${CMAKE_CURRENT_LIST_DIR}/../lib/web-socket-networking/lib/networking/include)  # Adjusted path to networking headers
# include_directories(${CMAKE_CURRENT_LIST_DIR}/../cpp-tree-sitter/include)

# # Collect all headers and source files
# file(GLOB_RECURSE GAME_HEADERS "${CMAKE_CURRENT_LIST_DIR}/include/*.h")
# file(GLOB_RECURSE GAME_SOURCES "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp")

# # Create the game library
# add_library(game ${GAME_SOURCES} ${GAME_HEADERS})


# target_link_libraries(game cpp-tree-sitter networking tree-sitter-json)

# # Create the RockPaperScissors executable
# add_executable(RockPaperScissors
#     game/src/main.cpp               
#     game/src/gameConfig.cpp
#     game/src/Player.cpp
# )

# # Link the game library to the executable
# target_link_libraries(RockPaperScissors game)


# Specify only the headers and source files for testing
set(GAME_HEADERS 
    ${CMAKE_CURRENT_LIST_DIR}/include/GameParser.h
)

set(GAME_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/src/GameParser.cpp
)

# Create the parser library
add_library(game_parser STATIC ${GAME_SOURCES} ${GAME_HEADERS})

# Include directories for the game project
target_include_directories(
    game_parser PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
)

target_link_libraries(game_parser
    tree-sitter-json
    cpp-tree-sitter  
)

set_target_properties(game_parser PROPERTIES
    PUBLIC_HEADER "include/GameParser.h"
)

# For creating main.cpp in /src 

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






