
find_package(OpenGL)

find_path(
    GLEW_INCLUDE_DIR GL/glew.h
    PATHS /usr/include /usr/local/include /opt/local/include
)
mark_as_advanced(GLEW_INCLUDE_DIR)

if(WIN32)
    set(GLEW_LIBRARY_NAME glew32)
else(WIN32)
    set(GLEW_LIBRARY_NAME GLEW)
endif(WIN32)
mark_as_advanced(GLEW_LIBRARY_NAME)

find_library(GLEW_LIBRARY_FULLPATH ${GLEW_LIBRARY_NAME} PATHS /opt/local/lib)
get_filename_component(GLEW_LIBRARY_DIRS ${GLEW_LIBRARY_FULLPATH} PATH)
mark_as_advanced(GLEW_LIBRARY_FULLPATH GLEW_LIBRARY_DIRS)

set(GLEW_LIBRARIES ${GLEW_LIBRARY_NAME} ${OPENGL_LIBRARIES})
set(GLEW_INCLUDE_DIRS ${GLEW_INCLUDE_DIR})
mark_as_advanced(GLEW_LIBRARIES GLEW_INCLUDE_DIRS)
