cmake_minimum_required(VERSION 3.3)

# project variables
project(spdlog_setup VERSION 0.1.0 LANGUAGES CXX)

# general fixed compiler settings
if(${MSVC})
  set(DEBUG_FLAGS "/W4;")
else()
  set(DEBUG_FLAGS "-Wall;")
endif()

add_compile_options("$<$<CONFIG:DEBUG>:${DEBUG_FLAGS}>")
include(CMakeToolsHelpers OPTIONAL)

# Catch does not have modular CMake mechanism
add_subdirectory(deps/cpptoml)
add_subdirectory(deps/spdlog)
add_subdirectory(deps/tag_fmt)

# spdlog_setup
add_library(spdlog_setup INTERFACE)

target_include_directories(spdlog_setup
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
    $<INSTALL_INTERFACE:include>)

target_link_libraries(spdlog_setup
  INTERFACE
    cpptoml
    spdlog
    tag_fmt)

install(TARGETS spdlog_setup EXPORT spdlog_setup)
install(FILES include/spdlog_setup.h DESTINATION include)

install(EXPORT spdlog_setup
  FILE spdlog_setup-targets.cmake
  NAMESPACE spdlog_setup::
  DESTINATION lib/cmake/spdlog_setup)

include(CMakePackageConfigHelpers)

configure_package_config_file(
  cmake/spdlog_setup-config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/cmake/spdlog_setup-config.cmake"
  INSTALL_DESTINATION lib/cmake/spdlog_setup
  NO_CHECK_REQUIRED_COMPONENTS_MACRO)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/cmake/spdlog_setup-config-version.cmake"
  VERSION ${spdlog_setup_VERSION}
  COMPATIBILITY SameMajorVersion)

install(
  FILES
  "${CMAKE_CURRENT_BINARY_DIR}/cmake/spdlog_setup-config.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake/spdlog_setup-config-version.cmake"
  DESTINATION lib/cmake/spdlog_setup)

# spdlog_setup_unit_test
if(SPDLOG_SETUP_INCLUDE_UNIT_TESTS)
  add_executable(spdlog_setup_unit_test
    src/unit_test/unit_test.cpp)

  set_property(TARGET spdlog_setup_unit_test PROPERTY CXX_STANDARD 14)

  target_include_directories(spdlog_setup_unit_test
    PRIVATE
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/deps/Catch/include>)

  target_link_libraries(spdlog_setup_unit_test
    PRIVATE
      spdlog_setup)
endif()
