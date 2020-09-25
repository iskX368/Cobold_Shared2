// UserFCall.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "UserSHM.h"
#include "UserFCall.h"
#include "LMInfo.h"
#include "UserSP.h"
#include "CoboldUtilities.h"
#include "Functions.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: If this DLL is dynamically linked against the MFC DLLs,
//		any functions exported from this DLL which call into
//		MFC must have the AFX_MANAGE_STATE macro added at the
//		very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC. This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

// returning false on error
bool DoFunction(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount)
{
	/////////////////////////////////
	//
	// Important: Do not call MFC function in this function!!!
	//			 Most MFC calls will result in errors.
	//
	/////////////////////////////////
	CFunctions *pF = new CFunctions();


	if(!theUserFCallApp.bUserSHMDllIsBound) 
	{
		CString Msg;
		if(IsProcess64())
			Msg.Format("UserFCall_x64.dll not found.");
		else
			Msg.Format("UserFCall_x86.dll not found.");
		AfxMessageBox(Msg,MB_ICONEXCLAMATION|MB_OK);

		return false;
	}

	if(ui32WordCount < 3)
	{
		CString Msg;
		Msg.Format("UserFCall command\nhas not enough input to proceed.");
		AfxMessageBox(Msg,MB_ICONEXCLAMATION|MB_OK);

		return false;
	}
	CString command = csInput[2];
	command.MakeLower();
	pF->SetCommand(command);

	/////////////////////////////////
	//
	// place test code here
	// (when testing make sure to uncomment the return statement
	//
	/////////////////////////////////
	//CString mt,temp;
	//mt.Format("ui32WordCount = %d\n\n",ui32WordCount);
	//for(int i=0;i<ui32WordCount;i++)
	//{
	//	temp.Format("csInput[%d] = %s\n",i,csInput[i]);
	//	mt = mt + temp;
	//}
	//AfxMessageBox(mt);
	//return true;
	/////////////////////////////////
	//
	// end of test code
	//
	/////////////////////////////////

	if(CString("angle").Find(command) == 0)
		return pF->Angle(csInput, ui32InputMaximumIndex, ui32WordCount);

	if(CString("beep").Find(command) == 0)
		return pF->Beep(csInput, ui32InputMaximumIndex, ui32WordCount);

	if(CString("distance").Find(command) == 0)
		return pF->Distance(csInput, ui32InputMaximumIndex, ui32WordCount);

	if(CString("dynintegral").Find(command) == 0)
		return pF->DynIntegral(csInput, ui32InputMaximumIndex, ui32WordCount);

	if(CString("echo").Find(command) == 0)
		return pF->Echo(csInput, ui32InputMaximumIndex, ui32WordCount);

	if(CString("export_ascii_multi").Find(command) == 0)
		return pF->ExportAsciiMulti(csInput,ui32InputMaximumIndex,ui32WordCount);

	if(CString("export_tif_16bit").Find(command) == 0)
		return pF->ExportTIF16Bit(csInput, ui32InputMaximumIndex, ui32WordCount);

	if(CString("export_tif_24bit_rgb").Find(command) == 0)
		return pF->ExportTIF24BitRGB(csInput, ui32InputMaximumIndex, ui32WordCount);

	if(CString("fitg").Find(command) == 0)
		return pF->FitG(csInput, ui32InputMaximumIndex, ui32WordCount);

	if(CString("gauss").Find(command) == 0)
		return pF->Gauss(csInput, ui32InputMaximumIndex, ui32WordCount);

	if(CString("get_coordinate_index").Find(command) == 0)
		return pF->GetCoordinateIndex(csInput, ui32InputMaximumIndex, ui32WordCount);

	if(CString("get_mean_without_zeros").Find(command) == 0)
		return pF->GetMeanWithoutZeros(csInput, ui32InputMaximumIndex, ui32WordCount);

	if(CString("gethighestbin").Find(command) == 0)
		return pF->GetHighestBin(csInput, ui32InputMaximumIndex, ui32WordCount);

	if(CString("help").Find(command) == 0)
		return pF->Help();

	if(CString("linreg").Find(command) == 0)
		return pF->LinReg(csInput, ui32InputMaximumIndex, ui32WordCount);

	if(CString("load_img").Find(command) == 0)
		return pF->LoadImg(csInput, ui32InputMaximumIndex, ui32WordCount);

	if(CString("locate").Find(command) == 0)
		return pF->Locate(csInput, ui32InputMaximumIndex, ui32WordCount);

	if(CString("multidefine1").Find(command) == 0)
		return pF->MultiDefine1(csInput, ui32InputMaximumIndex, ui32WordCount);

	if(CString("multidefine2").Find(command) == 0)
		return pF->MultiDefine2(csInput, ui32InputMaximumIndex, ui32WordCount);

	if(CString("normalize").Find(command) == 0)
		return pF->Normalize(csInput, ui32InputMaximumIndex, ui32WordCount);

	if(CString("normalize_integral_y").Find(command) == 0)
		return pF->NormalizeIntegralY(csInput, ui32InputMaximumIndex, ui32WordCount);

	if(CString("normalize_y").Find(command) == 0)
		return pF->Normalize_Y(csInput, ui32InputMaximumIndex, ui32WordCount);

	if(CString("parameter_bitmask").Find(command) == 0)
		return pF->ParameterBitMask(csInput, ui32InputMaximumIndex, ui32WordCount);

	if(CString("phd_analysis").Find(command) == 0)
		return pF->PhdAnalysis(csInput, ui32InputMaximumIndex, ui32WordCount);

	if(CString("proj1d").Find(command) == 0)
		return pF->Proj1D(csInput, ui32InputMaximumIndex, ui32WordCount);

	if(CString("rebin").Find(command) == 0)
		return pF->ReBin(csInput, ui32InputMaximumIndex, ui32WordCount);

	if(CString("rescale").Find(command) == 0)
		return pF->Rescale(csInput, ui32InputMaximumIndex, ui32WordCount);

	if(CString("setdanresolutionparameters").Find(command) == 0)
		return pF->SetDAnResolutionParameters(csInput, ui32InputMaximumIndex, ui32WordCount);

	if(CString("setdaqcoordinates").Find(command) == 0)
		return pF->SetDAqCoordinates(csInput, ui32InputMaximumIndex, ui32WordCount);

	if(CString("sethitmatrixcoordinatesandspectrum").Find(command) == 0)
		return pF->SetHitMatrixCoordinatesAndSpectrum(csInput, ui32InputMaximumIndex, ui32WordCount);

	if(CString("wait").Find(command) == 0)
		return pF->Wait(csInput, ui32InputMaximumIndex, ui32WordCount);


	//@ユーザー関数を実行
	switch (__int32 ret = pF->pLUFs->ExecuteFunction(command, csInput, ui32InputMaximumIndex, ui32WordCount)) {
	case LAES2::UserFunctions_Ret::NothingDone:
		break;
	case LAES2::UserFunctions_Ret::Success:
		return true;
	default:
		CString eMess;
		eMess.Format("Error in LAES2_UserFunctions : ErrorCode = %d", ret);
		AfxMessageBox(eMess, MB_ICONERROR | MB_OK);
		break;
		return false;
	}




	return false;	// indicate error no command processed
}
