cmake_minimum_required(VERSION 3.5)
project(ChatRoomUnix)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES )
add_executable(Client ${SOURCE_FILES} Client/client.c Client/client.h)
add_executable(Server ${SOURCE_FILES} Server/server.c Server/server.h)