BREAK=ON

set BUILD_DIR=_build
set USE_NMAKE=1

if defined USE_NMAKE goto generator_nmake
    set CMAKE_GENERATOR=Visual Studio 9 2008
    goto generator_found
:generator_nmake
    set CMAKE_GENERATOR=NMake Makefiles
:generator_found


rem === Check environment variables ===========================================

if defined %CONFIG% goto config_defined

set CONFIG=Release

:config_defined


if defined %PLATFORM% goto platform_defined

set PLATFORM=Win32

:platform_defined


if defined %NUMCORES% goto numcores_defined

set NUMCORES=1

:numcores_defined

rem === Prepare build =========================================================

if exist %BUILD_DIR% goto build_dir_exists

mkdir %BUILD_DIR%

:build_dir_exists


call "%VS90COMNTOOLS%..\..\VC\vcvarsall.bat"

cd %BUILD_DIR%

cmake -G "%CMAKE_GENERATOR%" ..
if ERRORLEVEL 1 goto error

if defined USE_NMAKE goto build_with_namke
rem === VC build ==============================================================

vcbuild /M%NUMCORES% PRO60.sln "%CONFIG%|%PLATFORM%"
rem nmake
if ERRORLEVEL 1 goto error

vcbuild TEST.vcproj
if ERRORLEVEL 1 goto error

exit 0

rem === NMake build ===========================================================
:build_with_namke

nmake
if ERRORLEVEL 1 goto error

nmake test
if ERRORLEVEL 1 goto error

exit 0

:error
rem XXX
pause
exit 1
