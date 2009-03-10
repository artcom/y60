
y60_begin_application(rendergirl)

y60_add_assets(
    SCRIPTS ADD_TO_PATH
)

y60_add_assets(GLADE)

y60_add_launcher(
    rendergirl
    ENGINE g60
    MAIN_SCRIPT rendergirl.js
    BUILD_WORKING_DIR SOURCE
)

y60_end_application(rendergirl)

