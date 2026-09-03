cmake_minimum_required(VERSION 3.28)
project(FITSHelper VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the build type." FORCE)
endif()

########################################
# Dependencies #########################
########################################

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
  set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
endif()

find_program(MOLD_PROGRAM mold)
if(MOLD_PROGRAM)
  add_link_options(-fuse-ld=mold)
endif()

include(FetchContent)
FetchContent_Declare(cpp-dump
  GIT_REPOSITORY https://github.com/philip82148/cpp-dump
  GIT_TAG v0.7.0)
FetchContent_MakeAvailable(cpp-dump)

########################################
## Astroutils Library ##################
########################################

add_subdirectory(astroutils)

########################################
## Executable ##########################
########################################

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")
add_executable(fits-helper ${SOURCES})
target_precompile_headers(fits-helper PRIVATE src/pch.h)
target_include_directories(fits-helper PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_compile_options(fits-helper PRIVATE
  -Wall -Wextra -Wpedantic
  $<$<CXX_COMPILER_ID:GNU,Clang>:-fdiagnostics-color=always>)
target_compile_definitions(fits-helper PRIVATE
  NAME="${PROJECT_NAME}"
  VERSION="${PROJECT_VERSION}")
target_link_libraries(fits-helper PRIVATE astroutils cpp-dump)

# include(cmake/sanitizers.cmake)