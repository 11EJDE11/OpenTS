# Limit the generated solution to supported configurations.
set(CMAKE_CONFIGURATION_TYPES Debug Release CACHE STRING "" FORCE)

# OpenTS currently supports 32-bit (x86) builds only.
if(NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(FATAL_ERROR "OpenTS must be built as 32-bit x86. Reconfigure with -A Win32.")
endif()

#
# ---------------------------------------------------------
# Directories that build as their own project
# ---------------------------------------------------------
#
set(SUBPROJECT_DIRS
    "language"
    "vqalib"
)

#
# ---------------------------------------------------------
# Gather all potential OpenTS files
# ---------------------------------------------------------
#
file(GLOB_RECURSE OPENTS_RAW CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hh"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.rc"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.asm"

    # Submodules of OpenTS
    "${CMAKE_CURRENT_SOURCE_DIR}/wolapi/*"
)

#
# ---------------------------------------------------------
# Remove files that belong to a subproject
# ---------------------------------------------------------
#
set(OPENTS_SRC "")
foreach(f ${OPENTS_RAW})
    set(EXCLUDED FALSE)
    foreach(dir ${SUBPROJECT_DIRS})
        if(f MATCHES "^${CMAKE_CURRENT_SOURCE_DIR}/${dir}/.*")
            set(EXCLUDED TRUE)
        endif()
    endforeach()

    if(NOT EXCLUDED)
        list(APPEND OPENTS_SRC "${f}")
    endif()
endforeach()

#
# ---------------------------------------------------------
# Create actual OpenTS executable (Win32)
# ---------------------------------------------------------
#
message(STATUS "${PROJECT_NAME}: Creating '${PROJECT_NAME}' executable...")

add_executable(OpenTS WIN32 ${OPENTS_SRC})
target_compile_features(OpenTS PRIVATE cxx_std_20)

#
# ---------------------------------------------------------
# Output paths
# ---------------------------------------------------------
#
message(STATUS "${PROJECT_NAME}: Setting binary output paths...")

set_target_properties(OpenTS PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"

    # Built under the shipped names, so the executable, the symbol file the debug record
    # names, and the map reach the run directory without a rename.
    OUTPUT_NAME_DEBUG "GameD"
    OUTPUT_NAME_RELEASE "Game"
    PDB_NAME_DEBUG "GameD"
    PDB_NAME_RELEASE "Game"
)

set_target_properties(OpenTS PROPERTIES
    VS_DEBUGGER_COMMAND "${TS_RUN_DIR}/$<TARGET_FILE_NAME:OpenTS>"
    VS_DEBUGGER_WORKING_DIRECTORY "${TS_RUN_DIR}"
)

#
# ---------------------------------------------------------
# Compile flags
# ---------------------------------------------------------
#
message(STATUS "${PROJECT_NAME}: Applying compiler flags...")

set(OPENTS_COMPILE_OPTIONS

    # ================= DEBUG =================
    $<$<CONFIG:Debug>:

        # ---------- C++ ----------
        # /arch:IA32 -- no SSE/SSE2; use the x87 FPU for floating point (matches
        # the reference compiler's x87 floating-point behavior).
        $<$<COMPILE_LANGUAGE:CXX>:
            /Zi /Od /RTC1 /Gr /GR /MP /EHsc /Oy- /MTd /Zc:__cplusplus /arch:IA32
        >

        # ---------- C ----------
        $<$<COMPILE_LANGUAGE:C>:
            /Zi /Od /RTC1 /MP /Oy- /arch:IA32
        >

        # ---------- MASM ----------
        $<$<COMPILE_LANGUAGE:ASM_MASM>:
            /Zi /c /coff /Cx /safeseh
        >
    >

    # ================= RELEASE =================
    $<$<CONFIG:Release>:

        # ---------- C++ ----------
        # /arch:IA32 -- no SSE/SSE2; use the x87 FPU for floating point (matches
        # the reference compiler's x87 floating-point behavior).
        $<$<COMPILE_LANGUAGE:CXX>:
            /Zi /O2 /GF /Gr /GR /MP /EHsc /MT /Zc:__cplusplus /arch:IA32
        >

        # ---------- C ----------
        $<$<COMPILE_LANGUAGE:C>:
            /Zi /O2 /GF /MP /arch:IA32
        >

        # ---------- MASM ----------
        $<$<COMPILE_LANGUAGE:ASM_MASM>:
            /Zi /c /coff /Cx /safeseh
        >
    >
)

target_compile_options(OpenTS PRIVATE ${OPENTS_COMPILE_OPTIONS})

#
# ---------------------------------------------------------
# Subprojects
# ---------------------------------------------------------
#
# The VQA player carries its own project, and hands OpenTS its include path along with the
# library, so the movie code is reached the same way from anywhere in the solution.
add_subdirectory(vqalib)

message(STATUS "${PROJECT_NAME}: Adding include paths...")
target_include_directories(OpenTS PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

message(STATUS "${PROJECT_NAME}: Adding compilier definitions...")
target_compile_definitions(OpenTS PRIVATE
    WIN32
    _WINDOWS
    _MBCS
)

#
# ---------------------------------------------------------
# Windows libraries and linker options
# ---------------------------------------------------------
#
message(STATUS "${PROJECT_NAME}: Applying linker options...")

# Required libraries:
#  Comctl32.lib
#  ddraw.lib
#  DSound.lib
#  Version.lib
#  Winmm.lib
#  wsock32.lib
#  + all Win32 libs
#

target_link_libraries(OpenTS PRIVATE
    VQALib

    comctl32
    ddraw
    dsound
    version
    winmm
    wsock32
    kernel32 user32 gdi32 winspool comdlg32 advapi32 shell32
    ole32 oleaut32 uuid odbc32 odbccp32
)

if (MSVC)
    target_link_options(OpenTS PRIVATE
        /SUBSYSTEM:WINDOWS
        /DEBUG
        /MAP
    )
endif()

#
# ---------------------------------------------------------
# MASM assembly support
# ---------------------------------------------------------
#
message(STATUS "${PROJECT_NAME}: Adding MASM support...")

enable_language(ASM_MASM)

# All .asm files compile automatically using ML.EXE with appropriate flags.
foreach(f ${OPENTS_SRC})
    if(f MATCHES "\\.asm$")
        set_source_files_properties(${f} PROPERTIES
            LANGUAGE ASM_MASM
            COMPILE_FLAGS "-Zi -c -coff -Cx"
        )
    endif()
endforeach()

#
# ---------------------------------------------------------
# Visual Studio source groups
# ---------------------------------------------------------
#
message(STATUS "${PROJECT_NAME}: Setting up Visual Studio filters...")

# Init Source Files (_*.cpp)
foreach(f ${OPENTS_SRC})
    if(f MATCHES "[/\\\\]_.*\\.cpp$")
        source_group("Source Files\\Init Source Files" FILES ${f})
    endif()
endforeach()

# Init Header Files (_*.h)
foreach(f ${OPENTS_SRC})
    if(f MATCHES "[/\\\\]_.*\\.h$")
        source_group("Header Files\\Init Header Files" FILES ${f})
    endif()
endforeach()

# Define Header Files (*.hh)
foreach(f ${OPENTS_SRC})
    if(f MATCHES "\\.hh$")
        source_group("Header Files\\Define Header Files" FILES ${f})
    endif()
endforeach()

# Assembly Files
foreach(f ${OPENTS_SRC})
    if(f MATCHES "\\.asm$")
        source_group("Assembly Files" FILES ${f})
    endif()
endforeach()

# Resource Files
foreach(f ${OPENTS_SRC})
    if(f MATCHES "\\.(ico|cur|bmp|gif|jpe?g|rc)$")
        source_group("Resource Files" FILES ${f})
    endif()
endforeach()

# List of all interface filenames (headers + C stubs)
set(INTERFACE_FILES
    iaihouse.h       iaihouse_i.c
    iapp.h           iapp_i.c
    iblockci.h       iblockci_i.c
    iblowfish.h      iblowfish_i.c
    iflyctrl.h       iflyctrl_i.c
    ihouse.h         ihouse_i.c
    ilinkstm.h
    iloco.h          iloco_i.c
    ilocos.h         ilocos_i.c
    imap.h           imap_i.c
    ipiggy.h         ipiggy_i.c
    ipowerevents.h   ipowerevents_i.c
    ipublichouse.h   ipublichouse_i.c
    irtti.h          irtti_i.c
    isun.h           isun_i.c
    iswizzle.h       iswizzle_i.c
    wolapi/wolapi_i.c
)

# Convert to full paths relative to source dir
set(FULL_INTERFACE_FILES "")
foreach(f IN LISTS INTERFACE_FILES)
    list(APPEND FULL_INTERFACE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/${f}")
endforeach()

# Add them to the "Interface Files" group and exclude *_i.c from build
foreach(f IN LISTS FULL_INTERFACE_FILES)

    # Put into VS filter
    source_group("Interface Files" FILES "${f}")

    # Exclude *_i.c from build, but DO NOT mark headers as header-only
    if(f MATCHES "_i\\.c$")
        set_source_files_properties("${f}" PROPERTIES HEADER_FILE_ONLY TRUE)
    endif()

endforeach()

# General source files
source_group("Source Files" REGULAR_EXPRESSION ".*\\.(c|cpp)$")
source_group("Header Files" REGULAR_EXPRESSION ".*\\.(h|hpp)$")

#
# ---------------------------------------------------------
# Populate the local run directory
# ---------------------------------------------------------
#
message(STATUS "${PROJECT_NAME}: Creating post-build commands...")
add_custom_command(TARGET OpenTS POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        "${TS_RUN_DIR}"
)

add_custom_command(TARGET OpenTS POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:OpenTS>"
        "${TS_RUN_DIR}"
)

# Copy the linker-generated .pdb alongside the exe
add_custom_command(TARGET OpenTS POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_PDB_FILE:OpenTS>"
        "${TS_RUN_DIR}"
)

# Copy the linker-generated .map (sits next to the exe, no GenEx for it)
add_custom_command(TARGET OpenTS POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE_DIR:OpenTS>/$<TARGET_FILE_BASE_NAME:OpenTS>.map"
        "${TS_RUN_DIR}"
)

#
# ---------------------------------------------------------
# Visual Studio startup project
# ---------------------------------------------------------
#
set_property(GLOBAL PROPERTY VS_STARTUP_PROJECT OpenTS)

# ---------------------------------------------------------
# Add the .natvis file for Visual Studio debugging
# ---------------------------------------------------------
set(NATVIS_FILE "${CMAKE_CURRENT_SOURCE_DIR}/sun.natvis")
source_group("Natvis Files" FILES ${NATVIS_FILE})
target_sources(OpenTS PRIVATE ${NATVIS_FILE})
target_link_options(OpenTS PRIVATE "/NATVIS:${NATVIS_FILE}")
