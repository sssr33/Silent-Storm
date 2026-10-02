cmake_minimum_required(VERSION 4.2)

# Use a static CRT so debug checks run without debug CRT DLLs.
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
project(A5Tests LANGUAGES CXX)

if(NOT MSVC OR NOT WIN32 OR NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(FATAL_ERROR "These checks require MSVC targeting Win32 (use -A Win32).")
endif()

enable_testing()
add_subdirectory(Small)
