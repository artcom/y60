
add_subdirectory(ProcFunctions)
add_subdirectory(EventLoop)
add_subdirectory(JSSound)
add_subdirectory(Nagios)
add_subdirectory(SynergyServer)
add_subdirectory(testRendererExtension)

add_subdirectory(codecs)
add_subdirectory(example)
add_subdirectory(graphics)
add_subdirectory(formats)
add_subdirectory(input)
add_subdirectory(networking)
add_subdirectory(video)

option(Y60_APE_EXPERIMENT "Enable experimental *monkey wrappers" NO)
if(Y60_APE_EXPERIMENT)
    add_subdirectory(yape)
endif(Y60_APE_EXPERIMENT)

