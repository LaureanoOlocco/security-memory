cmake_minimum_required(VERSION 3.27 FATAL_ERROR)

# Nombre del proyecto
project(

    "MemoryProgram"
    VERSION 1.0.0
    LANGUAGES C
)

# Habilitar compilación con símbolos de depuración
set(CMAKE_BUILD_TYPE Debug)

# Establecer el directorio de los archivos de cabecera
include_directories(include)
include_directories(lib/memory/include)


file(GLOB_RECURSE MAIN_SRC "src/main.c")

# Añadir el ejecutable

add_executable(memory_program ${MAIN_SRC})

add_subdirectory(lib/memory)


target_include_directories(memory_program PUBLIC lib/memory/include)

# Enlazar la biblioteca estática con el ejecutable
target_link_libraries(memory_program memory)