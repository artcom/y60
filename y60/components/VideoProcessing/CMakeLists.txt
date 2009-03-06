
set(SOURCES
AdaptiveBackground.cpp
BackgroundSubtraction.cpp
Blobs.cpp
Canny.cpp
ColorDetection.cpp
Contrast.cpp
Convert.cpp
DumbScale.cpp
FastBlur.cpp
Histogram.cpp
Mask.cpp
MaskPerson.cpp
ShotDetection.cpp
VideoProcessing.cpp
)

set(HEADERS
AdaptiveBackground.h
Algorithm.h
BackgroundSubtraction.h
Blobs.h
Canny.h
ColorDetection.h
Contrast.h
Convert.h
DumbScale.h
FastBlur.h
Histogram.h
Mask.h
MaskPerson.h
ShotDetection.h
VideoProcessing.h
cv_algo.h
more_algo.h
y60_videoprocessing_settings.h
)

y60_add_component(
    y60VideoProcessing
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib
)
