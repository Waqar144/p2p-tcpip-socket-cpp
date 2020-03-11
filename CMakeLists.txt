cmake_minimum_required(VERSION 3.0)

project(cpp_tcpip)
set(CMAKE_CXX_STANDARD 17)

add_library(P2PSocket P2PSocket.cpp
                      P2PSocket.hpp
                      P2PSocketException.hpp
                      Peer.hpp
                      Peer.cpp
                      Peers.cpp
                      Peers.hpp
                      peersmessages.h
                      peersmessages.cpp
                      SocketResource.cpp
                      SocketResource.hpp
                      Event.hpp
                      Event.cpp
                      Events.cpp
                      Events.hpp
                      Eventregister.cpp
                      Eventregister.hpp)

add_executable(main main.cpp)
add_executable(main2 main2.cpp)

target_link_libraries(main PUBLIC P2PSocket)
target_link_libraries(main2 PUBLIC P2PSocket)
