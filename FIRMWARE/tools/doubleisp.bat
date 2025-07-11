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
 echo Connect ISP adapter 
 pause 
 rem load the first image to the FF CPU
 %ispexe% /%1 /c1 %partman%
 %ispexe% /%1 /c1 %firmware%
 echo.

 rem load the second image to the APP CPU
 %ispexe% /%1  /i %partman%
 %ispexe% /%1  /i %firmware%
:fini 
 set ispexe=
 set partman=
 set firmware=
 echo.