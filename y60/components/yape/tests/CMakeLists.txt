# XXX need spidermonkey defs
add_definitions( -DXP_UNIX )

y60_add_component(
    y60hello_world
    SOURCES hello_world.cpp hello_world_module.cpp
    HEADERS hello_world.h
    DEPENDS yape ${JSDEPENDS} y60jslib
    EXTERNS Boost OPENEXR
)

