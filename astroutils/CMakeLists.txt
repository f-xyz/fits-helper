cmake_minimum_required(VERSION 3.28)
project(astroutils VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the build type." FORCE)
endif()

########################################
# Dependencies #########################
########################################

find_package(OpenCV REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(CFITSIO REQUIRED IMPORTED_TARGET cfitsio)

########################################
# Library ##############################
########################################

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")
add_library(astroutils SHARED ${SOURCES})
target_precompile_headers(astroutils PRIVATE src/pch.h)
target_include_directories(astroutils PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_link_libraries(astroutils PUBLIC
  stdc++exp
  tbb
  PkgConfig::CFITSIO
  ${OpenCV_LIBS}
  $<$<CONFIG:Debug>:--coverage>)
target_compile_options(astroutils PRIVATE
  -Wall -Wextra -Wpedantic
  $<$<CXX_COMPILER_ID:GNU,Clang>:-fdiagnostics-color=always>
  $<$<CONFIG:Debug>:--coverage>)

########################################
# Unit Tests ###########################
########################################

if (PROJECT_IS_TOP_LEVEL)
  include(FetchContent)
  FetchContent_Declare(googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.18.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
  FetchContent_MakeAvailable(googletest)

  FetchContent_Declare(cpp-dump
    GIT_REPOSITORY https://github.com/philip82148/cpp-dump
    GIT_TAG v0.7.0)
  FetchContent_MakeAvailable(cpp-dump)

  file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS "tests/*.cpp")
  add_executable(tests ${TEST_SOURCES})
  target_precompile_headers(tests PRIVATE "tests/pch.h")
  target_link_libraries(tests PRIVATE
    astroutils
    gmock_main
    cpp-dump 
    $<$<CONFIG:Debug>:--coverage>)
  target_compile_options(tests PRIVATE
    -Wall -Wextra -Wpedantic
    $<$<CXX_COMPILER_ID:GNU,Clang>:-fdiagnostics-color=always>)

  include(GoogleTest)
  enable_testing()
  gtest_discover_tests(tests)
endif()