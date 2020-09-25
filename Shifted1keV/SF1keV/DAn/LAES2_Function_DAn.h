#pragma once


/////////////////////////////////////////////////////////////////////////////
// global definition of variables

extern LARGE_INTEGER	li32TimeStamp;	// TimeStamp time information
extern CTime ctLMFStartTime;		// Start Time of LMF

extern __int32 i32TimeStamp;		// parameter 2
extern double dTimeScaling;		// parameter 5 - Time Scaling (ticks per second)

extern __int32 i32DAq_ID;			// parameter 8

extern double dTDCResolution;		// parameter 20
extern __int32 i32TDCDataType;		// parameter 21

extern __int32	i32NumberOfChannels;// parameter 32
extern __int32	i32NumberOfHits;	// parameter 33
extern __int32 i32DataFormat;		// parameter 40

extern __int32 i32Conversion;		// parameter 1000
extern __int32 i32HexAnode;		// parameter 1002
extern __int32 i32PhiConversion;	// parameter 1003
extern double dDnlCorrection;		// parameter 1004
extern __int32 i32StartDAqData;	// parameter 1005
extern __int32 i32StartDAnData;	// parameter 1006
extern __int32 i32UseHit;			// parameter 1007

extern double dTPCalX;				// parameter 1010
extern double dTPCalY;				// parameter 1011
extern double dTPCalZ;				// parameter 1012

extern double dCOx;				// parameter 1020
extern double dCOy;				// parameter 1021
extern double dRotA;				// parameter 1022

extern double dCRPhix;				// parameter 1023
extern double dCRPhiy;				// parameter 1024

extern __int32 i32Cmcp;			// parameter 1025
extern __int32 i32Cx1, i32Cx2;		// parameter 1026 to 1031
extern __int32 i32Cy1, i32Cy2;
extern __int32 i32Cz1, i32Cz2;

extern __int32 i32TOFChannel;		// parameter 1032

extern double dOPx;				// parameter 1035
extern double dOPy;				// parameter 1036
extern double dOPw;				// parameter 1037
extern double dOSum;				// parameter 1038
extern bool	bAntiMoire;			// parameter 1039

extern __int32 i32StartDAqTDCData;

extern double dEventCounter;	// dEventCounter for the data in LM-file

extern RateAveragingClass* racpRateAveragingInstance;

/////////////////////////////////////////////////////////////////////////////
// MFM info variables
extern bool bIsMFM;							// flag for indicating MFM
extern bool bFirstFile;						// flag for indicating first file in MFM
extern bool bLastFile;							// flag for indicating last file in MFM

///////////////////////////////
// externals
extern CUserSpectrum* pDAnUserSP;

//前方宣言．ソースで使うので．
double GetReflectionValue(__int32 C1, __int32 C2, __int32 counts[], CDoubleArray& EventData);


namespace LAES2
{

	//ヘルプで表示する解析名
	TCHAR* GetAnalysisName();
	//追加のグローバル変数を初期化
	__int32 AnalysisInitialize(CDoubleArray& EventData, CDoubleArray& Parameter, CDoubleArray& WeighParameter);
	//解析
	__int32 AnalysisProcessEvent(CDoubleArray& EventData, CDoubleArray& Parameter, CDoubleArray& WeighParameter, LMFPreEventData& preEventData);
	//メモリ解放など
	__int32 AnalysisFinalize(CDoubleArray& EventData, CDoubleArray& Parameter, CDoubleArray& WeighParameter);

} // namespace LAES2

