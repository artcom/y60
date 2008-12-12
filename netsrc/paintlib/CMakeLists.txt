
cmake_minimum_required(VERSION 2.6)

project(paintlib)

find_package(PNG)
find_package(GIF)
find_package(TIFF)
find_package(JPEG)

add_subdirectory(src)

install(
    EXPORT paintlib
    DESTINATION lib/cmake
)
