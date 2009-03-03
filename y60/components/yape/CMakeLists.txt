set( LIB_HEADERS
    class_wrapper.h
    exception.h
    invoke.h
    preprocessor.h
    signature.h
    signature_utils.h
    module.h
    module_initializer.h
    module_loader.h
    y60_ape_settings.h
    yape.h
)
# XXX need spidermonkey defs
add_definitions( -DXP_UNIX )

ac_add_library(
    yape
    SOURCES dummy.cpp
    HEADERS ${LIB_HEADERS} dummy.h
    DEPENDS y60jsbase
    EXTERNS Boost
#    HEADER_ONLY
)

add_subdirectory( tests )
