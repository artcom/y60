
find_path( PTHREADVC_INCLUDE_DIR pthread.h )
mark_as_advanced( PTHREADVC_INCLUDE_DIR )


find_library(PTHREADVC_LIBRARY pthreadVC)
mark_as_advanced( PTHREADVC_LIBRARY )

include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( PTHREADVC DEFAULT_MSG PTHREADVC_LIBRARY
        PTHREADVC_INCLUDE_DIR )

if( PTHREADVC_FOUND )
    set( PTHREADVC_LIBRARIES ${PTHREADVC_LIBRARY} )
    set( PTHREADVC_INCLUDE_DIRS ${PTHREADVC_INCLUDE_DIR} )
else( PTHREADVC_FOUND )
    set( PTHREADVC_LIBRARIES )
    set( PTHREADVC_INCLUDE_DIRS )
endif( PTHREADVC_FOUND )

