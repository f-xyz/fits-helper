cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0135 NEW)
project(astroutils VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -fdiagnostics-color=always")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} --coverage")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} --coverage")
set(CMAKE_BUILD_TYPE Debug)

########################################

find_package(OpenCV REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(CFITSIO REQUIRED cfitsio)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  set(CMAKE_C_COMPILER_LAUNCHER   ${CCACHE_PROGRAM})
  set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
endif()

include(FetchContent)
FetchContent_Declare(cpp-dump
  GIT_REPOSITORY https://github.com/philip82148/cpp-dump
  GIT_TAG main)
FetchContent_MakeAvailable(cpp-dump)

FetchContent_Declare(googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.18.0.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
FetchContent_MakeAvailable(googletest)
enable_testing()

########################################

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")
add_library(astroutils SHARED ${SOURCES})
target_precompile_headers(astroutils INTERFACE src/pch.h)
target_include_directories(astroutils PUBLIC
  include
  ${OpenCV_INCLUDE_DIRS}
  ${CFITSIO_INCLUDE_DIRS})
target_link_libraries(astroutils PUBLIC
  stdc++exp
  tbb
  ${OpenCV_LIBS}
  ${CFITSIO_LIBRARIES})

########################################

file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS "tests/*.cpp")
add_executable(tests ${TEST_SOURCES})
target_precompile_headers(tests PRIVATE tests/pch.h)
target_include_directories(tests PRIVATE
  cpp_dump
  gmock_main)
target_link_libraries(tests PRIVATE
  astroutils
  gmock_main
  cpp-dump)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_compile_options(tests PRIVATE -fcondition-coverage)
endif()

include(GoogleTest)
gtest_discover_tests(tests)

########################################

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_options(astroutils PUBLIC
    $<$<CONFIG:Debug>:-fsanitize=address,undefined -fno-omit-frame-pointer -g>
  )
  target_link_options(astroutils PUBLIC
    $<$<CONFIG:Debug>:-fsanitize=address,undefined>
  )
endif()