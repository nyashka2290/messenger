cmake_minimum_required(VERSION 3.25)
project(messenger)

set(CMAKE_CXX_STANDARD 20)

set(BOOST_ROOT "/usr/local")
find_package(Boost 1.86.0 REQUIRED COMPONENTS system)

add_executable(messenger client.cpp)

target_include_directories(messenger PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(messenger PRIVATE ${Boost_LIBRARIES})

add_executable(server server.cpp)

target_include_directories(server PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(server PRIVATE ${Boost_LIBRARIES})
