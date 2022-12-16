cmake_minimum_required(VERSION 3.22)
project(XuLang VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
add_compile_options(-Wall)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

add_subdirectory("${CMAKE_SOURCE_DIR}/ast")
add_subdirectory("${CMAKE_SOURCE_DIR}/parser")
add_executable(ast2json.out ${CMAKE_SOURCE_DIR}/ast2json.cc)
target_link_libraries(ast2json.out parser ast)
