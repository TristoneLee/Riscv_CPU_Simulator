cmake_minimum_required(VERSION 3.16)
project(simulator)

set(CMAKE_CXX_STANDARD 20)

include_directories(
        ${PROJECT_SOURCE_DIR}/src/tools
        ${PROJECT_SOURCE_DIR}/src/components
        ${PROJECT_SOURCE_DIR}/src/main
)
set(src_dir
        ${PROJECT_SOURCE_DIR}/src/main/test.cpp
        src/components/BranchPredict.hpp)
add_executable(code ${src_dir})