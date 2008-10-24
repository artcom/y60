
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

ac_add_library(
  zip
  SOURCES ${SOURCES}
  HEADERS ${HEADERS}
  TESTS ${TESTS}
  DEPENDS aslbase
  EXTERNS ZLIB
)
