REM --- This batch file must run from the DOS-box in the folder <_batch_build>
REM --------------------------------------------------------------------------

@echo off
cls
echo Build Start Time:
time /T
echo.

echo ****** Setting up Environment.............
echo.

REM --- cd to root directory <FD-SW> ---
cd ..
set ROOT_DIR=%CD%

REM --- set path ---
PATH=%ROOT_DIR%\_dev_tools;%PATH%

REM --- set VARIABLES (replace "\" with "/") ---
set BADNAME=%ROOT_DIR%/_embOS_CortexM_IAR/Start
set EMBOS=%BADNAME:\=/%
set BADNAME=
set CC_CORTEX_M3=C:/Program Files/IAR Systems/Embedded Workbench 6.30/arm
set MNINCLLUDE=%ROOT_DIR:\=/%/../FIRMWARE/includes %ROOT_DIR:\=/%/../FIRMWARE/inc_FFAP %ROOT_DIR:\=/%/../FIRMWARE/mn_instrum/noinstrum

cd .\target\mak

echo ****** Cleaning all Subfolders.............
echo.
rem make clean_ff_svi_debug
rem make clean_ff_svi_release

if not '%1' == 'e' goto noerase
shift

erase %ROOT_DIR%\target\appl\fbif\inc\*.* /Q
erase %ROOT_DIR%\target\appl\fbif\src\*.* /Q
erase %ROOT_DIR%\*.txt /S/Q
cd %ROOT_DIR%\target\mak\FF_SVI_DBG\lib
erase ffbl.a
erase hw_krn.a
erase hw_main.a
erase osif.emb_main.a
erase osif_krn.emb.a

:noerase

rem ******************************************************************************

echo ****** Building FW.............
echo.

set hextool=%ROOT_DIR%\..\firmware\tools\mnhextool.exe
set elftool="C:\Program Files\IAR Systems\Embedded Workbench 6.30\ARM\bin\ielftool"
set reloctool=%ROOT_DIR%\..\firmware\tools\streloc.exe

rem build Bank 1 firmware
attrib -r %ROOT_DIR%\target\mak\FF_SVI_DBG\svi_pos_ff_d.icf
copy %ROOT_DIR%\target\mak\FF_SVI_DBG\svi_pos1_ff_d.icf %ROOT_DIR%\target\mak\FF_SVI_DBG\svi_pos_ff_d.icf
del  %ROOT_DIR%\target\mak\FF_SVI_DBG\exe\svi_ff_debug.out
cd %ROOT_DIR%\target\mak
make ff_svi_debug
echo.

%elftool% --ihex %ROOT_DIR%\target\mak\FF_SVI_DBG\exe\svi_ff_debug.out %ROOT_DIR%\target\mak\FF_SVI_DBG\exe\svi_ff_debug1.hex


rem build Bank 0 firmware
attrib -r %ROOT_DIR%\target\mak\FF_SVI_DBG\svi_pos_ff_d.icf
copy %ROOT_DIR%\target\mak\FF_SVI_DBG\svi_pos0_ff_d.icf %ROOT_DIR%\target\mak\FF_SVI_DBG\svi_pos_ff_d.icf
del  %ROOT_DIR%\target\mak\FF_SVI_DBG\exe\svi_ff_debug.out
cd %ROOT_DIR%\target\mak
make ff_svi_debug
echo.

%elftool% --ihex %ROOT_DIR%\target\mak\FF_SVI_DBG\exe\svi_ff_debug.out %ROOT_DIR%\target\mak\FF_SVI_DBG\exe\svi_ff_debug0.hex
%hextool% -i %ROOT_DIR%\target\mak\FF_SVI_DBG\exe\svi_ff_debug0.hex -p FFAP -b %ROOT_DIR%\target\mak\FF_SVI_DBG\exe\svi_ff_debug.mns

%reloctool% %ROOT_DIR%\target\mak\FF_SVI_DBG\exe\svi_ff_debug0.hex %ROOT_DIR%\target\mak\FF_SVI_DBG\exe\svi_ff_debug1.hex %ROOT_DIR%\target\mak\FF_SVI_DBG\exe\svi_ff_reloc.ffd

echo ***   Binary files are in the folder:
echo       %ROOT_DIR%\target\mak\FF_SVI_DBG\exe  ***
echo.

echo %ROOT_DIR%\target\mak\FF_SVI_DBG\exe\svi_ff_reloc.ffd headers:
echo.
%reloctool%  %ROOT_DIR%\target\mak\FF_SVI_DBG\exe\svi_ff_reloc.ffd /v
echo.

cd %ROOT_DIR%\_batch_build

call unset elftool hextool reloctool CC_CORTEX_M3 MNINCLLUDE

echo Build Complete Time:
time /T
echo.
REM pause
echo on
rem exit
