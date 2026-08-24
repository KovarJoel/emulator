cmake_minimum_required(VERSION 3.23)
project(Emulator CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

##################################

include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.8.1 # or a later release
)

FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG v7.0.3
)

FetchContent_MakeAvailable(
  Catch2
  ftxui
)

##################################

add_library(compiler_flags INTERFACE)

target_compile_options(compiler_flags INTERFACE
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /permissive->
  $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Werror -pedantic>
)

if (MINGW) 
  target_link_libraries(compiler_flags INTERFACE
    stdc++exp
  )
endif()

##################################

add_library(core STATIC)

file(GLOB_RECURSE SRC_CORE_CPP_FILES src/core/*.cpp)

target_sources(core PRIVATE
  ${SRC_CORE_CPP_FILES}
)

target_include_directories(core PUBLIC
  src
)

target_link_libraries(core PRIVATE
  compiler_flags
)

##################################

add_executable(assembler)

file(GLOB_RECURSE SRC_ASSEMBLER_CPP_FILES src/assembler/*.cpp)

target_sources(assembler PRIVATE
  ${SRC_ASSEMBLER_CPP_FILES}
)

target_link_libraries(assembler PRIVATE
  compiler_flags
  core
)

##################################

add_executable(emulator)

file(GLOB_RECURSE SRC_EMULATOR_CPP_FILES src/emulator/*.cpp)

target_sources(emulator PRIVATE
  ${SRC_EMULATOR_CPP_FILES}
)

target_link_libraries(emulator PRIVATE
  compiler_flags
  core
  ftxui::ftxui
)

# ##################################

include(CTest)
enable_testing()

add_executable(tests)

file(GLOB_RECURSE TESTS_CPP_FILES tests/*.cpp)
target_sources(tests PRIVATE
  ${TESTS_CPP_FILES}
)

target_link_libraries(tests PRIVATE
  compiler_flags
  core
  Catch2::Catch2WithMain
)

include(Catch)
catch_discover_tests(tests)
