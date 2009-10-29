
find_path(
    TUIO_INCLUDE_DIR
    TuioServer.h
    PATH_SUFFIXES TUIO
)

find_library(
    TUIO_LIBRARY
    NAMES libTUIO tuio
)

if(WIN32)
    find_library(
        TUIO_LIBRARY_D
        NAMES libTUIOd
    )
endif(WIN32)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(TUIO DEFAULT_MSG TUIO_LIBRARY TUIO_INCLUDE_DIR)

mark_as_advanced(TUIO_INCLUDE_DIR TUIO_LIBRARY TUIO_LIBRARY_D)
