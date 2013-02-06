
if(CAIRO_FOUND AND PANGO_FOUND AND PANGOCAIRO_FOUND)
    add_subdirectory(Cairo)
    add_subdirectory(Pango)
endif(CAIRO_FOUND AND PANGO_FOUND AND PANGOCAIRO_FOUND)

add_subdirectory(EdgeBlender)
add_subdirectory(GLBinding)
