
cmake_minimum_required(VERSION 2.6.2)

project(OSCPACK)

include(AcMake)

if(WIN32)
    list(APPEND REQ_PACKAGES WINSOCK Winmm)
endif(WIN32)

include_directories(.) 

ac_add_project(
    OSCPACK 
    REQUIRED_PACKAGES
        ${REQ_PACKAGES}
    DEFINITIONS
        -DAC_BUILT_WITH_CMAKE
)

add_subdirectory(osc)
add_subdirectory(ip)

ac_end_project(OSCPACK)



