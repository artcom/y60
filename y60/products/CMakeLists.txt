
add_subdirectory(y60)

if(Y60_WITH_GTK)
    add_subdirectory(g60)
endif(Y60_WITH_GTK)

add_subdirectory(CompareImage)

add_subdirectory(GenCompressedTex)
add_subdirectory(ViewACM)
add_subdirectory(GenACM)
add_subdirectory(GenCubemap)
add_subdirectory(GenMovie)

if(Y60_WITH_GTK)
    add_subdirectory(rendergirl)
endif(Y60_WITH_GTK)

if(MAYA_FOUND)
    add_subdirectory(maya-xport)
    add_subdirectory(maya-utils)
endif(MAYA_FOUND)

if(C4D_FOUND)
    add_subdirectory(cinemax)
endif(C4D_FOUND)
