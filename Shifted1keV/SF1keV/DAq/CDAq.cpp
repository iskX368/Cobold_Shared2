// CDAq.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"

#include "CoboldUtilities.h"
#include "UserSHM.h"
#include "LMFPreEventData.h"
#include "CDAq.h"

#include "LMInfo.h"
#include "UserSP.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDAqApp

BEGIN_MESSAGE_MAP(CDAqApp, CWinApp)
	//{{AFX_MSG_MAP(CDAqApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDAqApp construction
CDAqApp theDAqApp;

CUserSpectrum *pUserSP;
CDAQ_API CSourceCodeInformation *pCodeInformation;

bool bIsEventMode;
__int64 i64EventCounter;
__int64 i64NumberofEventsToAcquire;
CTimeSpan ctsTimeToAcquire;
CTime ctStartTime;
bool bIsFirstReadCall;
bool bIsTerminating;

/////////////////////////////////////////////////////////////////////////////
CString *pDllFilePathName;

CDAqApp::CDAqApp()
{
	// get the dll's loaded path-file name
	pDllFilePathName = new CString("");
	char *pcStr = new char[MAX_PATH];
	if(pcStr) 
	{
		GetModuleFileName((HINSTANCE)&__ImageBase,pcStr,MAX_PATH);
		pDllFilePathName->Append(pcStr);
		delete[] pcStr; 
		pcStr = nullptr;
	}
	bIsEventMode = true;
}

BOOL CDAqApp::InitInstance()
{
	if(!CWinApp::InitInstance()) 
		return FALSE;

	// prepare user access to certain spectrum commands
	pUserSP = (CUserSpectrum *)new CUserSpectrum();

	// prepare source code information
	pCodeInformation = (CSourceCodeInformation *)new CSourceCodeInformation();
	pCodeInformation->coaCodeInformation.Add(new CCodeInformation("IDS_SOURCE_CPP", "CDAQ", "CDAq.cpp"));
	pCodeInformation->coaCodeInformation.Add(new CCodeInformation("IDS_SOURCE_CPP", "STDAFX", "StdAfx.cpp"));
	pCodeInformation->coaCodeInformation.Add(new CCodeInformation("IDS_SOURCE_CPP", "TDC8PCI2STANDARD", "TDC8PCI2Standard.cpp"));

	pCodeInformation->coaCodeInformation.Add(new CCodeInformation("IDS_SOURCE_H", "CDAQ", "CDAq.h"));
	pCodeInformation->coaCodeInformation.Add(new CCodeInformation("IDS_SOURCE_H", "CHECKPROCESS", "CheckProcess.H"));
	pCodeInformation->coaCodeInformation.Add(new CCodeInformation("IDS_SOURCE_H", "COBOLDUTILITIES", "CoboldUtilities.h"));
	pCodeInformation->coaCodeInformation.Add(new CCodeInformation("IDS_SOURCE_H", "COBOLDRECENTFILELIST", "CoboldRecentFileList.h"));
	pCodeInformation->coaCodeInformation.Add(new CCodeInformation("IDS_SOURCE_H", "GENERALIO", "GeneralIO.h"));
	pCodeInformation->coaCodeInformation.Add(new CCodeInformation("IDS_SOURCE_H", "HIGHRESOLUTIONTIMER", "HighResolutionTimer.h"));
	pCodeInformation->coaCodeInformation.Add(new CCodeInformation("IDS_SOURCE_H", "LMFILEDIALOG", "LMFileDialog.h"));
	pCodeInformation->coaCodeInformation.Add(new CCodeInformation("IDS_SOURCE_H", "LMFPREEVENTDATA", "LMFPreEventData.h"));
	pCodeInformation->coaCodeInformation.Add(new CCodeInformation("IDS_SOURCE_H", "LMINFO", "LMInfo.h"));
	pCodeInformation->coaCodeInformation.Add(new CCodeInformation("IDS_SOURCE_H", "MEMACC", "MemAcc.h"));
	pCodeInformation->coaCodeInformation.Add(new CCodeInformation("IDS_SOURCE_H", "RESOURCE", "resource.h"));
	pCodeInformation->coaCodeInformation.Add(new CCodeInformation("IDS_SOURCE_H", "STDAFX", "StdAfx.h"));
	pCodeInformation->coaCodeInformation.Add(new CCodeInformation("IDS_SOURCE_H", "USERSP", "UserSP.h"));

	return TRUE;
}

int CDAqApp::ExitInstance()
{
	// release UserSP
	if(pUserSP) 
	{
		delete pUserSP; 
		pUserSP = nullptr;
	}
	
	// release CodeInformation
	if(pCodeInformation)
	{
		pCodeInformation->coaCodeInformation.RemoveAll();
		pCodeInformation->coaCodeInformation.FreeExtra();
		delete pCodeInformation;
		pCodeInformation = nullptr;
	}
	
	return CWinApp::ExitInstance();
}

CString &DAqGetDllPathName()
{
	return *pDllFilePathName;
}

CDAQ_API CSourceCodeInformation* DAqGetSourceCodeInformation()
{
	return pCodeInformation;
}

CDAQ_API bool DAqIsTerminating()
{
	return bIsTerminating;
}

// true to break in DAqOnlineReadEvent
bool ShouldFinishTakingData()
{
	if(bIsEventMode)
	{
		if(i64EventCounter >= i64NumberofEventsToAcquire)
		{
			bIsTerminating = true;
			return true;
		}
	}
	else
	{
		if(CTime::GetCurrentTime() - ctStartTime > ctsTimeToAcquire) 
		{
			bIsTerminating = true;
			return true;
		}
	}

	if(IsDAqTerminating())																	// test if DAq should terminate
		return true;																				// and break out of the loop

	return false;
}