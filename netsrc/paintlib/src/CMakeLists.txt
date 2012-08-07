
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


Filter/plfiltercolorize.cpp
Filter/plfiltercontrast.cpp
Filter/plfilter.cpp
Filter/plfiltercrop.cpp
Filter/plfilterflip.cpp
Filter/plfilterfliprgb.cpp
Filter/plfiltergetalpha.cpp
Filter/plfiltergrayscale.cpp
Filter/plfilterintensity.cpp
Filter/plfilterlightness.cpp
Filter/plfiltermirror.cpp
Filter/plfilterquantize.cpp
Filter/plfilterresizebilinear.cpp
Filter/plfilterresizebox.cpp
Filter/plfilterresize.cpp
Filter/plfilterresizegaussian.cpp
Filter/plfilterresizehamming.cpp
Filter/plfilterrotate.cpp
Filter/plfilterthreshold.cpp
Filter/plfiltervideoinvert.cpp
Filter/plhsvconvert.cpp
Filter/plinplacefilter.cpp
)

if (WIN32)
    list( APPEND SOURCES win/plwemfdec.cpp win/plressrc.cpp win/plwinbmp.cpp)
endif (WIN32)


set(HEADERS
config.h
jmemdest.h
jmemsrc.h
paintlib_settings.h
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

set(FILTER_HEADERS
Filter/pl2passscale.h
Filter/plcontribdefs.h
Filter/plfiltercolorize.h
Filter/plfiltercontrast.h
Filter/plfiltercrop.h
Filter/plfilterfill.h
Filter/plfilterfillrect.h
Filter/plfilterflip.h
Filter/plfilterfliprgb.h
Filter/plfiltergetalpha.h
Filter/plfiltergrayscale.h
Filter/plfilter.h
Filter/plfilterintensity.h
Filter/plfilterlightness.h
Filter/plfiltermirror.h
Filter/plfilterpixel.h
Filter/plfilterquantize.h
Filter/plfilterresizebilinear.h
Filter/plfilterresizebox.h
Filter/plfilterresizegaussian.h
Filter/plfilterresize.h
Filter/plfilterresizehamming.h
Filter/plfilterrotate.h
Filter/plfilterthreshold.h
Filter/plfiltervideoinvert.h
Filter/plhsvconvert.h
Filter/plinplacefilter.h
)

# stage our headers
foreach(HEADER ${HEADERS} ${FILTER_HEADERS})
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/${HEADER}
        ${CMAKE_CURRENT_BINARY_DIR}/../include/paintlib/${HEADER}
        COPYONLY
    )
endforeach(HEADER ${HEADERS})

ac_add_library(
    paintlib paintlib
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    EXTERNS GIF PNG TIFF JPEG OPENEXR CURL
    VERSION ${VERSION_STRING}
    SOVERSION ${VERSION_MAJOR}
    RUNTIME_INSTALL_COMPONENT ${RUNTIME_COMPONENT}
    DEVELOPMENT_INSTALL_COMPONENT ${DEVELOPMENT_COMPONENT}
)

include_directories(. ./Filter)

_ac_add_include_path(paintlib ${CMAKE_CURRENT_BINARY_DIR}/../include YES)

# add debug definition
set_property(
    TARGET paintlib
    PROPERTY COMPILE_DEFINITIONS_DEBUG _DEBUG
)

# XXX: dirty hack to work around how we reference plfilter headers
install(
    DIRECTORY Filter
    DESTINATION include/paintlib
    COMPONENT ${DEVELOPMENT_COMPONENT}
    FILES_MATCHING
        PATTERN "*.h"
        PATTERN ".svn" EXCLUDE
)
