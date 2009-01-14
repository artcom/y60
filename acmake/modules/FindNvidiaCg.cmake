
find_package( OpenGL )
find_path( NVIDIA_CG_INCLUDE_DIRS cg.h 
        PATH_SUFFIXES Cg GL )
find_library( NVIDIA_CG_LIBRARY_FULL_PATH NAMES cg Cg )
mark_as_advanced( NVIDIA_CG_LIBRARY_FULL_PATH )
get_filename_component( NVIDIA_CG_LIBRARY_DIRS
        ${NVIDIA_CG_LIBRARY_FULL_PATH} PATH )
set(NVIDIA_CG_LIBRARIES Cg CgGL ${OPENGL_LIBRARIES})

find_package_handle_standard_args(NVIDIA_CG DEFAULT_MSG
        NVIDIA_CG_LIBRARIES NVIDIA_CG_INCLUDE_DIRS)
