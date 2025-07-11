cls
@ECHO OFF
SETLOCAL ENABLEDELAYEDEXPANSION

rem strip quotes
set arg5=%1
call :DeQuote2 arg5

if "%DEBUG%"=="1" echo Running LCx_Release_Build Version 2

echo this takes one argument the label in VSS for the release e.g Beta_008_01
rem BUILD_PRJ is the directory under c:\mncbbuild\cmdbuild\Rel

set BUILD_PRJ=project_LCX
if "%DEBUG%"=="1" echo BUILD_PRJ=%BUILD_PRJ%
set arg1=\official
set arg2="\\Avo111sfs1\shared$\Projects\MNCB\Firmware Current Build LCx"
set arg3="\\mymasoneilan\en\Engineering\innovations\Team Documents1\Projects\LCX (Low cost)\Firmware"
set arg4=%BUILD_PRJ%

if "%DEBUG%"=="1" echo Release_Build_Helper %arg1% %arg2% %arg3% %arg4% %arg5%
if "%DEBUG%"=="1" pause

if "%DEBUG%"=="1" echo removing copy to directory
rem rmdir %arg1%
rem mkdir %arg1%

:CALLBUILD
Release_Build_Helper %arg1% %arg2% %arg3% %arg4% %arg5%

rem Dequoting function
:DeQuote2
FOR %%G IN (%*) DO (
SET DeQuote.Variable=%%G
CALL SET DeQuote.Contents=%%!DeQuote.Variable!%%

IF [!DeQuote.Contents:~0^,1!]==[^"] (
IF [!DeQuote.Contents:~-1!]==[^"] (
SET DeQuote.Contents=!DeQuote.Contents:~1,-1!
) ELSE (GOTO :EOF no end quote)
) ELSE (GOTO :EOF no beginning quote)

SET !DeQuote.Variable!=!DeQuote.Contents!
SET DeQuote.Variable=
SET DeQuote.Contents=
)
GOTO :EOF