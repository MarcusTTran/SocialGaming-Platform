add_subdirectory(gtest)

add_executable(runAllTests
Dummy.cpp
# Add test file names here
)

set_target_properties(runAllTests
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test"
)

set_target_properties(runAllTests
  PROPERTIES
  LINK_FLAGS "${CXX_COVERAGE_FLAGS}"
)

target_link_libraries(runAllTests
  PRIVATE
    GTest::gmock GTest::gtest GTest::gtest_main
    game
    # Add libraries to test here
)

add_test(NAME AllTests
  COMMAND ${CMAKE_CURRENT_BINARY_DIR}/runAllTests
)

