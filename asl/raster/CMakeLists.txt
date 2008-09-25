
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

install(
  FILES ${HEADERS}
  DESTINATION include/asl/raster
)

foreach(test ${TESTS})
  add_executable(test${test} test${test}.tst.cpp)
  target_link_libraries(test${test} aslbase)
  add_test(${test} test${test})
endforeach(test)
