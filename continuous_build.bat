BREAK=ON

set BUILD_DIR=_build
set CMAKE_GENERATOR=Visual Studio 9 2008

if exist %BUILD_DIR% goto build_dir_exists

mkdir %BUILD_DIR%

:build_dir_exists
cd %BUILD_DIR%

cmake -G "%CMAKE_GENERATOR%" ..
vcbuild PRO60.sln "Release|Win32"
