cmake_minimum_required(VERSION 3.20)

project(GameOfLife
    VERSION 1.0.0
    LANGUAGES CXX
)

set(BINARY_NAME gol)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -flto -funroll-loops")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)

file(GLOB_RECURSE PROJECT_SOURCES
    CONFIGURE_DEPENDS
    src/*.cpp
)

file(GLOB_RECURSE PROJECT_HEADERS
    CONFIGURE_DEPENDS
    src/*.hpp
)

add_executable(${BINARY_NAME}
    ${PROJECT_SOURCES}
    ${PROJECT_HEADERS}
)

target_include_directories(${BINARY_NAME} PRIVATE
    src
    ${SDL2_INCLUDE_DIRS}
)

target_compile_options(${BINARY_NAME} PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Werror
)

target_link_libraries(${BINARY_NAME} PRIVATE
    ${SDL2_LIBRARIES}
)