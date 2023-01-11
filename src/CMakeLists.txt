cmake_minimum_required(VERSION 3.22)
project(XuLang VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
add_compile_options(-Wall)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

add_subdirectory("${CMAKE_SOURCE_DIR}/utils")
add_subdirectory("${CMAKE_SOURCE_DIR}/ast")
add_subdirectory("${CMAKE_SOURCE_DIR}/parser")
add_subdirectory("${CMAKE_SOURCE_DIR}/check")

add_executable(lexer.out ./lexer.cc)
target_link_libraries(lexer.out parser ast utils)

add_executable(ast2json.out ./ast2json.cc)
target_link_libraries(ast2json.out parser ast utils)

add_executable(check.out ./check.cc)
target_link_libraries(check.out check parser ast utils)

find_package(LLVM REQUIRED CONFIG)
llvm_map_components_to_libnames(LLVM_LIBS core passes)
add_executable(codegen.out ./codegen.cc ./codegen/codegen.cc)
target_link_libraries(codegen.out check parser ast utils ${LLVM_LIBS})
