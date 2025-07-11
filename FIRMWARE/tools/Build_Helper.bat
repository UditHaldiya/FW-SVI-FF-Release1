if "DEBUG"=="1" goto HELP
if "%1"=="" goto HELP

goto BUILD

:HELP
echo Build_Helper Version 1.0
echo Param %1 = project to build e.g. LCX
echo Param %2 = target e.g. BUILD MODULES HCIMPORT
echo Param %3 = optional argument in the form of DIR=REL
echo Param %4 = optional argument in the form of DIR=REL

:BUILD
gnumake proj=%1 %2 %3 %2

