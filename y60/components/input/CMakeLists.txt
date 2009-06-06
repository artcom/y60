
add_subdirectory(ASSDriver)
add_subdirectory(DSADriver)

if(LINUX)
    add_subdirectory(PowermateInputExtension)
    #add_subdirectory(Wiimote)
endif(LINUX)

if(WIN32)
    #add_subdirectory(DInput)
    #add_subdirectory(DInputFFB)
endif(WIN32)

