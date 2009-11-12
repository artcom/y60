
add_subdirectory(ASSCore)
add_subdirectory(ASSSchema)
add_subdirectory(ASSEventSource)
add_subdirectory(ASSOscClient)

if(TUIO_LIBRARY) 
    add_subdirectory(ASSTuioServer)
endif(TUIO_LIBRARY) 

