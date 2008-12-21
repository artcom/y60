
include(AcMake)

macro(y60_add_component COMPONENT_NAME)
    ac_add_plugin(
        ${COMPONENT_NAME} y60/components
        ${ARGN}
    )
endmacro(y60_add_component)

