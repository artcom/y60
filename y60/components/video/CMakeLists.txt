
#add_subdirectory(DC1394)
#add_subdirectory(PortVideo)
add_subdirectory(VideoProcessing)

if(LINUX AND GSTREAMER_FOUND)
    add_subdirectory(GStreamerCapture)
endif(LINUX AND GSTREAMER_FOUND)

