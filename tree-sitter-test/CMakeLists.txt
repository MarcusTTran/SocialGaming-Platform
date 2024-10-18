add_executable(ts_test main.cpp)

target_link_libraries(ts_test 
    tree-sitter-json
    cpp-tree-sitter 
)

# set_target_properties(ts_test
#                       PROPERTIES
#                       LINKER_LANGUAGE CXX
#                       CXX_STANDARD 17
#                       PREFIX ""
# )

install(TARGETS ts_test
  RUNTIME DESTINATION bin
)
