
#add_subdirectory(DC1394)
#add_subdirectory(PortVideo)
add_subdirectory(VideoProcessing)

if(LINUX)

    add_subdirectory(DVB)

    if(GSTREAMER_FOUND)
        add_subdirectory(GStreamerCapture)
    endif(GSTREAMER_FOUND)

endif(LINUX)

if(WIN32)
    #add_subdirectory(WMPPlayer)
endif(WIN32)

