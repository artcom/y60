
ac_add_executable(
    compareimage
    SOURCES
       compareimage.cpp
       plfilter3x3.cpp
       plfiltersubtract.cpp
    DEPENDS
       aslbase paintlib
    EXTERNS
       OPENEXR
)
