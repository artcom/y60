set VSCOMNTOOLS=%1
cd %VSCOMNTOOLS%
call vsvars32.bat
set v_help=%WINDOWSSDKDIR%
set v_help=%v_help:\\=\%
set v_help=%v_help:\=\\%
echo set(WINDOWSSDKDIR "%v_help%" ) > %2
