if( WIN32 )
    find_path(GLIB2_INCLUDE_DIR glib.h 
              PATH_SUFFIXES glib-2.0
              PATHS $ENV{GTKMM_BASEPATH}/include )

    find_path(GLIB2_CONFIG_INCLUDE_DIR glibconfig.h 
              PATH_SUFFIXES glib-2.0/include
              PATHS $ENV{GTKMM_BASEPATH}/lib )

    list( APPEND GLIB2_INCLUDE_DIR ${GLIB2_CONFIG_INCLUDE_DIR})

    find_library(GLIB2_LIBRARIES 
                 NAMES glib-2.0
                 PATHS $ENV{GTKMM_BASEPATH}/lib )
 
    get_filename_component( GLIB2_LIBRARY_DIR ${GLIB2_LIBRARIES} PATH)

    mark_as_advanced(GLIB2_LIBRARY_DIR GLIB2_INCLUDE_DIR 
            GLIB2_CONFIG_INCLUDE_DIR GLIB2_LIBRARIES)
else( WIN32 )
    pkg_search_module( GLIB2 REQUIRED glib-2.0 )
endif( WIN32 )

