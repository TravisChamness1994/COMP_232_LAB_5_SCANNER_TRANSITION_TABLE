cmake_minimum_required(VERSION 3.8)
project(scannerTransTable)

set(CMAKE_C_STANDARD 11)

set(SOURCE_FILES
        src/main.c
        src/scanner.h
        src/scanner.c)

include_directories(AFTER src)

add_executable(scanner ${SOURCE_FILES})