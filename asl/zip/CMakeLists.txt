
set(SOURCE
DirectoryPackage.cpp
PackageManager.cpp
ZipPackage.cpp
ZipReader.cpp
ZipWriter.cpp

ioapi.c
iowin32.c
mztools.c
unzip.c
zip.c
)

set(HEADERS
DirectoryPackage.h
IPackage.h
PackageManager.h
ZipPackage.h
ZipReader.h
ZipWriter.h
)

set(TESTS
#PackageManager
ZipFile
)

if(APPLE)
  set(SOURCE ${SOURCE})
  set(HEADERS ${HEADERS})
endif(APPLE)

if(LINUX)
  set(SOURCE ${SOURCE})
  set(HEADERS ${HEADERS})
endif(LINUX)

if(WIN32)
  set(SOURCE ${SOURCE})
  set(HEADERS ${HEADERS})
endif(WIN32)

add_library(aslzip SHARED ${SOURCE})
set_target_properties(aslzip PROPERTIES PUBLIC_HEADER "${HEADERS}")
target_link_libraries(aslzip aslbase ${ZLIB_LIBRARIES})

install(
  TARGETS aslzip
  LIBRARY
    DESTINATION lib
  PUBLIC_HEADER 
    DESTINATION include/asl/zip
)

foreach(test ${TESTS})
  add_executable(test${test} test${test}.tst.cpp)
  target_link_libraries(test${test} aslzip)
  add_test(${test} test${test})
endforeach(test)

