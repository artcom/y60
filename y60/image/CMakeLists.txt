
set(SOURCES
ConnectedComponent.cpp
HeightToNormalMap.cpp
Image.cpp
ImageLoader.cpp
PLFilterFactory.cpp
PLFilterGaussianBlur.cpp
PLFilterResizePadded.cpp
PixelEncoding.cpp
TextureAtlas.cpp
)

set(HEADERS
ConnectedComponent.h
HeightToNormalMap.h
I60Header.h
IPaintLibExtension.h
ITextureManager.h
Image.h
ImageLoader.h
ImageTags.h
PLFilterFactory.h
PLFilterGaussianBlur.h
PLFilterResizePadded.h
PixelEncoding.h
TextureAtlas.h
y60_image_settings.h
)

set(TESTS
HeightToNormalMap
ImageLoader
Raster
TextureAtlas
)

if(NOT WIN32)
    find_package(Boost COMPONENTS thread)
endif(NOT WIN32)

list(APPEND EXTERNALS Boost)

ac_add_library(
    y60image y60/image
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS aslbase asldom aslmath aslzip
            y60base paintlib textureatlas
    EXTERNS ${EXTERNALS}
    TESTS   ${TESTS}
)
