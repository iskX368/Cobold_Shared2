///////////////////////////////////////////////////////////////////////////
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


//@ここでインクルード
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
// Parameter 30 = Event Open Time in ｵs for detecting an event.
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
	//@取得処理名称をここで変更　元はStandard
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

		
		//@ここに追加
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


		
		//@ここでlmfヘッダ情報を外部ファイルへ記述
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
	uipPCIIOAdr[TDCRANGE]  = i32EventOpenTime*10000/5;										// ｵs*1e6/500
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
}