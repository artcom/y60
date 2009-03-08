set( LIB_HEADERS
arguments.h
class.h
exception.h
invoke.h
module.h
signature.h
typelist.h
y60_ape_settings.h
yape.h
)

ac_add_library(
    yape
    HEADERS ${LIB_HEADERS}
    DEPENDS y60jsbase
    EXTERNS Boost
)

add_subdirectory( tests )
