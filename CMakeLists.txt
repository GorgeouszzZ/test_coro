cmake_minimum_required(VERSION 3.1)

set(CMAKE_C_COMPILE /usr/bin/gcc-10)
set(CMAKE_CXX_COMPILE /usr/bin/g++-10)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(test_coro)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcoroutines -fno-exceptions -O3")
set(LINK_FLAGS "-lnuma")
link_libraries(${LINK_FLAGS})

add_executable(test_coro test_coro.cpp)