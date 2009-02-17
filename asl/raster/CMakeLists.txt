
set(SOURCES
dummy.cpp
)

set(HEADERS
asl_raster_settings.h
dummy.h
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

list( APPEND EXTERNALS OPENEXR )

ac_add_library(
    aslraster asl/raster
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS aslbase
    TESTS   ${TESTS}
    EXTERNS ${EXTERNALS}
)
