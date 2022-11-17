cmake_minimum_required(VERSION 3.14)

set(this iti_lzw)

project(${this})

set(CMAKE_CXX_STANDARD 11)
set(${this} C CXX)

add_subdirectory(src)

add_executable(${this} src/main.cpp)

target_link_libraries(${this} PUBLIC
  LZW
)
    
target_include_directories(${this} PUBLIC 
  ${CMAKE_SOURCE_DIR}/include/
)
    
