
find_path(READLINE_INCLUDE_DIR readline/readline.h)
find_library(READLINE_LIBRARY readline)
if(READLINE_LIBRARY AND READLINE_INCLUDE_DIR)
    set(Y60_AJS_HAVE_READLINE ON)
endif(READLINE_LIBRARY AND READLINE_INCLUDE_DIR)

function(ape_set_version PREFIX MAJOR MINOR PATCH)
    set(${PREFIX}_VERSION_MAJOR ${MAJOR} PARENT_SCOPE)
    set(${PREFIX}_VERSION_MINOR ${MINOR} PARENT_SCOPE)
    set(${PREFIX}_VERSION_PATCH ${PATCH} PARENT_SCOPE)
    set(${PREFIX}_VERSION_EXTRA ${ARGN} PARENT_SCOPE)
endfunction(ape_set_version)

ape_set_version( Y60_AJS 0 1 0 )

ac_configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/y60_ajs_config.h.in
        ${CMAKE_CURRENT_BINARY_DIR}/y60_ajs_config.h
        "${CMAKE_CURRENT_LIST_FILE}"
)

ac_add_executable(
    ajs
    SOURCES ajs.cpp custom_globals.cpp custom_globals.h
    DEPENDS yape ${JSDEPENDS}
    EXTERNS Boost ${JSEXTERN}
)

