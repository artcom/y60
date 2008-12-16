
if(UNIX)
    if(APPLE)
        # Apple Mac OS X
        add_definitions(-DXP_UNIX)
    else(APPLE)
        # Linux
        add_definitions(-DXP_UNIX -DHAVE_VA_COPY -DVA_COPY=va_copy)
    endif(APPLE)
else(UNIX)
    # Microsoft Windows
    add_definitions(-DXP_WIN -D_WINDOWS -D_X86_=1 -DEXPORT_JS_API -DJSFILE)
endif(UNIX)

