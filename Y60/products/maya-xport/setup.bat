

IF NOT DEFINED MAYA_VERSION (
    set MAYA_VERSION=8.5
    ECHO MAYA_VERSION not set, assuming %MAYA_VERSION%.
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

if NOT EXIST %MAYA_SDK%\modules (
    ECHO Creating module directory %MAYA_SDK%\modules.
    MKDIR %MAYA_SDK%\modules
)

ECHO Creating installation directory %INSTALL_PATH%
MKDIR %INSTALL_PATH%\plug-ins
MKDIR %INSTALL_PATH%\scripts

ECHO Installing mayaY60-Exporter files...
XCOPY /Y scripts\* "%ProgramFiles%\ART+COM\mayaY60export\scripts"
XCOPY /Y plug-ins\* "%ProgramFiles%\ART+COM\mayaY60export\plug-ins"
XCOPY /Y *.dll "%MAYA_SDK%\bin"

ECHO Creating module file %MODULE_FILE%
ECHO + mayaY60export %MAYA_VERSION% %INSTALL_PATH%> %MODULE_FILE%

ECHO done.

pause
