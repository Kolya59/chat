cmake_minimum_required(VERSION 3.15)
project(chat C)

set(CMAKE_C_STANDARD 11)

add_executable(chat main.c)
#add_executable(chat main.c server.c client.c)