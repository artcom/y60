
#add_subdirectory(DC1394)
#add_subdirectory(PortVideo)
add_subdirectory(VideoProcessing)

if(LINUX)

    add_subdirectory(DVB)

    if(GSTREAMER_FOUND)
        add_subdirectory(GStreamerCapture)
    endif(GSTREAMER_FOUND)

endif(LINUX)

# XXX Proper checks missing... [DS]
if(WIN32)
    #add_subdirectory(WMPPlayer)
    if(DIRECTX_FOUND)    
        add_subdirectory(DShowCapture)
    endif(DIRECTX_FOUND)        
    if (ALLIED_GIGABIT_FOUND)
        add_subdirectory(AlliedEthernetCameraCapture)
    endif(ALLIED_GIGABIT_FOUND)        
endif(WIN32)

