if "%DEBUG%"=="1" echo Start CopyToLocal V1
if "%DEBUG%"=="1" echo 1=%1
if "%DEBUG%"=="1" pause
set LocalDestDir=%1
REM TFS:3537: Fault matrix is copied to local and official builds
if "%DEBUG%"=="1" echo Copying Fault Matrix
Copy \mncbbuild\LCXmncb_FAULT_matrix.xls %LocalDestDir%
if ERRORLEVEL 1 echo failed to copy fault matrix
if "%DEBUG%"=="1" echo Copying lint.log \mncbbuild\cmdbuild\Rel\%BUILD_PRJ%\lint.log %LocalDestDir%
copy \mncbbuild\cmdbuild\Rel\%BUILD_PRJ%\lint.log %LocalDestDir%
if ERRORLEVEL 1 echo failed to copy lint.log
if "%DEBUG%"=="1" echo End CopyToLocal V1
