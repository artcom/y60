
add_subdirectory(audio)
add_subdirectory(codecs)
add_subdirectory(example)
add_subdirectory(graphics)
add_subdirectory(formats)
add_subdirectory(input)
add_subdirectory(networking)
add_subdirectory(system)
add_subdirectory(testing)
add_subdirectory(video)

option(Y60_APE_EXPERIMENT "Enable experimental *monkey wrappers" NO)
if(Y60_APE_EXPERIMENT)
    add_subdirectory(yape)
endif(Y60_APE_EXPERIMENT)

