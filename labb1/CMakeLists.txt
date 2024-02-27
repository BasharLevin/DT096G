cmake_minimum_required(VERSION 3.23)
project(labb1)

set(CMAKE_CXX_STANDARD 20)

set(Headers lexer.h parser.h)
add_executable(labb1 ${PROJECT_SOURCE_DIR}/ main.cpp parser.cpp lexer.cpp)
SET(CMAKE_BUILD_TYPE Debug)
