 @echo off
 
 set ispexe=isp.exe
 set partman=..\partman\config\partman.ffd
 set firmware=cmdbuild\Dbg\project_FFAP\ffapp.ffd
 
 echo %cd%

 if not '%1' == '' goto contin
 echo Comport number required for ISP adapter
 %ispexe% /l
 goto fini
:contin
 if exist %partman% goto ckfw 	
 echo No file %partman%
 goto fini;
:ckfw 
 if exist %firmware% goto fwok
 echo No file %firmware%
 goto fini;
:fwok
 echo.
 echo Connect ISP adapter and set switch cable to FF CPU
 pause 

 rem load the first image to the FF CPU
 %ispexe% /%1 %partman%
 %ispexe% /%1 %firmware%
 echo.
 echo Set switch cable to APP CPU
 pause 
 rem load the second image to the FF CPU
 %ispexe% /%1 /i %partman%
 %ispexe% /%1 /i %firmware%
:fini 
 set ispexe=
 set partman=
 set firmware=
 echo.