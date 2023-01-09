project(XuLang)
include_directories(${CMAKE_SOURCE_DIR})
add_custom_command(
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/parser.y
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp
    COMMAND bison -Wall -d -o ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp ${CMAKE_CURRENT_SOURCE_DIR}/parser.y
)
add_custom_command(
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/token.l
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/tokens.cpp
    COMMAND flex -o ${CMAKE_CURRENT_BINARY_DIR}/tokens.cpp ${CMAKE_CURRENT_SOURCE_DIR}/token.l
)
add_library(parser SHARED
            ${PROJECT_BINARY_DIR}/tokens.cpp
            ${PROJECT_BINARY_DIR}/parser.cpp
            ./parser.cc
            )
