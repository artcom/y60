set( LIB_HEADERS
ape_thing.h
arguments.h
caller.h
class.h
exception.h
function.h
invoke.h
invoke2.h
module.h
signature.h
typelist.h
y60_ape_config.h
y60_ape_settings.h
yape.h
)

# look for templog
find_path(
        TEMPLOG_INCLUDE_DIR templog/logging.h
        PATHS ${CMAKE_CURRENT_SOURCE_DIR}
)

if(TEMPLOG_INCLUDE_DIR)
    message("Found templog")
    set(Y60_APE_HAVE_TEMPLOG ON)
    include_directories( ${TEMPLOG_INCLUDE_DIR} )
endif(TEMPLOG_INCLUDE_DIR)

# XXX talk to ingo about per library config headers
ac_configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/y60_ape_config.h.in
        ${CMAKE_CURRENT_BINARY_DIR}/y60_ape_config.h
        "${CMAKE_CURRENT_LIST_FILE}"
)

ac_add_library(
    yape
    HEADERS ${LIB_HEADERS} ${TEMPLOG_HEADERS}
    DEPENDS y60jsbase 
    EXTERNS Boost 
)

set(Y60_APE_LINK_ARGS
    DEPENDS ${JSDEPENDS} y60jslib 
    EXTERNS ${JSEXTERN}
)

add_subdirectory( examples )
add_subdirectory( tests )
