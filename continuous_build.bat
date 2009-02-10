BREAK=ON

set BUILD_DIR="_build"
set CMAKE_GENERATOR="Visual Studio 9 2008"
set VCBUILD_ARGS="PRO60.sln Release|Win32"

echo "=== Running on operating system %OS%"


if exist %BUILD_DIR% goto build_dir_exists

mkdir %BUILD_DIR%

echo === Build directory %BUILD_DIR% does not exist. Creating.
:build_dir_exists

cd %BUILD_DIR%

echo === Configuring cmake build for %CMAKE_GENERATOR%
cmake -G %CMAKE_GENERATOR% ..

echo "=== Running vcbuild %VCBUILD_ARGS%"
vcbuild %VCBUILD_ARGS%
