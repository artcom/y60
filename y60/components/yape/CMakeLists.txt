set( LIB_HEADERS
    class_wrapper.h
    detail/invoke.h
    detail/preprocessor.h
    detail/signature.h
    detail/signature_utils.h
    module.h
    module_initializer.h
    yape.h
)
# XXX need spidermonkey defs
add_definitions( -DXP_UNIX )

y60_add_component(
    HelloWorld
    SOURCES hello_world_wrapper.cpp
    HEADERS ${LIB_HEADERS}
    DEPENDS ${JSDEPENDS} y60jsbase
)

