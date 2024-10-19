if (NETWORKING_EMSCRIPTEN_BUILD)
  message(STATUS "Skipping server and NCurses client under emscripten build.")
else()
  # Add server subdirectory
  if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/server")
    add_subdirectory(server)
  else()
    message(WARNING "Server directory not found in tools.")
  endif()
endif()

