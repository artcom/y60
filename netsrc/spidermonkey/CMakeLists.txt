
cmake_minimum_required(VERSION 2.6)

project(spidermonkey)

set(SOURCES
jsapi.c
jsarena.c
jsarray.c
jsatom.c
jsbool.c
jscntxt.c
jsdate.c
jsdbgapi.c
jsdhash.c
jsdtoa.c
jsemit.c
jsexn.c
jsfile.c
jsfun.c
jsgc.c
jshash.c
jsinterp.c
jslock.c
jslog2.c
jslong.c
jsmath.c
jsnum.c
jsobj.c
jsopcode.c
jsparse.c
jsprf.c
jsprt.c
jsregexp.c
jsscan.c
jsscope.c
jsscript.c
jsstr.c
jsutil.c
jsxdrapi.c
prmjtime.c
)

set(HEADERS
jsapi.h
jsarena.h
jsarray.h
jsatom.h
jsautocfg_ilp32.h
jsautocfg_lp64.h
jsbit.h
jsbool.h
jsclist.h
jscntxt.h
jscompat.h
jsconfig.h
jscpucfg.h
jsdate.h
jsdbgapi.h
jsdhash.h
jsdtoa.h
jsemit.h
jsexn.h
jsfile.h
jsfile.msg
jsfun.h
jsgc.h
jshash.h
jsinterp.h
jslibmath.h
jslock.h
jslong.h
jsmath.h
js.msg
jsnum.h
jsobj.h
jsopcode.h
jsopcode.tbl
jsosdep.h
jsotypes.h
jsparse.h
jsprf.h
jsprvtd.h
jspubtd.h
jsregexp.h
jsscan.h
jsscope.h
jsscript.h
jsshell.msg
jsstddef.h
jsstr.h
jstypes.h
jsutil.h
jsxdrapi.h
prmjtime.h
resource.h
)

include_directories(.)

if(UNIX)
    if(APPLE)
        # Apple Mac OS X
        add_definitions(-DXP_UNIX)
    else(APPLE)
        # Linux
        add_definitions(-DXP_UNIX -DHAVE_VA_COPY -DVA_COPY=va_copy)
    endif(APPLE)
else(UNIX)
    # Microsoft Windows
    add_definitions(-DXP_WIN -D_WINDOWS -D_X86_=1 -DEXPORT_JS_API -DJSFILE)
endif(UNIX)


add_library(
    spidermonkey SHARED ${SOURCES}
)

set_target_properties(
    spidermonkey
    PROPERTIES
        PUBLIC_HEADER "${HEADERS}"
)

add_executable(
    monkey
    js.c
)

target_link_libraries(monkey spidermonkey m)

install(
    TARGETS monkey
    EXPORT  spidermonkey
    RUNTIME
        DESTINATION bin
)

install(
    TARGETS spidermonkey
    EXPORT  spidermonkey
    LIBRARY
        DESTINATION lib
    PUBLIC_HEADER
        DESTINATION include/js/spidermonkey
)

install(
    EXPORT spidermonkey
    DESTINATION lib/cmake
)

