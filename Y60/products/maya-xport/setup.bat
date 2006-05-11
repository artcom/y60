@echo off

set MAYA_VERSION=7.0
set MAYA_PATH=%ProgramFiles%\Alias\Maya%MAYA_VERSION%
set INSTALL_PATH=%ProgramFiles%\ART+COM\mayaY60export
set MODULE_FILE=%MAYA_PATH%\modules\mayaY60export.txt

if NOT EXIST %MAYA_PATH% (
    ECHO Maya not found in default path %MAYA_PATH%
    EXIT 1
)

ECHO Creating installation directory %INSTALL_PATH%
MKDIR %INSTALL_PATH%\plug-ins
MKDIR %INSTALL_PATH%\scripts

ECHO Installing mayaY60-Exporter files...
COPY /Y scripts\* "%ProgramFiles%\ART+COM\mayaY60export\scripts"
COPY /Y plug-ins\* "%ProgramFiles%\ART+COM\mayaY60export\plug-ins"
COPY /Y *.dll "%MAYA_PATH%\bin"

ECHO Creating module file %MODULE_FILE%
ECHO "+ mayaY60export %MAYA_VERSION% %INSTALL_PATH%" > %MODULE_FILE%

echo done.

pause
