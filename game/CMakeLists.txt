# Include directories for the game project
include_directories(${CMAKE_CURRENT_LIST_DIR}/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../lib/web-socket-networking/lib/networking/include)  # Adjusted path to networking headers
include_directories(${CMAKE_CURRENT_LIST_DIR}/../cpp-tree-sitter/include)

# Collect all headers and source files
file(GLOB_RECURSE GAME_HEADERS "${CMAKE_CURRENT_LIST_DIR}/include/*.h")
file(GLOB_RECURSE GAME_SOURCES "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp")

# Create the game library
add_library(game ${GAME_SOURCES} ${GAME_HEADERS})

target_link_libraries(game cpp-tree-sitter networking)

# Create the RockPaperScissors executable
add_executable(RockPaperScissors
    src/main.cpp               
    src/rockPaperScissors.cpp
    src/gameConfig.cpp
    src/Player.cpp
)

# Link the game library to the executable
target_link_libraries(RockPaperScissors game)

