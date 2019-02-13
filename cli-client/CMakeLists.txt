cmake_minimum_required(VERSION 3.1)
project(file_sender_client)


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wshadow -Wundef -Wno-missing-field-initializers ")
find_package (Threads)
find_package(Boost 1.62 COMPONENTS system regex filesystem REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/include)

add_library(netlib STATIC
     ../connection/AbstractConnection.cpp
     ../server/Server.cpp
     ../server/ServerConfigurator.cpp
     ../log/Log.cpp
     ../files/FileHolder.cpp
     ../exceptions/CustomException.cpp
     ../exceptions/ConnectionException.cpp
     ../exceptions/FileException.cpp
     ../connection/ServerConnection.cpp
     ../connection/ClientConnection.cpp
     ../exceptions/ConfigException.cpp
     ../connection/ConnectionPool.cpp
     ../libs/sources/directory_helpers.cpp
)

add_executable(file_sender  main.cpp)

target_link_libraries(file_sender netlib ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} -lpthread -lboost_thread -lboost_serialization)
