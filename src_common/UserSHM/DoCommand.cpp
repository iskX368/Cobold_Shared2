#include "stdafx.h"
#include "CoboldUtilities.h"
#include "UserSHM.h"
#include "LMInfo.h"
#include "USerSP.h"
//#include "CoordinatesMath.h"
#include "TCPIP.h"
#include "Delay.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CUserSpectrum *pSHMUserSP;
extern TCPIP_thread_class * TCPIP_thread_instance;
extern DelayClass * Delay_instance;

using namespace std;

__int32 Marker_roi[3];
bool Marker_array_initialized;

///////////////////////////////////////////////////////
// Functions accessible from the Cobold command line:
///////////////////////////////////////////////////////
USHM_API bool DoFunction(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount)
{
	/////////////////////////////////
	//
	// Important: Do not call MFC function in this function!!!
	//			  Most MFC calls will result in errors.
	//
	/////////////////////////////////

	CDoubleArray &Parameters = GetDAqDAnParameters();
	CDoubleArray &EventData  = GetDAqDAnEventData();
	
	if(ui32WordCount < 3)
	{
		AfxMessageBox("The command has not enough arguments to proceed.",MB_ICONEXCLAMATION|MB_OK);
		return true;
	}
	CString command = csInput[2];
	command.MakeLower();

	if (command == "help")
	{
		CString msg = "";
		msg += "The following commands are available in UserSHM.dll:\n\n";
		msg += "delay_command,[seconds],[command]\n";
		msg += "tcpip,[port_number] remote control via Telnet\n";
		AfxMessageBox(msg,MB_ICONEXCLAMATION|MB_OK);
		return true;
	}
	
	if (command == "tcpip" && ui32WordCount > 3) 
	{
		if (TCPIP_thread_instance)
		{
			AfxMessageBox("remote control via TCPIP already active.",MB_ICONEXCLAMATION|MB_OK);
			return true;
		}
		__int32 port = atoi((char*)LPCTSTR(csInput[3]));
		TCPIP_thread_instance = new TCPIP_thread_class(port);
		if (ui32WordCount > 4)
		{
			if (csInput[4] == CString("echo")) 
				TCPIP_thread_instance->do_echo = true;
		}
		TCPIP_thread_instance->StartThread();
		return true;
	}

	if (command == "delay_command")
	{
		if(ui32WordCount < 5) 
		{
			CString msg;
			msg.Format("wrong number of arguments for \"delay_command\"");
			AfxMessageBox(msg,MB_ICONEXCLAMATION|MB_OK);
			return true;
		}

		if (Delay_instance) 
		{
			if (!Delay_instance->bThreadIsRunning)
			{
				Delay_instance->delay_seconds = atof(LPCSTR(csInput[3]));
				CString cs_temp = csInput[4];
				for (unsigned __int32 i=5;i<ui32WordCount;++i)
					cs_temp += CString(",") + csInput[i];
				sprintf_s((char*)Delay_instance->command,1000,LPCTSTR(cs_temp));
				Delay_instance->StartThread();
			}
		}
		return true;
	}

	if(command == "set_roi" && ui32WordCount >= 4) 
	{
		if (!Marker_array_initialized)
		{
			Marker_array_initialized = true;
			memset(Marker_roi,0,sizeof(__int32)*3);
		}
		__int32 i_xy = pSHMUserSP->FindSpectrum("xy clean (mm)");
		if (i_xy < 0) 
			return true;
		__int32 target_index = atoi((char*)LPCTSTR(csInput[3]));
		if (target_index < 1 || target_index > 3) 
			return true;

		CString Cobold_command;

		if (SharedClassInstance->bDAqIsRunningOffline || SharedClassInstance->bDAqIsRunningOnline)
		{
			Cobold_command.Format("pause;"); ExecuteCommand(Cobold_command);
			Sleep(100);
		}

		Cobold_command.Format("view %i;",i_xy+1); ExecuteCommand(Cobold_command);
		double x1,y1,x2,y2;
		pSHMUserSP->GetXYBoxCursor(x1,y1,x2,y2, true);
		Sleep(50);
		if (x2 < x1) 
		{
			double a=x1;
			x1 = x2; 
			x2 = a;
		}
		if (y2 < y1) 
		{
			double a=y1;
			y1 = y2; 
			y2 = a;
		}

		//Cobold_command.Format("marker Orthogonal,%lf,%lf",x1,y1); ExecuteCommand(Cobold_command);
		//Cobold_command.Format("marker Orthogonal,%lf,%lf",x2,y2); ExecuteCommand(Cobold_command);
		Cobold_command.Format("marker b,%lf,%lf,%lf,%lf",x1,y1,x2,y2); ExecuteCommand(Cobold_command);

		__int32 HighestIndex = -1;

		for (__int32 i=0;i<3;++i)
		{
			if (HighestIndex < Marker_roi[i]) 
				HighestIndex = Marker_roi[i];
		}

		if (target_index == 1) 
		{
			Cobold_command.Format("condition PosX,%lf,%lf,ROIx_1;",x1,x2); ExecuteCommand(Cobold_command);
			Cobold_command.Format("condition PosY,%lf,%lf,ROIy_1;",y1,y2); ExecuteCommand(Cobold_command);
			//Cobold_command.Format("condition ROIx_1,and,ROIy_1,ROI_1;"); ExecuteCommand(Cobold_command);
			__int32 i = pSHMUserSP->FindSpectrum("TOF_ROI_1");
			Cobold_command.Format("clear %i;",i+1);	ExecuteCommand(Cobold_command);
		}
		if (target_index == 2)
		{
			Cobold_command.Format("condition PosX,%lf,%lf,ROIx_2;",x1,x2); ExecuteCommand(Cobold_command);
			Cobold_command.Format("condition PosY,%lf,%lf,ROIy_2;",y1,y2); ExecuteCommand(Cobold_command);
			//Cobold_command.Format("condition ROIx_2,and,ROIy_2,ROI_2;"); ExecuteCommand(Cobold_command);
			__int32 i = pSHMUserSP->FindSpectrum("TOF_ROI_2");
			Cobold_command.Format("clear %i;",i+1); ExecuteCommand(Cobold_command);
		}
		if (target_index == 3)
		{
			Cobold_command.Format("condition PosX,%lf,%lf,ROIx_3;",x1,x2); ExecuteCommand(Cobold_command);
			Cobold_command.Format("condition PosY,%lf,%lf,ROIy_3;",y1,y2); ExecuteCommand(Cobold_command);
			//Cobold_command.Format("condition ROIx_3,and,ROIy_3,ROI_3;"); ExecuteCommand(Cobold_command);
			__int32 i = pSHMUserSP->FindSpectrum("TOF_ROI_3");
			Cobold_command.Format("clear %i;",i+1); ExecuteCommand(Cobold_command);
		}

		Sleep(300);

		bool deletion_occured = false;
		if (Marker_roi[target_index-1] > 0)
		{
			Cobold_command.Format("remove marker,%i,%i;",i_xy+1,2*Marker_roi[target_index-1]-1);
			ExecuteCommand(Cobold_command);
			//ExecuteCommand(Cobold_command);
			__int32 temp = Marker_roi[target_index-1];
			for (__int32 i=0;i<3;++i)
			{
				if (Marker_roi[i] >= temp)
					Marker_roi[i] -= 1;
			}
			Marker_roi[target_index-1] = HighestIndex;
		} 
		else
			Marker_roi[target_index-1] = HighestIndex+1;

		if (Marker_roi[target_index-1] < 1)
			Marker_roi[target_index-1] = 1;

		if (SharedClassInstance->bDAqIsRunningOffline || SharedClassInstance->bDAqIsRunningOnline)
		{
			Cobold_command.Format("start;"); ExecuteCommand(Cobold_command);
			Sleep(100);
		}

		return true;
	}

	if(command == "delete_roi" && ui32WordCount >= 4)
	{
		if (!Marker_array_initialized)
		{
			Marker_array_initialized = true;
			memset(Marker_roi,0,sizeof(__int32)*3);
		}

		__int32 i_xy = pSHMUserSP->FindSpectrum("xy clean (mm)");
		if (i_xy < 0) 
			return true;

		__int32 target_index = atoi((char*)LPCTSTR(csInput[3]));
		if (target_index < 1 || target_index > 3) 
			return true;

		CString Cobold_command;
		if (SharedClassInstance->bDAqIsRunningOffline || SharedClassInstance->bDAqIsRunningOnline)
		{
			Cobold_command.Format("pause;"); ExecuteCommand(Cobold_command);
			Sleep(100);
		}

		if (target_index == 1) 
		{
			Cobold_command.Format("condition PosX,-1.e100,1.e100,ROIx_1;"); ExecuteCommand(Cobold_command);
			Cobold_command.Format("condition PosY,-1.e100,1.e100,ROIy_1;"); ExecuteCommand(Cobold_command);
			__int32 i = pSHMUserSP->FindSpectrum("TOF_ROI_1");
			Cobold_command.Format("clear %i;",i+1);	ExecuteCommand(Cobold_command);
		}
		if (target_index == 2)
		{
			Cobold_command.Format("condition PosX,-1.e100,1.e100,ROIx_2;"); ExecuteCommand(Cobold_command);
			Cobold_command.Format("condition PosY,-1.e100,1.e100,ROIy_2;"); ExecuteCommand(Cobold_command);
			__int32 i = pSHMUserSP->FindSpectrum("TOF_ROI_2");
			Cobold_command.Format("clear %i;",i+1); ExecuteCommand(Cobold_command);
		}
		if (target_index == 3)
		{
			Cobold_command.Format("condition PosX,-1.e100,1.e100,ROIx_3;"); ExecuteCommand(Cobold_command);
			Cobold_command.Format("condition PosY,-1.e100,1.e100,ROIy_3;"); ExecuteCommand(Cobold_command);
			__int32 i = pSHMUserSP->FindSpectrum("TOF_ROI_3");
			Cobold_command.Format("clear %i;",i+1); ExecuteCommand(Cobold_command);
		}

		Sleep(300);

		if (Marker_roi[target_index-1] > 0)
		{
			Cobold_command.Format("remove marker,%i,%i;",i_xy+1,2*Marker_roi[target_index-1]-1);
			ExecuteCommand(Cobold_command);
			//ExecuteCommand(Cobold_command);
			__int32 temp = Marker_roi[target_index-1];
			for (__int32 i=0;i<3;++i)
			{
				if (Marker_roi[i] >= temp) 
					Marker_roi[i] -= 1;
			}
			Marker_roi[target_index-1] = 0;
		}

		if (SharedClassInstance->bDAqIsRunningOffline || SharedClassInstance->bDAqIsRunningOnline)
		{
			Cobold_command.Format("start;"); ExecuteCommand(Cobold_command);
			Sleep(100);
		}

		return true;
	}

	//if (command == "reseteventdata")
	//{
	//	if (EventData.GetSize() > 0) 
	//	{
	//		EventData.RemoveAll();
	//		EventData.FreeExtra();
	//	}
	//	return true;
	//}
	
	CString msg;
	msg.Format("unknown command \"%s",csInput[2]);
	for (unsigned __int32 i=3; i< ui32WordCount;++i) msg += CString(",") + csInput[i];
	msg += "\"";
	AfxMessageBox(msg,MB_ICONEXCLAMATION|MB_OK);
	return true;
}