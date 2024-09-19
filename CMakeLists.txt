cmake_minimum_required(VERSION 3.29)
project(dds)

set(CMAKE_CXX_STANDARD 20)

add_executable(dds main.cpp
        drugs.h)
