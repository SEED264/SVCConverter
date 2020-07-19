cmake_minimum_required(VERSION 3.10.0)
# Set compiler to clang
set(CMAKE_C_COMPILER   clang)
set(CMAKE_CXX_COMPILER clang++)

project(SVCConverter CXX)

add_executable(${PROJECT_NAME} src/main.cc)