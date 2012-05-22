
# XXX Proper checks missing... [DS]
if(WIN32)
    if(DIRECTX_FOUND)    
        add_subdirectory(DShowCapture)
    endif(DIRECTX_FOUND)        
    if (ALLIED_GIGABIT_FOUND)
        add_subdirectory(AlliedEthernetCameraCapture)
    endif(ALLIED_GIGABIT_FOUND)        
endif(WIN32)

