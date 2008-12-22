
find_package(OpenGL)

find_path( GLEW_HEADER glew.h /usr/include/GL /usr/local/include/GL /opt/local/include/GL)
get_filename_component( GLEW_INCLUDE_DIRS ${GLEW_HEADER} PATH )
string( REGEX REPLACE "GL[/\\]?$" "" GLEW_INCLUDE_DIRS ${GLEW_INCLUDE_DIRS} )

find_library( GLEW_LIBRARY_FULLPATH NAMES GLEW PATHS /opt/local/lib )

get_filename_component( GLEW_LIBRARY_DIRS ${GLEW_LIBRARY_FULLPATH} PATH)
set(GLEW_LIBRARIES GLEW ${OPENGL_LIBRARIES})

#message( "==============" ${GLEW_INCLUDE_DIRS})
#message( "==============" ${GLEW_LIBRARIES})
#message( "==============" ${GLEW_LIBRARY_DIRS})
