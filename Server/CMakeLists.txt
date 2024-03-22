cmake_minimum_required(VERSION 3.18)
project(Server)

set(CMAKE_CXX_STANDARD 20)

find_package(Threads REQUIRED)
# Include MessagePack headers
include_directories(${MSGPACK_INCLUDE_DIRS})

# Include MessagePack headers

find_package (SQLite3)
include_directories(${SQLite3_INCLUDE_DIRS})

add_executable(Server main.cpp
        server/BoostServer.cpp
        client/ClientInfo.cpp
        utils/Utils.h
        utils/Utils.cpp database/SqlManager.h database/SqlManager.cpp)


target_link_libraries(Server Threads::Threads ${MSGPACK_LIBRARIES} ${SQLite3_LIBRARIES})
