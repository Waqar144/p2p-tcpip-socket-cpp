cmake_minimum_required(VERSION 3.0)

project(cpp_tcpip)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(src)

include_directories(src)

add_executable(main main.cpp)
add_executable(main2 main2.cpp)

target_link_libraries(main PUBLIC P2PSocket)
target_link_libraries(main2 PUBLIC P2PSocket)
