
add_subdirectory(y60)

if(Y60_WITH_GTK)
    add_subdirectory(g60)
endif(Y60_WITH_GTK)

add_subdirectory(CheckSchema)
add_subdirectory(CompareImage)
add_subdirectory(GenCompressedTex)
add_subdirectory(GenCubemap)
add_subdirectory(GenMovie)
add_subdirectory(greplace)
add_subdirectory(namesearch)
add_subdirectory(watchdog)
#add_subdirectory(cinemax)

