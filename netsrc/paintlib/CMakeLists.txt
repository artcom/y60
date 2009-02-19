
cmake_minimum_required(VERSION 2.6.2)

project(Paintlib)

include(AcMake)

ac_add_project(
    Paintlib
    REQUIRED_PACKAGES PNG GIF TIFF JPEG CURL OpenEXR
    DEFINITIONS -DAC_BUILT_WITH_CMAKE
)

add_subdirectory(src)

ac_end_project(Paintlib)
