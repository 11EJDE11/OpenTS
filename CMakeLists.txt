cmake_minimum_required(VERSION 3.23)

project(OpenTS VERSION 0.1.0 LANGUAGES C CXX ASM_MASM)

if(NOT MSVC OR MSVC_VERSION LESS 1930)
    message(FATAL_ERROR "OpenTS requires the Visual Studio 2022 MSVC toolchain.")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(TS_RUN_DIR "${CMAKE_SOURCE_DIR}/Run" CACHE PATH "Tiberian Sun run directory")

add_subdirectory(code)
add_subdirectory(code/language)
