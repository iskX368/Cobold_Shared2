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

// CUserFCallApp
BEGIN_MESSAGE_MAP(CUserFCallApp, CWinApp)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CUserFCallApp construction
CUserFCallApp theUserFCallApp;

// CUserFCallApp construction and destruction
// CUserFCallApp constructor
CUserFCallApp::CUserFCallApp() : m_nInheritCounter(0)
{
}

///////////////////////////////////
// CUserFCallApp Helper Functions

// global definitions
typedef SharedClass *(*MyProc4)(void);
MyProc4 dllSHMGetSharedClassInstance;

// CUserFCallApp initialization
BOOL CUserFCallApp::InitInstance()
{
	if(!CWinApp::InitInstance()) 
		return false;

	m_nInheritCounter += 1;
	this->SharedClassInstance = nullptr;
	this->bUserSHMDllIsBound = false;
	this->hUserSHMLibHandle = nullptr;
	BindUserSHMDll();
	return TRUE;
}

// CUserFCallApp exit instance
__int32 CUserFCallApp::ExitInstance()
{
	if(this->hUserSHMLibHandle) 
		FreeLibrary(hUserSHMLibHandle); 
	if(m_nInheritCounter <= 1)
	{
		;	
	}
	m_nInheritCounter = m_nInheritCounter > 1 ? --m_nInheritCounter : 0;
	return CWinApp::ExitInstance();
}

// CUserFCallApp SHM bind
bool CUserFCallApp::BindUserSHMDll() 
{
	if(bUserSHMDllIsBound) 
		return true;

	if(!hUserSHMLibHandle)
	{
		if(IsProcess64())
			hUserSHMLibHandle = LoadLibrary("UserSHM_x64.dll");
		else
			hUserSHMLibHandle = LoadLibrary("UserSHM_x86.dll");
	}

	bUserSHMDllIsBound = false;
	if(hUserSHMLibHandle) 
	{
		dllSHMGetSharedClassInstance = (MyProc4)GetProcAddress(hUserSHMLibHandle,"SHMGetSharedClassInstance");
		SharedClassInstance = dllSHMGetSharedClassInstance();
		if(SharedClassInstance) 
			bUserSHMDllIsBound = true; 
		else 
			bUserSHMDllIsBound = false;
	}
	return bUserSHMDllIsBound;
}