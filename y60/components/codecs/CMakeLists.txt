
add_subdirectory(FFMpegDecoder1)
add_subdirectory(FFMpegDecoder2)
add_subdirectory(FFMpegDecoder3)

if(OSX)
    #add_subdirectory(QuicktimeDecoder)
endif(OSX)

if(WIN32)
    add_subdirectory(WMADecoder)
    add_subdirectory(WMVDecoder)
endif(WIN32)

