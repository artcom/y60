
cmake_minimum_required(VERSION 2.6.2)

project(OSCPACK)

include(AcMake)

include_directories(.) 

ac_add_project(
    OSCPACK 
    DEFINITIONS
        -DAC_BUILT_WITH_CMAKE
)

# XXX: fix this up
if(WIN32)
    set(WS2_32_LIBRARIES Ws2_32) # XXX: write find package
    set(WINMM_LIBRARIES Winmm) # XXX: write find package
    list(APPEND EXTERNS WS2_32 WINMM)
endif(WIN32)

add_subdirectory(osc)
add_subdirectory(ip)

ac_end_project(OSCPACK)
