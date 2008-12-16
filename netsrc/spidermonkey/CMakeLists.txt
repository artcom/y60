
cmake_minimum_required(VERSION 2.6)

project(Spidermonkey)

set(CMAKE_MODULE_PATH ${Spidermonkey_SOURCE_DIR})

include(SpidermonkeyDefinitions)
include(SpidermonkeyDependencies)

set(SOURCES
    jsapi.c jsarena.c jsarray.c jsatom.c jsbool.c jscntxt.c jsdate.c
    jsdbgapi.c jsdhash.c jsdtoa.c jsemit.c jsexn.c jsfile.c jsfun.c
    jsgc.c jshash.c jsinterp.c jslock.c jslog2.c jslong.c jsmath.c
    jsnum.c jsobj.c jsopcode.c jsparse.c jsprf.c jsprt.c jsregexp.c
    jsscan.c jsscope.c jsscript.c jsstr.c jsutil.c jsxdrapi.c
    prmjtime.c
)

# XXX: there are probably non-public headers in here
set(HEADERS
    jsapi.h jsarena.h jsarray.h jsatom.h jsbit.h jsbool.h
    jsclist.h jscntxt.h jscompat.h jsconfig.h jscpucfg.h
    jsdate.h jsdbgapi.h jsdhash.h jsdtoa.h jsemit.h jsexn.h
    jsfile.h jsfile.msg jsfun.h jsgc.h jshash.h jsinterp.h
    jslibmath.h jslock.h jslong.h jsmath.h js.msg jsnum.h
    jsobj.h jsopcode.h jsopcode.tbl jsosdep.h jsotypes.h
    jsparse.h jsprf.h jsprvtd.h jspubtd.h jsregexp.h jsscan.h
    jsscope.h jsscript.h jsshell.msg jsstddef.h jsstr.h
    jstypes.h jsutil.h jsxdrapi.h prmjtime.h resource.h
    jsautocfg_ilp32.h jsautocfg_lp64.h
)


include_directories(.)

add_library(
    spidermonkey SHARED ${SOURCES}
)

set_target_properties(
    spidermonkey
    PROPERTIES
        PUBLIC_HEADER "${HEADERS}"
)

install(
    TARGETS spidermonkey
    EXPORT Spidermonkey
    LIBRARY
        DESTINATION lib
    PUBLIC_HEADER
        DESTINATION include/js/spidermonkey
)


add_executable(
    monkey
    js.c
)

target_link_libraries(monkey spidermonkey m)

install(
    TARGETS monkey
    EXPORT Spidermonkey
    RUNTIME
        DESTINATION bin
)


configure_file(
    ${Spidermonkey_SOURCE_DIR}/FindSpidermonkey.cmake.in
    ${Spidermonkey_BINARY_DIR}/FindSpidermonkey.cmake
    @ONLY
)

install(
    FILES ${Spidermonkey_BINARY_DIR}/FindSpidermonkey.cmake
    DESTINATION lib/cmake
)


install(
    FILES SpidermonkeyDependencies.cmake SpidermonkeyDefinitions.cmake
    DESTINATION lib/cmake
)


install(
    EXPORT Spidermonkey
    DESTINATION lib/cmake
    FILE SpidermonkeyExport.cmake
)
