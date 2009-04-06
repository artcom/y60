set( LIB_HEADERS
    class.h
    context.h
    detail/ape_exceptions.h
    detail/ape_thing.h
    detail/arguments.h
    detail/class_descriptor.h
    detail/class_wrapper.h
    detail/context_decorator.h
    detail/cv_qualifier_macros.h
    detail/function_descriptor.h
    detail/function_wrapper.h
    detail/invoke.h
    detail/logger_settings.h
    detail/meta.h
    detail/monkey_headers.h
    detail/monkey_utilities.h
    detail/property_descriptor.h
    detail/property_wrapper.h
    detail/scope.h
    detail/signature.h
    detail/typelist.h
    dynamic_loader.h
    error_reporter.h
    js_engine.h
    js_value.h
    module.h
    module_loader.h
    property.h
    runtime.h
    script.h
    y60_ape_settings.h
    yape.h
)

set(SOURCES
    detail/scope.cpp
    dynamic_loader.cpp
    module_loader.cpp
)

find_package( Boost COMPONENTS system program_options filesystem)

find_path(TEMPLOG_INCLUDE_DIR templog/logging.h)
if(TEMPLOG_INCLUDE_DIR)
    set(Y60_APE_HAVE_TEMPLOG ON)
    include_directories( ${TEMPLOG_INCLUDE_DIR} )
endif(TEMPLOG_INCLUDE_DIR)

ac_configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/y60_ape_config.h.in
        ${CMAKE_CURRENT_BINARY_DIR}/y60_ape_config.h
        "${CMAKE_CURRENT_LIST_FILE}"
)

ac_add_library(
    yape
    HEADERS ${LIB_HEADERS}
    SOURCES ${SOURCES}
    DEPENDS y60jsbase y60jslib ${JSDEPENDS}
    EXTERNS Boost ${JSEXTERN} READLINE
)

set(Y60_APE_LINK_ARGS
    DEPENDS yape ${JSDEPENDS} y60jslib 
    EXTERNS ${JSEXTERN}
)

add_subdirectory( tests )
add_subdirectory( benchmarks )
add_subdirectory( examples )
add_subdirectory( ajs )

