 rem @echo off	
 rem #1 = rel
 rem label ?
 rem check for VSS available
 rem make a CD
 set reldir=..\..\Release
 set ss=ss
 set proj=mncb\dev\Release1\firmware
 set root=mncbbuild
 set direct=%root%\dev\firmware
 set user="Release Builder"
 set pass=rbuilder
 set SSDIR=p:\sourcesafe\HART
 set destdrv=s:\projects\mncb\current mncb software\firmware
 set flag=
 set tipe=
 set dest=
 if '%1' == '' goto notrel
 set flag=rel_
 set tipe=DIR=rel
 set dest=rel\
:notrel

 %ss% whoami -Y%user%,%pass%
 if errorlevel 1 goto novss

 cd \
 rem rm -f -r %direct%
 rm -f -r %root%

 md %root%
 md %root%\dev
 md %root%\dev\firmware
 rem md %direct%
 cd %direct%
 %ss% cp $/%proj% -Y%user%,%pass%
 %ss% workfold c:\%direct% -Y%user%,%pass%
 rem pause
 %ss% get * -R -GTM -Y%user%,%pass%

 rem echo %errorlevel%
 rem %ss% get * -R -GTM -Y%user%,%pass%

 %ss% workfold c:\mncb -Y%user%,%pass%
 %ss% cp $/mncb -Y%user%,%pass%
 echo.
 cd dev\firmware
 rem \dos\pkzip25 -add -rec -pat=full mncbsrc *
 md %reldir%


 rem pause press a key to build the firmware
 gnumake BUILD %tipe%
 if not exist cmdbuild\%dest%mncb.d79 goto failed
 rem copy cmdbuild\*.a79 %reldir%
 if not exist tools\convert.exe goto lcltools
 tools\convert cmdbuild\%dest%mncb.a79 %reldir%

 rem \dos\pkzip25 -add -rec -pat=full mncbsrc includes\mntimestamp.h cmdbuild\mncb.map
 rem call mncbcpy

 goto converted
:lcltools
 \tools\convert cmdbuild\mncb.a79 %reldir%
:converted
 echo The FIRMWARE build is complete
 rem pause
 goto noboot



 cd ..\ispbootstrap
 gnumake BUILD
 if not exist cmdbuild\bootstrap.d79 goto failed
 copy cmdbuild\bootstrap.a79 %reldir%
 echo The BOOTSTRAP build is complete



:noboot
 echo Ready to copy release to "%destdrv%"
 pause
 rem copy %reldir%\svi_2* "\%flag%svi_2*"
 REM copy %reldir%\svi_2* "%destdrv%\%flag%svi_2*"
 PUSHD .
 CD %reldir%
 FOR %%f IN (svi_2*) DO copy %%f "%destdrv%\%flag%%%f"
 POPD
 rem pause
 goto fini




:novss
 echo SS.EXE is not in the path
 pause
 goto fini



:failed
 echo The build has failed
 pause
:fini
 pause
