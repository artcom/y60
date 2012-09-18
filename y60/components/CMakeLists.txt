
add_subdirectory(audio)
add_subdirectory(codecs)
add_subdirectory(crypto)
add_subdirectory(graphics)
add_subdirectory(formats)
add_subdirectory(input)
add_subdirectory(networking)
add_subdirectory(system)
add_subdirectory(video)

option(Y60_APE_EXPERIMENT "Enable experimental *monkey wrappers" NO)
if(Y60_APE_EXPERIMENT)
    add_subdirectory(yape)
endif(Y60_APE_EXPERIMENT)

cpack_add_component_group( y60_components_runtime
                           DISPLAY_NAME "Y60 Components"
                           DESCRIPTION "Y60 Components runtime libraries")
cpack_add_component_group( y60_components_development
                           DISPLAY_NAME "Y60 Components Development"
                           DESCRIPTION "Y60 Components header files and import libraries")
