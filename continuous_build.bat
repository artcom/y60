BREAK=ON

set BUILD_DIR=_build
set CMAKE_GENERATOR=Visual Studio 9 2008
set VCBUILD_ARGS="PRO60.sln Release|Win32"

rem === Running on operating system %OS%

if exist %BUILD_DIR% goto build_dir_exists

mkdir %BUILD_DIR%

:build_dir_exists
cd %BUILD_DIR%

rem === Configuring cmake build for %CMAKE_GENERATOR%
cmake -G %CMAKE_GENERATOR% ..

rem Running vcbuild %VCBUILD_ARGS%
vcbuild %VCBUILD_ARGS%
