

ac_add_executable(
    ac-check-schema
    SOURCES
       CheckSchema.cpp
    DEPENDS aslbase asldom
)
