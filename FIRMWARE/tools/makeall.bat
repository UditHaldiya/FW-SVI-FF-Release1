 @echo off

 set partmanpath=..\partman

 rem build the APP cpu .hex files
 call buildapp

 rem build the FF cpu .hex files
 call buildff

 set tdir=%cd%
 cd %partmanpath%\config
 call buildpm
 cd %tdir% 

 set tdir=
 set partmanpath=	

 rem combne the .hex files into .ffd files
 call makeffd
