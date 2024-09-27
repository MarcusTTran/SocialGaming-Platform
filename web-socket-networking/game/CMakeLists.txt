add_executable(game_server rock_paper_scissors_server.cpp Player.cpp)

# Link the networking library
target_link_libraries(game_server PRIVATE networking)

# Add include directories for the networking library
target_include_directories(game_server PRIVATE ${PROJECT_SOURCE_DIR}/lib/networking/include)

# Link directories where networking is located
target_link_directories(game_server PRIVATE ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})

# Optionally link Boost if required
find_package(Boost 1.72 REQUIRED)
target_include_directories(game_server PRIVATE ${Boost_INCLUDE_DIRS})

