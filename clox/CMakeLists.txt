cmake_minimum_required(VERSION 3.20)
project(clox)

set(CMAKE_C_STANDARD 11)

# Gather all source files in the current directory
file(GLOB SOURCES *.c)
file(GLOB HEADERS *.h)

# Create the executable using the gathered lists
add_executable(clox ${SOURCES} ${HEADERS}
        debug.h
        debug.c
        value.h
        value.c
        vm.h
        vm.c
        compiler.h
        compiler.c
        scanner.h
        scanner.c
        object.h
        object.c
)