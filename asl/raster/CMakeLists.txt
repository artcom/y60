
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
  raster
  HEADERS ${HEADERS}
  DEPENDS aslbase
  TESTS   ${TESTS}
  HEADER_ONLY
)

