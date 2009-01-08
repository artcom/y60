
set(HEADERS
contain.h
fraction.h
palgo.h
piterator.h
pixel_functions.h
pixels.h
raster.h
standard_pixel_types.h
subraster.h
)

set(TESTS
fraction
PixelRaster
Pixels
)

list( INSERT EXTERNALS 0 OPENEXR )

if(WIN32)
    list( INSERT EXTERNALS 0 PTHREADVC )
endif(WIN32)	

ac_add_library(
    aslraster asl/raster
    HEADERS ${HEADERS}
    DEPENDS aslbase
    TESTS   ${TESTS}
    EXTERNS ${EXTERNALS}
    HEADER_ONLY
)

