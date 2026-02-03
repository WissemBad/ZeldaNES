cmake_minimum_required(VERSION 3.20)
project(NUPRC C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Options de compilation strictes
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(SDL2TTF REQUIRED SDL2_ttf)
pkg_check_modules(SDL2IMAGE REQUIRED SDL2_image)
pkg_check_modules(SDL2MIXER REQUIRED SDL2_mixer)

add_executable(NUPRC
        src/main.c
        src/game.c
        src/render.c
        src/character.c
        src/enemy.c
        src/map.c
        src/link.c
        src/iomanager.c
        src/utils.c
        src/hud.c
)

target_include_directories(NUPRC PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${SDL2_INCLUDE_DIRS}
        ${SDL2TTF_INCLUDE_DIRS}
        ${SDL2IMAGE_INCLUDE_DIRS}
        ${SDL2MIXER_INCLUDE_DIRS}
)

target_link_directories(NUPRC PRIVATE
        ${SDL2_LIBRARY_DIRS}
        ${SDL2TTF_LIBRARY_DIRS}
        ${SDL2IMAGE_LIBRARY_DIRS}
        ${SDL2MIXER_LIBRARY_DIRS}
)

target_link_libraries(NUPRC PRIVATE
        ${SDL2_LIBRARIES}
        ${SDL2TTF_LIBRARIES}
        ${SDL2IMAGE_LIBRARIES}
        ${SDL2MIXER_LIBRARIES}
)

# Copier les assets dans le répertoire de build
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})

