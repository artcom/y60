# XXX need spidermonkey defs
add_definitions( -DXP_UNIX )

y60_add_component(
    ape_test_simple
    SOURCES simple.cpp
    ${Y60_APE_LINK_ARGS}
)

y60_add_component(
    ape_test_functions
    SOURCES functions.cpp
    ${Y60_APE_LINK_ARGS}
)

