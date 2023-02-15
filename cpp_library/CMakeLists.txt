cmake_minimum_required(VERSION 3.10)
project(BONCYTHON  CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenMP)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")

add_executable(test test.cpp SnapIO.cpp tipsyIO.cpp)
add_executable(merge_bonsai_tipsy merge_bonsai_tipsy.cpp SnapIO.cpp tipsyIO.cpp)
add_executable(divide_bonsai divide_bonsai.cpp SnapIO.cpp tipsyIO.cpp)
#target_include_directories(test PRIVATE Bonsai/runtime/include )

