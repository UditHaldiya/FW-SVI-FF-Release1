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
set TARGET_BINARY_DD=%ROOT_DIR%\target\appl\fbif\ddl\0008
set TARGET_BINARY_FW=%ROOT_DIR%\target\mak\FF_SVI_DBG\exe
cd .\target\mak

echo ****** Cleaning all Subfolders.............
echo.
make clean_ff_svi_debug
make clean_ff_svi_release
erase %ROOT_DIR%\target\appl\fbif\inc\*.* /Q
erase %ROOT_DIR%\target\appl\fbif\src\*.* /Q
erase %ROOT_DIR%\*.txt /S/Q
erase %TARGET_BINARY_DD%\*.* /Q
erase %TARGET_BINARY_FW%\*.* /Q

cd %ROOT_DIR%\target\mak\FF_SVI_DBG\lib
erase ffbl.a
erase hw_krn.a
erase hw_main.a
erase osif.emb_main.a
erase osif_krn.emb.a

echo.
echo ****** BUILD FOLDERS ARE CLEANED.............
echo.
cd %ROOT_DIR%\_batch_build
echo Build Complete Time:
time /T
echo.
REM pause
echo on
rem exit
