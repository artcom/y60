set(SOURCES
plfiltercolorize.cpp
plfiltercontrast.cpp
plfilter.cpp
plfiltercrop.cpp
plfilterflip.cpp
plfilterfliprgb.cpp
plfiltergetalpha.cpp
plfiltergrayscale.cpp
plfilterintensity.cpp
plfilterlightness.cpp
plfiltermirror.cpp
plfilterquantize.cpp
plfilterresizebilinear.cpp
plfilterresizebox.cpp
plfilterresize.cpp
plfilterresizegaussian.cpp
plfilterresizehamming.cpp
plfilterrotate.cpp
plfilterthreshold.cpp
plfiltervideoinvert.cpp
plhsvconvert.cpp
plinplacefilter.cpp
)

set(HEADERS
pl2passscale.h
plcontribdefs.h
plfiltercolorize.h
plfiltercontrast.h
plfiltercrop.h
plfilterfill.h
plfilterfillrect.h
plfilterflip.h
plfilterfliprgb.h
plfiltergetalpha.h
plfiltergrayscale.h
plfilter.h
plfilterintensity.h
plfilterlightness.h
plfiltermirror.h
plfilterpixel.h
plfilterquantize.h
plfilterresizebilinear.h
plfilterresizebox.h
plfilterresizegaussian.h
plfilterresize.h
plfilterresizehamming.h
plfilterrotate.h
plfilterthreshold.h
plfiltervideoinvert.h
plhsvconvert.h
plinplacefilter.h
)

include_directories(..)
include_directories(.)

add_library(
    paintlibfilter SHARED ${SOURCES}
)

set_target_properties(
    paintlibfilter
    PROPERTIES
        PUBLIC_HEADER "${HEADERS}"
)

install(
    TARGETS paintlibfilter
    EXPORT  paintlib
    LIBRARY
         DESTINATION lib
    PUBLIC_HEADER
         DESTINATION include/paintlib/Filter
)


