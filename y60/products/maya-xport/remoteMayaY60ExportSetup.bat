@echo off

IF NOT DEFINED MAYA_VERSION (
    set MAYA_VERSION=8.5
    ECHO Maya version not set, defaulting to %MAYA_VERSION%.
)

IF NOT DEFINED MAYA_SDK (
    set MAYA_SDK=%ProgramFiles%\Autodesk\Maya%MAYA_VERSION%
)

set INSTALL_PATH=%ProgramFiles%\ART+COM\mayaY60export
set MODULE_FILE=%MAYA_SDK%\modules\mayaY60export.txt

if NOT EXIST %MAYA_SDK% (
    ECHO Maya not found in default path %MAYA_SDK%
    EXIT 1
)

ECHO Creating installation directory %INSTALL_PATH%
MKDIR %INSTALL_PATH%\plug-ins
MKDIR %INSTALL_PATH%\scripts

echo Installing mayaY60-Exporter files from balthasar...
XCOPY /Y \\balthasar\dist\mayaY60-export_setup\scripts\* "%ProgramFiles%\ART+COM\mayaY60export\scripts"
XCOPY /Y \\balthasar\dist\mayaY60-export_setup\plug-ins\* "%ProgramFiles%\ART+COM\mayaY60export\plug-ins"
XCOPY /Y \\balthasar\dist\mayaY60-export_setup\*.dll "%MAYA_SDK%\bin"

ECHO Creating module file %MODULE_FILE%
ECHO + mayaY60export %MAYA_VERSION% %INSTALL_PATH% > %MODULE_FILE%

echo done.

pause
