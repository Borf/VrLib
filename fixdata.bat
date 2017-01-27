@echo off
REM echo VrLib is in %~DP0
REM echo Executable is in %1
REM echo Project is in %2
cd %~DP0
IF NOT EXIST ..\..\..\Runtime GOTO byebye
IF NOT EXIST "%~2data" GOTO byebye
echo Detected we're in VrEnvironment
xcopy /q /y %1 ..\..\..\Runtime
robocopy /nc /ns /np /njh /njs /nfl /ndl /e "%~2data" "..\..\..\Runtime\data"
if NOT EXIST w: GOTO byebye
if NOT EXIST x: GOTO byebye
if NOT EXIST y: GOTO byebye
if NOT EXIST z: GOTO byebye
xcopy /q /y %1 w:\Runtime
xcopy /q /y %1 x:\Runtime
xcopy /q /y %1 y:\Runtime
xcopy /q /y %1 z:\Runtime


:byebye
exit 0

