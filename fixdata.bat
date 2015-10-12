@echo off
REM echo VrLib is in %~DP0
REM echo Executable is in %1
REM echo Project is in %2
cd %~DP0
IF NOT EXIST ..\..\..\Runtime GOTO byebye
IF NOT EXIST "%~2data" GOTO byebye
echo Detected we're in VrEnvironment
xcopy /q /y "%1" ..\..\..\Runtime
robocopy /nc /ns /np /njh /njs /nfl /ndl /e "%~2data" "..\..\..\Runtime\data"
:byebye
exit 0