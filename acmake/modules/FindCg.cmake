
find_package(OpenGL)

set(CG_INCLUDE_DIRS ${OPENGL_INCLUDE_DIRS})
set(CG_LIBRARY_DIRS ${OPENGL_LIBRARY_DIRS})
set(CG_LIBRARIES Cg CgGL ${OPENGL_LIBRARIES})