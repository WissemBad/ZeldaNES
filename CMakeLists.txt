cmake_minimum_required(VERSION 4.1)
project(ME_NUPRC C)

set(CMAKE_C_STANDARD 23)

add_executable(ME_NUPRC main.c
        character.c
        character.h
        tray.c
        tray.h
        settings.h)
