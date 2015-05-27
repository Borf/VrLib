echo VrLib is in %~DP0
echo Executable is in %1
echo Project is in %2
cd %~DP0
IF NOT EXIST ..\..\..\Runtime GOTO byebye
echo Detected we're in VrEnvironment
xcopy /y "%1" ..\..\..\Runtime > robocopy.log
robocopy /nc /ns /np /njh /njs /nfl /e "%~2data" "..\..\..\Runtime\data" > robocopy.log
del robocopy.log
:byebye