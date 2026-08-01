cmake_minimum_required(VERSION 3.15)
project(FITSHelper VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

find_package(OpenCV REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(CFITSIO REQUIRED cfitsio)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  set(CMAKE_C_COMPILER_LAUNCHER   ${CCACHE_PROGRAM})
  set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
endif()

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")

add_executable(fits-helper ${SOURCES})

target_include_directories(fits-helper PRIVATE
  include
  ${OpenCV_INCLUDE_DIRS}
  ${CFITSIO_INCLUDE_DIRS}
)

target_link_libraries(fits-helper PRIVATE
  stdc++exp
  ${OpenCV_LIBS}
  ${CFITSIO_LIBRARIES}
)