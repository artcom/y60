# XXX need spidermonkey defs
#add_definitions( -DXP_UNIX )

y60_add_component(
    ape_bench_compile_time
    SOURCES n_functions.cpp
    ${Y60_APE_LINK_ARGS}
)
