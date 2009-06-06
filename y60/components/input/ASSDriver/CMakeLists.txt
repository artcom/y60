
add_subdirectory(ASSCore)
add_subdirectory(ASSSchema)
add_subdirectory(ASSEventSource)
add_subdirectory(ASSOscClient)

ac_add_installer_component(
    ASS_runtime
    DISPLAY_NAME "Proximatrix Runtime and Driver"
    DESCRIPTION "Support libraries for ART+COM Proximatrix multitouch arrays"
    DEPENDS Y60_runtime
    INSTALL_TYPES runtime
)

ac_add_installer_component(
    ASS_development
    DISPLAY_NAME "Proximatrix Development"
    DESCRIPTION "Development files for extending the ART+COM Proximatrix driver"
    DEPENDS Y60_runtime Y60_development ASS_runtime
    INSTALL_TYPES runtime development
)
