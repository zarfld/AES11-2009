cmake_minimum_required(VERSION 3.20)

project(AES11_2009 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_TESTS "Build C++ tests" ON)
option(ENABLE_COVERAGE "Enable code coverage instrumentation (GCC/Clang only)" OFF)
option(ENABLE_FAULT_INJECTION "Enable AES11 fault injection hooks for testing" OFF)

# Optional coverage flags for GCC/Clang; no effect on MSVC/Windows
if(ENABLE_COVERAGE)
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    message(STATUS "Enabling coverage flags (-O0 -g --coverage)")
    add_compile_options(-O0 -g --coverage)
    add_link_options(--coverage)
  else()
    message(WARNING "ENABLE_COVERAGE requested but not supported for compiler: ${CMAKE_CXX_COMPILER_ID}")
  endif()
endif()

# Fault injection compile definition (disabled by default). When enabled, code paths
# guarded by #ifdef AES11_FAULT_INJECTION will simulate controlled failures for tests.
if(ENABLE_FAULT_INJECTION)
  message(STATUS "Fault injection ENABLED (AES11_FAULT_INJECTION=1)")
  add_compile_definitions(AES11_FAULT_INJECTION=1)
else()
  message(STATUS "Fault injection disabled (ENABLE_FAULT_INJECTION=OFF)")
endif()

add_library(aes11_standards STATIC
    lib/Standards/AES/AES11/2009/core/timing_snapshot_service.cpp
  lib/Standards/AES/AES11/2009/core/timing_window_processor.cpp
  lib/Standards/AES/AES11/2009/core/dars_protocol.cpp
  lib/Standards/AES/AES11/2009/sync/synchronization_manager.cpp
  lib/Standards/AES/AES11/2009/core/capture_range.cpp
  lib/Standards/AES/AES11/2009/core/phase_tolerance.cpp
  lib/Standards/AES/AES11/2009/sync/gps_reference_sync.cpp
  lib/Standards/AES/AES11/2009/core/channel_status_utils.cpp
  lib/Standards/AES/AES11/2009/core/aes3_adapter.cpp
  lib/Standards/AES/AES11/2009/core/sample_rate_validation.cpp
  lib/Standards/AES/AES11/2009/core/aes5_adapter.cpp
  lib/Standards/Common/reliability/metrics.cpp
  lib/Standards/Common/reliability/events.cpp
)

target_include_directories(aes11_standards PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/Standards
)

# When building tests, compile the standards library with fault injection enabled
# so tests can toggle fault points at runtime without affecting release builds.
if(BUILD_TESTS)
  target_compile_definitions(aes11_standards PUBLIC AES11_FAULT_INJECTION=1)
endif()

# Optional AES5 integration via git submodule similar to AES3
option(AES5_INTEGRATION "Enable integration with AES5 repository (prefers git submodule)" ON)
if(AES5_INTEGRRATION)
  message(WARNING "Typo in AES5 integration option variable detected; disabling integration.")
endif()
if(AES5_INTEGRATION)
  set(AES5_SUBMODULE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/AES5-2018)
  if(EXISTS ${AES5_SUBMODULE_DIR}/lib/Standards/AES/AES5/2018/core)
    message(STATUS "Using AES5 git submodule at external/AES5-2018")
    # Collect include roots required for AES5 headers with deep relative paths.
    target_include_directories(aes11_standards PUBLIC
      ${AES5_SUBMODULE_DIR}/lib/Standards
      ${AES5_SUBMODULE_DIR}/lib/Standards/Common/interfaces
    )
    target_compile_definitions(aes11_standards PUBLIC AES5_INTEGRATION=1)
  else()
    message(WARNING "AES5 submodule not found at external/AES5-2018. Disable AES5 integration or initialize submodule.")
  endif()
endif()

# Optional AES3 integration toggle (external repo). Default OFF in this repo.
option(AES3_INTEGRATION "Enable integration with external AES3 repository (prefers git submodule)" OFF)
if(AES3_INTEGRATION)
  # Prefer git submodule at external/AES3-2009. Fallback to FetchContent if missing.
  set(AES3_SUBMODULE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/AES3-2009)
  if(EXISTS ${AES3_SUBMODULE_DIR}/05-implementation/CMakeLists.txt)
    message(STATUS "Using AES3 git submodule at external/AES3-2009")
    # Disable building AES3 tests/benchmarks to avoid duplicate GTest discovery issues in our integration build.
    set(ENABLE_TESTING OFF CACHE BOOL "Disable AES3 submodule tests for integration" FORCE)
    set(ENABLE_BENCHMARKS OFF CACHE BOOL "Disable AES3 submodule benchmarks for integration" FORCE)
    add_subdirectory(external/AES3-2009/05-implementation aes3_2009_build EXCLUDE_FROM_ALL)
    # Link to specific Part libraries needed for preamble handling.
    if(TARGET aes3_part3_transport)
      target_link_libraries(aes11_standards PUBLIC aes3_part3_transport)
    endif()
    if(TARGET aes3_part1_audio_coding)
      target_link_libraries(aes11_standards PUBLIC aes3_part1_audio_coding)
    endif()
    target_include_directories(aes11_standards PUBLIC
      ${AES3_SUBMODULE_DIR}/05-implementation/include
      ${AES3_SUBMODULE_DIR}/05-implementation/src
    )
    target_compile_definitions(aes11_standards PUBLIC AES3_INTEGRATION=1)
  else()
    message(WARNING "AES3 submodule not found; falling back to FetchContent download.")
    include(FetchContent)
    FetchContent_Declare(
      aes3_2009
      GIT_REPOSITORY https://github.com/zarfld/AES3-2009.git
      GIT_TAG        main
    )
    FetchContent_MakeAvailable(aes3_2009)
    # Attempt to locate implementation CMake inside fetched source
    if(EXISTS ${aes3_2009_SOURCE_DIR}/05-implementation/CMakeLists.txt)
  set(ENABLE_TESTING OFF CACHE BOOL "Disable AES3 fetched tests for integration" FORCE)
  set(ENABLE_BENCHMARKS OFF CACHE BOOL "Disable AES3 fetched benchmarks for integration" FORCE)
  add_subdirectory(${aes3_2009_SOURCE_DIR}/05-implementation aes3_2009_build EXCLUDE_FROM_ALL)
      if(TARGET aes3_part3_transport)
        target_link_libraries(aes11_standards PUBLIC aes3_part3_transport)
      endif()
      target_include_directories(aes11_standards PUBLIC
        ${aes3_2009_SOURCE_DIR}/05-implementation/include
        ${aes3_2009_SOURCE_DIR}/05-implementation/src
      )
      target_compile_definitions(aes11_standards PUBLIC AES3_INTEGRATION=1)
    else()
      message(WARNING "AES3 implementation CMake not found; integration incomplete.")
    endif()
  endif()
endif()

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
