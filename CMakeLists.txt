cmake_minimum_required(VERSION 3.5)
project(ChatRoomUnix)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -std=c99")
find_package(Threads)
set(SOURCE_FILES Common/socketHelpers.c Common/socketHelpers.h Common/signalHelpers.c Common/signalHelpers.h)
add_executable(Client ${SOURCE_FILES} Client/client.c Client/client.h)
add_executable(Server ${SOURCE_FILES} Server/server.c Server/server.h Server/List.c Server/List.h Server/ThreadHandling.c Server/ThreadHandling.h Server/Globals.h)