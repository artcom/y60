
cmake_minimum_required(VERSION 2.6)

project(Spidermonkey)

set(CMAKE_MODULE_PATH ${Spidermonkey_SOURCE_DIR})

include(SpidermonkeyDefinitions)
include(SpidermonkeyDependencies)

add_definitions(${SPIDERMONKEY_DEFINITIONS})

set(SOURCES
    jsapi.c jsarena.c jsarray.c jsatom.c jsbool.c jscntxt.c jsdate.c
    jsdbgapi.c jsdhash.c jsdtoa.c jsemit.c jsexn.c jsfile.c jsfun.c
    jsgc.c jshash.c jsinterp.c jslock.c jslog2.c jslong.c jsmath.c
    jsnum.c jsobj.c jsopcode.c jsparse.c jsprf.c jsprt.c jsregexp.c
    jsscan.c jsscope.c jsscript.c jsstr.c jsutil.c jsxdrapi.c
    prmjtime.c
)

if (WIN32)
    # Make CMake build all libs in a common directory. Since we build
    # DLLs the tests need to find their libraries even while they are not
    # installed in their final locations (during testrun that is).
    set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")
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

    add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_WARNINGS
            -D_SCL_SECURE_NO_WARNINGS)
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


include_directories(.)

add_library(
    spidermonkey SHARED ${SOURCES}
)

if(SPIDERMONKEY_INTEGRATED_BUILD)
foreach (H_FILE ${HEADERS})
  configure_file(
      ${CMAKE_CURRENT_SOURCE_DIR}/${H_FILE}
      ${CMAKE_CURRENT_BINARY_DIR}/include/js/spidermonkey/${H_FILE}
      COPYONLY)
endforeach(H_FILE)
endif(SPIDERMONKEY_INTEGRATED_BUILD)


set_target_properties(
    spidermonkey
    PROPERTIES
        PUBLIC_HEADER "${HEADERS}"
	COMPILE_DEFINITIONS "EXPORT_JS_API"
)

install(
    TARGETS spidermonkey
    EXPORT Spidermonkey
        DESTINATION lib
    PUBLIC_HEADER
        DESTINATION include/js/spidermonkey
)


add_executable(
    smonkey
    js.c
)

if (WIN32)
    target_link_libraries(smonkey spidermonkey)
else (WIN32)
    target_link_libraries(smonkey spidermonkey m)
endif (WIN32)

install(
    TARGETS smonkey
    EXPORT Spidermonkey
    RUNTIME
        DESTINATION bin
)

if(NOT SPIDERMONKEY_INTEGRATED_BUILD)
configure_file(
    ${Spidermonkey_SOURCE_DIR}/FindSpidermonkey.cmake.in
    ${Spidermonkey_BINARY_DIR}/FindSpidermonkey.cmake
    @ONLY
)

install(
    FILES ${Spidermonkey_BINARY_DIR}/FindSpidermonkey.cmake
    DESTINATION share/cmake-2.6/Modules
)


install(
    FILES SpidermonkeyDependencies.cmake SpidermonkeyDefinitions.cmake
    DESTINATION share/cmake-2.6/Modules
)


install(
    EXPORT Spidermonkey
    DESTINATION share/cmake-2.6/Modules
    FILE SpidermonkeyExport.cmake
)
endif(NOT SPIDERMONKEY_INTEGRATED_BUILD)
