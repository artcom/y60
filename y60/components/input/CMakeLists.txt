add_subdirectory(Gesture)
add_subdirectory(ASSDriver)
add_subdirectory(DSADriver)

if(TUIO_FOUND)
    add_subdirectory(TUIO)
endif(TUIO_FOUND)

if(LINUX)
    add_subdirectory(Powermate)
    #add_subdirectory(Wiimote)
endif(LINUX)

if(WIN32)
    add_subdirectory(DInput)
#    add_subdirectory(DInputFFB)
endif(WIN32)

