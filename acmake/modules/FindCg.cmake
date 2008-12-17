
find_package(OpenGL)

if (APPLE AND UNIX)
    find_library(CG_LIBRARIES Cg )
else (APPLE AND UNIX)
    set(CG_INCLUDE_DIRS ${OPENGL_INCLUDE_DIRS})
    set(CG_LIBRARY_DIRS ${OPENGL_LIBRARY_DIRS})
# XXX This assumes Cg is in a standard location. Better use find_library too?
    set(CG_LIBRARIES Cg CgGL ${OPENGL_LIBRARIES})
endif (APPLE AND UNIX)
