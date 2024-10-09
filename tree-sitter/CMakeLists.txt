# Use the library in a demo program.
add_executable(demo
    demo.cpp
)

set_target_properties(demo
                      PROPERTIES
                      LINKER_LANGUAGE CXX
                      CXX_STANDARD 17
                      PREFIX ""
)

target_sources(demo
  PRIVATE
    demo.cpp
)

target_link_libraries(demo
  tree-sitter-json
  cpp-tree-sitter
)

install(TARGETS demo
  RUNTIME DESTINATION bin
)
