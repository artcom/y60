
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

include_directories(.)
include_directories(./Filter)

if(OPENEXR_INCLUDE_DIR)
    set(OPENEXR_INCLUDE_DIRS ${OPENEXR_INCLUDE_DIR})
endif(OPENEXR_INCLUDE_DIR)
if(OPENEXR_LIBRARY_DIR)
    set(OPENEXR_LIBRARY_DIRS ${OPENEXR_LIBRARY_DIR})
endif(OPENEXR_LIBRARY_DIR)

include_directories(${PNG_INCLUDE_DIR} ${GIF_INCLUDE_DIR} ${TIFF_INCLUDE_DIR}
        ${JPEG_INCLUDE_DIR} ${CURL_INCLUDE_DIR} ${OPENEXR_INCLUDE_DIRS})
link_directories(${PNG_LIBRARY_DIR} ${GIF_LIBRARY_DIR} ${TIFF_LIBRARY_DIR}
        ${JPEG_LIBRARY_DIR} ${CURL_LIBRARY_DIR} ${OPENEXR_LIBRARY_DIRS})

        
foreach( DEF ${OPENEXR_DEFINITIONS} )
    add_definitions( -D${DEF} )
endforeach( DEF ${OPENEXR_DEFINITIONS} )

add_library(
    paintlib SHARED ${SOURCES} ${HEADERS}
)

set_target_properties(
    paintlib
    PROPERTIES
        PUBLIC_HEADER "${HEADERS}"
)

# XXX: need to expose all libraries as transitive link deps
set(OPTIMIZED_LIBRARIES
    ${PNG_LIBRARY}
    ${GIF_LIBRARY}
    ${TIFF_LIBRARY}
    ${JPEG_LIBRARY}
    ${CURL_LIBRARY}
    ${OPENEXR_LIBRARIES}
)

set(DEBUG_LIBRARIES)
if(PNG_LIBRARY_D)
    list(APPEND DEBUG_LIBRARIES ${PNG_LIBRARY_D})
endif(PNG_LIBRARY_D)
if(GIF_LIBRARY_D)
    list(APPEND DEBUG_LIBRARIES ${GIF_LIBRARY_D})
endif(GIF_LIBRARY_D)
if(TIFF_LIBRARY_D)
    list(APPEND DEBUG_LIBRARIES ${TIFF_LIBRARY_D})
endif(TIFF_LIBRARY_D)
if(JPEG_LIBRARY_D)
    list(APPEND DEBUG_LIBRARIES ${JPEG_LIBRARY_D})
endif(JPEG_LIBRARY_D)
if(OPENEXR_LIBRARIES_D)
    list(APPEND DEBUG_LIBRARIES ${OPENEXR_LIBRARIES_D})
endif(OPENEXR_LIBRARIES_D)

if(DEBUG_LIBRARIES)
    foreach( SUBLIB ${OPTIMIZED_LIBRARIES})
        target_link_libraries(paintlib optimized ${SUBLIB})
    endforeach( SUBLIB ${OPTIMIZED_LIBRARIES})
    foreach( SUBLIB ${DEBUG_LIBRARIES})
        target_link_libraries(paintlib debug ${SUBLIB})
    endforeach( SUBLIB ${DEBUG_LIBRARIES})
else(DEBUG_LIBRARIES)
    target_link_libraries(paintlib ${OPTIMIZED_LIBRARIES})
endif(DEBUG_LIBRARIES)

if(PAINTLIB_INTEGRATED_BUILD)
foreach (H_FILE ${HEADERS})
  configure_file(
      ${CMAKE_CURRENT_SOURCE_DIR}/${H_FILE}
      ${CMAKE_CURRENT_BINARY_DIR}/../include/paintlib/${H_FILE}
      COPYONLY)
endforeach(H_FILE)
foreach(H_FILE ${FILTER_HEADERS})
  configure_file(
      ${CMAKE_CURRENT_SOURCE_DIR}/${H_FILE}
      ${CMAKE_CURRENT_BINARY_DIR}/../include/paintlib/${H_FILE}
      COPYONLY)
endforeach(H_FILE)
endif(PAINTLIB_INTEGRATED_BUILD)

# install additional headers from Filter/ directory to an appropriate destination
install(
    DIRECTORY Filter
    DESTINATION include/paintlib
    FILES_MATCHING PATTERN "*.h"
    PATTERN .svn EXCLUDE
)


install(
    TARGETS paintlib
    EXPORT paintlib
        DESTINATION lib
    PUBLIC_HEADER
        DESTINATION include/paintlib
)
