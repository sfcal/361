cmake_minimum_required(VERSION 3.17)
project(mysh C)

set(CMAKE_C_STANDARD 11)

add_executable(mysh shell-with-builtin.c get_path.c where.c which.c sh.h get_path.h)