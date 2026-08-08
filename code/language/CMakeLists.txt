file(GLOB_RECURSE LANG_SRC CONFIGURE_DEPENDS
    "*.cpp" "*.c" "*.cc" "*.rc"
    "*.h" "*.hpp" "*.hm"
)

add_library(Language SHARED ${LANG_SRC})
target_compile_features(Language PRIVATE cxx_std_20)

if (MSVC)
    target_link_options(Language PRIVATE "/NOENTRY" "/NODEFAULTLIB")
endif()

add_custom_command(TARGET Language POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${TS_RUN_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:Language>"
        "${TS_RUN_DIR}/$<TARGET_FILE_NAME:Language>"
)
