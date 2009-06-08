
find_path(
    DCMTK_INCLUDE_DIR dcmtk/dcmimage/dipitiff.h
    PATHS /usr/include
)
mark_as_advanced(DCMTK_INCLUDE_DIR)

set(DCMTK_SUBLIBS
    dcmdata
    dcmdsig
    dcmimage
    dcmimgle
    dcmjpeg
    dcmnet
    dcmpstat
    dcmqrdb
    dcmsr
    dcmtls
    dcmwlm
)

set(DCMTK_LIBRARIES)
foreach(SUBLIB ${DCMTK_SUBLIBS})
    string(TOUPPER ${SUBLIB} SUBLIB_UPPER)
    find_library(DCMTK_${SUBLIB_UPPER}_LIBRARY ${SUBLIB})
    list(APPEND DCMTK_LIBRARIES ${SUBLIB})
endforeach(SUBLIB)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    DCMTK DCMTK_INCLUDE_DIR DCMTK_LIBRARIES
)

