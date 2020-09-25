@echo off
setlocal
pushd %~dp0


echo //------------------------------------------------------------//
echo // Test0                                                      //
echo //------------------------------------------------------------//


set Ret_Success=0
set Ret_Failed=1

set PATH_PROJ=C:\Program Files\RoentDek Handels GmbH\CoboldPC 2011 R5-2-x64 (Visual Studio .Net 2010 Compilation) V10.1.1412.2\LAES2_TDC8PCI2_HEX\Cobold_Shared2\Elastic1keV\src_DAqDAn\Lib_CurrentDAqInfo
set DIR_TEMP=C:\Program Files\RoentDek Handels GmbH\CoboldPC 2011 R5-2-x64 (Visual Studio .Net 2010 Compilation) V10.1.1412.2\LAES2_TDC8PCI2_HEX\Cobold_Shared2\temp\


MSBuild "%PATH_PROJ%" /t:build /p:Configuration=Debug;Platform="x86";OutDir="C:\Program Files\RoentDek Handels GmbH\CoboldPC 2011 R5-2-x64 (Visual Studio .Net 2010 Compilation) V10.1.1412.2\LAES2_TDC8PCI2_HEX\Cobold_Shared2\temp"
if %ERRORLEVEL% neq 0 (
	echo Complie_By_x86_DAn failed,  ErrorLevel:%ERRORLEVEL%
	exit /b %Ret_Failed%
)

exit /b %Ret_Success%