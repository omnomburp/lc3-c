cmake_minimum_required(VERSION 3.10)

project(lc3-c)

set(SOURCE_FILES
    lc3_instructions.c  
    main.c)

add_executable(lc3-c ${SOURCE_FILES})