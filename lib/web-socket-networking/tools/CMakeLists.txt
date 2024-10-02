if (NETWORKING_EMSCRIPTEN_BUILD)
  message(STATUS "Skipping server and NCurses client under emscripten build.")
else()
  # Add server subdirectory
  if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/server")
    add_subdirectory(server)
  else()
    message(WARNING "Server directory not found in tools.")
  endif()

  # Add NCurses client if enabled and directory exists
  if (ENABLE_NCURSES_CLIENT)
    if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/client-ncurses")
      add_subdirectory(client-ncurses)
    else()
      message(WARNING "NCurses client directory not found in tools.")
    endif()
  endif()
endif()

# Add Flutter client if enabled and directory exists
if (ENABLE_FLUTTER_CLIENT)
  if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/client_flutter")
    add_subdirectory(client_flutter)
  else()
    message(WARNING "Flutter client directory not found in tools.")
  endif()
endif()

# Add FTXUI client if enabled and directory exists
if (ENABLE_FTXUI_CLIENT)
  if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/client-ftxui")
    add_subdirectory(client-ftxui)
  else()
    message(WARNING "FTXUI client directory not found in tools.")
  endif()
endif()

