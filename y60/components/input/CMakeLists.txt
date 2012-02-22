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
    # Note: WMTouch needs the Win7 SDK http://www.microsoft.com/download/en/details.aspx?id=3138
    #       This should work even when compiling on WinXP. Of course, Win7 touch events
    #       will only work on Window 7+
    add_subdirectory(WMTouch)
#    add_subdirectory(DInputFFB)
endif(WIN32)

