if "%DEBUG%"=="1" echo Start CopyToNet V1
if "%DEBUG%"=="1" echo LocalDestDir=%1
if "%DEBUG%"=="1" echo NetDestDir=%2
set LocalDestDir=%1

set NetDestDir=%2
call :DeQuote2 NetDestDir

if "%DEBUG%"=="1" pause

if exist "%NetDestDir%" goto netdirmade
if "%DEBUG%"=="1" echo %date% %time%   CREATE DIRECTORY  %NetDestDir%
mkdir "%NetDestDir%"

:netdirmade

if "%DEBUG%"=="1" echo %date% %time%   START -- Copies from %LocalDestDir% to %NetDestDir%
if "%DEBUG%"=="1" echo %date% %time%     Copy %LocalDestDir%\*.* %NetDestDir%\*.*
copy %LocalDestDir%\*.* "%NetDestDir%\*.*"
if "%DEBUG%"=="1" echo %date% %time%
if "%DEBUG%"=="1" echo %date% %time%   END -- Copies to S:\ %LocalDestDir%

if exist %3 goto sharepointdirmade
if "%DEBUG%"=="1" echo mkdir %2
mkdir %2
if "%DEBUG%"=="1" echo %date% %time%   CREATE DIRECTORY  %3
:sharepointdirmade

if "%DEBUG%"=="1" echo %date% %time%   START -- copy %LocalDestDir%\*.* %3
copy %LocalDestDir%\*.* "%3\*.*"
rem copy %1\*.* %3
rem copy \mncbbuild\DLTmncb_FAULT_matrix.xls %3
echo %date% %time% Copy Done copy %1\*.* %3  >> %1\core_release_log.log
if "%DEBUG%"=="1" echo %date% %time%   END -- Copy S:\ to SharePoint

:end

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