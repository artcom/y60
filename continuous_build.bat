if not defined %VERBOSE% @ECHO OFF
BREAK=ON

set BUILD_DIR=_build

rem === Check parameters ======================================================

if defined %BUILD_TYPE% goto config_defined

set BUILD_TYPE=Release

:config_defined
set CMAKE_ARGS=%CMAKE_ARGS% -D CMAKE_BUILD_TYPE=%BUILD_TYPE%


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

if defined USE_NMAKE goto build_with_namke
rem === VC build ==============================================================

cmake -G "Visual Studio 9 2008" ..
if ERRORLEVEL 1 goto error

vcbuild /M%NUMCORES% PRO60.sln "%BUILD_TYPE%|%PLATFORM%"
rem nmake
if ERRORLEVEL 1 goto error

vcbuild TEST.vcproj
if ERRORLEVEL 1 goto error

exit 0

rem === NMake build ===========================================================
:build_with_namke

cmake -G "NMake Makefiles" %CMAKE_ARGS% ..
if ERRORLEVEL 1 goto error

nmake
if ERRORLEVEL 1 goto error

nmake test
if ERRORLEVEL 1 goto error

exit 0

:error
rem XXX
pause
exit 1
