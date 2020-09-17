cmake_minimum_required(VERSION 3.3)

# project variables
project(spdlog_setup VERSION 0.3.1 LANGUAGES CXX)
set(SPDLOG_MIN_VERSION "1.0.0")
set(CPPTOML_MIN_VERSION "0.1.0")

#---------------------------------------------------------------------------------------
# Set SPDLOG_SETUP_MASTER_PROJECT to ON if we are building spdlog_setup
#---------------------------------------------------------------------------------------
# Check if spdlog_setup is being used directly or via add_subdirectory, but allow overriding
if (NOT DEFINED SPDLOG_SETUP_MASTER_PROJECT)
  if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(SPDLOG_SETUP_MASTER_PROJECT ON)
  else()
    set(SPDLOG_SETUP_MASTER_PROJECT OFF)
  endif()
endif ()

# general fixed compiler settings
if(${MSVC})
  set(DEBUG_FLAGS /W4)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  # using regular Clang or AppleClang
  set(DEBUG_FLAGS -Wall)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")
else()
  set(DEBUG_FLAGS -Wall)
endif()

option(SPDLOG_SETUP_INSTALL "Generate the install target" ${SPDLOG_SETUP_MASTER_PROJECT})

option(SPDLOG_SETUP_INCLUDE_UNIT_TESTS "Build with unittests" OFF)

option(SPDLOG_SETUP_CPPTOML_EXTERNAL "Use external CPPTOML library instead of bundled" OFF)
if(SPDLOG_SETUP_CPPTOML_EXTERNAL)
  configure_file(cmake/cpptoml_config.h.in "${CMAKE_CURRENT_BINARY_DIR}/include/spdlog_setup/details/third_party/cpptoml.h" @ONLY)
endif()

# allow thread library to be used for linking
set(THREADS_PREFER_PTHREAD_FLAG ON)
set(THREADS_PTHREAD_ARG "0" CACHE STRING "Result from TRY_RUN" FORCE)
find_package(Threads REQUIRED)

add_compile_options("$<$<CONFIG:DEBUG>:${DEBUG_FLAGS}>")
include(CMakeToolsHelpers OPTIONAL)

if (EXISTS ${CMAKE_SOURCE_DIR}/deps/spdlog/CMakeLists.txt)
  add_subdirectory(deps/spdlog)
else()
  # allow usage of installed dependency
  find_package(spdlog ${SPDLOG_MIN_VERSION} REQUIRED)
  add_library(${PROJECT_NAME}_spdlog INTERFACE IMPORTED)
endif()

if(SPDLOG_SETUP_CPPTOML_EXTERNAL)
if (EXISTS ${CMAKE_SOURCE_DIR}/deps/cpptoml/CMakeLists.txt)
  add_subdirectory(deps/cpptoml)
else()
  # allow usage of installed dependency
  find_package(cpptoml ${CPPTOML_MIN_VERSION} REQUIRED)
  add_library(${PROJECT_NAME}_cpptoml INTERFACE IMPORTED)
endif()
endif()

# spdlog_setup
add_library(spdlog_setup INTERFACE)

target_include_directories(spdlog_setup
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
    $<INSTALL_INTERFACE:include>)

if(TARGET spdlog::spdlog_header_only)
  target_link_libraries(spdlog_setup
    INTERFACE
      spdlog::spdlog_header_only)
else()
  target_link_libraries(spdlog_setup
      INTERFACE
      spdlog)
endif()

if(SPDLOG_SETUP_INSTALL)
  install(TARGETS spdlog_setup EXPORT spdlog_setup)
  install(DIRECTORY include/spdlog_setup DESTINATION include)

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

  if(SPDLOG_SETUP_CPPTOML_EXTERNAL)
  install(
    FILES
    "${CMAKE_CURRENT_BINARY_DIR}/include/spdlog_setup/details/third_party/cpptoml.h"
    DESTINATION include/spdlog_setup/details/third_party)
  endif()
endif()

# spdlog_setup_unit_test
FILE(GLOB unit_test_cpps src/unit_test/*.cpp)
if(SPDLOG_SETUP_INCLUDE_UNIT_TESTS)
  add_executable(spdlog_setup_unit_test
    ${unit_test_cpps})

  set_property(TARGET spdlog_setup_unit_test PROPERTY CXX_STANDARD 11)

  enable_testing()
  add_test(spdlog_setup_unit_test spdlog_setup_unit_test -s)

  # Catch2 do not have modular CMake mechanism
  target_include_directories(spdlog_setup_unit_test
    PRIVATE
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/deps/Catch2/single_include>)

  if(SPDLOG_SETUP_CPPTOML_EXTERNAL)
    target_include_directories(spdlog_setup_unit_test
      PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/deps/cpptoml/include>)
  endif()

  target_link_libraries(spdlog_setup_unit_test
    PRIVATE
      spdlog_setup
      Threads::Threads)

  if(SPDLOG_SETUP_INCLUDE_TEST_COVERAGE)
    target_compile_options(spdlog_setup_unit_test
      PRIVATE
        -coverage)

    target_link_libraries(spdlog_setup_unit_test
      PRIVATE
        --coverage)
  endif()
endif()
