cmake_minimum_required(VERSION 3.14)

set(this iti_lzw)

project(${this})

set(CMAKE_CXX_STANDARD 11)
set(${this} C CXX)

include(FetchContent)
FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
)
FetchContent_MakeAvailable(argparse)

add_subdirectory(src)

add_executable(${this} src/main.cpp)

target_link_libraries(${this} PUBLIC
  Report
  LZW
  argparse
)
    
target_include_directories(${this} PUBLIC 
  include/
)
    
