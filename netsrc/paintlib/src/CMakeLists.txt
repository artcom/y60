
add_subdirectory(Filter)


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

include_directories(${PNG_INCLUDE_DIR} ${GIF_INCLUDE_DIR} ${TIFF_INCLUDE_DIR} ${JPEG_INCLUDE_DIR}
                    ${CURL_INCLUDE_DIR})
link_directories(${PNG_LIBRARY_DIR} ${GIF_LIBRARY_DIR} ${TIFF_LIBRARY_DIR} ${JPEG_LIBRARY_DIR}
                 ${CURL_LIBRARY_DIR})

add_library(
    paintlib SHARED ${SOURCES}
)

set_target_properties(
    paintlib
    PROPERTIES
        PUBLIC_HEADER "${HEADERS}"
)

target_link_libraries(paintlib paintlibfilter ${PNG_LIBRARY} ${GIF_LIBRARY} ${TIFF_LIBRARY} ${JPEG_LIBRARY}
                ${CURL_LIBRARY} Half)

install(
    TARGETS paintlib
    EXPORT paintlib
    LIBRARY
        DESTINATION lib
    PUBLIC_HEADER
        DESTINATION include/paintlib
)

