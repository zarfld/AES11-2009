add_executable(aes11_tests
  test_timesrc_snapshot.cpp
  test_sync_selection.cpp
  test_timing_window_processor.cpp
  test_dars_protocol.cpp
  test_capture_range.cpp
  test_phase_tolerance.cpp
  test_gps_reference_sync.cpp
  test_channel_status_utils.cpp
  test_channel_status_bitfields.cpp
  test_aes3_adapter.cpp
  test_channel_status_annex_a.cpp
  test_datetime_channel_status.cpp
  test_trp_detection_scan.cpp
  test_sample_rate_validation.cpp
  test_aes5_adapter.cpp
)

target_link_libraries(aes11_tests
  PRIVATE
    aes11_standards
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(aes11_tests)
