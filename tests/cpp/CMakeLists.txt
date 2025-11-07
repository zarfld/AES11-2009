add_executable(aes11_tests
  test_timesrc_snapshot.cpp
  test_sync_selection.cpp
  test_timing_window_processor.cpp
  test_dars_protocol.cpp
  test_capture_range.cpp
)

target_link_libraries(aes11_tests
  PRIVATE
    aes11_standards
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(aes11_tests)
