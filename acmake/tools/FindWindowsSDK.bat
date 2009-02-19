@echo off
rem This script tries to find the Windows SDK by calling vsvars32
set VSCOMNTOOLS=%1
cd %VSCOMNTOOLS%
call vsvars32.bat > NUL
set v_help=%WINDOWSSDKDIR%
set v_help=%v_help:\\=\%
set v_help=%v_help:\=\\%
echo set(WINDOWSSDK_DIR "%v_help%" ) > %2
