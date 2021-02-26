cmake_minimum_required(VERSION 3.16)
project(CHIP_8 C)

set(CMAKE_C_STANDARD 99)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

add_executable(CHIP_8 main.c core/CHIP-8.h core/CHIP-8.c core/instructions.h core/instructions.c)
target_link_libraries(CHIP_8 ${CURSES_LIBRARIES})
