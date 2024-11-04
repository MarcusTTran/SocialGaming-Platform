include_directories(${CMAKE_SOURCE_DIR}/game/include)



add_executable(game_server
  chatserver.cpp
)

set_target_properties(game_server
                      PROPERTIES
                      LINKER_LANGUAGE CXX
                      CXX_STANDARD 23
                      PREFIX ""
)

find_package(Threads REQUIRED)

target_link_libraries(game_server
  networking
  game_parser
  ${CMAKE_THREAD_LIBS_INIT}
)

# Specify the output directory for the executable during the build
set_target_properties(game_server PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

install(TARGETS game_server
  RUNTIME DESTINATION bin
)

