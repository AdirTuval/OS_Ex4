cmake_minimum_required(VERSION 3.15)
project(OS_Ex4)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(OS_Ex4 MemoryConstants.h PhysicalMemory.cpp PhysicalMemory.h SimpleTest.cpp VirtualMemory.h VirtualMemory.cpp)