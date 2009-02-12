BREAK=ON
if defined %VERBOSE% goto be_verbose

echo OFF
goto verbose_done

:be_verbose

rem dump environemnt
set

:verbose_done


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

if exist %BUILD_DIR%\%BUILD_TYPE% goto build_type_dir_exists

mkdir %BUILD_DIR%\%BUILD_TYPE%

:build_type_dir_exists

call "%VS90COMNTOOLS%..\..\VC\vcvarsall.bat"

cd %BUILD_DIR%\%BUILD_TYPE%

if defined USE_NMAKE goto build_with_namke
rem === VC build ==============================================================

cmake -G "Visual Studio 9 2008" ..\..
if ERRORLEVEL 1 goto error

vcbuild /M%NUMCORES% PRO60.sln "%BUILD_TYPE%|%PLATFORM%"
if ERRORLEVEL 1 goto error

if defined %SKIP_TESTS% goto vcbuild_tests_skipped

vcbuild TEST.vcproj
if ERRORLEVEL 1 goto error

:vcbuild_tests_skipped

exit 0

rem === NMake build ===========================================================
:build_with_namke

cmake -G "NMake Makefiles" %CMAKE_ARGS% ..\..
if ERRORLEVEL 1 goto error

nmake
if ERRORLEVEL 1 goto error

if defined %SKIP_TESTS% goto nmake_tests_skipped

nmake test
if ERRORLEVEL 1 goto error

:nmake_tests_skipped

exit 0

:error
rem XXX usefull for debugging, but *DO NOT COMMIT* ;-)
rem pause
exit 1
