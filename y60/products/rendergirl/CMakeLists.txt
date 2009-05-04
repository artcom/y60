
y60_begin_application(
    rendergirl
    DISPLAY_NAME "Rendergirl"
    DESCRIPTION  "Gtk-based Y60 scene viewer"
    DEPENDS G60_runtime
    INSTALL_TYPES development
)

y60_add_assets(
    SCRIPTS ADD_TO_PATH
)

y60_add_assets(
    GLADE
)

y60_add_launcher(
    "Rendergirl"
    DESCRIPTION "Rendergirl Scene Viewer"
    ENGINE g60
    MAIN_SCRIPT rendergirl.js
    BUILD_WORKING_DIR SOURCE
    CATEGORIES
        Graphics
        3DGraphics
        Viewer
        GTK
    MIME_TYPES
        model/x-artcom-x60
        model/x-artcom-b60
        model/x-artcom-d60
)

y60_end_application(rendergirl)

