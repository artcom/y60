
add_subdirectory(y60)

if(Y60_WITH_GTK)
    add_subdirectory(g60)
endif(Y60_WITH_GTK)

add_subdirectory(CompareImage)

add_subdirectory(GenCompressedTex)
add_subdirectory(GenCubemap)
add_subdirectory(GenMovie)

ac_add_installer_component(
    Y60_utilities
    DISPLAY_NAME "Y60 Utilities"
    DESCRIPTION "Utilities for generating movies, cubemaps and compressed textures."
    DEPENDS Y60_runtime
)

if(Y60_WITH_GTK)
    add_subdirectory(rendergirl)
endif(Y60_WITH_GTK)

if(MAYA_FOUND)
    add_subdirectory(maya-xport)
endif(MAYA_FOUND)

if(C4D_FOUND)
    add_subdirectory(cinemax)
endif(C4D_FOUND)
