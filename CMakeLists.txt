cmake_minimum_required(VERSION 3.3)

# project variables
project(spdlog_setup VERSION 0.3.0 LANGUAGES CXX)
set(SPDLOG_MIN_VERSION "0.14.0")

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

# allow thread library to be used for linking
# set(THREADS_PREFER_PTHREAD_FLAG ON)
# set(THREADS_PTHREAD_ARG "0" CACHE STRING "Result from TRY_RUN" FORCE)
# find_package(Threads REQUIRED)

add_compile_options("$<$<CONFIG:DEBUG>:${DEBUG_FLAGS}>")
include(CMakeToolsHelpers OPTIONAL)

# conan setup
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
  message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
  file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/master/conan.cmake"
                "${CMAKE_BINARY_DIR}/conan.cmake")
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

conan_cmake_run(CONANFILE conanfile.txt
                BASIC_SETUP CMAKE_TARGETS
                BUILD missing)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

# if (EXISTS ${CMAKE_SOURCE_DIR}/deps/spdlog/CMakeLists.txt)
#   add_subdirectory(deps/spdlog)
# else()
#   # allow usage of installed dependency
#   find_package(spdlog ${SPDLOG_MIN_VERSION} REQUIRED)
#   add_library(spdlog INTERFACE IMPORTED)
# endif()

# spdlog_setup
add_library(spdlog_setup INTERFACE)
target_link_libraries(spdlog_setup INTERFACE ${CONAN_LIBS})

target_include_directories(spdlog_setup
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
    $<INSTALL_INTERFACE:include>)

# target_link_libraries(spdlog_setup
#   INTERFACE
#     spdlog)

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

# spdlog_setup_unit_test
if(SPDLOG_SETUP_INCLUDE_UNIT_TESTS)
  add_executable(spdlog_setup_unit_test
    src/unit_test/unit_test.cpp)

  set_property(TARGET spdlog_setup_unit_test PROPERTY CXX_STANDARD 11)

  enable_testing()
  add_test(spdlog_setup_unit_test spdlog_setup_unit_test -s)

  # build dependencies
  target_link_libraries(spdlog_setup_unit_test
    PRIVATE
      spdlog_setup
      ${CONAN_LIBS})
  
  # target_include_directories(spdlog_setup_unit_test
  #   PRIVATE
  #     $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/deps/Catch2/single_include>)

  # target_link_libraries(spdlog_setup_unit_test
  #   PRIVATE
  #     spdlog_setup
  #     Threads::Threads)

  if(SPDLOG_SETUP_INCLUDE_TEST_COVERAGE)
    target_compile_options(spdlog_setup_unit_test
      PRIVATE
        -coverage)

    target_link_libraries(spdlog_setup_unit_test
      PRIVATE
        --coverage)
  endif()
endif()
