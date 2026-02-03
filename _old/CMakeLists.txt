cmake_minimum_required(VERSION 3.20)
project(NUPRC C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(SDL2TTF REQUIRED SDL2_ttf)

add_executable(NUPRC
        src/main.c
        src/character.c
        src/grid.c
        src/IOManager.c
        src/game.c)

target_include_directories(NUPRC PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${SDL2_INCLUDE_DIRS}
        ${SDL2TTF_INCLUDE_DIRS})

target_link_directories(NUPRC PRIVATE
        ${SDL2_LIBRARY_DIRS}
        ${SDL2TTF_LIBRARY_DIRS})

target_link_libraries(NUPRC PRIVATE
        ${SDL2_LIBRARIES}
        ${SDL2TTF_LIBRARIES})
