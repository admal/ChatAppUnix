cmake_minimum_required(VERSION 3.5)
project(ChatRoomUnix)
find_package( Threads )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -std=c99")
find_package(Threads)
set(SOURCE_FILES Common/socketHelpers.c Common/socketHelpers.h Common/signalHelpers.c Common/signalHelpers.h)

add_executable(Client ${SOURCE_FILES} Client/client.c Client/client.h Common/Messaging.h Common/Messaging.c Common/Messaging.h Client/ListenThread.c Client/ListenThread.h Client/ClientGlobals.h)
target_link_libraries( Client ${CMAKE_THREAD_LIBS_INIT} )

add_executable(Server ${SOURCE_FILES} Server/server.c Server/server.h Server/List.c Server/List.h Server/ThreadHandling.c Server/ThreadHandling.h Server/Globals.h Common/Messaging.h Common/Messaging.c Common/Messaging.h)
target_link_libraries( Server ${CMAKE_THREAD_LIBS_INIT} )