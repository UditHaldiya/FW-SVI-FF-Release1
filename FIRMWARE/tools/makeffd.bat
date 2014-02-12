 @echo off
 if exist buildhelpers\. goto contin
 echo Wrong starting directory - expected firmware.
 goto fini
:contin 
 set hex2ffd=hex2ffd.exe
 set FFpath=..\fd-sw\target\mak\FF_SVI_DBG\exe
 set apppath=cmdbuild\Dbg\
 set ffbk0=%ffpath%\fffp_0.hex
 set ffbk1=%ffpath%\fffp_1.hex
 set appbk0=%apppath%\project_FFAP_0\ffap.hex
 set appbk1=%apppath%\project_FFAP\ffap.hex
 set output=%apppath%\project_FFAP\ffapp.ffd
 
 %hex2ffd% %ffbk0% %ffbk1% %appbk0% %appbk1% %output%
 if errorlevel 1 goto bad
 %hex2ffd% /v %output%
 goto fini
:bad
 del %output%
:fini
 set hex2ffd=
 set ffbk0=
 set ffbk1=
 set appbk0=
 set appbk1=
 set output=
