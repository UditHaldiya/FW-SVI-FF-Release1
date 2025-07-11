 if "%iartools%" == "" set iartools=c:\program files\iar systems\Embedded Workbench 5.50
 if exist "%iartools%\arm\bin\iccarm.exe" goto fini
 echo compiler not found "%iartools%\arm\bin\iccarm.exe"
 pause
:fini
