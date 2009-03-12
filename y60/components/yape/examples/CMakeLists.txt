# XXX need spidermonkey defs
add_definitions( -DXP_UNIX )

y60_add_component(
    ape_ex_hello_world
    SOURCES hello_world_module.cpp
    ${Y60_APE_LINK_ARGS}
)

