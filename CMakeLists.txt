cmake_minimum_required(VERSION 3.23)
project(Emulator CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

##################################

include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.8.1 # or a later release
)

FetchContent_MakeAvailable(
  Catch2
)

##################################

add_library(core STATIC)

file(GLOB_RECURSE SRC_CPP_FILES core/src/*.cpp)

target_sources(core PRIVATE
  ${SRC_CPP_FILES}
)

target_include_directories(core PUBLIC
  core/src
)

target_compile_options(core PRIVATE
  -Wall -Wextra -Werror -pedantic -g -O0
)

##################################

# add_executable(emulator main.cpp)
# 
# target_link_libraries(emulator PRIVATE
#   core
# )
# 
# ##################################

include(CTest)
enable_testing()

add_executable(tests)

file(GLOB_RECURSE TESTS_CPP_FILES tests/*.cpp)
target_sources(tests PRIVATE
  ${TESTS_CPP_FILES}
)

target_link_libraries(tests PRIVATE
  core
  Catch2::Catch2WithMain
)

target_compile_options(tests PRIVATE
  -g -O0
)

include(Catch)
catch_discover_tests(tests)
