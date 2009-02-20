
ac_add_executable(
    y60-compare-image
    SOURCES
       compareimage.cpp
       plfilter3x3.cpp
       plfiltersubtract.cpp
    DEPENDS
       aslbase paintlib
    EXTERNS
       OPENEXR
)
