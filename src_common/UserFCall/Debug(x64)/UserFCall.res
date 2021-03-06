        ��  ��                  �      �� ��     0 	        �4   V S _ V E R S I O N _ I N F O     ���    
  � 
  �?                        �   S t r i n g F i l e I n f o   �   0 4 0 9 0 4 e 4   d &  C o m m e n t s   C o b o l d P C   U s e r   f u n c t i o n   c a l l   F u n c t i o n s   2 	  C o m p a n y N a m e     R o e n t D e k     X   F i l e D e s c r i p t i o n     C o b o l d P C   2 0 1 1   U s e r F C a l l   >   F i l e V e r s i o n     1 0 ,   1 ,   1 4 1 2 ,   2     <   I n t e r n a l N a m e   U s e r F C a l l   d l l   T   L e g a l C o p y r i g h t   C o p y r i g h t   � � �   1 9 9 5 - 2 0 1 4   D   O r i g i n a l F i l e n a m e   U s e r F C a l l   d l l   X   P r o d u c t N a m e     U s e r F C a l l   f o r   C o b o l d P C   2 0 1 1   B   P r o d u c t V e r s i o n   1 0 ,   1 ,   1 4 1 2 ,   2        R e v i s i o n   R 5     D    V a r F i l e I n f o     $    T r a n s l a t i o n     	�C  H   S O U R C E _ U S E R F C A L L _ C P P   ���      0 	        // UserFCall.cpp : Defines the initialization routines for the DLL.
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
} 