cmake_minimum_required(VERSION 3.23)

project(OpenTS VERSION 0.1.0 LANGUAGES C CXX ASM_MASM)

if(NOT MSVC OR MSVC_VERSION LESS 1930)
    message(FATAL_ERROR "OpenTS requires the Visual Studio 2022 MSVC toolchain.")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(TS_RUN_DIR "${CMAKE_SOURCE_DIR}/Run" CACHE PATH "Tiberian Sun run directory")

#
# ---------------------------------------------------------
# Build identity stamp
# ---------------------------------------------------------
#
# The debug log opens with the version and commit that produced the binary. Stamping runs
# once here so the header exists before the first compile, and again per build through
# OpenTSBuildStamp so that committing does not require a reconfigure to be reflected.
set(OPENTS_GENERATED_DIR "${CMAKE_BINARY_DIR}/generated")
set(OPENTS_STAMP_HEADER "${OPENTS_GENERATED_DIR}/opents_build.h")
file(MAKE_DIRECTORY "${OPENTS_GENERATED_DIR}")

set(OPENTS_STAMP_ARGS
    "-DOPENTS_VERSION=${PROJECT_VERSION}"
    "-DOPENTS_SOURCE_DIR=${CMAKE_SOURCE_DIR}"
    "-DOPENTS_STAMP_HEADER=${OPENTS_STAMP_HEADER}"
    -P "${CMAKE_SOURCE_DIR}/cmake/GitStamp.cmake"
)

execute_process(COMMAND ${CMAKE_COMMAND} ${OPENTS_STAMP_ARGS})

add_custom_target(OpenTSBuildStamp ALL
    COMMAND ${CMAKE_COMMAND} ${OPENTS_STAMP_ARGS}
    COMMENT "Stamping the build identity"
    VERBATIM
)

add_subdirectory(thirdparty)
add_subdirectory(code)
add_subdirectory(code/language)

enable_testing()
add_subdirectory(tests)
