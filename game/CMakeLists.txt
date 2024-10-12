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




# Include directories for the game project
include_directories(${CMAKE_CURRENT_LIST_DIR}/include)

# Add the required include directories for tree-sitter dependencies
include_directories(${CMAKE_SOURCE_DIR}/cpp-tree-sitter/include)
include_directories(${CMAKE_SOURCE_DIR}/tree-sitter-socialgaming/src)
include_directories(${CMAKE_SOURCE_DIR}/tree-sitter/lib/include)  # Ensure core tree-sitter headers are accessible

# Specify only the headers and source files for testing
set(GAME_HEADERS 
    ${CMAKE_CURRENT_LIST_DIR}/include/GameParser.h
)

set(GAME_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/src/GameParser.cpp
)

# Create the game library with the minimal set of files
add_library(game ${GAME_SOURCES} ${GAME_HEADERS})

# Link the `game` library with required dependencies
# Make sure to include the necessary tree-sitter libraries
target_link_libraries(game cpp-tree-sitter tree-sitter-socialgaming tree-sitter-json)
