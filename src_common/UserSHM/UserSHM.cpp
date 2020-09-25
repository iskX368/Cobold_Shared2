// UserSHM.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "CoboldUtilities.h"
#include "UserSHM.h"
#include "TCPIP.h"
#include "Delay.h"
#include "LMInfo.h"
#include "UserSP.h"

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
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

// CUserSHMApp
BEGIN_MESSAGE_MAP(CUserSHMApp, CWinApp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDAnApp construction
USHM_API SharedClass *SharedClassInstance;

TCPIP_thread_class *TCPIP_thread_instance;
DelayClass *Delay_instance;
CUserSpectrum *pSHMUserSP;
CUserSHMApp theUserSHMApp;

// CUserSHMApp construction
CUserSHMApp::CUserSHMApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

// CUserSHMApp init instance
BOOL CUserSHMApp::InitInstance()
{
	if (!CWinApp::InitInstance())
		return false;

	if(!pSHMUserSP) 
		pSHMUserSP = (CUserSpectrum *)new CUserSpectrum();

	if(!SharedClassInstance) 
	{
		SharedClassInstance = new SharedClass();
		SharedClassInstance->bDAqIsRunningOnline = false;
		SharedClassInstance->bDAqIsRunningOffline = false;
		SharedClassInstance->bDAnIsRunning = false;
		SharedClassInstance->bRequestStopDAq = false;
		SharedClassInstance->adc_peak_infos_array = nullptr;
		SharedClassInstance->UserSHM_pointer1 = nullptr;
		SharedClassInstance->UserSHM_pointer2 = nullptr;
		SharedClassInstance->UserSHM_pointer3 = nullptr;
		SharedClassInstance->UserSHM_pointer4 = nullptr;
	}

	if (Delay_instance)
	{
		Delay_instance->TerminateThread();
		delete Delay_instance;
		Delay_instance = nullptr;
	}

	Delay_instance = new DelayClass();

	TCPIP_thread_instance = nullptr;

	return TRUE;
}

// CUserSHMApp exit instance
__int32 CUserSHMApp::ExitInstance()
{
	if(pSHMUserSP)
	{
		delete pSHMUserSP; 
		pSHMUserSP = nullptr;
	}

	if (TCPIP_thread_instance) {
		TCPIP_thread_instance->TerminateThread();
		delete TCPIP_thread_instance;
		TCPIP_thread_instance = nullptr;
	}

	if (Delay_instance) {
		Delay_instance->TerminateThread();
		delete Delay_instance;
		Delay_instance = nullptr;
	}

	if(SharedClassInstance) 
	{
		delete SharedClassInstance;
		SharedClassInstance = nullptr;
	}

	return CWinApp::ExitInstance();
}

USHM_API SharedClass *SHMGetSharedClassInstance(void)
{
	return SharedClassInstance;
}
