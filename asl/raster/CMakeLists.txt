
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

ac_add_library(
    aslraster asl/raster
    HEADERS ${HEADERS}
    DEPENDS aslbase
    TESTS   ${TESTS}
    EXTERNS OPEN_EXR
    HEADER_ONLY
)

