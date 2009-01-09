
if(UNIX)
    if(APPLE)
        # Apple Mac OS X
        set(SPIDERMONKEY_DEFINITIONS -DXP_UNIX)
    else(APPLE)
        # Linux
        set(SPIDERMONKEY_DEFINITIONS -DXP_UNIX -DHAVE_VA_COPY -DVA_COPY=va_copy)
    endif(APPLE)
else(UNIX)
    # Microsoft Windows
    set(SPIDERMONKEY_DEFINITIONS -DXP_WIN -D_WINDOWS -D_X86_=1 -DJSFILE)
endif(UNIX)

