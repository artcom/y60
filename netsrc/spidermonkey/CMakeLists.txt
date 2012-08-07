
cmake_minimum_required(VERSION 2.6.2)

project(Spidermonkey)

include(AcMake)

if(UNIX) # Apple and Linux, that is ...
    set(SPIDERMONKEY_DEFINITIONS -DXP_UNIX -DHAVE_VA_COPY -DVA_COPY=va_copy)
endif(UNIX)

if(WIN32)
    # Microsoft Windows
    set(SPIDERMONKEY_DEFINITIONS -DXP_WIN -D_WINDOWS -D_X86_=1 -DJSFILE)

    # prevent the linker from merging identical functions
    # this breaks spidermonkey because it uses function pointer comparison
    set(CMAKE_EXE_LINKER_FLAGS    "${CMAKE_EXE_LINKER_FLAGS} /OPT:REF /OPT:NOICF")
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /OPT:REF /OPT:NOICF")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /OPT:REF /OPT:NOICF")
    
endif(WIN32)

ac_add_project(
    Spidermonkey
    DEFINITIONS "${SPIDERMONKEY_DEFINITIONS}"
)

set(SOURCES
    jsapi.c jsarena.c jsarray.c jsatom.c jsbool.c jscntxt.c jsdate.c
    jsdbgapi.c jsdhash.c jsdtoa.c jsemit.c jsexn.c jsfile.c jsfun.c
    jsgc.c jshash.c jsinterp.c jslock.c jslog2.c jslong.c jsmath.c
    jsnum.c jsobj.c jsopcode.c jsparse.c jsprf.c jsprt.c jsregexp.c
    jsscan.c jsscope.c jsscript.c jsstr.c jsutil.c jsxdrapi.c
    prmjtime.c
)

if (WIN32)
    # XXX This is a separate library in the original spidermonkey build
    #     Lets keep things simple and just mix in the sources for now.
    list( APPEND SOURCES fdlibm/e_acos.c fdlibm/e_acosh.c fdlibm/e_asin.c
            fdlibm/e_atan2.c fdlibm/e_atanh.c fdlibm/e_cosh.c fdlibm/e_exp.c
            fdlibm/e_fmod.c fdlibm/e_gamma.c fdlibm/e_gamma_r.c
            fdlibm/e_hypot.c fdlibm/e_j0.c fdlibm/e_j1.c fdlibm/e_jn.c
            fdlibm/e_lgamma.c fdlibm/e_lgamma_r.c fdlibm/e_log.c
            fdlibm/e_log10.c fdlibm/e_pow.c fdlibm/e_rem_pio2.c
            fdlibm/e_remainder.c fdlibm/e_scalb.c fdlibm/e_sinh.c
            fdlibm/e_sqrt.c fdlibm/k_cos.c fdlibm/k_rem_pio2.c fdlibm/k_sin.c
            fdlibm/k_standard.c fdlibm/k_tan.c fdlibm/s_asinh.c fdlibm/s_atan.c
            fdlibm/s_cbrt.c fdlibm/s_ceil.c fdlibm/s_copysign.c fdlibm/s_cos.c
            fdlibm/s_erf.c fdlibm/s_expm1.c fdlibm/s_fabs.c fdlibm/s_finite.c
            fdlibm/s_floor.c fdlibm/s_frexp.c fdlibm/s_ilogb.c fdlibm/s_isnan.c
            fdlibm/s_ldexp.c fdlibm/s_lib_version.c fdlibm/s_log1p.c
            fdlibm/s_logb.c fdlibm/s_matherr.c fdlibm/s_modf.c
            fdlibm/s_nextafter.c fdlibm/s_rint.c fdlibm/s_scalbn.c
            fdlibm/s_signgam.c fdlibm/s_significand.c fdlibm/s_sin.c
            fdlibm/s_tan.c fdlibm/s_tanh.c fdlibm/w_acos.c fdlibm/w_acosh.c
            fdlibm/w_asin.c fdlibm/w_atan2.c fdlibm/w_atanh.c fdlibm/w_cosh.c
            fdlibm/w_exp.c fdlibm/w_fmod.c fdlibm/w_gamma.c fdlibm/w_gamma_r.c
            fdlibm/w_hypot.c fdlibm/w_j0.c fdlibm/w_j1.c fdlibm/w_jn.c
            fdlibm/w_lgamma.c fdlibm/w_lgamma_r.c fdlibm/w_log.c
            fdlibm/w_log10.c fdlibm/w_pow.c fdlibm/w_remainder.c
            fdlibm/w_scalb.c fdlibm/w_sinh.c fdlibm/w_sqrt.c)
endif (WIN32)

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

# stage our headers
foreach(HEADER ${HEADERS})
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/${HEADER}
        ${CMAKE_CURRENT_BINARY_DIR}/include/js/spidermonkey/${HEADER}
        COPYONLY
    )
endforeach(HEADER ${HEADERS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

ac_add_library(
    spidermonkey js/spidermonkey
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    VERSION ${VERSION_STRING}
    SOVERSION ${VERSION_MAJOR}
    RUNTIME_INSTALL_COMPONENT ${RUNTIME_COMPONENT}
    DEVELOPMENT_INSTALL_COMPONENT ${DEVELOPMENT_COMPONENT}
)

_ac_add_include_path(spidermonkey ${CMAKE_CURRENT_BINARY_DIR}/include YES)


set_target_properties(
    spidermonkey
    PROPERTIES
        COMPILE_DEFINITIONS "EXPORT_JS_API"
        COMPILE_DEFINITIONS_DEBUG "DEBUG"
)
# disable a bunch of warnings inside of spidermonkey - we assume they know what they're doing
if (WIN32)
    ac_add_target_properties(spidermonkey COMPILE_FLAGS "/wd4055 /wd4214 /wd4127 /wd4706 /wd4202 /wd4100 /wd4101 /wd4244 /wd4702 /wd4131 /wd4232 /wd4700")
else (WIN32)
    ac_add_target_properties(spidermonkey COMPILE_FLAGS "-w")
endif (WIN32)


ac_add_executable(
    smonkey
    SOURCES js.c
    DEPENDS spidermonkey
    INSTALL_COMPONENT ${RUNTIME_COMPONENT}
)

ac_end_project(Spidermonkey)
