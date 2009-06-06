
add_subdirectory(ASSDriver)
add_subdirectory(DSADriver)

if(LINUX)
    add_subdirectory(PowermateInputExtension)
    #add_subdirectory(Wiimote)
endif(LINUX)
