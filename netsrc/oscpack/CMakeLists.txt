
cmake_minimum_required(VERSION 2.6.2)

project(OSCPACK)

include(AcMake)

if(WIN32)
    list(APPEND REQ_PACKAGES WINSOCK Winmm)
endif(WIN32)

include_directories(.) 

include(TestBigEndian)

test_big_endian(OSCPACK_BIG_ENDIAN)

if(OSCPACK_BIG_ENDIAN)
    set(OSCPACK_ENDIAN_DEFINITION -DOSC_HOST_BIG_ENDIAN)
else(OSCPACK_BIG_ENDIAN)
    set(OSCPACK_ENDIAN_DEFINITION -DOSC_HOST_LITTLE_ENDIAN)
endif(OSCPACK_BIG_ENDIAN)

ac_add_project(
    OSCPACK 
    REQUIRED_PACKAGES
        ${REQ_PACKAGES}
    DEFINITIONS
        ${OSCPACK_ENDIAN_DEFINITION}
)

add_subdirectory(osc)
add_subdirectory(ip)

ac_end_project(OSCPACK)



