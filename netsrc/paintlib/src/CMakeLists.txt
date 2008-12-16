set(SOURCES
tif_msrc.c
jmemdest.cpp
jmemsrc.cpp
planybmp.cpp
planydec.cpp
plbitmap.cpp
plbmpbase.cpp
plbmpdec.cpp
plbmpenc.cpp
plbmpinfo.cpp
pldatasink.cpp
pldatasrc.cpp
#pldicomdec.cpp
#pldirectfbbmp.cpp
plexcept.cpp
plexif.cpp
plfilesink.cpp
plfilesrc.cpp
plgifdec.cpp
pliff85dec.cpp
pljpegdec.cpp
pljpegenc.cpp
plmemsink.cpp
plmemsrc.cpp
plobject.cpp
plpcxdec.cpp
plpgmdec.cpp
plpicdec.cpp
plpicenc.cpp
plpictdec.cpp
plpixelformat.cpp
plpngdec.cpp
plpngenc.cpp
plppmdec.cpp
plpsddec.cpp
#plsdlbmp.cpp
plsgidec.cpp
plsubbmp.cpp
pltgadec.cpp
pltiffdec.cpp
pltiffenc.cpp
pltiffencex.cpp
plurlsrc.cpp
)

set(HEADERS
config.h
jmemdest.h
jmemsrc.h
planybmp.h
planydec.h
plbitmap.h
plbmpbase.h
plbmpdec.h
plbmpenc.h
plbmpinfo.h
plcountedpointer.h
pldatasink.h
pldatasrc.h
pldebug.h
pldecoderfactory.h
pldicomdec.h
pldirectfbbmp.h
plexcept.h
plexif.h
plfilesink.h
plfilesrc.h
plgifdec.h
pliff85dec.h
pliff85.h
pljpegdec.h
pljpegenc.h
plmemsink.h
plmemsrc.h
plobject.h
ploptable.h
plpaintlibdefs.h
plpcxdec.h
plpcx.h
plpgmdec.h
plpgm.h
plpicdec.h
plpicenc.h
plpictdec.h
plpixel16.h
plpixel24.h
plpixel32.h
plpixel8.h
plpixeldefs.h
plpixelformat.h
plpngdec.h
plpngenc.h
plpoint.h
plppmdec.h
plppm.h
plprognot.h
plpsddec.h
plrect.h
plsgidec.h
plsubbmp.h
pltgadec.h
pltga.h
pltiffdec.h
pltiffencex.h
pltiffenc.h
plurlsrc.h
plwindefs.h
qdraw.h
tif_msrc.h
)

include_directories(.)
include_directories(./Filter)

include_directories(${PNG_INCLUDE_DIRS} ${GIF_INCLUDE_DIRS} ${TIFF_INCLUDE_DIRS} ${JPEG_INCLUDE_DIRS})
link_directories(${PNG_LIBRARY_DIRS} ${GIF_LIBRARY_DIRS} ${TIFF_LIBRARY_DIRS} ${JPEG_LIBRARY_DIRS})

add_library(
    paintlib SHARED ${SOURCES}
)

set_target_properties(
    paintlib
    PROPERTIES
        PUBLIC_HEADER "${HEADERS}"
)

target_link_libraries(paintlib paintlibfilter ${PNG_LIBRARIES} ${GIF_LIBRARIES} ${TIFF_LIBRARIES} ${JPEG_LIBRARIES})

install(
    TARGETS paintlib
    EXPORT paintlib
    LIBRARY
        DESTINATION lib
    PUBLIC_HEADER
        DESTINATION include/paintlib
)

add_subdirectory(Filter)

