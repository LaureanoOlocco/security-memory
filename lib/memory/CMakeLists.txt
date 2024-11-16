cmake_minimum_required(VERSION 3.10)

# Set the project name
project(memory)

include_directories(include)

# Add the library
add_library(memory STATIC
    src/memory.c
)

# Set C++ standard
set_target_properties(memory PROPERTIES
    C_STANDARD 17
)