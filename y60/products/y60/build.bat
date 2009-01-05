call "%VS90COMNTOOLS%\vsvars32.bat"
set CHERE_INVOKING=1
%SYSTEMDRIVE%\cygwin\bin\bash.exe --login %PRO%\src\Y60\products\y60\build.sh

