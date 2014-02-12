rem set DONT_BUILD=YES

if "%DEBUG%"=="1" echo Release_Build_Helper Version 2.1
if "%DEBUG%"=="1" echo 1=LocalDestDir=%1
set LocalDestDir=%1
if "%DEBUG%"=="1" echo 2=S Drive Directiory=%2
set SDriveDir=%2
if "%DEBUG%"=="1" echo 3=Share Point Directory=%3
set SharePoint=%3
if "%DEBUG%"=="1" echo 4=%4
if "%DEBUG%"=="1" echo 5=ReleaseLabel = %5
set ReleaseLabel=%5
if "%DEBUG%"=="1" echo 6=%6
if "%DEBUG%"=="1" pause

if "%DEBUG%"=="1" echo Testing LocalDestDir

if not exist \mncbbuild goto NoMNCBBackup

:NoMNCBBackup

if exist %LocalDestDir% goto localdirdone
mkdir %LocalDestDir%
if "%DEBUG%"=="1" echo Making directory
if "%DEBUG%"=="1" pause
:localdirdone
xcopy %LocalDestDir% %LocalDestDir%_bak /S/I
del %LocalDestDir%\*.*
echo %date% %time% START > %LocalDestDir%\core_release_log.txt

echo %date% %time%   START -- Build > %LocalDestDir%\core_release_log.txt
echo 4=%4

if "%ReleaseLabel%"=="" goto TESTBUILD
	echo RELEASE BUILD BY LABEL

	Rem set BUILD_COMMAND_JS="gnumake OFFICIAL MNS_OFFICIAL_DIR=%LocalDestDir% VSSARG=-Vl%ReleaseLabel%"

	set BUILD_COMMAND_JS=OFFICIAL proj=LCX MNS_OFFICIAL_DIR=%LocalDestDir% VSSARG=-Vl"%ReleaseLabel%"
	echo %date% %time% %BUILD_COMMAND_JS% >> %LocalDestDir%\core_release_log.txt
	echo %date% %time% %BUILD_COMMAND_JS%

	Cmd /c Exit /B 0
    if "%DONT_BUILD%"=="" gnumake %BUILD_COMMAND_JS%
	if ERRORLEVEL 1 goto BUILD_FAILED

	call :DeQuote2 SDriveDir
		set SDriveDir=%SDriveDir%\%ReleaseLabel%
                if "%DEBUG%"=="1" echo SDriveDir=%SDriveDir%

	call :DeQuote2 SharePoint
		set SharePoint=%SharePoint%\%ReleaseLabel%
    		if "%DEBUG%"=="1" echo SharePoint=%SharePoint%

	goto BUILD_DONE

:TESTBUILD
	echo TESTBUILD of tip
	echo %date% %time% gnumake OFFICIAL  MNS_OFFICIAL_DIR=%LocalDestDir%  >> %LocalDestDir%\core_release_log.txt
	echo gnumake OFFICIAL  MNS_OFFICIAL_DIR=%LocalDestDir%
    Cmd /c Exit /B 0
	if "%DONT_BUILD%"=="" gnumake OFFICIAL proj=LCX MNS_OFFICIAL_DIR=%LocalDestDir%
	if ERRORLEVEL 1 goto BUILD_FAILED

	call :DeQuote2 SDriveDir
		set SDriveDir=%SDriveDir%\%ReleaseLabel%
                if "%DEBUG%"=="1" echo SDriveDir=%SDriveDir%

	call :DeQuote2 SharePoint
		set SharePoint=%SharePoint%\%ReleaseLabel%
    		if "%DEBUG%"=="1" echo SharePoint=%SharePoint%

	goto BUILD_DONE

:BUILD_DONE
	echo %date% %time%   END -- Build  >> %LocalDestDir%\core_release_log.txt

	echo Build succeeded
	call CopyToLocal %LocalDestDir%
	call Release_Build_Copy %LocalDestDir% "%SDriveDir%"
	call Release_Build_Copy %LocalDestDir% "%SharePoint%"
	echo %date% %time% FINISH  >> %LocalDestDir%\core_release_log.txt

goto BUILD_SUCCESS

:BUILD_FAILED

pause
echo Build Failed
\mncbbuild\cmdbuild\Rel\%BUILD_PRJ%\lint.log

:goto END
:BUILD_SUCCESS
:END

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