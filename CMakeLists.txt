cmake_minimum_required(VERSION 3.20)

project(AES11_2009 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_TESTS "Build C++ tests" ON)

add_library(aes11_standards STATIC
    lib/Standards/AES/AES11/2009/core/timing_snapshot_service.cpp
  lib/Standards/AES/AES11/2009/core/timing_window_processor.cpp
  lib/Standards/AES/AES11/2009/core/dars_protocol.cpp
  lib/Standards/AES/AES11/2009/sync/synchronization_manager.cpp
  lib/Standards/AES/AES11/2009/core/capture_range.cpp
  lib/Standards/AES/AES11/2009/core/phase_tolerance.cpp
)

target_include_directories(aes11_standards PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/Standards
)

if(BUILD_TESTS)
  include(FetchContent)
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
  )
  # For Windows: Prevent overriding the parent project's compiler/linker settings
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)

  enable_testing()
  add_subdirectory(tests/cpp)
endif()
