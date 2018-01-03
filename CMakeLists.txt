cmake_minimum_required(VERSION 3.7)
project(messangerserver)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES server.c server.h messageControlServer.c messageControlServer.h)
add_executable(messangerserver ${SOURCE_FILES})