#project name
project(findMissingWord)

#cmake minimum version
cmake_minimum_required(VERSION 2.8)

#compiler
set(CMAKE_C_COMPILER "g++")
#set(CMAKE_CXX_STANDARD 11)

#binary name
set(BIN_NAME findMissingWord.exe)

#source list
set(SRC_LIST
    ${CMAKE_SOURCE_DIR}/src/wordFileCompare.cpp
    ${CMAKE_SOURCE_DIR}/src/findMissingWord.c
)

#include files
include_directories(BIN_NAME
#	${CMAKE_SOURCE_DIR}/include
)

#make executable file
add_executable(${BIN_NAME} ${SRC_LIST})
