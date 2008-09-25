
cmake_minimum_required(VERSION 2.6)

enable_testing()

project(asl)

set(CMAKE_MODULE_PATH ../acmake ${CMAKE_MODULE_PATH})

include(FindZLIB)

include(FindPkgConfig)

pkg_search_module(GLIB2 REQUIRED glib-2.0)
pkg_search_module(ALSA  REQUIRED alsa)
pkg_search_module(CURL  REQUIRED libcurl)

include(SvnGenerateCxx)


include_directories(../)

if(UNIX)
  set(LINUX true)
  add_definitions(-DLINUX)
  link_libraries(m dl pthread rt stdc++)
endif(UNIX)

add_subdirectory(base)
add_subdirectory(math)
add_subdirectory(audio)
add_subdirectory(raster)
add_subdirectory(net)
add_subdirectory(ipc)
add_subdirectory(dom)
add_subdirectory(xpath)
add_subdirectory(serial)
add_subdirectory(zip)

