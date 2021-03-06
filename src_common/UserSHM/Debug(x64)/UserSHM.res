        ��  ��                  �      �� ��     0 	        �4   V S _ V E R S I O N _ I N F O     ���    
  � 
  �?                        �   S t r i n g F i l e I n f o   �   0 4 0 9 0 4 e 4   d &  C o m m e n t s   C o b o l d P C   U s e r   S h a r e d   M e m o r y   F u n c t i o n s   2 	  C o m p a n y N a m e     R o e n t D e k     T   F i l e D e s c r i p t i o n     C o b o l d P C   2 0 1 1   U s e r S H M   >   F i l e V e r s i o n     1 0 ,   1 ,   1 4 1 2 ,   2     8   I n t e r n a l N a m e   U s e r S H M   d l l   T   L e g a l C o p y r i g h t   C o p y r i g h t   � � �   1 9 9 5 - 2 0 1 4   @   O r i g i n a l F i l e n a m e   U s e r S H M   d l l   T   P r o d u c t N a m e     U s e r S H M   f o r   C o b o l d P C   2 0 1 1   B   P r o d u c t V e r s i o n   1 0 ,   1 ,   1 4 1 2 ,   2        R e v i s i o n   R 5     D    V a r F i l e I n f o     $    T r a n s l a t i o n     	��  D   S O U R C E _ U S E R S H M _ C P P   ���      0 	        // UserSHM.cpp : Defines the initialization routines for the DLL.
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
 