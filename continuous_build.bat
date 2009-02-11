BREAK=ON

set BUILD_DIR=_build
set CMAKE_GENERATOR=Visual Studio 9 2008

if exist %BUILD_DIR% goto build_dir_exists

mkdir %BUILD_DIR%

:build_dir_exists
cd %BUILD_DIR%

call "%VS90COMNTOOLS%..\..\VC\vcvarsall.bat"

cmake -G "%CMAKE_GENERATOR%" ..
if ERRORLEVEL 1 goto error

vcbuild PRO60.sln "Release|Win32"
if ERRORLEVEL 1 goto error

exit 0

:error
exit 1
