
ac_add_executable(
    ac-compare-image
    SOURCES
       compareimage.cpp
       plfilter3x3.cpp
       plfiltersubtract.cpp
    DEPENDS
       aslbase paintlib
    EXTERNS
        OPENEXR
)
