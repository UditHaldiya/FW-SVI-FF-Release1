cls
@ECHO OFF
Echo Using Modules to check out modules, build to do initial pre build steps, ide to create ide project
call Build_Helper LCX MODULES
call Build_Helper LCX BUILD
call Build_Helper LCX IDE