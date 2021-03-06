        ’’  ’’                    @   I D S _ S O U R C E _ C P P   C D A Q       0           // CDAq.cpp : Defines the initialization routines for the DLL.
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
}Ź   D   I D S _ S O U R C E _ C P P   S T D A F X       0           // stdafx.cpp : source file that includes just the standard includes
//	CDAq.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

  ņĮ  X   I D S _ S O U R C E _ C P P   T D C 8 P C I 2 S T A N D A R D       0           ///////////////////////////////////////////////////////////////////////////
//	Standard User defined acquisition part called from CoboldPC main program
//  (c) by RoentDek Handels GmbH
//
//  RoentDek Standard DAq
/////////////////////////////////////////////////////////////////////////////
// Hardware dependent part
//
// TDC8/PCI(B) Standard Module
// (PCI over MemoryAccess)
//
// TDC8 with n hit 1 <= n <= 16
// 500 ps/channel
//
// V1 - 28.09.2001
// V2 - 05.02.2002					Totally reconfigured
// V3 - 08.04.2002					Date confirmation to DAn
// V4 - 13.05.2002					Missing parameter inserted into UserDefHeader
// V4.2 - 19.06.2002				PCI via MemoryAccess 1.3.1 implemented
// V4.3 - 14.11.2002				Adapt for Version 6 Prom
// V4.5 - 18.09.2003				TDC8PCI B only!!!!
// V5.0 - 01.02.2004				Hex Gap implemented
// V5.1 - V5.4						no changes
// V5.5 - 30.08.2005				Integration of old V4 Data Format
//								    extending parameter saving in header
//									LMFVersion defined in UserDefHeader
//									DAqGetLastLMFName function available
// V5.5 - 30.01.2006				In CommonStart mode hit order reversed
//									Multi Hit order now correct
// V5.6 - 30.08.2007				UserDefine Data format (variable event length)
//									LMF Version changed to 6
// V7.01 - 16.07.2008				DAQ_VERSION7, LMF_VERSION7 and LM_FILE_VERSION9 now
//									new user-header info added (see DAqOnlineInitialize)
//									DAqGetLastStartTime() introduced as new DAq function
//									DAqGetLastStartTime() added after Start() (TDC)
//									EventStructure changed... no support for intermediate beta (2008) support
//										36bit EventCounter added to EventStructure
//										hit array now of type unsigned __int16
//									EventData array is cleared (by CoboldPC) before ReadEvent routines. Internal TDC array must be zeroed manually.
//									internal EventCounter changed to int64
//									Modified for int64 support
// V7.02 - 20.11.2008				TimeStamp excluded from EventArray, will be handled separately
// V7.03 - 21.01.2009				Error corrected for PreV5 LMFs
// V7.04 - 27.05.2009				DAqInitialize call modified: add __int64 MaxEvents
// V7.05 - 06.08.2009				LMF_VERSION8 
//									including sources into LMF
//									parameter index now in the form x-1 instead of y=(x-1) for readability
// V7.06 - 06.10.2009				Parameters 50, 51 shifted to 51, 52
// V7.07 - 24.10.2009				LMF_VERSION9 created to eliminate offline read error of LMF_VERSION8
//										Offline read of LMF testes for CoboldPC 2002 U9, CoboldPC 2008 812 
//										and CoboldPC 2008 R2 (909 + Update 09.10.2009)
//									Start Time now correct with R2 909.1
// V7.08 - 08.03.2010				x64 adaption
// V10.00 - 02.03.2012				changed to DAQ_VERSION10
//									changed to LMF_VERSION10
//                                  changed CHECKID
//									changed function names
//										DAqOnlineInitialize -> DAqLMFWriteUserHeader
//										DAqOfflineInitialize -> DAqLMFReadUserHeader
//									index of Parameters and WeighParameters now starting at 0 NOT 1
//									DAq Parameters from RoentDek now uses Parameters 0-999
//									include of "hm1\hm1.h" eliminated
//									Moved Parameter 50 to 55
//									Moved Parameter 51 to 56
//									new parameters introduced 53
// V10.01 - 31.07.2012				implemented AutoFlush call
//									implemented DAQ termination detection
// V10.02 - 15.08.2013				LMF file on network detection implemented
//									modifications to several functions to implement time termination for data taking
// V10.03 - 20.11.2013				CHECKID_DAQCCF changed
// V10.04 - 11.11.2014				DAqGetLastLMFName function secured by try catch environment
///////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "math.h"
#include "float.h"

#include "CoboldUtilities.h"

#include "LMInfo.h"
#include "LMFPreEventData.h"

#include "CDAq.h"

// Dlls
#include "GeneralIO.h"
#include "HighResolutionTimer.h"


//@±±ÅCN[h
#include "LAES2_Extension.h"
#include "Lib_CurrentDAqInfo.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// definitions for this DAq version information

#define CHECKID_DAQCCF	201311200000	// 20.11.2013-00:00

#define DAQ_VERSION10	20110208		// 08.02.2011
#define DAQ_VERSION7	20080507		// 07.05.2008
#define DAQ_VERSION6	20090901
#define DAQ_VERSION5	20020408
#define DAQ_VERSION4	20020408

#define LMF_VERSION10	0x000a
#define LMF_VERSION9	0x0009
#define LMF_VERSION8	0x0008
#define LMF_VERSION7	0x0007
#define LMF_VERSION6	0x0006
#define LMF_VERSION5	0x0005

#define DAQ_SUBVERSION	0x0004

/////////////////////////////////////////////////////////////////////////////
// definitions for this DAq

//DAQ_ID	DAQ_ID_TDC8

#define MAXIMUM_NUMBER_OF_HITS		16
#define MAXIMUM_NUMBER_OF_CHANNELS	8

/////////////////////////////////////////////////////////////////////////////
// Parameter description used in this hardware dependent part!
// -----------------------------------------------------------
//
// Parameter  2 = Save TimeStamp
//                0 = no Timestamp,
//                1 = 32Bit Timestamp	(Low.Low, Low.high)
//                2 = 64Bit Timestamp	(Low.Low, Low.high, High.Low, High.high)
// Parameter  3 = System Timeout Time in s
//				  (after this time with no event the system will be reset)
// Parameter  5 = TimeScaling (Internally set, tics per second)
// Parameter  6 = DAq Version # (Internally set)
// Parameter  7 = Time Reference of DAq Initialize (Start time of LMF) (Internally set)
// Parameter  8 = DAQ_ID (Internally set)
//					DAQ_ID_RAW			0x000000		// for RAW (no Data)
//					DAQ_ID_HM1			0x000001		// for HM1 (single)
//					DAQ_ID_TDC8			0x000002		// for TDC8/ISA/PCI
//					DAQ_ID_CAMAC		0x000003		// for CAMAC
//					DAQ_ID_2HM1			0x000004		// for 2 HM1
//					DAQ_ID_2TDC8		0x000005		// for 2 TDC8
// Parameter  9 = DAq Sub-Version # (Internally set)
// Parameter 14 = trigger mode for common input
//			    0 = Common Start Mode
//				1 = Common Stop Mode
//
// Parameter 20 = Resolution of TDC in ns (internally set)
// Parameter 21 = TDC data type information (internally set)
//			0 = Not defined
//			1 = Channel information
//			2 = Time information (in ns)
//
// Parameter 30 = Event Open Time in µs for detecting an event.
// Parameter 32 = number of Channels (reread during offline)
//				1..8
// Parameter 33 = number of hits 1..16 (reread during offline)
// Parameter 40 = DataFormat (Internally set)
//
// Parameter 45 = gate delay
//				  gate delay      *33ns+150ns = gate delay[ns]
// Parameter 46 = acquisition gate open time
//				  gate open time  *33ns = gate open time[ns]
// Parameter 47 = write empty events
//              0 discard empty events
//              1 write empty events
// Parameter 48 = trigger falling edge
//              0 disable
//              1 enable
// Parameter 49 = trigger rising edge
//              0 disable
//              1 enable
//
// Parameter 50 = check number to test DAq and CCF for compatibility
// Parameter 53 = NumberOfDAqLoops	(normally 1)
// Parameter 55 = EmptyCounter(Sum)					(since PCIVersion8)
// Parameter 56 = EmptyCounter(since last Event)	(since PCIVersion8)

// ------------------------------------------------
// Event Structure written by this hardware-insert!
// ------------------------------------------------
//		unsigned __int64 hdr1
//		unsigned __int64 hdr2
//		[unsigned __int32 TimeStamp low Part]		(if parameter 2 >= 1)
//		[unsigned __int32 TimeStamp high Part]		(if parameter 2 > 1)
// for channel 1..N
//		unsigned __int16 #hitsChannelX
//		signed __int32 hit1ChannelX..hitNChannelX
//--------------------------
// hdr1 = EventMarker + EventLength
//		bit 63..56		always 0xff
//		bit 55..00		EventLegth in Bytes of this event
// hdr2
//		bit 63..00		event counter

/////////////////////////////////////////////////////////////////////////////
// variables definitions

unsigned __int32 ui32TimeStamp;				// Parameter 2
unsigned __int32 ui32DAqVersion;			// Parameter 6
unsigned __int32 ui32LMFVersion;			// Parameter 9
unsigned __int64 ui64NumberOfChannels;		// Parameter 32
unsigned __int64 ui64NumberOfHits;			// Parameter 33
__int32 i32NumberOfDAqLoops;				// Parameter 53 (default = 1) (can be -1, for special mode)

__int64 i64MaxEvents;

unsigned __int32 ui32DAq_ID;
unsigned __int32 ui32LMHeaderVersion;
__int32 i32SystemTimeOut;

__int32 i32StatusCount;
__int32 i32TriggerModeCommon;
unsigned __int16 TDC[MAXIMUM_NUMBER_OF_CHANNELS][MAXIMUM_NUMBER_OF_HITS+1];
double dResolution;
__int32 i32EventOpenTime;

__int32 _i32SystemTimeOut;
unsigned __int64 _ui64NumberOfHits;

bool bIsPCIVersion8;

// PCI support
unsigned __int32 iIoAdr;
#include "MemAcc.h"
BADDR MyDevice[6];
unsigned __int16 MyVendor;
unsigned __int16 MyDeviceID;
unsigned __int32 *iPCIIoAdr;
HANDLE MyDeviceHandle;

// version 5 Prom
#define SHIFT 9						//11 Bits -2 
#define FIFOTOP						(0x0 << SHIFT)
#define FIFOGET						(0x1 << SHIFT)
#define FIFOPUT						(FIFOGET)
#define FIFOCOUNT					(0x2 << SHIFT)
#define FIFOSIZE					(0x3 << SHIFT)
#define TDCSTATUS					(0x4 << SHIFT)
#define TDCRANGE					(0x5 << SHIFT)
#define TDCINFO						(0x6 << SHIFT)
#define TDCRES						(0x7 << SHIFT)
#define TDCREADEMPTYCOUNT			(0x8 << SHIFT)
#define TDCCLEARREADEMPTYCOUNT		(0x9 << SHIFT)
#define TDCCOMMONSTARTGATE			(0xa << SHIFT)

//Byte Registers
#define TDCCHANNELS(ptr) ((ptr[TDCINFO]) >> 24)
#define TDCBITS(ptr)     (((ptr[TDCINFO]) >> 16) & 0xff)
#define TDCMAXHITS(ptr)  (((ptr[TDCINFO]) >>  8) & 0xff)
#define TDCHITS(ptr)     (((ptr[TDCINFO]) >>  0) & 0xff)

unsigned __int32 _ui32LastEventFirstFIFOData;
unsigned __int32 _ui32EventToggleFlag;
unsigned char *pucTDCChannel;	// channel at offset 2 from 32 bit information (INTEL FORMAT!)
//unsigned __int32 *puiFIFOGet;
unsigned __int32 ui32FIFOData;
unsigned __int16 ui16TDCData;
unsigned char ucTDCChannel;
__int32 i32GateDelay;
__int32 i32GateOpen;
bool bEmptyEvents;
bool bFallingEdge;
bool bRisingEdge;

/////////////////////////////////////////////////////////////////////////////
// functions definitions

__int32 PCIGetTDC_B(CDoubleArray &Parameter,unsigned __int16 TDC[MAXIMUM_NUMBER_OF_CHANNELS][MAXIMUM_NUMBER_OF_HITS+1], unsigned __int32 *pui32PCIIoAdr);
void ResetTDC(unsigned __int32 *ui32pPCIIOAdr, bool &bEmptyEventsX, bool &bFallingEdgeX, bool &bRisingEdgeX);

bool UserInit(CDoubleArray &Parameter);
void UserFinalize();
bool DAqGetOnlineParameters(CDoubleArray &Parameter);
bool DAqGetOfflineParameters(CArchive *pAr, CDoubleArray &Parameter, unsigned __int32 ui32LMHeaderVersion, unsigned __int64 ui64LMUserHeaderLength);

extern CSourceCodeInformation *pCodeInformation;

/////////////////////////////////////////////////////////////////////////////
// Cobold information string 
//	(no file support here!)
//	returns an LPCTSTR (myInfoString)
CString myInfoString;
CDAQ_API LPCTSTR DAqGetInformationString()
{
	CString DAqDebugInfo;
#ifdef _DEBUG
	DAqDebugInfo = "D";
#endif
	//@ę¾¼Ģš±±ÅĻX@³ĶStandard
	myInfoString.Format("TDC8PCI2 %s (%sV %08d - %03d, %04d)",LAES2::GetAquisitionName(),DAqDebugInfo,DAQ_VERSION10,DAQ_SUBVERSION,LMF_VERSION10);
	return LPCTSTR(myInfoString);
}

/////////////////////////////////////////////////////////////////////////////
// Cobold information of last LMF name
//	(no file support here!)
//	returns an LPCSTR (myDAqFileName)
//	pAr is the CArchive to access the opened LMFile. (can be nullptr)
CString myDAqFileName = CString("");
#define DAqSetLastLMFName(pAr)	DAqGetLastLMFName(pAr)
CDAQ_API LPCTSTR DAqGetLastLMFName(CArchive *pAr)
{
	if(pAr)
	{
		TRY
			myDAqFileName = pAr->GetFile()->GetFilePath();
		CATCH_ALL( e )
			myDAqFileName.Empty();
		END_CATCH_ALL
	}
	return LPCTSTR(myDAqFileName);
}

/////////////////////////////////////////////////////////////////////////////
// Cobold information effective start time
//	(no file support here!)
//	returns CTime (ctLstStartTime)
CTime ctLastStartTime;
CDAQ_API CTime *DAqGetLastStartTime()
{
	return &ctLastStartTime;
}

/////////////////////////////////////////////////////////////////////////////
// Cobold information about LMHeaderVersion
//	(no file support here!)
//	_ui32LMHeaderVersion is the main header version id of the LMF
CDAQ_API void DAqSetLMHeaderVersion(unsigned __int32 _ui32LMHeaderVersion)
{
	ui32LMHeaderVersion = _ui32LMHeaderVersion;
}

/////////////////////////////////////////////////////////////////////////////
// Cobold get parameter information from DAq
// typically retrieves the time resolution into parameter 20
//	(no file support here!)
//	Parameter is the parameter array of CoboldPC in double
//	pDataFormat for returning information about the data format of the EventData array
// returns true if failed
// -- Parameter 5 and 20 will be modified
CDAQ_API bool DAqGetParameters(CArchive *pAr, CDoubleArray &Parameter, unsigned __int32 LMHeaderVersion, unsigned __int64 LMUserHeaderLength)
{
	if(pAr)
		return DAqGetOfflineParameters(pAr, Parameter, LMHeaderVersion, LMUserHeaderLength);
	else
		return DAqGetOnlineParameters(Parameter);
}

/////////////////////////////////////////////////////////////////////////////
// Cobold hardware Online functions
//	(no file support here!)
//	Parameter is the parameter array of CoboldPC in double
//	pDataFormat for returning information about the data format of the EventData array
CDAQ_API __int64 DAqInitialize(CDoubleArray &Parameter, unsigned __int32 *pDataFormat)
{
#ifdef WIN64
	return ErrorMessageRunningOnX64OS("DAqInitialize");
#else
	if(IsProcess64())
		return ErrorMessageRunningOnX64OS("DAqInitialize");

	// check for sufficient parameters of this insert
	if(Parameter.GetSize() < 54)
	{
		AfxMessageBox(CString("Do first the sufficient definitions\n")+
			CString("on parameters before\n")+
			CString("using the NEW function."));
		return 0;
	}

	// Parameter 53 (display only every nth event)
	i32NumberOfDAqLoops = RoundToNearestInt32(Parameter[53]);
	if(i32NumberOfDAqLoops == 0)
		i32NumberOfDAqLoops = 1;
	if(i32NumberOfDAqLoops < -1)
	{
		i32NumberOfDAqLoops = 1;
		CString sValues;
		sValues.Format("   -1,0 or >=1");
		AfxMessageBox(CString("Parameter 53 set to wrong value!\n")+
			CString("Possible values are:\n")+
			sValues);
		return -1;
	}

	ui32DAqVersion = DAQ_VERSION10;
	Parameter[6] = (double)ui32DAqVersion;
	ui32LMFVersion = LMF_VERSION10;
	Parameter[9] = (double)ui32LMFVersion;

	// define the Data format for main program
	*pDataFormat = LM_USERDEF;
	Parameter[40] = (double)(__int32)*pDataFormat;		// needed __int32 because USERDEF value is -1

	// DAq data type
	Parameter[21] = (double)(DT_CHANNEL);

	// find the TDC board
	if(!maOpenLibrary())
	{
		char sError[255];
		maGetLastState(sError);
		AfxMessageBox("MemoryAccess library error: " + (CString)sError, MB_ICONSTOP);
		return false;
	}

	MyVendor = 0x10dc;
	MyDeviceID = 0x2004;	// TillDC
	if(maGetDeviceBaseAddress(&MyVendor,&MyDeviceID,iIoAdr,MyDevice) != 1)
	{
		AfxMessageBox(CString("No TDC8/PCI could be found.\n") +
			CString("Do first the sufficient definitions\n") +
			CString("on parameter 1 (IO address\n") +
			CString("of the TDC card) before\n") +
			CString("using the NEW function."));
		return false;
	}
	iPCIIoAdr = (unsigned __int32 *)maMapPhysToLinear(MyDevice[0].BaseAddress,MyDevice[0].Size, &MyDeviceHandle);
	if(!iPCIIoAdr)
	{
		AfxMessageBox("Can not map Physical to Linear Memory!");
		return false;
	}
	pucTDCChannel = ((unsigned char *)&ui32FIFOData)+3;		// channel at offset 2 from 32 bit information (INTEL FORMAT!)

	// transfer parameter 2 iTimeSamp
	ui32TimeStamp = RoundToNearestUInt32(Parameter[2]);
	if(ui32TimeStamp < 0 || ui32TimeStamp > 2)
	{
		AfxMessageBox(CString("Parameter 2 set to wrong value!\n")+
			CString("Possible values are:\n")+
			CString("   0 = no timestamp\n")+
			CString("   1 = 32bit timestamp\n")+
			CString("   2 = 64bit timestamp"));
		return 0;
	}

	// transfer parameter 3 to iSystemTimeOut
	i32SystemTimeOut = RoundToNearestInt32(Parameter[3]);
	_i32SystemTimeOut = i32SystemTimeOut*1000000;

	// transfer parameter 5 to dHighResTimerFrequency
	// Initialize the high resolution timer for later use
	HighResTimerInit();
	// get computers frequency (ticks per second)
	Parameter[5] = HighResTimerFrequency();

	// transfer parameter 14 iTriggerModeCommon
	i32TriggerModeCommon = RoundToNearestInt32(Parameter[14]);
	if(i32TriggerModeCommon < 0 || i32TriggerModeCommon > 1)
	{
		AfxMessageBox(CString("Parameter 14 set to wrong value!\n")+
			CString("Possible values are:\n")+
			CString("   0 = common start\n")+
			CString("   1 = common stop"));
		return 0;
	}

	// parameter 20 internally set
	dResolution = 0.500;									// resolution of TDC8 is 500ps
	Parameter[20] = dResolution;

	// transfer parameter 30 iEventOpenTime
	i32EventOpenTime = RoundToNearestInt32(Parameter[30]);

	// transfer parameter 32 iNumberOfChannels
	ui64NumberOfChannels = RoundToNearestUInt32(Parameter[32]);
	if(ui64NumberOfChannels < 1 || ui64NumberOfChannels > MAXIMUM_NUMBER_OF_CHANNELS)
	{
		CString sValues;
		sValues.Format("   1..%d",MAXIMUM_NUMBER_OF_CHANNELS);
		AfxMessageBox(CString("Parameter 32 set to wrong value!\n")+
			CString("Possible values are:\n")+
			sValues);
		return 0;
	}

	// transfer parameter 33 iNumberOfHits
	ui64NumberOfHits = RoundToNearestUInt64(Parameter[33]);
	if(ui64NumberOfHits < 1 || ui64NumberOfHits > MAXIMUM_NUMBER_OF_HITS)
	{
		CString sValues;
		sValues.Format("   1..%d",MAXIMUM_NUMBER_OF_HITS);
		AfxMessageBox(CString("Parameter 33 set to wrong value!\n")+
			CString("Possible values are:\n")+
			sValues);
		return 0;
	}
	_ui64NumberOfHits = ui64NumberOfHits >= 16 ? 0 : ui64NumberOfHits;		// 0 = 16 Hits, else n Hits, nMax = 15

	// transfer parameter 45 iGateDelay
	i32GateDelay = RoundToNearestInt32(Parameter[45]);
	if((i32GateDelay < 0) || i32GateDelay >= 0x100)
	{
		CString sValues;
		sValues.Format("   0..255 or 0x00..0xff");
		AfxMessageBox(CString("Parameter 45 set to wrong value!\n")+
			CString("Possible values are:\n")+
			sValues);
		return 0;
	}

	// transfer parameter 46 iGateOpen
	i32GateOpen = RoundToNearestInt32(Parameter[46]);
	if((i32GateOpen < 0) || i32GateOpen >= 0x10000)
	{
		CString sValues;
		sValues.Format("   0..65535 or 0x0000..0xffff");
		AfxMessageBox(CString("Parameter 46 set to wrong value!\n")+
			CString("Possible values are:\n")+
			sValues);
		return 0;
	}

	// transfer parameter 47 bEmptyEvents
	bEmptyEvents = RoundToNearestInt32(Parameter[47]) ? true : false;

	// transfer parameter 48 bFallingEdge
	bFallingEdge = RoundToNearestInt32(Parameter[48]) ? true : false;

	// transfer parameter 49 bRisingEdge
	bRisingEdge = RoundToNearestInt32(Parameter[49]) ? true : false;

	// parameter 7 internally set
	time_t ltime;
	time( &ltime );
	Parameter[7] = (double)ltime;
	
	// parameter 8 internally set
	Parameter[8] = DAQ_ID_TDC8;

	////////////////////////
	// TDC8 Stuff
	////////////////////////
	// test version
	__int32 _iPCIVersion = iPCIIoAdr[TDCSTATUS];
	_iPCIVersion = (_iPCIVersion & 0x00ff0000) >> 16;
	if(_iPCIVersion >= 8)
	{
		Parameter[55] = 0;
		Parameter[56] = 0;
		bIsPCIVersion8 = true;
		__int32 _iDummy = iPCIIoAdr[TDCCLEARREADEMPTYCOUNT];
		_iDummy = iPCIIoAdr[TDCREADEMPTYCOUNT];
		Parameter[55] = _iDummy;
		Parameter[56] = _iDummy;
	}
	else
	{
		bIsPCIVersion8 = false;
	}

	/// 
	/// Initialize and Reset the TDCs
	///
	ResetTDC(iPCIIoAdr, bEmptyEvents, bFallingEdge, bRisingEdge);

	if(RoundToNearestUInt64(Parameter[50]) != CHECKID_DAQCCF) 
	{
		AfxMessageBox(CString("Parameter 50 indicates that the CCF\n") +
			CString("and DAq are not compatible\n"), MB_OK|MB_ICONSTOP);
		return false;
	}

	if(!UserInit(Parameter)) 
		return -1;

	// return number of elements needed in EventData array!
	return ui64NumberOfChannels * (ui64NumberOfHits + 1);
#endif
}

/////////////////////////////////////////////////////////////////////////////
// DAqFinalize
//	Parameter is the parameter array of CoboldPC in double
//  returns true if successful
CDAQ_API bool DAqFinalize(CDoubleArray &Parameter)
{
#ifdef WIN64
	return ErrorMessageRunningOnX64OS("DAqFinalize");
#else
	if(IsProcess64())
		return ErrorMessageRunningOnX64OS("DAqFinalize");

	maUnmapPhysicalMemory(MyDeviceHandle, iPCIIoAdr);
	maCloseLibrary();
	return true;
#endif
}

/////////////////////////////////////////////////////////////////////////////
// Cobold Online function DAqLMFWriteUserHeader
//	(file support here!)
//	pAr is the CArchive to access the opened LMFile. (can be nullptr)
//	Parameter is the parameter array of CoboldPC in double
//	pLMUserHeaderLength for returning information about UserHeaderLength
//  returns true if successful
CDAQ_API bool DAqLMFWriteUserHeader(CArchive *pAr,CDoubleArray &Parameter, NEWInformation newInfo, unsigned __int64 *pui64LMUserHeaderLength)
{
#ifdef WIN64
	return ErrorMessageRunningOnX64OS("DAqLMFWriteUserHeader");
#else
	if(IsProcess64())
		return ErrorMessageRunningOnX64OS("DAqLMFWriteUserHeader");

	// user define part to prepare to write an event
	// and declaring additional header information

	bIsEventMode = newInfo.bIsEventMode;
	bIsFirstReadCall = true;
	bIsTerminating = false;
	if(bIsEventMode)
	{
		i64NumberofEventsToAcquire = newInfo.maxEvents;
	}
	else
	{
		ctsTimeToAcquire = newInfo.maxTime;
	}

	i64EventCounter = 0;

	if(pAr)
	{
		// store the actual LMFName
		DAqSetLastLMFName(pAr);

		
		//@±±ÉĒĮ
		//LAES2::SaveCurrentDAqInfo(pAr, true);
		CurrentDAqInfoA::Save(static_cast<std::basic_string<CurrentDAqInfoA::type_char>>(pAr->GetFile()->GetFilePath()), true);


		CString csDAqInfo = DAqGetInformationString();
		CStringArray csSourceCodeCPPList;

		*pui64LMUserHeaderLength = sizeof(unsigned __int64)*3 
			+ sizeof(__int32)*22 
			+ sizeof(double)*2
			+ csDAqInfo.GetLength();	// user defined header

		// handle CString array
		*pui64LMUserHeaderLength += sizeof(unsigned __int32);					// add for number of source strings
		CStringArray *pcsArray = new CStringArray();							// create CString array
		unsigned __int32 ui32NumberOfSourceStrings = 0;
		// mainly for source code saving
		if((ui32LMHeaderVersion & DAQ_SOURCE_CODE) == DAQ_SOURCE_CODE)			// upper bit set indicates DAq Source should be written
		{																		// so prepare the string array
			CString dllName = DAqGetDllPathName();								// get the loaded dll pathname
			for(__int32 ui32Count=0;ui32Count < pCodeInformation->GetCount(); ui32Count++)
			{																	// even index = FileName, odd index = Code
				pcsArray->Add(pCodeInformation->GetFileNameAt(ui32Count));		// append the FileName information
				//pcsArray->Add(csDAqSourceCode.GetAt(ui32Count));				// append CStrings -> DAq Source Code
				pcsArray->Add(pCodeInformation->GetSourceCode(
					dllName,
					pCodeInformation->GetIDAt(ui32Count),
					pCodeInformation->GetTypeAt(ui32Count)));					// append CStrings -> DAq Source Code
			}
			ui32NumberOfSourceStrings = (unsigned __int32)pcsArray->GetCount();					// add length of Array as __int32
			for(unsigned __int32 ui32Count=0;ui32Count<ui32NumberOfSourceStrings; ui32Count++)	// now increase LMUserHeaderLength by added string information 
				*pui64LMUserHeaderLength += sizeof(unsigned __int32) + pcsArray->GetAt(ui32Count).GetLength();	// length of each string in the array + string length
		}

		TRY
			*pAr << (unsigned __int32)ui32LMHeaderVersion;						// LMF file version ID
			*pAr << (unsigned __int64)*pui64LMUserHeaderLength;						// actual header length always 1st value
			*pAr << (__int32)ui32DAqVersion;									// Version is always 2nd value
			*pAr << (__int32)DAQ_ID_TDC8;										// DAQ_ID is always 3ed value
			*pAr << Parameter[5];												// frequency is always 4th value
			*pAr << (__int32)0;													// IO address (parameter 1) always 5th value
			*pAr << RoundToNearestInt32(Parameter[2]);							// TimeInfo (parameter 2) always 6th value
			*pAr << (__int32)csDAqInfo.GetLength();								// Length of DAqInfo always 7th value
			pAr->Write(LPCTSTR(csDAqInfo),csDAqInfo.GetLength());				// DAqInfo always 8th value
			*pAr << (__int32)ui32LMFVersion;									// LMFVersion always 9th value
	
			*pAr << ui32NumberOfSourceStrings;
			for(unsigned __int32 ui32Count=0;ui32Count<ui32NumberOfSourceStrings; ui32Count++)	// now add the CString array
			{
				*pAr << (unsigned __int32)pcsArray->GetAt(ui32Count).GetLength();				// write DAqSourceCode to header
				pAr->Write(LPCTSTR(pcsArray->GetAt(ui32Count)),pcsArray->GetAt(ui32Count).GetLength());
			}
			delete pcsArray;
			pcsArray = nullptr;

			*pAr << RoundToNearestInt32(Parameter[3]);
			*pAr << RoundToNearestInt32(Parameter[7]);
			*pAr << RoundToNearestInt32(Parameter[14]);
			*pAr << Parameter[20];
			*pAr << RoundToNearestInt32(Parameter[21]);
			*pAr << RoundToNearestInt32(Parameter[30]);
			*pAr << RoundToNearestUInt64(Parameter[32]);
			*pAr << RoundToNearestUInt64(Parameter[33]);
			*pAr << RoundToNearestInt32(Parameter[40]);															// DataFormat
			//*pAr << (__int32)(Parameter[40] + (0.1 * ((Parameter[40] > 0) - (Parameter[40] < 0))));	// p[39] + (0.1*sign(p[39])
			*pAr << (__int32)false;												// indicator for 2nd module data
			*pAr << RoundToNearestInt32(Parameter[45]);
			*pAr << RoundToNearestInt32(Parameter[46]);
			*pAr << RoundToNearestInt32(Parameter[47]);
			*pAr << RoundToNearestInt32(Parameter[48]);
			*pAr << RoundToNearestInt32(Parameter[49]);
			*pAr << RoundToNearestInt32(Parameter[55]);
			*pAr << RoundToNearestInt32(Parameter[56]);
			*pAr << RoundToNearestInt32(Parameter[53]);
		CATCH( CFileException, e )
			return false;
		AND_CATCH( CArchiveException, e )
			return false;
		END_CATCH
	}
	else
		*pui64LMUserHeaderLength = 0;											// no user defined header
	return true;
#endif
}

/////////////////////////////////////////////////////////////////////////////
// Cobold Online function to read an Event
//	(file support here!)
//	pAr is the CArchive to access the opened LMFile. (can be nullptr)
//	EventData is the event array of CoboldPC in double
//	Parameter is the parameter array of CoboldPC in double
__time64_t t64LastGoodEvent = 0;
CDAQ_API __int32 DAqOnlineReadEvent(CArchive *pAr, CDoubleArray &EventData, CDoubleArray &Parameter, LMFPreEventData &preEventData)
{
#ifdef WIN64
	return ErrorMessageRunningOnX64OS("DAqOnlineReadEvent");
#else
	// this function can not run on x64 OS!!!
	// following two line uncommented due to runtime speed
	// if(IsProcess64())
	//	return ErrorMessageRunningOnX64OS("DAqOnlineReadEvent");

	unsigned __int32 myStat = iPCIIoAdr[TDCSTATUS];
	
	if(bIsFirstReadCall)								// if it's the first call in the loop then
	{
		ctStartTime = CTime::GetCurrentTime();			// get the current time as start time
		bIsFirstReadCall = false;						// reset flag
	}

	if(i32NumberOfDAqLoops == -1) 
		return 1;

	__int32 NumberOfDAqLoopsCounter;
	unsigned __int16 N[MAXIMUM_NUMBER_OF_CHANNELS];

	ProcessDAqFlushing(pAr);																// give process the possibility to flush event data

	for(NumberOfDAqLoopsCounter=0;NumberOfDAqLoopsCounter < i32NumberOfDAqLoops; ++NumberOfDAqLoopsCounter)
	{
		if(ShouldFinishTakingData())															// test if DAq should finish data taking
			break;	

		__time64_t _actTime;
	   _time64( &_actTime );
	   __time64_t diffTime;
	   diffTime = _actTime - t64LastGoodEvent;

		if(diffTime > i32SystemTimeOut)
		{
			if(NumberOfDAqLoopsCounter == 0)
			{
				ResetTDC(iPCIIoAdr, bEmptyEvents, bFallingEdge, bRisingEdge);
			}
			t64LastGoodEvent = _actTime;
			return -(__int32)(true);
		}

		int StatusFlag;
		StatusFlag = PCIGetTDC_B(Parameter,TDC,iPCIIoAdr);

		if(!StatusFlag)
			if(NumberOfDAqLoopsCounter == 0)
				return -(__int32)(true);
			else
				break;

		// reset timeout counter
		t64LastGoodEvent = _actTime;

		// get TimeStamp information
		QueryPerformanceCounter(&preEventData.TimeStamp);

		// increase EventCounter
		i64EventCounter += 1;

		myStat = iPCIIoAdr[TDCSTATUS];

		// verify n and create N Array
		memset(N,0,sizeof(__int16)*MAXIMUM_NUMBER_OF_CHANNELS);
		for(unsigned __int64 iCount=0;iCount<ui64NumberOfChannels;iCount++)
		{
			unsigned __int16 n = TDC[iCount][0];									// get number of hits for this channel
			N[iCount] = n > (unsigned __int16)ui64NumberOfHits ? (unsigned __int16)ui64NumberOfHits : n;	// cut down n if necessary to maximum of selected hits
		}

		// EventLength information in 64Bit
		unsigned __int64 HeaderLength = ui32TimeStamp + 4;								// +2 for HeaderLength itself, +2 for EventCounter
		HeaderLength <<= 1;															// now in sizeof int16
		for(unsigned __int64 iCount=0;iCount<ui64NumberOfChannels;iCount++)			// add one for every TDC data word that will be written
			HeaderLength += N[iCount];
		HeaderLength += ui64NumberOfChannels;										// add N array size (unsigned __int16)
		HeaderLength <<= 1;															// now in sizeof byte
		HeaderLength = (HeaderLength & 0x00ffffffffffffff) | 0xff00000000000000;	// set 0xff in bits 63..56 as EventMarker

		// transfer header info to preEventData
		preEventData.hdr1 = HeaderLength;
		preEventData.hdr2 = i64EventCounter;

		// writing List Mode Data
		if(pAr)
		{
			TRY
				*pAr << HeaderLength;	
				*pAr << i64EventCounter;

				// write time stamp info (1 or 2 * 32Bit)
				if(ui32TimeStamp == 1)
					*pAr << preEventData.TimeStamp.LowPart;							// 32 Bit Low part
				if(ui32TimeStamp == 2)
					*pAr << preEventData.TimeStamp.QuadPart;						// 64 Bit quad part

				for(unsigned __int64 iCount=0;iCount<ui64NumberOfChannels;iCount++)
				{
					*pAr << N[iCount];												// store hits for this channel
					for(__int32 nCount=1;nCount<=(__int32)N[iCount];nCount++)		// transfer selected hits
						*pAr << TDC[iCount][nCount];
				}
			CATCH_ALL( e )
				return false;
			END_CATCH_ALL
		}

		// and now the TDC data (last data block if i32NumberOfDAqLoops > 1)
		for(unsigned __int64 iCount=0;iCount<ui64NumberOfChannels;iCount++)
		{
			EventData[(unsigned __int32)((iCount * (ui64NumberOfHits+1)))] = (double)N[iCount];
			for(__int32 nCount=1;nCount<=(__int32)N[iCount];nCount++)
				EventData[(unsigned __int32)(nCount + (iCount * (ui64NumberOfHits+1)))] = TDC[iCount][nCount];
		}

	}

	return NumberOfDAqLoopsCounter;
#endif
}

/////////////////////////////////////////////////////////////////////////////
// Cobold Online function DAqOnlineFinalize
//	(file support here!)
//	pAr is the CArchive to access the opened LMFile. (can be nullptr)
//	Parameter is the parameter array of CoboldPC in double
//  returns true if successful
CDAQ_API bool DAqOnlineFinalize(CArchive *pAr, CDoubleArray &Parameter)
{
#ifdef WIN64
	return ErrorMessageRunningOnX64OS("DAqOnlineFinalize");
#else
	if(IsProcess64())
		return ErrorMessageRunningOnX64OS("DAqOnlineFinalize");

	// user define part to finish writing information
	// and declaring additional header information
	// (rewriting of predefined area in user header)

	if(pAr) 
		pAr->Flush();

	UserFinalize();

	return true;
#endif
}

/////////////////////////////////////////////////////////////////////////////
// Cobold Offline function DAqLMFReadUserHeader
// (file support here!)
//	pAr is the CArchive to access the opened LMFile. (can be nullptr)
//	Parameter is the parameter array of CoboldPC in double
//	LMUserHeaderLength is the length of the additional user define header
//					   this has to be read here if != 0!
//  returns true if successful
CDAQ_API bool DAqLMFReadUserHeader(CArchive *pAr, CDoubleArray &Parameter, NEWInformation newInfo, unsigned __int64 LMUserHeaderLength)
{
	// user define part to prepare to read an event 

	bIsEventMode = newInfo.bIsEventMode;
	bIsFirstReadCall = true;
	bIsTerminating = false;
	if(bIsEventMode)
	{
		i64NumberofEventsToAcquire = newInfo.maxEvents;
	}
	else
	{
		ctsTimeToAcquire = newInfo.maxTime;
	}

	i64EventCounter = 0;

	// read these internal parameters to be available for offline analysis
	if(pAr)
	{
		// store the actual LMFName		// get the length of the main header. It is the actual file position here!
		pAr->Flush();
		CFile *pf = pAr->GetFile();

		__int64 LMFMAINHEADER = pf->GetPosition();

		DAqSetLastLMFName(pAr);


		
		//@±±Ålmfwb_īńšOt@CÖLq
		//LAES2::SaveCurrentDAqInfo(pAr, false);
		auto cdiret = CurrentDAqInfoA::Save(static_cast<std::basic_string<CurrentDAqInfoA::type_char>>(pAr->GetFile()->GetFilePath()), false);
		


		if(LMUserHeaderLength > 4)
		{
			char cDummy;
			__int32 iDummy, iDummy2;
			unsigned __int64 ui64Dummy;
			double dDummy;
			bool bSkipFlag;

			TRY
				if((ui32LMHeaderVersion & MASK_LMF_FILEVERSION) == LM_FILE_VERSION9)
				{
					*pAr >> iDummy;											// LM_HEADER_VERSION(9)
					*pAr >> ui64Dummy;										// HeaderLength, always 1st value
				}
				else if((ui32LMHeaderVersion & MASK_LMF_FILEVERSION) == LM_FILE_VERSION)
				{
					*pAr >> iDummy;											// HeaderLength, always 1st value
					ui64Dummy = iDummy;
				}
				else
				{
					AfxMessageBox("LMF header size is not compatible\nwith this DAq module!",MB_ICONERROR);
					return false;
				}

				if(ui64Dummy != LMUserHeaderLength)							// test header length
				{
					AfxMessageBox("LMF header size is not compatible\nwith this DAq module!",MB_ICONERROR);
					return false;
				}

				*pAr >> ui32DAqVersion;										// DAqVersion, always 2nd value
				if((ui32DAqVersion != DAQ_VERSION5) && (ui32DAqVersion != DAQ_VERSION6) && (ui32DAqVersion != DAQ_VERSION7) && (ui32DAqVersion != DAQ_VERSION10))
				{
					CString MsgText;
					MsgText.Format("LMF DAq-Version (%d) is not compatible\nwith this DAq module (DAq-ID %d)!",ui32DAqVersion,DAQ_ID_TDC8);
					MsgText += "\n\nYou may ignore this on your own risk!\n\nTo Ignore press YES";
					int iAnswer = AfxMessageBox(MsgText,MB_ICONQUESTION|MB_YESNO);
					if(iAnswer == IDNO)
						return false;
				}
				Parameter[6] = ui32DAqVersion;

				*pAr >> iDummy;												// DAQ_ID is always 3ed value (Parameter 8)
				Parameter[8] = iDummy;
				if(iDummy != DAQ_ID_TDC8)
				{
					// indicate DAQ_ID error
					CString MsgText;
					MsgText.Format("LMF DAq-ID (%d) is not compatible\nwith this DAq module (DAq-ID %d)!",iDummy,DAQ_ID_TDC8);
					MsgText += "\n\nYou may ignore this on your own risk!\n\nTo Ignore press YES";
					int iAnswer = AfxMessageBox(MsgText,MB_ICONQUESTION|MB_YESNO);
					if(iAnswer == IDNO)
						return false;
				}

				*pAr >> Parameter[5];										// frequency is always 4th value
				*pAr >> iDummy;												// IO address is always 5th value	
				Parameter[1] = iDummy;
				*pAr >> ui32TimeStamp;											// TimeInfo is always 6th value
				Parameter[2] = ui32TimeStamp;
				__int32 _iDAqInfoLength;
				*pAr >> _iDAqInfoLength;										// Length of DAqInfo is always 7th value
				for(__int32 iCount=0;iCount<_iDAqInfoLength;iCount++)			// DAqInfo string is always 8th value
					pAr->Read(&cDummy,1);

				// now test for old file format (missing LMFVersion)
				*pAr >> iDummy;
				*pAr >> iDummy2;

				if((iDummy2 < 0x100) && (ui32DAqVersion >= DAQ_VERSION4))
				{
					Parameter[9] = iDummy;										// LMFVersion always 9th value
					ui32LMFVersion = iDummy;
					if(ui32LMFVersion >= LMF_VERSION10)							// handle new CStringArray information
					{
						unsigned __int32 ui32CStringCount = iDummy2;			// # of defined CStrings

						for(unsigned __int32 ui32Count=0;ui32Count<ui32CStringCount;ui32Count++)	// now read every CString
						{
							*pAr >> iDummy;										// how many characters to read
							for(unsigned __int32 ui32Count2=0;ui32Count2<(unsigned int)iDummy;ui32Count2++)		// read character by character
								pAr->Read(&cDummy,1);							// skip over DAq-source code
						}
						*pAr >> iDummy2;
					}

					Parameter[3] = iDummy2;
					*pAr >> iDummy;
					Parameter[7] = iDummy;
					bSkipFlag = false;
				}
				else
				{
					// source code information not available in this case
					Parameter[3] = iDummy;
					Parameter[7] = iDummy2;
					bSkipFlag = true;
				}

				*pAr >> iDummy;
				Parameter[14] = iDummy;
				*pAr >> dDummy;
				Parameter[20] = dDummy;
				if(!bSkipFlag)
				{
					*pAr >> iDummy;
					Parameter[21] = iDummy;
				}
				*pAr >> iDummy;
				Parameter[30] = iDummy;
				if((ui32LMHeaderVersion & MASK_LMF_FILEVERSION) == LM_FILE_VERSION9)
				{
					*pAr >> ui64NumberOfChannels;							// # of channels
					Parameter[32] = (double)ui64NumberOfChannels;
					*pAr >> ui64NumberOfHits;								// # of maxhits
					Parameter[33] = (double)ui64NumberOfHits;				
				}
				else
				{
					unsigned __int32 uiDummy;
					*pAr >> uiDummy;
					ui64NumberOfChannels = uiDummy;
					Parameter[32] = (double)ui64NumberOfChannels;
					*pAr >> uiDummy;
					ui64NumberOfHits = uiDummy;
					Parameter[33] = (double)ui64NumberOfHits;
				}
				*pAr >> iDummy;
				Parameter[40] = iDummy;
				*pAr >> iDummy;											// indicator for 2nd module data
				if(!bSkipFlag)
				{
					*pAr >> iDummy;
					Parameter[45] = iDummy;
					*pAr >> iDummy;
					Parameter[46] = iDummy;
					*pAr >> iDummy;
					Parameter[47] = iDummy;
					*pAr >> iDummy;
					Parameter[48] = iDummy;
					*pAr >> iDummy;
					Parameter[49] = iDummy;
					*pAr >> iDummy;
					Parameter[55] = iDummy;
					*pAr >> iDummy;
					Parameter[56] = iDummy;
				}
				if(ui32LMFVersion >= LMF_VERSION10)
				{
					*pAr >> iDummy;
					Parameter[53] = iDummy;
				}
				pAr->Flush();
				LONGLONG mylen = pf->GetPosition();
				mylen *= 1;
				if(ui32LMFVersion < LMF_VERSION10)
				{
					// for old LMF pre version 10
					// no matter what... seek to the start of the list mode eventdata
					// that is beginning at LMFMAINHEADER + LMUserHeader
					pAr->Flush();
					pf->Seek(LMFMAINHEADER+LMUserHeaderLength,CFile::begin);
					pAr->Flush();
				}

			CATCH( CArchiveException, e )
				// indicate read file error
				__int32 iAnswer = AfxMessageBox("LMF UserHeader could not be read\nwithout errors.\n\nYou may ignore this on your own risk!\n\nTo Ignore press YES",MB_ICONQUESTION|MB_YESNO);
				if(iAnswer == IDYES)
					return true;
				else
					return false;
			END_CATCH
		}
		else
		{
			// indicate LMF size error
			__int32 iAnswer = AfxMessageBox("LMF UserHeader size is not compatible\nwith this DAq module!\n\nYou may ignore this on your own risk!\n\nTo Ignore press YES",MB_ICONQUESTION|MB_YESNO);
			if(iAnswer == IDYES)
				return true;
			else
				return false;
		}
	}

	if(!UserInit(Parameter)) 
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Cobold Offline function DAqOfflineReadEvent
//	(file support here!)
//	pAr is the CArchive to access the opened LMFile. (can be nullptr)
//	EventData is the event array of CoboldPC in double
//	Parameter is the parameter array of CoboldPC in double
//	return TRUE if data is ok
//	return FALSE to indicate an error
CDAQ_API __int32 DAqOfflineReadEvent(CArchive *pAr, CDoubleArray &EventData,CDoubleArray &Parameter, LMFPreEventData &preEventData)
{
	// 1st: read data from pAr if != nullptr

	// 2nd: transfer data to EventData array

	if(pAr)
	{
		TRY
			unsigned __int32 index = 0;
			unsigned __int32 ui32Data = 0;
			unsigned __int16 usData = 0;

			unsigned __int64 HeaderLength;
			//    read EventHeader
			*pAr >> HeaderLength;
			if((HeaderLength & 0xff00000000000000) != 0xff00000000000000)
			{
				CString msg;
				msg.Format("Wrong information in header of event\n   %I64i!\nThe file is damaged.",i64EventCounter); 
				AfxMessageBox(msg,MB_ICONERROR);
				return false;
			}
			preEventData.hdr1 = HeaderLength & 0x00ffffffffffffff;

			//    process EventCounter (__int64)
			*pAr >> i64EventCounter;
			preEventData.hdr2 = i64EventCounter;

			//    read time stamp info
			preEventData.TimeStamp.QuadPart = 0;
			if(ui32TimeStamp == 1)
				*pAr >> preEventData.TimeStamp.LowPart;		// 32 Bit Low part
			if(ui32TimeStamp == 2)
				*pAr >> preEventData.TimeStamp.QuadPart;	// if selected add 64 Bit quad part

			// now read the data
			for(__int32 iCount=0;iCount<ui64NumberOfChannels;iCount++)
			{
				unsigned __int16 n;							// = TDC[iCount][0];
				*pAr >> n;
				EventData[(unsigned __int32)((iCount * (ui64NumberOfHits+1)))] = (double)n;
				for(int nCount=1;nCount<=(__int32)n;nCount++)
				{
					*pAr >> usData;
					EventData[(unsigned __int32)(iCount*(ui64NumberOfHits+1) + nCount)] = usData;
				}
			}
		CATCH_ALL( e )
			return false;
		END_CATCH_ALL
	}
	ShouldFinishTakingData();													// test if DAq should finish data taking and set exit flag
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Cobold Online function DAqOfflineFinalize
//	(file support here!)
//	pAr is the CArchive to access the opened LMFile. (can be nullptr)
//	Parameter is the parameter array of CoboldPC in double
//	return true if successful
CDAQ_API bool DAqOfflineFinalize(CArchive *pAr, CDoubleArray &Parameter)
{
	UserFinalize();

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Helper function UserInit
//	returns information if UserInit was successful (true if read was successful)
bool UserInit(CDoubleArray &Parameter)
{
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Helper function UserFinalize
void UserFinalize() 
{
}

/////////////////////////////////////////////////////////////////////////////
// Helper function PCIGetTDC_B
//	returns information if read was successful (true if read was successful)
__int32 PCIGetTDC_B(CDoubleArray &Parameter,unsigned __int16 TDC[MAXIMUM_NUMBER_OF_CHANNELS][MAXIMUM_NUMBER_OF_HITS+1], unsigned __int32 *ui32pPCIIoAdr)
{
	unsigned __int16 _TDC[MAXIMUM_NUMBER_OF_CHANNELS][MAXIMUM_NUMBER_OF_HITS+1];
	unsigned __int32 *pui32FIFOGet = &ui32pPCIIoAdr[FIFOGET];

	// now process the data
	bool bOneTrueDataWord = false;
	bool bFirstLoop = true;
	do
	{
		// get the data
		ui32FIFOData = *pui32FIFOGet;
		if((ui32FIFOData & 0x80000000))
		{
			if(!bOneTrueDataWord)
			{
				// ResetTDC(uipPCIIoAdr,bEmptyEvents,bFallingEdge,bRisingEdge);
				memset(TDC,0,sizeof(_TDC));		// clear TDC array
			}
			return false;
		}

		if(bFirstLoop)
		{
			memset(_TDC,0,sizeof(_TDC));		// clear _TDC array
			memset(TDC,0,sizeof(_TDC));			// clear TDC array
			bFirstLoop = false;
		}

		bool bEmptyEvent = ui32FIFOData & 0x20000000 ? true : false;
		ui16TDCData = ui32FIFOData & 0x0000ffff;
		ucTDCChannel = *pucTDCChannel & 0x07;
		if(!bEmptyEvent)
		{
			if(ucTDCChannel < MAXIMUM_NUMBER_OF_CHANNELS)
			{
				// increase Hit Counter;
				_TDC[ucTDCChannel][0]++;

				// validate hit counter range 
				if(_TDC[ucTDCChannel][0] <= MAXIMUM_NUMBER_OF_HITS)
				{
					// if Hit # ok then store it
					_TDC[ucTDCChannel][_TDC[ucTDCChannel][0]] = ui16TDCData;
					bOneTrueDataWord = true;
				}
			}
		}
		else
		{
			// if first read data word has no data then
			// signal no event!
			if(bEmptyEvents)
			{
				return true;
			}
		}
	}
	while (!(ui32FIFOData & 0x08000000));		// read as long as EventToggleFlag doesn't change

	if(!bOneTrueDataWord)
		return false;

	// now transfer the Data from _TDC to TDC
	// physical first time sorted first in array!

	for(__int32 iCount=0;iCount < MAXIMUM_NUMBER_OF_CHANNELS;iCount++)
		TDC[iCount][0] = _TDC[iCount][0];
	for(__int32 iCountCH=0;iCountCH < MAXIMUM_NUMBER_OF_CHANNELS;iCountCH++)
	{
		__int32 _iHits = _TDC[iCountCH][0];
		for(__int32 iCountH=0;iCountH < _iHits;iCountH++)
			TDC[iCountCH][iCountH+1] = _TDC[iCountCH][_iHits - iCountH];
	}

	__int32 _i32Counter = ui32pPCIIoAdr[TDCCLEARREADEMPTYCOUNT];

	// do Counter parameters
	Parameter[55] = Parameter[55] + (double)_i32Counter;
	Parameter[56] = _i32Counter;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Helper function ResetTDC
void ResetTDC(unsigned __int32 *uipPCIIOAdr, bool &bEmptyEventsX, bool &bFallingEdgeX, bool &bRisingEdgeX)
{
	////////////////////////
	/// TDC8 Stuff
	////////////////////////
	// config, reset, enable
	uipPCIIOAdr[TDCSTATUS] = 0x00000000;													// reset and disable
	uipPCIIOAdr[TDCSTATUS] = 0x80000000;													// reset and disable

	// Common start acquisition gate
	// bit 00 - 15 = gate open time  *33ns = gate open time[ns]
	// bit 16 - 23 = gate delay      *33ns+150ns = gate delay[ns]
	uipPCIIOAdr[TDCCOMMONSTARTGATE] = i32GateOpen + (i32GateDelay << 16);

	uipPCIIOAdr[TDCINFO]   = 0; //_iNumberOfHits;											// info sets the #of Hits (1...16)
	uipPCIIOAdr[TDCRANGE]  = i32EventOpenTime*10000/5;										// µs*1e6/500
	__int32 iData =
		1 | // TDC enable
		(((0) & 0x00000001) << 1) |															// iPCICardMode = 0 -> Single Card Mode
		(((bEmptyEventsX)&0x00000001) << 2) |
		(((!i32TriggerModeCommon)&0x00000001) << 8) |
		(((bFallingEdgeX)&0x00000001) << 10) |
		(((bRisingEdgeX)&0x00000001) << 9);
	uipPCIIOAdr[TDCSTATUS] = iData;
}

bool DAqGetOnlineParameters(CDoubleArray &Parameter)
{
#ifdef WIN64
	ErrorMessageRunningOnX64OS("DAqInitialize");
	return true;
#else
	dResolution = 0.500;						// reolution of TDC8 is 500ps
	Parameter[20] = dResolution;

	HighResTimerInit();
	// get computers frequency (ticks per second)
	Parameter[5] = HighResTimerFrequency();
	return false;
#endif
}

bool DAqGetOfflineParameters(CArchive *pAr, CDoubleArray &Parameter, unsigned __int32 LMHeaderVersion, unsigned __int64 LMUserHeaderLength)
{
	HighResTimerInit();

	// read these internal parameters to be available for offline analysis
	if(pAr)
	{
		if(LMUserHeaderLength > 4)
		{
			__int32 i32Dummy;
			unsigned __int64 ui64Dummy;
			CString csDummy;

			TRY
			{
				if((LMHeaderVersion & MASK_LMF_FILEVERSION) == LM_FILE_VERSION9)
				{
					*pAr >> i32Dummy;													// LM_HEADER_VERSION(9)
					*pAr >> ui64Dummy;													// HeaderLength, always 1st value
				}
				else if((LMHeaderVersion & MASK_LMF_FILEVERSION) == LM_FILE_VERSION)
				{
					// no ui32LMHeaderVersion!!
					*pAr >> i32Dummy;													// HeaderLength, always 1st value
					ui64Dummy = i32Dummy;
				}
				else
				{
					//AfxMessageBox(CString("LMF header size is not compatible\n") + 
					//	CString("with this DAq module!"),
					//	MB_ICONERROR);
					return true;
				}
				if(ui64Dummy != LMUserHeaderLength)										// test header length
				{
					//AfxMessageBox(CString("LMF header size is not compatible\n") +
					//	CString("with this DAq module!"),
					//	MB_ICONERROR);
					return true;
				}

				*pAr >> ui32DAqVersion;													// DAqVersion, always 2nd value
				if((ui32DAqVersion != DAQ_VERSION5) && (ui32DAqVersion != DAQ_VERSION6) && (ui32DAqVersion != DAQ_VERSION7) && (ui32DAqVersion != DAQ_VERSION10))
				{
					//CString MsgText;
					//MsgText.Format(CString("LMF DAq-Version (%d) is not compatible\n") + 
					//	CString("with this DAq module!\n") +
					//	CString("You may ignore this on your own risk!\n") +
					//	CString("\n") + 
					//	CString("To Ignore press YES"),
					//	ui32DAqVersion);
					//__int32 iAnswer = AfxMessageBox(MsgText,MB_ICONQUESTION|MB_YESNO);
					//if(iAnswer == IDNO)
					//	return false;
					return true;
				}

				*pAr >> ui32DAq_ID;														// ui32DAq_ID is always 3rd value (Parameter 8)
				if(ui32DAq_ID != DAQ_ID_TDC8)
				{
					//CString MsgText;
					//MsgText.Format(CString("LMF DAq-ID (%d) is not compatible\n") + 
					//	CString("with this DAq module!\n") + 
					//	CString("You may ignore this on your own risk!\n") +
					//	CString("\n") + 
					//	CString("To Ignore press YES"),
					//	ui32DAq_ID);
					//__int32 iAnswer = AfxMessageBox(MsgText,MB_ICONQUESTION|MB_YESNO);
					//if(iAnswer == IDNO)
					//	return false;
					return true;
				}

				*pAr >> Parameter[5];													// timestamp frequency is always 4th value
				dResolution = 0.500;						// resolution of TDC8 is 500ps
				Parameter[20] = dResolution;
			}
			CATCH( CArchiveException, e )
				return true;
			END_CATCH
		}
		else
		{
			// indicate error
			// user defined header is not right in size
			//AfxMessageBox("LMF UserHeader size is not compatible\nwith this DAq module!",MB_ICONERROR);
			return true;
		}
	}
	return false;
}  ¶  <   I D S _ S O U R C E _ H   C D A Q       0           // CDAq.h : main header file for the CDAq DLL
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#ifndef _DEBUG
extern "C" IMAGE_DOS_HEADER __ImageBase;
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef CDAQ_EXPORTS
#define CDAQ_API __declspec(dllexport)
#else
#define CDAQ_API __declspec(dllimport)
#endif

#define INCLUDE_HARDWARE_SUPPORT

// definitions for main program event data format
#define LM_BYTE					1	//  8bit integer
#define LM_SHORT				2	// 16bit integer
#define LM_LONG					3	// 32bit integer
#define	LM_FLOAT				4   // 32bit IEEE float
#define LM_DOUBLE				5	// 64bit IEEE float
#define LM_CAMAC				6	// 24bit integer
#define LM_DOUBLELONG			7	// 64bit integer
#define LM_SBYTE				8	// signed 8bit integer
#define LM_SSHORT				9	// signed 16bit integer
#define LM_SLONG				10	// signed 32bit integer
#define LM_SDOUBLELONG			11	// signed 64bit integer
#define LM_LASTKNOWNDATAFORMAT	LM_SDOUBLELONG
#define LM_USERDEF				-1	// user will handle the reading 
									// of file and filling of EventData Array
									// except reading the main header file

class LMFPreEventData;
class CDAqApp;

extern CDAqApp theDAqApp;
extern CString &DAqGetDllPathName();
extern bool ShouldFinishTakingData();
extern bool bIsEventMode;
extern __int64 i64EventCounter;
extern __int64 i64NumberofEventsToAcquire;
extern CTimeSpan ctsTimeToAcquire;
extern CTime ctStartTime;
extern bool bIsFirstReadCall;
extern bool bIsTerminating;

CDAQ_API CSourceCodeInformation* DAqGetSourceCodeInformation();

CDAQ_API bool DAqIsTerminating();
CDAQ_API LPCTSTR DAqGetInformationString();
CDAQ_API LPCTSTR DAqGetLastLMFName(CArchive *pAr = nullptr);	// if pAr != nullptr then get the function Name 
																//    (call only in DAqLMFWriteUserHeader or DAqLMFReadUserHeader
																// else get last stored value
CDAQ_API CTime *DAqGetLastStartTime();
CDAQ_API void DAqSetLMHeaderVersion(unsigned __int32 _ui32LMHeaderVersion);
CDAQ_API bool DAqGetParameters(CArchive *pAr, CDoubleArray &Parameter, unsigned __int32 LMHeaderVersion, unsigned __int64 LMUserHeaderLength);

CDAQ_API __int64 DAqInitialize(CDoubleArray &Parameter, unsigned __int32 *pDataFormat);
CDAQ_API bool DAqFinalize(CDoubleArray &Parameter);

CDAQ_API bool DAqLMFWriteUserHeader(CArchive *pAr, CDoubleArray &Parameter, NEWInformation newInfo, unsigned __int64 *pLMUserHeaderLength = nullptr);
CDAQ_API bool DAqLMFReadUserHeader(CArchive *pAr, CDoubleArray &Parameter, NEWInformation newInfo, unsigned __int64 LMUserHeaderLength);

CDAQ_API bool DAqOnlineFinalize(CArchive *pAr, CDoubleArray &Parameter);
CDAQ_API bool DAqOfflineFinalize(CArchive *pAr, CDoubleArray &Parameter);

CDAQ_API __int32 DAqOnlineReadEvent(CArchive *pAr, CDoubleArray &EventData, CDoubleArray &Parameter, LMFPreEventData &preEventData);
CDAQ_API __int32 DAqOfflineReadEvent(CArchive *pAr, CDoubleArray &EventData, CDoubleArray &Parameter, LMFPreEventData &preEventData);

/////////////////////////////////////////////////////////////////////////////
// CDAqApp
// See CDAq.cpp for the implementation of this class
//
class CDAqApp : public CWinApp
{
public:
	CDAqApp();
	int ExitInstance();
	BOOL InitInstance();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDAqApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CDAqApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
}
#endif

  ½  L   I D S _ S O U R C E _ H   C H E C K P R O C E S S       0           /******************************************************************************\
*       This is a part of the Microsoft Source Code Samples. 
*		  Copyright (C) 1994-1996 Microsoft Corporation.
*       All rights reserved. 
*       This source code is only intended as a supplement to 
*       Microsoft Development Tools and/or WinHelp documentation.
*       See these sources for detailed information regarding the 
*       Microsoft samples programs.
\******************************************************************************/

#define TITLE_SIZE          64
#define PROCESS_SIZE        MAX_PATH

//
// task list structure
//
typedef struct _TASK_LIST 
{
    DWORD       dwProcessId;
    DWORD       dwInheritedFromProcessId;
    BOOL        flags;
    HANDLE      hwnd;
    CHAR        ProcessName[PROCESS_SIZE];
    CHAR        WindowTitle[TITLE_SIZE];
} TASK_LIST, *PTASK_LIST;

typedef struct _TASK_LIST_ENUM 
{
    PTASK_LIST  tlist;
    DWORD       numtasks;
} TASK_LIST_ENUM, *PTASK_LIST_ENUM;

//
// Function pointer types for accessing platform-specific functions
//
typedef DWORD (*LPGetTaskList)(PTASK_LIST, DWORD);
typedef BOOL  (*LPEnableDebugPriv)(VOID);   7  T   I D S _ S O U R C E _ H   C O B O L D U T I L I T I E S         0           // CoboldUtilities.h : main header file for the COBOLDUTILITIES DLL
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#define COMPRESSIONTYPE_UNDEFINED	-1
#define COMPRESSIONTYPE_NONE		0
#define COMPRESSIONTYPE_GZIP		1
#define COMPRESSIONTYPE_DEFLATE		2

/////////////////////////////////////////////////////////////////////////////
// CCoboldUtiltiesApp
// See CoboldUtilities.cpp for the implementation of this class
//
#ifndef CCOBOLDUTILITIESAPP_DEF
#define CCOBOLDUTILITIESAPP_DEF

class CCoboldUtiltiesApp : public CWinApp
{
public:
	CCoboldUtiltiesApp();
	~CCoboldUtiltiesApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCoboldUtiltiesApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CCoboldUtiltiesApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#include "CheckProcess.h"

#ifdef COBOLDUTILITIES_EXPORTS
#define COBOLDUTILITIES_APP __declspec(dllexport)
#else
#define COBOLDUTILITIES_APP __declspec(dllimport)
#endif

/////////////////////////////////////////////////////////////////////////////
// ATARI Time structure
typedef struct 
{	
	unsigned short 		hsec	: 5;
	unsigned short 		min		: 6;
	unsigned short 		hour	: 5;
}ATime;

typedef struct 
{	
	unsigned short 		day		: 5;
	unsigned short 		month	: 4;
	unsigned short 		year	: 7;
}ADate;

typedef struct 
{
	ATime 		time;	
	ADate 		date;
}ADateTime;

typedef ULONGLONG (*dllGetTickCounts64)();

class COBOLDUTILITIES_APP CNumberTesting
{
public:
	// Construction
	CNumberTesting();
	~CNumberTesting();

	// Implementation
	bool IsNaN(double val);
	bool IsInfinity(double val);
	bool IsPositiveInfinity(double val);
	bool IsNevativeInfinity(double val);
	bool IsNaNorInfinity(double val);
};

class COBOLDUTILITIES_APP CFileIOHelper
{
public:
	// Construction
	CFileIOHelper(void);
	virtual ~CFileIOHelper(void);
protected:
	void *m_pFileIoHelper;
	CString m_csRet;
	
public:
	// Implementation
	CString& GetFileName(CString PathName);
	CString& GetFileName(CString PathName, bool &displayError);
	CString& GetFileNameWithoutExtension(CString PathName);
	CString& GetFileNameWithoutExtension(CString PathName, bool &displayError);
	CString& GetPath(CString PathName);
	CString& GetPath(CString PathName, bool &displayError);
	CString& GetExtension(CString PathName);
	CString& GetExtension(CString PathName, bool &displayError);
	bool FileExists(CString PathName);
	bool DirectoryExists(CString Path);
	bool DirectoryCreate(CString Path);
	void ShowInformationForm(CString Title, CString Data, bool holdFlag = true);
	void CloseInformationForm();
	CString& SearchFilename(CString baseName, CString extensionName, CString x86Extension, CString x64Extension, bool forceRightOS);
	CString& GetSpecialFolder(CString FolderName);
	bool FileCopy(CString To, CString From);
	bool IsReadOnly(CString PathName);
	bool FileDelete(CString PathName);
	bool CreateLockFile(CString PathName);
	bool AppendTextToLockFile(CString PathName, CString Text);
};

// This class is exported from the CoboldSecurity.dll
//template<class double, class double> 
class COBOLDUTILITIES_APP CDoubleArray : public CObject
{
public:
	DECLARE_SERIAL(CDoubleArray)

// Construction
	CDoubleArray();
	CDoubleArray(__int64 NumberOfElements);
	CDoubleArray(__int64 NumberOfXElements, __int64 NumberOfYElements);

// Attributes
	inline __int64 GetSize() const 
	{
		return m_nSize;
	}
	int GetUpperBound() const;
	void SetSize(__int64 nNewSize, __int64 nGrowBy = -1);
	void SetMatixSize(__int64 nNewXSize, __int64 nNewYSize, __int64 nGrowBy = -1);

// Operations
	// Clean up
	void FreeExtra();
	void RemoveAll();

	// Accessing elements
	inline double GetAt(__int64 nIndex) const 
		{ 
			ASSERT(nIndex >= 0 && nIndex < m_nSize);
			double val = m_pData[nIndex];
			return (!pNTesting->IsNaNorInfinity(val) ? val : 0.0);
	}
	inline void SetAt(__int64 nIndex, double newElement) 
	{ 
		ASSERT(nIndex >= 0 && nIndex < m_nSize); 
	    if(!pNTesting->IsNaNorInfinity(newElement))
			m_pData[nIndex] = newElement;
		else
			m_pData[nIndex] = 0.0;
	}
	inline double& ElementAt(__int64 nIndex) 
	{
		static double zero = 0.0;
		ASSERT(nIndex >=0 && nIndex< m_nSize); 
		static double val = m_pData[nIndex];
		return (!pNTesting->IsNaNorInfinity(val)) ? val : zero;
	}
	void Empty();

	// Potentially growing the array
	void SetAtGrow(__int64 nIndex, double newElement);
	__int64 Add(double newElement) 
	{
		__int64 nIndex = m_nSize; 
		if(!pNTesting->IsNaNorInfinity(newElement))
			SetAtGrow(nIndex,newElement);
		else
			SetAtGrow(nIndex,0.0);
		return nIndex;
	}

	// overloaded operator helpers
	virtual inline double operator[](__int64 nIndex) const 
	{
		return GetAt(nIndex);
	}
	virtual inline double& operator[](__int64 nIndex)	
	{
		return m_pData[nIndex];
	}
	virtual const CDoubleArray& operator = (const CDoubleArray& sData);

	// Operations that move elements around
	void InsertAt(__int64 nIndex, double newElement, __int64 nCount = 1);
	void RemoveAt(__int64 nIndex, __int64 nCount = 1);
	void InsertAt(__int64 nStartIndex, CDoubleArray* pNewArray);

	// Matrix operation
	void SetAtMatrix(__int64 posx, __int64 posy,double Value);
	double GetAtMatrix(__int64 posx, __int64 posy);
	inline __int64 GetSizeX() const 
	{
		return SizeX;
	}
	inline __int64 GetSizeY() const 
	{
		return SizeY;
	}

	// Expand operation
	bool CDoubleArray::ExpandTo(__int64 xchannels);
	bool CDoubleArray::ExpandTo(__int64 xchannels, __int64 ychannels);


protected:
	void AssignCopy(__int64 nSrcLen, double *Data);

// Implementation
public:
	double* m_pData;		// the actual array of data
	bool m_LoadError;
	
protected:
	__int32	m_VersionNumber;
	__int64 m_nSize;		// # of elements (upperBound - 1)
	__int64 m_nMaxSize;		// max allocated
	__int64 m_nGrowBy;		// grow amount

	__int64 SizeX;			// Size of Matrix in X
	__int64 SizeY;			// Size of Matrix in Y
	
	__int32 m_CompressionType;
	CNumberTesting *pNTesting;

public:
	~CDoubleArray();
	virtual void Serialize(CArchive& ar);

	void Dump(CDumpContext&) const;
	void AssertValid() const;
};

/////////////////////////////////////////////////////////////////////////////
// helper class CCodeInformation for CSourceCodeInformation
class COBOLDUTILITIES_APP CCodeInformation : public CObject
{
protected:
	CString	csCodeID;
	CString	csCodeType;
	CString	csCodeFileName;

public:
	// constructor and destructor
	CCodeInformation();
	CCodeInformation(CString CodeType, CString CodeID, CString CodeFileName);
	~CCodeInformation();

public:
	// implementation
	CString &GetType();
	CString &GetID();
	CString &GetFileName();
};

/////////////////////////////////////////////////////////////////////////////
// class to handle source code information of dlls
class COBOLDUTILITIES_APP CSourceCodeInformation
{
public:
	CObArray		coaCodeInformation;
	CString			csSourceCode;

public:
	// constructor and destructor
	CSourceCodeInformation();
	~CSourceCodeInformation();

public:
	// implementation
	INT_PTR GetCount();
	CString &GetSourceCode(CString csDllFilePathName, CString csSourceID, CString csSourceType);

	CString &GetTypeAt(unsigned __int32 ui32Index);
	CString &GetIDAt(unsigned __int32 ui32Index);
	CString &GetFileNameAt(unsigned __int32 ui32Index);
};

/////////////////////////////////////////////////////////////////////////////
// definition for exported functions
class CCoboldApp;
class CMainFrame;
class CCoboldDoc;

/////////////////////////////////////////////////////////////////////////////
// definition for exported functions

COBOLDUTILITIES_APP __int32 CheckDate(CString Module);

COBOLDUTILITIES_APP COLORREF GetTNGColor(CDC *pDC, __int32 ColorIndex);
COBOLDUTILITIES_APP COLORREF GetTNGScreenColor(__int32 ColorIndex);
COBOLDUTILITIES_APP COLORREF GetTNGPrintColor(__int32 ColorIndex);
COBOLDUTILITIES_APP void GetTNGDefaultColors(void);
COBOLDUTILITIES_APP void SetTNGScreenColor(__int32 ColorIndex, COLORREF Color);
COBOLDUTILITIES_APP void SetTNGPrintColor(__int32 ColorIndex, COLORREF Color);
COBOLDUTILITIES_APP void TogglePrintScreenColors(void);
COBOLDUTILITIES_APP void ReadColorDefinitions(void);
COBOLDUTILITIES_APP void ReadDelimiterDefinitions(void *pvView);
COBOLDUTILITIES_APP void WriteColorDefinitions(void);
COBOLDUTILITIES_APP void WriteDelimiterDefinitions(void *pvView);
COBOLDUTILITIES_APP CString Int64ToDelimitedCString(CString format, __int64 i64Value);
COBOLDUTILITIES_APP CString DoubleToDelimitedCString(double dValue);

// Conversion functions
COBOLDUTILITIES_APP double GetCStringNumber(CString cAddress);
COBOLDUTILITIES_APP void DoubleAtari(double register *var);
COBOLDUTILITIES_APP void LongAtari(unsigned long register *var);
COBOLDUTILITIES_APP void IntAtari(unsigned short register *var);
/// Conversions end

// myTime functions
extern COBOLDUTILITIES_APP dllGetTickCounts64 fGetTickCount64;
COBOLDUTILITIES_APP void ToAtariTime(CTime *pCTime, ADateTime *pATime);
COBOLDUTILITIES_APP void ToCTime(CTime *pCTime, ADateTime *pATime);
COBOLDUTILITIES_APP bool InitTickCount64Function();
/// myTime end

// PumpMessages functions
COBOLDUTILITIES_APP void PumpDlgMessages(CWnd *pWnd);
COBOLDUTILITIES_APP void PumpDlgMessagesNoIdle(CWnd *pWnd);
COBOLDUTILITIES_APP void PumpMessages(bool bIdle=true);
COBOLDUTILITIES_APP void PumpMessagesNoIdle();
/// PumpMessage end

COBOLDUTILITIES_APP bool ReadWindowPlacement(WINDOWPLACEMENT *pwp, UINT *puiFlags, char *pszSection, char *pszWindowFlag, char *pszWindowPos);
COBOLDUTILITIES_APP void WriteWindowPlacement(CWnd *pWnd, char *pszSection, char *pszWindowFlag, char *pszWindowPos);
COBOLDUTILITIES_APP bool TestFlatBarAvailable(void);

COBOLDUTILITIES_APP __int32 GetTaskListNT(PTASK_LIST  pTask, DWORD dwNumTasks);
COBOLDUTILITIES_APP BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);
COBOLDUTILITIES_APP __int32 CountApplications(CString VersionFileDescription);

COBOLDUTILITIES_APP __int32 FileNotFoundMessage(CStringArray *pActiveBatchFile, CString BatchName, bool bQuitMessage = false);

COBOLDUTILITIES_APP HRESULT GetComCtlVersion(LPDWORD pdwMajor, LPDWORD pdwMinor);

COBOLDUTILITIES_APP void DeleteLocalRegistryKey(HKEY myKey, char RegPath[MAX_PATH]);

COBOLDUTILITIES_APP void CorrectGP1DifferentialNonlinearity(CDoubleArray &EventData, CDoubleArray &Parameters, __int32 rawdata, __int32 iRes);

// Application functions
COBOLDUTILITIES_APP CString &GetApplicationPath();
COBOLDUTILITIES_APP CCoboldApp *GetCoboldApp();
/// Application end

COBOLDUTILITIES_APP CString GetSerialNumber();
COBOLDUTILITIES_APP __int32 GetSerialNumberKey();
COBOLDUTILITIES_APP bool IsSerialNumberOK();
COBOLDUTILITIES_APP bool IsSerialNumberHM1();
COBOLDUTILITIES_APP bool IsSerialNumberDemo();
COBOLDUTILITIES_APP bool IsDemoExpired(bool flag = false);
COBOLDUTILITIES_APP void maInit();

COBOLDUTILITIES_APP void OrthogonalToRPhi(double x,double y,double &r,double &phi);

COBOLDUTILITIES_APP void SetActualDAqPathName(CString csPathName);
COBOLDUTILITIES_APP void SetActualDAnPathName(CString csPathName);
COBOLDUTILITIES_APP void SetActualFCPathName(CString csPathName);
COBOLDUTILITIES_APP CString& GetActualDAqPathName();
COBOLDUTILITIES_APP CString& GetActualDAnPathName();
COBOLDUTILITIES_APP CString& GetActualFCPathName();

COBOLDUTILITIES_APP bool IsCoboldExecutingCommand();
COBOLDUTILITIES_APP void ExecuteCommand(CString csCommandLine);
COBOLDUTILITIES_APP CDoubleArray &GetDAqDAnParameters(CCoboldDoc *pDoc = nullptr);
COBOLDUTILITIES_APP CDoubleArray &GetDAqDAnWeighParameters(CCoboldDoc *pDoc = nullptr);
COBOLDUTILITIES_APP CDoubleArray &GetDAqDAnEventData(CCoboldDoc *pDoc = nullptr);
COBOLDUTILITIES_APP CCoboldDoc *GetDocumentPointer();

COBOLDUTILITIES_APP bool IsClearEventDataArraySet();
COBOLDUTILITIES_APP void SetClearEventDataArrayState(bool flag);

COBOLDUTILITIES_APP bool AreCompressCOMsAvailable(bool bInfoBox);

COBOLDUTILITIES_APP void DisplayLastError(LPTSTR lpszFunction);

COBOLDUTILITIES_APP bool IsWindows64();
COBOLDUTILITIES_APP bool IsProcess64();
COBOLDUTILITIES_APP bool IsVistaOrHigher();
COBOLDUTILITIES_APP bool ErrorMessageRunningOnX64OS(CString infoText);

COBOLDUTILITIES_APP CString& GetHID(CString csFilePath);
COBOLDUTILITIES_APP BOOL UpdateHID(CString csFilePath);

// Path functions
COBOLDUTILITIES_APP inline BOOL IsDirectorySeparator(TCHAR ch);
/// Path end

// LMF functions
COBOLDUTILITIES_APP bool IsLMFIdle();
COBOLDUTILITIES_APP bool IsDAqTerminating();
COBOLDUTILITIES_APP bool ProcessDAqFlushing(CArchive *pAr);
COBOLDUTILITIES_APP void SetInitialFlushTime(CTimeSpan ctsFlushTimeDelta);
COBOLDUTILITIES_APP CTimeSpan& GetInitialFlushTime();

// DAnDAqWizard
COBOLDUTILITIES_APP BOOL DAnDAqWizardShowForm();

// Config
extern COBOLDUTILITIES_APP void *pxmlProfile;
COBOLDUTILITIES_APP void *GetActualXMLProfile();

// RoundUtilities
COBOLDUTILITIES_APP __int32 RoundToNearestInt32(double dInput);
COBOLDUTILITIES_APP unsigned __int32 RoundToNearestUInt32(double dInput);
COBOLDUTILITIES_APP __int64 RoundToNearestInt64(double dInput);
COBOLDUTILITIES_APP unsigned __int64 RoundToNearestUInt64(double dInput);

// Strings
COBOLDUTILITIES_APP __int32 FindCharNotInQuotes(CString csString, char quoteCharacter, char searchCharacter);

// ThreadName
COBOLDUTILITIES_APP void SetThreadName( DWORD dwThreadID, const char* threadName);

// GetTextBoundingRect
COBOLDUTILITIES_APP BOOL GetTextBoundingRect(HDC hDC, int x, int y, CString csStr, CRect &rect);  // Holds bounding rectangle

// CoboldCommand
COBOLDUTILITIES_APP bool SendCoboldCommand(CString CommandLine);

//#ifdef __cplusplus
//}
//#endif

/////////////////////////////////////////////////////////////////////////////

#endif
     \   I D S _ S O U R C E _ H   C O B O L D R E C E N T F I L E L I S T       0           // CoboldRecentFileList.h : header file
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1995 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

/////////////////////////////////////////////////////////////////////////////
// CCoboldRecentFileList

#include <afxadv.h>

class CCoboldRecentFileList : public CRecentFileList
{
public:
	void UpdateSubMenu(CCmdUI* pCmdUI);
};

/////////////////////////////////////////////////////////////////////////////
ä  H   I D S _ S O U R C E _ H   G E N E R A L I O         0           /////////////////////////////////////////////////////////////////////////////
// "GeneralIO.h" 
//
// (c) 1997 RoentDek
// by Klaus Ullmann-Pfleger
/////////////////////////////////////////////////////////////////////////////

#define GIO_NOTTESTED	0
#define GIO_OK			1
#define GIO_ERROR		2

#ifdef _WIN32	// if Win9x, WinNT or W2K

__declspec (dllimport) int OpenPortsForNT(bool bDisplayError);

__declspec (dllimport) unsigned char InputPort(short Port);
__declspec (dllimport) unsigned short InputPortWord(short Port);
__declspec (dllimport) unsigned long InputPortDWord(short Port);

__declspec (dllimport) void OutputPort(short Port, unsigned char Data);
__declspec (dllimport) void OutputPortWord(short Port, unsigned short Data);
__declspec (dllimport) void OutputPortDWord(short Port, unsigned long Data);

#else			// if Windows 3.x or DOS (no GeneralIO.Dll available)

#define OpenPortsForNT(bDisplayError)	(GIO_OK)

#define InputPort(Port)					_inp(Port)
#define InputPortWord(Port)				_inpw(Port)
#define InputPortDWord(Port)			_inpd(Port)

#define OutputPort(Port,Data)			_outp(Port,Data)
#define OutputPortWord(Port,Data);		_outpw(Port,Data)
#define OutputPortDWord(Port,Data);		_outpd(Port,Data)

#endifB  \   I D S _ S O U R C E _ H   H I G H R E S O L U T I O N T I M E R         0           /////////////////////////////////////////////////////////////////////////////
// "HighResTimer.h" 
//
// (c) 1997 RoentDek
// by Klaus Ullmann-Pfleger
/////////////////////////////////////////////////////////////////////////////

// for _Win32 definition
//	  time unit is in us
// for not _Win32 definition
//    time unit is in us but will be reduced to ms
//	  not all functions are available

#ifdef _WIN32	// if Win9x, WinNT or W2K

__declspec (dllexport) double HighResTimerFrequency(void);
__declspec (dllimport) void HighResTimerInit(void);
__declspec (dllimport) void HighResTimerSetFinal(int uSeconds);
__declspec (dllimport) void HighResTimerSetFinal(int uSeconds, LARGE_INTEGER &HRTFinal, DWORD &LRTFinal);
__declspec (dllimport) bool HighResTimerTestFinal(LARGE_INTEGER &HRTFinal, DWORD &LRTFinal);
__declspec (dllimport) bool HighResTimerTestFinal(void);
__declspec (dllimport) void HighResTimerWait(int uSeconds);

#else			// if Windows 3.x or DOS (no GeneralIO.Dll available)

#define HighResTimerWait(uSeconds)			sleep(uSeconds/1000+1);

#endif
  '  L   I D S _ S O U R C E _ H   L M F I L E D I A L O G       0           #if !defined(AFX_LMFILEDIALOG_H__A9F64CD0_39F8_11D2_B17C_00A02465E0DD__INCLUDED_)
#define AFX_LMFILEDIALOG_H__A9F64CD0_39F8_11D2_B17C_00A02465E0DD__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// LMFileDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLMFileDialog dialog

class CLMFileDialog : public CFileDialog
{
	DECLARE_DYNAMIC(CLMFileDialog)

public:
	CLMFileDialog(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
	LPCTSTR lpszDefExt = nullptr,
	LPCTSTR lpszFileName = nullptr,
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
	LPCTSTR lpszFilter = nullptr,
	CWnd* pParentWnd = nullptr);
	CString m_LMComment;
	CString m_LMNumberOfEvents;
	CString m_LoadedCamacIni;

public:
protected:
	void ShowEmptyHeader();


protected:
	//{{AFX_MSG(CLMFileDialog)
	virtual BOOL OnInitDialog();
	virtual void OnFileNameChange();
	afx_msg void OnGetCamacIni();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LMFILEDIALOG_H__A9F64CD0_39F8_11D2_B17C_00A02465E0DD__INCLUDED_)
   T   I D S _ S O U R C E _ H   L M F P R E E V E N T D A T A         0           #pragma once

#ifndef LMFPreEventDataInclude
#define LMFPreEventDataInclude

#ifdef COBOLDUTILITIES_EXPORTS
	#define COBOLDUTILITIES_APP __declspec(dllexport)
#else
	#define COBOLDUTILITIES_APP __declspec(dllimport)
#endif

// LMF information of the actual/last NEW command
typedef struct struct_NEWInformation {
	bool	bHardware;		// flag for Type of the DAQ mode
	CString FileName;		// string filename single file
	CString FileBase;		// string filename base multi file
	bool	bAnalysis;		// flag for using the Analysis
	__int64 StartAtEvent;	// start at Event
	__int64 maxEvents;		// maxEvents per file from StartAtEvent
	CString Comment;		// file comment
	__int32 startBase;		// multi file mode start number
	__int32 endBase;		// multi file mode end number
	bool	bAutoClear;		// flag for using the auto clear between multi file LMFs
	bool	bAutoSave;		// flag for using the auto save between multi file LMFs
	CString CCFBase;		// string CCFBase name multi file

	bool	bMultiFile;		// flag for using multi file mode
	bool	bOverwriteFiles;// flag for overwriting files
	unsigned __int32 firstFoundNumber;	// for MF it's the first found file number
	unsigned __int32 lastFoundNumber;	// for MF it's the last found file number (to detect last file action)
	bool	bIsFileOnNetwork;	// flag for files on network paths
	CTimeSpan maxTime;		// maximum time per file from StartAtEvent time
	bool	bIsEventMode;	// flag indicating event or time information as end condition for LMF
} NEWInformation;

class COBOLDUTILITIES_APP LMFPreEventData
{
public:
	LMFPreEventData();
	virtual ~LMFPreEventData();
// data
	// hdr1 = EventMarker + EventLength
	//		bit 63..56		always 0xff
	//		bit 55..00		EventLegth in Bytes of this event
	// hdr2
	//		bit 63..00		event counter
	__int64	hdr1;
	__int64 hdr2;
	LARGE_INTEGER TimeStamp;	// low part for 32 bit, high and low part for 64 bit Timestamp
};

#endif >
  @   I D S _ S O U R C E _ H   L M I N F O       0           // LMInfo.h
// Informations file to control List-Mode Read
#pragma once

#define LM_FILE_VERSION		0x74656			// only 32 bit for int values
#define LM_FILE_VERSION9	0x74657			// partially 64 bit for int values (since 9.1.808.1)

#define LM_READFILE			-0x00000000
#define LM_NOFILEOPEN		-0x00000001
#define LM_CLOSEFILE		-0x00000002
#define LM_STARTFILE		-0x00000004
#define LM_HOLDFILE			-0x00000008
#define LM_ENDFILE			-0x00000010
#define LM_REWINDFILE		-0x00000020
#define LM_CLOSEING			-0x00000040

#define LM_HARDWARE			0x00000001
#define LM_FILEREAD			0x00000002
#define LM_HARDWAREWRITE	0x00000004
#define LM_SKIPINSERT		0x00000008

#define LMS_HRUNNING		0x00000004
#define LMS_FRUNNING		0x00000003
#define LMS_RUNNING			0x00000002
#define LMS_PAUSED			0x00000001
#define LMS_NONE			0x00000000

#define LMW_NOTSET			0x00000000
#define LMW_EOF				0x00000001
#define LMW_TIME			0x00000002
#define LMW_COUNTS			0x00000003
#define LMW_UNDEF			0xffffffff

// Data types possible in DAq/DAn
#define DT_NOTSET			0x00000000
#define DT_CHANNEL			0x00000001
#define DT_TIMEINPS			0x00000010
#define DT_TIMEINNS			0x00000011

#define DAQ_ID_RAW			0x000000		// for RAW (no Data)
#define DAQ_ID_HM1			0x000001		// for HM1 (single)
#define DAQ_ID_TDC8			0x000002		// for TDC8/ISA/PCI
#define DAQ_ID_CAMAC		0x000003		// for CAMAC
#define DAQ_ID_2HM1			0x000004		// for 2 HM1
#define DAQ_ID_2TDC8		0x000005		// for 2 TDC8
#define DAQ_ID_HM1_ABM		0x000006		// for HM1 (Advanced Burst Mode)
#define DAQ_ID_HM1_LR		0x000007		// for HM1 (Long Range Mode)
#define DAQ_ID_HPTDC		0x000008		// for HPTDC (TillDC)
#define DAQ_ID_TCPIP		0x000009		// for TCPIP (not supported yet)
#define DAQ_ID_HPTDCRAW		0x000010		// for HPTDC (TillDC) writing RAW data
#define DAQ_ID_FADC8		0x000011		// for FADC8
#define DAQ_ID_FADC4		0x000012		// for FADC4

// Source Code Included Flags
//	if LM_FILE_VERSION9 upper bits indicate presence of source code
#define DAQ_SOURCE_CODE		0x80000000
#define DAN_SOURCE_CODE		0x40000000
#define CCF_HISTORY_CODE	0x20000000

#define MASK_LMF_FILEVERSION	(~(DAQ_SOURCE_CODE|DAN_SOURCE_CODE|CCF_HISTORY_CODE))

// LMF main header structure
typedef struct struct_LMFMAINHEADER {
	unsigned __int32	LMHeaderVersion;
	unsigned __int32	LMDataFormat;
	unsigned __int64	LM64NumberOfCoordinates;
	unsigned __int64	LM64HeaderSize;
	unsigned __int64	LM64UserDefinedHeaderSize;
	unsigned __int64	LM64NumberOfEvents;
	CTime			LMStartTime;
	CTime			LMStopTime;
	CString			LMVersionString;
	CString			LMFilePathName;
	CString			LMComment;
} LMFMAINHEADER;

  0	  @   I D S _ S O U R C E _ H   M E M A C C       0           /*
MemAccess Library
Copyright (c) 1999-2004 Hai Li, Zeal SoftStudio.
Web: http://www.zealsoft.com (English)
	 http://zealsoft.yeah.net (Chinese)
E-Mail: support@zealsoft.com
*/
#ifndef MemAccess

#define MemAccess

#ifdef __cplusplus
extern "C" {
#endif

#include <windows.h>

#define V86_To_PhyAddress(seg,ofs)	((ULONG) \
			   (((unsigned long)(seg) << 4) | (unsigned)(ofs)))

// Determine whether the program is running under WinNT.
BOOL APIENTRY maIsWinNT();

// Open the diver.
BOOL APIENTRY maOpenLibrary();

// Close the driver.
BOOL APIENTRY maCloseLibrary();

// Return a byte at memory location nAddress.
BYTE APIENTRY maPeekB(DWORD nAddress);
// Return a word at memory location nAddress.
WORD APIENTRY maPeekW(DWORD nAddress);
// Return a double word at memory location nAddress.
DWORD APIENTRY maPeekD(DWORD nAddress);

// Store a byte at memory location nAddress.
BOOL APIENTRY maPokeB(DWORD nAddress, WORD nData);
// Store a word at memory location nAddress.
BOOL APIENTRY maPokeW(DWORD nAddress, WORD nData);
// Store a double word at memory location nAddress.
BOOL APIENTRY maPokeD(DWORD nAddress, DWORD dwData);

// Set the registration information.
void APIENTRY maLicenseInfo(LPSTR sUser, long dwKey);

// Error constants
const int ERROR_MA_DRIVER = 2;
const int ERROR_MA_NO_DEVICE_HANDLE = 9996;
const int ERROR_MA_SCM_CANT_CONNECT = 9998;

// Returns a value indicating the last operation is successful or not.
int APIENTRY maGetLastState(char *s);

// Returns the version of MemAccess Library.
WORD APIENTRY maGetVersion();

// Install the service under Windows NT/2000/XP
BOOL APIENTRY maInstallService();
// Remove the service under Windows NT/2000/XP
BOOL APIENTRY maRemoveService();
// Control the service under Windows NT/2000/XP
BOOL APIENTRY maControlService();

// Map the physical address to process address
void* APIENTRY maMapPhysToLinear(DWORD PhsAddr, DWORD dwSize, HANDLE* hMem);

// Unmap the address
BOOL APIENTRY maUnmapPhysicalMemory(HANDLE hMem, PVOID LinearAddr);

typedef struct {
	ULONG BaseAddress;
	ULONG Size;
	ULONG IOType;
} BADDR;

// Get the base address of a PCI device
LONG APIENTRY maGetDeviceBaseAddress(USHORT* VendorID, USHORT* DeviceID, LONG nIndex, BADDR baddrBuffer[6]);

#ifdef __cplusplus
}
#endif

#endif  D   I D S _ S O U R C E _ H   R E S O U R C E       0           //{{NO_DEPENDENCIES}}
// Microsoft Visual C++ generated include file.
// Used by CDAq.rc
//

// Next default values for new objects
// 
#ifdef APSTUDIO_INVOKED
#ifndef APSTUDIO_READONLY_SYMBOLS
#define _APS_NEXT_RESOURCE_VALUE        131
#define _APS_NEXT_COMMAND_VALUE         32771
#define _APS_NEXT_CONTROL_VALUE         1008
#define _APS_NEXT_SYMED_VALUE           101
#endif
#endif
    @   I D S _ S O U R C E _ H   S T D A F X       0           // stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#pragma once

#ifdef _CONFPLATRD_
#error mixing Win32/x64 settings of Project Configuration/Platform
#endif

//#define _BIND_TO_CURRENT_VCLIBS_VERSION	1
#define _BIND_TO_CURRENT_MFC_VERSION	1
#define _BIND_TO_CURRENT_CRT_VERSION	1

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC OLE automation classes
#endif // _AFX_NO_OLE_SUPPORT


#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>			// MFC ODBC database classes
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>			// MFC DAO database classes
#endif // _AFX_NO_DAO_SUPPORT

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC support for ribbons and control bars
#include "afxwinappex.h"  D   I D S _ S O U R C E _ H   U S E R S H M         0           // UserSHM.h : main header file for the UserSHM DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

// CUserSHMApp
// See UserSHM.cpp for the implementation of this class
//
class CUserSHMApp : public CWinApp
{
public:
	CUserSHMApp();

	// Overrides
public:
	virtual BOOL InitInstance();
	virtual __int32 ExitInstance();

	DECLARE_MESSAGE_MAP()
};

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */

	#ifdef USERSHM_EXPORTS
	#define USHM_API __declspec(dllexport)
	#else
	#define USHM_API __declspec(dllimport)
	#endif

	//////////////////////////////////////////////////////////////////////////
	// Forward declarations
	//////////////////////////////////////////////////////////////////////////

	class CDoubleArray;
	class CUserSpectrum;
	class CCoboldParser;

#define SHAREDCLASS_ARRAYSIZE 80

	/////////////////////////////////////////////////////////
	// SharedClass 

	class USHM_API SharedClass
	{
	public:
		SharedClass();
		~SharedClass();

		volatile double rate;
		volatile bool bDAqIsRunningOnline;
		volatile bool bDAqIsRunningOffline;
		volatile bool bDAnIsRunning;
		volatile bool bRequestStopDAq;
		CString csDAqNameAndPath;
		CString csDAnNameAndPath;

		__int16 *adc_trace[SHAREDCLASS_ARRAYSIZE];
		__int32 adc_trace_length[SHAREDCLASS_ARRAYSIZE];
		__int64 adc_trace_absolute_timestamp_ps[SHAREDCLASS_ARRAYSIZE];
		void *adc_peak_infos_array;
		__int32 adc_peak_infos_array_max_nof_hits;

		void * UserSHM_pointer1;
		void * UserSHM_pointer2;
		void * UserSHM_pointer3;
		void * UserSHM_pointer4;
	};

	//////////////////////////////////////////////////////////////////////////
	// Exported / Imported functions
	//////////////////////////////////////////////////////////////////////////

	USHM_API SharedClass *SHMGetSharedClassInstance(void);

	#ifdef USERSHM_EXPORTS
	USHM_API bool DoFunction(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount);
	#endif

	extern USHM_API SharedClass *SharedClassInstance;

#ifdef __cplusplus
}
#endif
 2  @   I D S _ S O U R C E _ H   U S E R S P       0           // UserSP.h : main header file for the UserSP DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef USERSP_EXPORTS
#define USP_API __declspec(dllexport)
#include "TUserSP.h"
#else
#define USP_API __declspec(dllimport)
#endif

#ifndef X_AXIS
#define X_AXIS          0x00000001
#endif
#ifndef Y_AXIS
#define Y_AXIS          0x00000002
#endif

class CDoubleArray;

// CUserSPApp
// See UserSP.cpp for the implementation of this class
//
class CUserSPApp : public CWinApp
{
public:
	CUserSPApp();

// Overrides
public:
	BOOL InitInstance();

// Possible Spectrum calling functions...
public:

	DECLARE_MESSAGE_MAP()
};

class CCoboldDocSpectrum;
class CParameter;
class CtUserSpectrum;

class USP_API CUserSpectrum
{
// constructor / destructor
public: 
	CUserSpectrum();
	CUserSpectrum(void *pCDoc);
	virtual ~CUserSpectrum();

// implementation
public:
	// spectra math functions
	void AddOneAt(__int32 iSpNr, double dChannel);
	void AddOneAt(__int32 iSpNr, double dChannelX, double dChannelY);
	void AddValueAt(__int32 iSpNr,double dChannel, double dValue);
	void AddValueAt(__int32 iSpNr,double dChannelX, double dChannelY, double dValue);
	void SetValueAt(__int32 iSpNr,double dChannel, double dValue);
	void SetValueAt(__int32 iSpNr,double dChannelX, double dChannelY, double dValue);
	double GetValueAt(__int32 iSpNr,double dChannel);
	double GetValueAt(__int32 iSpNr,double dChannelX, double dChannelY);
	double GetBinValueAt(__int32 iSpNr, __int64 iChannel);
	double GetBinValueAt(__int32 iSpNr, __int64 iChannelX, __int64 iChannelY);
	void SetBinValueAt(__int32 iSpNr, __int64 iChannel, double dValue);
	void SetBinValueAt(__int32 iSpNr, __int64 iChannelX, __int64 iChannelY, double dValue);
	bool GetHistogramDataArray(__int32 iSpNr, double *pData, __int64 MaxIndex);
	bool SetHistogramDataArray(__int32 iSpNr, double *pData, __int64 MaxIndex);
	bool ClearSpectrum(__int32 iSpNr);

	// spectra information functions
	//		index
	__int64 GetBinIndexX(__int32 iSpNr, double Channel);
	__int64 GetBinIndexY(__int32 iSpNr, double Channel);
	double GetBinChannelX(__int32 iSpNr, unsigned __int64 Channel);
	double GetBinChannelY(__int32 iSpNr, unsigned __int64 Channel);

	//		bins
	__int64 GetNumberOfXBins(__int32 iSpNr);
	__int64 GetNumberOfYBins(__int32 iSpNr);
	//		diverse information
	__int32 FindSpectrum(char *pcSpName);
	__int64 GetDisplayedSpectrumNumber();
	CString GetDisplayedSpectrumName();
	CString GetSpectrumName(__int32 iSpNr);
	__int32 GetSpectrumType(__int32 iSpNr, bool bDisplayError);
	double GetBinSizeX(__int32 iSpNr);
	double GetBinSizeY(__int32 iSpNr);
	void GetSpectrumDataInformation(__int32 iSpNr, double* &pdDataPointer, unsigned long &ulDimX, unsigned long &ulDimY);
	void SetGaussFitValues(__int32 iSpNr, double x0, double y0, double Sigma, double YOffset, double lowLimit, double highLimit, double usedLowLimit, double usedHighLimit);
	bool GetMultiFileInformation(__int32 &i32ActualFileNumber, CString &csActualFilePathName, CString &csBaseFilePathName, __int32 &i32StartNumber, __int32 &i32EndNumber, bool &bIsFirstFile, bool &bIsLastFile);
	void SignalStopMultiFile();
	void SignalStop();

	//		coordinate
	__int32 GetCoordinateIndexByName(char *pcCoordinateName);
	char * GetCoordinateNameByIndex(__int32 i32Index);
	// marker/cursor functions
	void GetXYPosition(double &xRet, double &yRet, __int32 AxisFlags);
	void GetXYCursor(double &xRet, double &yRet, bool bSnap);
	void GetXYBoxCursor(double &xStartRet, double &yStartRet, double &xEndRet, double &yEndRet, bool bSnap);
	void SetMarker(CString csType,CDoubleArray &InputData);
	double GetBinsizeX(__int32 iSpNr);
	double GetBinsizeY(__int32 iSpNr);

	// list mode information
	LMFMAINHEADER GetActualLMFMainHeader();

protected:
	CCoboldDocSpectrum *pSP;
//#ifdef USERSP_EXPORTS
	CtUserSpectrum *ptUserSp;
//#endif
public:
};

#ifdef __cplusplus
}
#endif
        ’’ ’’     0           4   V S _ V E R S I O N _ I N F O     ½ļž    
   
  ?   )                     ī   S t r i n g F i l e I n f o   Ź   0 4 0 9 0 4 b 0   R   C o m m e n t s   T D C 8 P C I ( B )   S t a n d a r d   -   L M F - V 6     2 	  C o m p a n y N a m e     R o e n t D e k     L   F i l e D e s c r i p t i o n     C o b o l d P C   2 0 1 1   D A q   >   F i l e V e r s i o n     1 0 ,   1 ,   1 4 1 2 ,   2     2 	  I n t e r n a l N a m e   C D A q . d l l     T   L e g a l C o p y r i g h t   C o p y r i g h t   ļ æ ½   1 9 9 5 - 2 0 1 4   8   O r i g i n a l F i l e n a m e   D A q . d l l   .   P r i v a t e B u i l d   0 0 0 1 0 0     L   P r o d u c t N a m e     D A q   f o r   C o b o l d P C   2 0 1 1   B   P r o d u c t V e r s i o n   1 0 ,   1 ,   1 4 1 2 ,   2        R e v i s i o n   R 5     D    V a r F i l e I n f o     $    T r a n s l a t i o n     	°