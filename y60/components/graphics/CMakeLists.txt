
if(CAIRO_FOUND AND PANGO_FOUND AND PANGOCAIRO_FOUND)
    add_subdirectory(Cairo)
endif(CAIRO_FOUND AND PANGO_FOUND AND PANGOCAIRO_FOUND)

add_subdirectory(EdgeBlender)
add_subdirectory(GLBinding)
add_subdirectory(GlurFilter)
add_subdirectory(GPUParticles)
add_subdirectory(WindowCW)

if(WIN32 OR LINUX)
    add_subdirectory(SimWater)
endif(WIN32 OR LINUX)
