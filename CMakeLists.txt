cmake_minimum_required(VERSION 3.15)
project(chat C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_EXE_LINKER_FLAGS "-lrt")
set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads)

add_executable(chat main.c main.h)

target_link_libraries(chat ${CMAKE_THREAD_LIBS_INIT})
