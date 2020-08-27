cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 17)
project(HexDump CXX)

add_executable(${PROJECT_NAME} Main.cpp)
target_link_libraries(${PROJECT_NAME} stdc++fs)
