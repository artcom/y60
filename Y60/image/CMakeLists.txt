
set(SOURCES
ConnectedComponent.cpp
HeightToNormalMap.cpp
Image.cpp
ImageLoader.cpp
PLFilterFactory.cpp
PLFilterGaussianBlur.cpp
PLFilterResizePadded.cpp
PixelEncoding.cpp
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
)

set(TESTS
HeightToNormalMap
ImageLoader
Raster
)

ac_add_library(
    y60image y60/image
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    EXTERNS OPENEXR CURL PNG TIFF JPEG GIF
    DEPENDS aslbase asldom aslmath aslzip
            y60base paintlib
    TESTS   ${TESTS}
)

