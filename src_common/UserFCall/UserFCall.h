// UserFCall.h : main header file for the UserFCall DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

	#ifdef USERFC_EXPORTS
	#define UFC_API __declspec(dllexport)
	#else
	#define UFC_API __declspec(dllimport)
	#endif

	class SharedClass;
	//class Histo3DClass;
	class CUserFCallApp;

	extern CUserFCallApp theUserFCallApp;	
	// CUserFCallApp
	// See UserFCall.cpp for the implementation of this class
	//
	class CUserFCallApp : public CWinApp
	{
	public:
		CUserFCallApp();
	public:
		__int32 m_nInheritCounter;
	// Overrides
	public:
		virtual BOOL InitInstance();
		__int32 ExitInstance();
		bool BindUserSHMDll();

		HMODULE hUserSHMLibHandle;
		SharedClass *SharedClassInstance;
		bool bUserSHMDllIsBound;

		DECLARE_MESSAGE_MAP()
	};

	UFC_API bool DoFunction(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount);

#ifdef __cplusplus
}

#endif
