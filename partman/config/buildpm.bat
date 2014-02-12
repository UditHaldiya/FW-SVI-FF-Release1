 @echo off
 set reloexe=..\..\firmware\tools\hex2ffd.exe
 set bank0bm=debug0\Exe\partman0
 set bank1bm=debug1\Exe\partman1
 call setenv
 set target=-build 
 if not '%1' == '' set target=%1
 set tipe=debug
 if not '%2' == '' set tipe=%2
 call "%iartools%\common\bin\iarbuild" partman.ewp %target% %tipe%
 call "%iartools%\common\bin\iarbuild" partman.ewp %target% %tipe%1

 %reloexe% /p %bank0bm%.hex %bank1bm%.hex %bank0bm%.hex %bank1bm%.hex partman.ffd
 %reloexe% /v partman.ffd

 set reloexe=
 set target=
 set tipe=
 set iartools=
 set bank0bm=
 set bank1bm=