set( LIB_HEADERS
    ape_thing.h
    arguments.h
    caller.h
    class.h
    cv_qualifier_macros.h
    exception.h
    function.h
    invoke.h
    logger_settings.h
    meta.h
    module.h
    monkey_utilities.h
    property.h
    signature.h
    typelist.h
    y60_ape_settings.h
    y60_ape_config.h
    yape.h
)

# look for templog
find_path(
        TEMPLOG_INCLUDE_DIR templog/logging.h
        PATHS ${CMAKE_CURRENT_SOURCE_DIR}
)

if(TEMPLOG_INCLUDE_DIR)
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
    DEPENDS y60jsbase y60jslib ${JSDEPENDS}
    EXTERNS Boost ${JSEXTERN}
)

set(Y60_APE_LINK_ARGS
    DEPENDS yape ${JSDEPENDS} y60jslib 
    EXTERNS ${JSEXTERN}
)

add_subdirectory( tests )
add_subdirectory( benchmarks )
add_subdirectory( examples )
