@echo off
setlocal
pushd %~dp0


echo //----------------------------------------------------------------------//
echo // Compile, Rename, and Move DAq and DAn dlls of Shifted from Ei = 1keV //
echo //----------------------------------------------------------------------//



rem//Solution Platform
set VISUALSTUDIO_PLATFORM=x64
rem//Configuration : Debug or Release (The actual config name passed to the compiler is "Release(x64)" When "Release" is selected)
set VISUALSTUDIO_CONFIG=Release
rem//Final output destination dir
set DIR_DST=C:\Program Files\RoentDek Handels GmbH\CoboldPC 2011 R5-2-x64 (Visual Studio .Net 2010 Compilation) V10.1.1412.2\
rem//1: Rename and move are executed even in Debug mode. 0: The operations are not carried out.
set DEBUG_RENAME_MOVE_OUTPUT=0
rem//Output dll name of DAn Project
set OUTPUT_FILENAME_DAn=DAn_SF1keV_200918
rem//Output dll name of DAq Project
set OUTPUT_FILENAME_DAq=DAq_SF1keV_200918
rem//1: DAn will be compiled., 0: not compiled.
set COMPILE_DAn=1
rem//1: DAq will be compiled., 0: not compiled.
set COMPILE_DAq=1



rem//File path to MSBuild
set PATH_MSBuild=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\Tools\VsDevCmd.bat
rem//Intermediate directory
set DIR_TEMP=C:\Program Files\RoentDek Handels GmbH\CoboldPC 2011 R5-2-x64 (Visual Studio .Net 2010 Compilation) V10.1.1412.2\LAES2_TDC8PCI2_HEX\Cobold_Shared2\temp\
rem//Solution directory
set DIR_SOLUTION=C:\Program Files\RoentDek Handels GmbH\CoboldPC 2011 R5-2-x64 (Visual Studio .Net 2010 Compilation) V10.1.1412.2\LAES2_TDC8PCI2_HEX\Cobold_Shared2\Shifted1keV\SF1keV\
rem//File path to DAn project
set PATH_PROJ_DAn=%DIR_SOLUTION%DAn\CDAn_SF1keV.vcxproj
rem//Preffix of the output dll file name for DAn
set NAME_DAn_HEADER=DAn_SF1keV
rem//File path to DAq project
set PATH_PROJ_DAq=%DIR_SOLUTION%DAq\CDAq_SF1keV.vcxproj
rem//Preffix of the output dll file name for DAq
set NAME_DAq_HEADER=DAq_SF1keV

rem//File path to a Visual Studio compile-log file
set PATH_LOG=%DIR_TEMP%MSBuild_DAqDAn_log.log




rem//Returned values
set Ret_Success=0
set Ret_Failed=1



rem//************************************************************************
echo %PATH_MSBuild%
echo %DIR_DST%

rem //Remove the last char : File path error if this is skipped(?)
rem set X=%DIR_TEMP%
set X="%DIR_TEMP:~0,-1%"
rem set X="%DIR_TEMP%"
rem echo %PATH_APP%
rem echo %X%

rem//Create the temp dir if not found
call :Create_TEMP_DIR

rem//Execute MSBuild
call "%PATH_MSBuild%"


rem//Compile projects for each case
if "%VISUALSTUDIO_PLATFORM%"=="x86" (
   if "%COMPILE_DAn%"=="1" (
		call :Complie_By_x86_DAn
	)
	if "%COMPILE_DAq%"=="1" (
		call :Complie_By_x86_DAq
	)
) else if "%VISUALSTUDIO_PLATFORM%"=="x64" (
	if "%COMPILE_DAn%"=="1" (
		call :Complie_By_x64_DAn
	)
	if "%COMPILE_DAq%"=="1" (
		call :Complie_By_x64_DAq
	)
) else (
	 echo ---------------------------------------------------------
    echo VISUALSTUDIO_PLATFORM ?: %VISUALSTUDIO_PLATFORM%
	 echo ---------------------------------------------------------
	 popd
	 pause
	 exit /b %Ret_Failed%
)

rem//Stop if the compiling failed
if %ERRORLEVEL% neq %Ret_Success% (
	popd
	pause
	exit /b %Ret_Failed%
)

rem//If Debug mode
if "%VISUALSTUDIO_CONFIG%"=="Debug" (
    if "DEBUG_RENAME_MOVE_OUTPUT" neq "OK" (
      echo ---------------------------------------------------------
		echo The .dll compiled in Debug mode has not been not copied.
		echo ---------------------------------------------------------
		popd
	   pause
	   exit /b %Ret_Failed%
	 )
)

rem//Move and rename the output dlls if compiles finished succsessfully 
if "%COMPILE_DAn%"=="1" (
    call :Copy_DAn_dll
	 call :Move_DAn_dll
)
if "%COMPILE_DAq%"=="1" (
    call :Copy_DAq_dll
	 call :Move_DAq_dll
)

popd
pause
exit /b %Ret_Success%
rem//************************************************************************


rem//------Create an intermediante dir --------
:Create_TEMP_DIR
mkdir "%DIR_TEMP%" > NUL 2>&1
if %ERRORLEVEL% neq 0 (
	 echo TEMP direcroty already exists
) else (
    echo TEMP direcroty created
)
exit /b %Ret_Success%
rem//------------------------------------



rem//------Compile DAn in x86--------
:Complie_By_x86_DAn
MSBuild "%PATH_PROJ_DAn%" /t:build /p:Configuration=%VISUALSTUDIO_CONFIG%;Platform="x86";OutDir=%X% /flp:logfile="%PATH_LOG%";verbosity=diagnostic
if %ERRORLEVEL% neq 0 (
	echo Complie_By_x86_DAn failed,  ErrorLevel:%ERRORLEVEL%
	exit /b %Ret_Failed%
)
exit /b %Ret_Success%
rem//-----------------------------


rem //------Compile DAn in x64--------
:Complie_By_x64_DAn
MSBuild "%PATH_PROJ_DAn%" /t:build /p:Configuration=%VISUALSTUDIO_CONFIG%(x64);Platform="x64";OutDir=%X% /flp:logfile="%PATH_LOG%";verbosity=diagnostic
if %ERRORLEVEL% neq 0 (
	echo Complie_By_x64_DAn failed,  ErrorLevel:%ERRORLEVEL%
	exit /b %Ret_Failed%
)
exit /b %Ret_Success%
rem //-----------------------------


rem//------Compile DAq in x86--------
:Complie_By_x86_DAq
MSBuild "%PATH_PROJ_DAq%" /t:build /p:Configuration=%VISUALSTUDIO_CONFIG%;Platform="x86";OutDir=%X% /flp:logfile="%PATH_LOG%";verbosity=diagnostic
if %ERRORLEVEL% neq 0 (
	echo Complie_By_x86_DAq failed,  ErrorLevel:%ERRORLEVEL%
	exit /b %Ret_Failed%
)
exit /b %Ret_Success%
rem//-----------------------------


rem //------Compile DAq in x64--------
:Complie_By_x64_DAq
MSBuild "%PATH_PROJ_DAq%" /t:build /p:Configuration=%VISUALSTUDIO_CONFIG%(x64);Platform="x64";OutDir=%X% /flp:logfile="%PATH_LOG%";verbosity=diagnostic
if %ERRORLEVEL% neq 0 (
	echo Complie_By_x64_Dq failed,  ErrorLevel:%ERRORLEVEL%
	exit /b %Ret_Failed%
)
exit /b %Ret_Success%
rem //-----------------------------


rem //-------Copy the output dll of DAn--------
:Copy_DAn_dll
echo Copy_DAn_dll---------
set FNAME_OLD_DAn=%DIR_TEMP%%NAME_DAn_HEADER%_%VISUALSTUDIO_PLATFORM%.dll
set FNAME_NEW_DAn=%DIR_TEMP%%OUTPUT_FILENAME_DAn%_%VISUALSTUDIO_PLATFORM%.dll
echo "%FNAME_OLD_DAn%"
echo "%FNAME_NEW_DAn%"
copy /y "%FNAME_OLD_DAn%" "%FNAME_NEW_DAn%"
if %ERRORLEVEL% neq 0 (
	echo Copy_DAn_dll failed,  ErrorLevel:%ERRORLEVEL%
	exit /b %Ret_Failed%
)
echo -------------------
exit /b %Ret_Success%
rem //---------------------------------


rem //-------Copy the output dll of DAq--------
:Copy_DAq_dll
echo Copy_DAq_dll---------
set FNAME_OLD_DAq=%DIR_TEMP%%NAME_DAq_HEADER%_%VISUALSTUDIO_PLATFORM%.dll
set FNAME_NEW_DAq=%DIR_TEMP%%OUTPUT_FILENAME_DAq%_%VISUALSTUDIO_PLATFORM%.dll
echo "%FNAME_OLD_DAq%"
echo "%FNAME_NEW_DAq%"
copy /y "%FNAME_OLD_DAq%" "%FNAME_NEW_DAq%"
if %ERRORLEVEL% neq 0 (
	echo Copy_DAq_dll failed,  ErrorLevel:%ERRORLEVEL%
	exit /b %Ret_Failed%
)
echo -------------------
exit /b %Ret_Success%
rem //---------------------------------



rem //-------Move the output dll of DAn--------
:Move_DAn_dll
echo Move_DAn_dll---------
echo "%FNAME_NEW_DAn%"
echo "%DIR_DST%"
move /Y "%FNAME_NEW_DAn%" "%DIR_DST%"
if %ERRORLEVEL% neq 0 (
	echo Move_DAn_dll failed,  ErrorLevel:%ERRORLEVEL%
	exit /b %Ret_Failed%
)
echo -----------------
exit /b %Ret_Success%
rem //---------------------------------

rem //-------Move the output dll of DAq--------
:Move_DAq_dll
echo Move_DAq_dll---------
echo "%FNAME_NEW_DAq%"
echo "%DIR_DST%"
move /Y "%FNAME_NEW_DAq%" "%DIR_DST%"
if %ERRORLEVEL% neq 0 (
	echo Move_DAq_dll failed,  ErrorLevel:%ERRORLEVEL%
	exit /b %Ret_Failed%
)
echo -----------------
exit /b %Ret_Success%
rem //---------------------------------