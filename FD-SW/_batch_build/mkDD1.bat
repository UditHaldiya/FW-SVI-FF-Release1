echo ***** Building Binary DD File ............
REM  --- Prepare environment ---

set FFTokenizerpath=c:\ff\tok\bin
set dictfile=c:\ff\ddl\standard.dct
set includepath=c:\ff\ddl
set releasepath=c:\ff\release
set imagepath=c:\ff\ddl\htk
set option4=

set manufacturer_ID=004745
set type_ID=0008

IF "%~2"=="-4" goto Version4
goto Tokenizing

:Version4
Set option4=-4

:Tokenizing

REM  --- Save the present path and switch to <ff\release> ---
set SAVEPATH=%CD%
cd %releasepath%

REM --- move here standard.sym file ---
if not exist standard.sym copy c:\ff\ddl\standard.sym .

REM if not exist %manufacturer_ID% md %manufacturer_ID%
if not exist %manufacturer_ID%\%type_ID% md %manufacturer_ID%\%type_ID%
REM Do not remove the next line -- the env.variable is used by the caller
set SOURCE_BINARY_DD=%releasepath%\%manufacturer_ID%\%type_ID%

echo BUILDING: %1

%FFTokenizerpath%\ffpretok.exe %option4% -w 718 -d %dictfile% -I%includepath% -R %releasepath% -p "%imagepath%" %1 _tmptok

call %FFTokenizerpath%\ff_tok32.exe _tmptok

:end
REM  --- Remove temp files ---
del _tmptok
del tok.opt

REM  --- Restore the path ---
cd %SAVEPATH%

REM  --- Restore environment ---

set FFTokenizerpath=
set dictfile=
set includepath=
set releasepath=
set imagepath=



