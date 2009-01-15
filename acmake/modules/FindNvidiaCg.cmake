
#find_package( OpenGL )
find_library( NVIDIA_CG_LIBRARIES NAMES Cg cg )
if(APPLE AND UNIX AND NVIDIA_CG_LIBRARIES MATCHES ".*\\.framework/?$")
        set( NVIDIA_CG ${NVIDIA_CG_LIBRARIES} )
    # TODO: handle non-framework versions on OSX?
else(APPLE AND UNIX AND NVIDIA_CG_LIBRARIES MATCHES ".*\\.framework/?$")
    find_path( NVIDIA_CG_INCLUDE_DIRS cg.h 
            PATH_SUFFIXES Cg GL )
    get_filename_component( NVIDIA_CG_LIBRARY_DIRS
            "${NVIDIA_CG_LIBRARIES}" PATH )
    set(NVIDIA_CG_LIBRARIES Cg CgGL ${OPENGL_LIBRARIES})

    find_package_handle_standard_args(NVIDIA_CG DEFAULT_MSG
            NVIDIA_CG_LIBRARIES NVIDIA_CG_INCLUDE_DIRS)
endif(APPLE AND UNIX AND NVIDIA_CG_LIBRARIES MATCHES ".*\\.framework/?$")

find_package_handle_standard_args(NVIDIA_CG DEFAULT_MSG
        NVIDIA_CG_LIBRARIES NVIDIA_CG_INCLUDE_DIRS)
