#include "StdAfx.h"
#include "UserSHM.h"
#include "CoboldParser.h"
#include "LMFPreEventData.h"
#include "CoboldUtilities.h"
#include "CDAn.h"
#include "DAnUtilities.h"
#include "CDAq.h"
#include "LMInfo.h"
#include "UserSP.h"


#include "LAES2_Function_DAn.h"


#include <memory>
#include <vector>
#include <stdexcept>


#include "Lib_Inputs.h"
#include "Lib_ScaleInformation.h"
#include "Lib_CCFileCondition.h"
#include "Lib_StringsForOutputFile.h"
#include "Lib_ConversionToDCS.h"
#include "Lib_DCSHistogram.h"
#include "Lib_ConversionToKES.h"


#define LAES2_ANALYSIS_200918

namespace LAES2 {


//-----------デフォルト------------//
#ifdef LAES2_ANALYSIS_DEFAULT
#ifndef LAES2_ANALYSIS_DEFINED
#define LAES2_ANALYSIS_DEFINED//二重定義防止

	//解析名称
	TCHAR cAnalysisName[64];
	TCHAR* GetAnalysisName()
	{
		strcpy_s(cAnalysisName, CString("Default").GetString());
		return cAnalysisName;
	}

	__int32 AnalysisInitialize(CDoubleArray& EventData, CDoubleArray& Parameter, CDoubleArray& WeighParameter)
	{
		return 0;
	}

	__int32 AnalysisProcessEvent(CDoubleArray& EventData, CDoubleArray& Parameter, CDoubleArray& WeighParameter, LMFPreEventData& preEventData)
	{

		double AbsoluteEventTime;			// ns since start
		double DeltaEventTime;				// This Time - PreviousTime
		double True = true;					// always true
		__int32 ConsistencyIndicator;
		__int32 PLLStatusLocked;			// totally locked then true else false

		__int32 counts[32];
		double x1, x2, y1, y2, z1, z2;
		double TOF_ns = 0.;
		double raw_x, raw_y, raw_w;
		double sumx, sumy, sumw, sumxyw, diffxy;
		double raw_sumx, raw_sumy, raw_sumw, raw_sumxyw, raw_diffxy;
		double PosX, PosY;
		double r, phi;
		double Xuv, Yuv, Xuw, Yuw, Xvw, Yvw;
		double dX, dY;

		//  Example of how to use spectrum commands
		//	pDAnUserSP->AddOneAt(2,20);			// add one in spectrum 2 at channel 20
		//	pDAnUserSP->AddValueAt(3,20,0.5);		// add 0.5 in spectrum 3 at channel 20

		memset(counts, 0, 32 * sizeof(__int32));

		double dMCPChannelData = 0.;

		// get time information if present
		if (i32TimeStamp)
		{
			// AbsoluteEventTime
			AbsoluteEventTime = GetEventTime(preEventData, Parameter);
			// AbsoluteDeltaEventTime
			DeltaEventTime = GetDeltaEventTime(preEventData, Parameter);
			// dEventCounter
		}

		if (racpRateAveragingInstance)
			if (theDAnApp.SharedClassInstance)
			{
				racpRateAveragingInstance->ui32EventCounter += unsigned __int32(theDAnApp.SharedClassInstance->rate);
				if (i32TimeStamp)
					racpRateAveragingInstance->CheckTimestamp(AbsoluteEventTime);
			}

		// dEventCounter
		bool bdEventCounterReset = RoundToNearestInt32(Parameter[1040]) ? true : false;		// parameter 1040
		if (bdEventCounterReset)
		{
			dEventCounter = 0;
			Parameter[1040] = false;
		}
		dEventCounter += 1;

		// Get Status Information
		if (i32DAq_ID == DAQ_ID_HM1_ABM)
		{
			counts[0] = counts[1] = counts[2] = 1;
			PLLStatusLocked = false;
		}
		else if ((i32DAq_ID == DAQ_ID_HM1) || (i32DAq_ID == DAQ_ID_2HM1))
		{
			for (__int32 ch = 0; ch < 4; ++ch)
			{
				counts[ch] = (RoundToNearestInt32(EventData[(i32StartDAqTDCData + ch * (i32NumberOfHits + 1))]) & 0x0007) - 1;
				counts[ch] = counts[ch] < 0 ? 0 : counts[ch]; // correct negative (missed) hit
			}

			PLLStatusLocked = (RoundToNearestInt32(EventData[(i32StartDAqTDCData + 0 * (i32NumberOfHits + 1))]) & 0x0080) &
				(RoundToNearestInt32(EventData[(i32StartDAqTDCData + 1 * (i32NumberOfHits + 1))]) & 0x0080) &
				(RoundToNearestInt32(EventData[(i32StartDAqTDCData + 2 * (i32NumberOfHits + 1))]) & 0x0080) &
				(RoundToNearestInt32(EventData[(i32StartDAqTDCData + 3 * (i32NumberOfHits + 1))]) & 0x0080);
			PLLStatusLocked = PLLStatusLocked ? true : false;

		}
		else if ((i32DAq_ID == DAQ_ID_TDC8) || (i32DAq_ID == DAQ_ID_2TDC8 || (i32DAq_ID == DAQ_ID_HPTDC) || (i32DAq_ID == DAQ_ID_HPTDCRAW)))
		{
			for (__int32 ch = 0; ch < i32NumberOfChannels; ++ch)
				counts[ch] = RoundToNearestInt32(EventData[(i32StartDAqTDCData + ch * (i32NumberOfHits + 1))]);

			PLLStatusLocked = false;
		}
		else	// for all other... this information is not valid
			PLLStatusLocked = false;

		// ConsistencyIndicator
		ConsistencyIndicator = 0;
		ConsistencyIndicator += counts[i32Cx1] > 0 ? 1 : 0;
		ConsistencyIndicator += counts[i32Cx2] > 0 ? 2 : 0;
		ConsistencyIndicator += counts[i32Cy1] > 0 ? 4 : 0;
		ConsistencyIndicator += counts[i32Cy2] > 0 ? 8 : 0;
		if (i32HexAnode)
		{
			ConsistencyIndicator += counts[i32Cz1] > 0 ? 16 : 0;
			ConsistencyIndicator += counts[i32Cz2] > 0 ? 32 : 0;
		}

		// Get Raw-Data
		if (i32DAq_ID == DAQ_ID_HM1_ABM)
			x1 = x2 = y1 = y2 = z1 = z2 = 0;
		else
		{
			x1 = EventData[(i32StartDAqTDCData + i32Cx1 * (i32NumberOfHits + 1) + i32UseHit)];
			x2 = EventData[(i32StartDAqTDCData + i32Cx2 * (i32NumberOfHits + 1) + i32UseHit)];
			y1 = EventData[(i32StartDAqTDCData + i32Cy1 * (i32NumberOfHits + 1) + i32UseHit)];
			y2 = EventData[(i32StartDAqTDCData + i32Cy2 * (i32NumberOfHits + 1) + i32UseHit)];
			if (i32HexAnode)
			{
				z1 = EventData[(i32StartDAqTDCData + i32Cz1 * (i32NumberOfHits + 1) + i32UseHit)];
				z2 = EventData[(i32StartDAqTDCData + i32Cz2 * (i32NumberOfHits + 1) + i32UseHit)];
			}
			else
				z1 = z2 = 0.;

			if (i32TOFChannel >= 0)
			{
				if (counts[i32TOFChannel] > 0)
					TOF_ns = EventData[(i32StartDAqTDCData + i32TOFChannel * (i32NumberOfHits + 1) + 1)];
				else
					TOF_ns = 0.;
			}
			else
				TOF_ns = 0.;

			if (i32Cmcp != -1)
			{
				dMCPChannelData = EventData[(i32Cmcp * (i32NumberOfHits + 1) + i32UseHit)];
				if (i32DAq_ID == DAQ_ID_HPTDC && Parameter[88] > 0.5)
					TOF_ns = dMCPChannelData;
				if (!(i32DAq_ID == DAQ_ID_HPTDC && Parameter[88] > 0.5))
				{
					x1 -= dMCPChannelData;
					x2 -= dMCPChannelData;
					y1 -= dMCPChannelData;
					y2 -= dMCPChannelData;
					if (i32HexAnode)
					{
						z1 -= dMCPChannelData;
						z2 -= dMCPChannelData;
					}
					else
						z1 = z2 = 0.;
				}
			}
		}

		// correct DNL if
		if ((i32DAq_ID == DAQ_ID_HM1) && dDnlCorrection)
		{
			x1 = CorrectGP1NDL(x1, dDnlCorrection);
			x2 = CorrectGP1NDL(x2, dDnlCorrection);
			y1 = CorrectGP1NDL(y1, dDnlCorrection);
			y2 = CorrectGP1NDL(y2, dDnlCorrection);
		}

		if (bAntiMoire)
		{
			x1 += Rnd() - 0.5;
			x2 += Rnd() - 0.5;
			y1 += Rnd() - 0.5;
			y2 += Rnd() - 0.5;
			if (i32HexAnode)
			{
				z1 += Rnd() - 0.5;
				z2 += Rnd() - 0.5;
			}
			if (i32TOFChannel >= 0)
				if (counts[i32TOFChannel] > 0)
					TOF_ns += Rnd() - 0.5;
		}

		// now get the "real" position
		if (i32DAq_ID == DAQ_ID_HM1_ABM)
		{
			raw_x = RoundToNearestInt32(EventData[(i32StartDAqTDCData + 0)]);
			raw_y = RoundToNearestInt32(EventData[(i32StartDAqTDCData + 1)]);
			raw_w = RoundToNearestInt32(EventData[(i32StartDAqTDCData + 2)]);

			raw_sumx = raw_sumy = raw_sumw = raw_sumxyw = raw_diffxy = -1.e200;
		}
		else
		{
			raw_x = (x1 - x2);
			raw_y = (y1 - y2);
			raw_sumx = x1 + x2;
			raw_sumy = y1 + y2;
			raw_sumxyw = raw_sumx + raw_sumy;
			raw_diffxy = raw_sumx - raw_sumy;

			if (i32HexAnode)
			{
				raw_w = (z1 - z2);
				raw_sumw = z1 + z2;
				raw_sumxyw += raw_sumw;
			}
			else
			{
				raw_w = -1.e200;
				raw_sumw = -1.e200;
			}
		}

		// do conversion ? then first to time (ns)
		if (i32Conversion)
		{
			x1 *= dTDCResolution;
			x2 *= dTDCResolution;
			y1 *= dTDCResolution;
			y2 *= dTDCResolution;
			dMCPChannelData *= dTDCResolution;
			if (i32HexAnode)
			{
				z1 *= dTDCResolution;
				z2 *= dTDCResolution;
			}
		}
		TOF_ns *= dTDCResolution;

		// sums and differences
		// sums and differences in channels if parameter 1000 set to "channel"
		// sums and differences in ns (TIME) if parameter 1000 set to other than "channel"
		sumx = (x1 + x2) + dOSum;
		sumy = (y1 + y2) + dOSum;
		if (i32HexAnode)
			sumw = (z1 + z2) + dOSum;
		else
			sumw = 0;
		if (i32HexAnode)
			sumxyw = (sumx + sumy + sumw) - (2 * dOSum);		// only one OSum -> -(2*dOSum)!!!
		else
			sumxyw = (sumx + sumy) - dOSum;				// only one OSum -> -(dOSum)!!!
		diffxy = (sumx - sumy) + dOSum;

		// convert also to position? then to position (mm)
		if (i32Conversion == 2)
		{
			x1 *= dTPCalX;
			x2 *= dTPCalX;
			y1 *= dTPCalY;
			y2 *= dTPCalY;
			if (i32HexAnode)
			{
				z1 *= dTPCalZ;
				z2 *= dTPCalZ;
			}
		}

		if (i32HexAnode)
		{
			double x = (x1 - x2) * 0.5;
			double y = (y1 - y2) * 0.5;
			PosX = x + dOPx;
			PosY = (x - 2. * y) / sqrt(3.) + dOPy;
		}
		else
		{
			double x = (x1 - x2) * 0.5;
			double y = (y1 - y2) * 0.5;
			PosX = x + dOPx;
			PosY = y + dOPy;
		}

		// do rotation
		if (dRotA)
		{
			double xRot, yRot;
			RotateXY(PosX, PosY, dRotA, i32PhiConversion, dCOx, dCOy, xRot, yRot);
			PosX = xRot;
			PosY = yRot;
		}

		// convert to r-phi
		OrthoToRPhi(PosX, PosY, i32PhiConversion, dCRPhix, dCRPhiy, r, phi);

		if (i32HexAnode)
		{
			double x = (x1 - x2) * 0.5;
			double y = (y1 - y2) * 0.5;
			double w = (z1 - z2) * 0.5 + dOPw;
			Xuv = x + dOPx;
			Yuv = (x - 2. * y) / sqrt(3.) + dOPy;
			Xuw = Xuv;
			Yuw = (2. * w - x) / sqrt(3.) + dOPy;
			Xvw = (y + w) + dOPx;
			Yvw = (w - y) / sqrt(3.) + dOPy;
			dX = Xuv - Xvw;
			dY = Yuv - Yvw;
		}
		else
			Xuv = Yuv = Xuw = Yuw = Xvw = Yvw = dX = dY = DBL_MAX;		// not hex anode -> set to DBL_MAX 

		/////////////////////////////////////
		// write all data back to coordinates
		/////////////////////////////////////
		__int32 address = 0;
		EventData[i32StartDAnData + address++] = AbsoluteEventTime;
		EventData[i32StartDAnData + address++] = DeltaEventTime;
		EventData[i32StartDAnData + address++] = dEventCounter;
		EventData[i32StartDAnData + address++] = True;
		EventData[i32StartDAnData + address++] = 0; // false
		EventData[i32StartDAnData + address++] = ConsistencyIndicator;
		EventData[i32StartDAnData + address++] = PLLStatusLocked;

		if (racpRateAveragingInstance)
			EventData[i32StartDAnData + address++] = racpRateAveragingInstance->ui32Rate;	// rate
		else
			EventData[i32StartDAnData + address++] = 0.; // rate

		EventData[i32StartDAnData + address++] = Parameter[1060];	// manually set Condition1

		for (__int32 ch = 0; ch < 8; ++ch)						// hit counter of the first 9 channels
			EventData[i32StartDAnData + address++] = counts[ch];

		EventData[i32StartDAnData + address++] = x1;
		EventData[i32StartDAnData + address++] = x2;
		EventData[i32StartDAnData + address++] = y1;
		EventData[i32StartDAnData + address++] = y2;
		EventData[i32StartDAnData + address++] = z1;
		EventData[i32StartDAnData + address++] = z2;

		EventData[i32StartDAnData + address++] = TOF_ns;

		EventData[i32StartDAnData + address++] = raw_x;
		EventData[i32StartDAnData + address++] = raw_y;
		EventData[i32StartDAnData + address++] = raw_w;

		EventData[i32StartDAnData + address++] = raw_sumx;
		EventData[i32StartDAnData + address++] = raw_sumy;
		EventData[i32StartDAnData + address++] = raw_sumw;
		EventData[i32StartDAnData + address++] = raw_sumxyw;
		EventData[i32StartDAnData + address++] = raw_diffxy;

		EventData[i32StartDAnData + address++] = sumx;
		EventData[i32StartDAnData + address++] = sumy;
		EventData[i32StartDAnData + address++] = sumw;
		EventData[i32StartDAnData + address++] = sumxyw;

		EventData[i32StartDAnData + address++] = diffxy;

		EventData[i32StartDAnData + address++] = PosX;
		EventData[i32StartDAnData + address++] = PosY;

		EventData[i32StartDAnData + address++] = r;
		EventData[i32StartDAnData + address++] = phi;

		EventData[i32StartDAnData + address++] = Xuv;
		EventData[i32StartDAnData + address++] = Yuv;
		EventData[i32StartDAnData + address++] = Xuw;
		EventData[i32StartDAnData + address++] = Yuw;
		EventData[i32StartDAnData + address++] = Xvw;
		EventData[i32StartDAnData + address++] = Yvw;

		EventData[i32StartDAnData + address++] = dX;
		EventData[i32StartDAnData + address++] = dY;

		double dMcpSignalDiff = -1.e200;
		if (i32NumberOfHits > 1)
		{
			if (i32Cmcp > -1)
			{
				if (counts[i32Cmcp] > 1)
				{
					dMcpSignalDiff = EventData[(i32StartDAqTDCData + i32Cmcp * (i32NumberOfHits + 1) + 2)] - EventData[(i32StartDAqTDCData + i32Cmcp * (i32NumberOfHits + 1) + 1)];
					dMcpSignalDiff *= dTDCResolution;
				}
			}
			else if (i32Cmcp == -1)
			{	// if Parameter 1025 is set to 0 then perform the calculations for Hit2-Hit1 on TDC Channel 8 (normally x1)
				__int32 _i32Cmcp = 7;
				if (counts[_i32Cmcp] > 1)
				{
					dMcpSignalDiff = EventData[(i32StartDAqTDCData + _i32Cmcp * (i32NumberOfHits + 1) + 2)] - EventData[(i32StartDAqTDCData + _i32Cmcp * (i32NumberOfHits + 1) + 1)];
					dMcpSignalDiff *= dTDCResolution;
				}
			}
		}
		EventData[i32StartDAnData + address++] = dMcpSignalDiff;

		EventData[i32StartDAnData + address++] = GetReflectionValue(i32Cx1, i32Cx2, counts, EventData);
		EventData[i32StartDAnData + address++] = GetReflectionValue(i32Cx2, i32Cx1, counts, EventData);
		EventData[i32StartDAnData + address++] = GetReflectionValue(i32Cy1, i32Cy2, counts, EventData);
		EventData[i32StartDAnData + address++] = GetReflectionValue(i32Cy2, i32Cy1, counts, EventData);
		if (i32HexAnode)
		{
			EventData[i32StartDAnData + address++] = GetReflectionValue(i32Cz1, i32Cz2, counts, EventData);
			EventData[i32StartDAnData + address++] = GetReflectionValue(i32Cz2, i32Cz1, counts, EventData);
		}
		else
		{
			EventData[i32StartDAnData + address++] = -1.e200;
			EventData[i32StartDAnData + address++] = -1.e210;
		}

		for (__int32 ch = 0; ch < i32NumberOfChannels; ++ch)								// const 1-i32NumberOfChannels
			EventData[i32StartDAnData + address++] = double(ch + 1);						// must be the last coordinates

		CCoboldParser* pParser = GetCCoboldParser();
		pParser->execute_all_commands();

		return true;

	}

	__int32 AnalysisFinalize(CDoubleArray& EventData, CDoubleArray& Parameter, CDoubleArray& WeighParameter){
		return 0;
	}

#endif // LAES2_ANALYSIS_DEFINED
#endif // LAES2_ANALYSIS_DEFAULT
//---------------------------------//




//-----------200313------------//
#ifdef LAES2_ANALYSIS_200313
#ifndef LAES2_ANALYSIS_DEFINED
#define LAES2_ANALYSIS_DEFINED//二重定義防止

	
	//Parameter 1100 : どの層組をPosに選ぶ？
	__int32 i32SelectLayerPair;
	enum LayerPair { //switchの選択肢
		UV = 0,
		VW = 1,
		UW = 2,
	};


	//データを書き込む外部ファイル
	CStdioFile file_output;		//破棄されるときファイル開きっぱなしならばデストラクタでclsoe()が呼ばれるので，このまま使ってよし
	std::unique_ptr<StringsForOutputFile> up2StringForOutputFile;
	
	std::basic_string<TCHAR> ConditionString_For_file_putput;



	//Parameter 1101 : 外部ファイルへ書き込む？
	__int32 i32WriteCoordinatesToFile;
	
	//Parameter 1102 : 外部ファイルへ書き込むときどのConditionを適用する？
	__int32 i32WriteCoordinatesToFile_Condition;
	
	//Parameter 1103 : 計算する気体原子（後で分子も）の種類
	__int32 i32SelectAtom;
	
	//Parameter 1104 : VarPhiのフォーマット
	__int32 i32VarPhiFormat;




	//Parameter1120 : 遅延発生器DG645の値[ns]
	double dTOF_Offset;


	//Parameter1110 : 校正プレートと散乱点の距離[mm]
	double dDistance_cp_sp;

	//Parameter 1111 : 散乱後の運動エネルギー1keVでの二次関数 r_detector_mm = F( scattering_angle_deg ) のゼロ次係数 : K0
	double dFtheta_deg_K0;

	//Parameter 1112 : 散乱後の運動エネルギー1keVでの二次関数 r_detector_mm = F( scattering_angle_deg ) の一次係数 : K1
	double dFtheta_deg_K1;

	//Parameter 1113 : 散乱後の運動エネルギー1keVでの二次関数 r_detector_mm = F( scattering_angle_deg ) の二次係数 : K2
	double dFtheta_deg_K2;

	//Parameter 1114 : 検出器半径分布のbin[mm]
	double dBinR;

	//Parameter 1115 : 検出器半径R分布の最大値[mm]
	double dRmax;

	//Parameter 1116 : 検出器方位角phiの散乱後方位角varphiへのオフセットphi0[deg]
	double dPhi0_deg;

	//Parameter 1117 : 検出器PosXのbin[mm]
	double dBinPosX;

	//Parameter 1118 : 検出器PosYのbin[mm]
	double dBinPosY;




	//C++プログラムでConditionを使う
	std::unique_ptr<CCFileCondition> up2CCFileCondition;



	//ヒストグラムとグラフのまとめ
	namespace Histo {
		//1000 eVでの検出器感度分布
		std::unique_ptr<DetectorSensitivity1D_1000_1000> up2DetectorSensitivity;

		std::unique_ptr<DetectorSensitivity2D_1000_1000> up2DetectorSensitivity2D;



		//実験データのヒストグラム　一次元
		std::unique_ptr<ScaledHistogram1D> up2CI;
		std::unique_ptr<ScaledHistogram1D> up2R;
		std::unique_ptr<ScaledHistogram1D> up2Theta_deg;

		//実験データのヒストグラム　二次元
		std::unique_ptr<ScaledHistogram2D> up2R_Phi_deg;
		std::unique_ptr<ScaledHistogram2D> up2Theta_deg_VarPhi_deg;



		//異常なし
		bool isOK = false;
	};



	//知りたいCoordinateをグローバル変数に代入
	namespace Crd {
		double AbsoluteEventTime;			// ns since start
		double DeltaEventTime;				// This Time - PreviousTime
		double dEventCounter;
		__int32 ConsistencyIndicator;

		__int32 counts[32];
		double x1, x2, y1, y2, z1, z2;
		double TOF_ns;
		double raw_x, raw_y, raw_w;
		double sumx, sumy, sumw, sumxyw, diffxy;
		double raw_sumx, raw_sumy, raw_sumw, raw_sumxyw, raw_diffxy;
		double PosX, PosY;
		double r, phi;
		double Xuv, Yuv, Xuw, Yuw, Xvw, Yvw;
		double dX, dY;


		//@------ここに変数を追加--------

		__int32 M_ConsistencyIndicator;		//MCP信号も考慮したConsistency Indecator
		__int32 NextHitIndicator;			//NextHitについてのConsistency Indecator
		__int32 M_NextHitIndicator;			//NextHitについてMCP信号も考慮したConsistency Indecator
		double TOF_ns_NoOffset;				//遅延発生器の時間を除いた飛行時間

		double Theta_deg;					//散乱後エネルギー1keVの極角
		double VarPhi_deg;					//散乱後エネルギー1keVの方位角
		//-------------------------------
	}




	//解析名称
	TCHAR cAnalysisName[64];
	//ヘルプで表示する解析名
	TCHAR* GetAnalysisName() {
		strcpy_s(cAnalysisName, CString("200313").GetString());
		return cAnalysisName;
	}


	__int32 AnalysisInitialize(CDoubleArray& EventData, CDoubleArray& Parameter, CDoubleArray& WeighParameter) {
		
		//追加のグローバル変数を初期化
		i32SelectLayerPair = RoundToNearestInt32(Parameter[1100]);

		i32WriteCoordinatesToFile = RoundToNearestInt32(Parameter[1101]);
		i32WriteCoordinatesToFile_Condition = RoundToNearestInt32(Parameter[1102]);
		i32SelectAtom = RoundToNearestInt32(Parameter[1103]);
		i32VarPhiFormat = RoundToNearestInt32(Parameter[1104]);

		dTOF_Offset = Parameter[1120];

		dDistance_cp_sp = Parameter[1110];
		dFtheta_deg_K0 = Parameter[1111];
		dFtheta_deg_K1 = Parameter[1112];
		dFtheta_deg_K2 = Parameter[1113];

		dBinR = Parameter[1114];
		dRmax = Parameter[1115];
		dPhi0_deg = Parameter[1116];
		dBinPosX = Parameter[1117];
		dBinPosY = Parameter[1118];

		//汎用
		CString str, str1;

		
		//ヒストグラム
		try {
			//区間の個数
			//I0=(0,R1], ... , I_(n-1)=(R_(n-1), R_n], I_n=(R_n, R_max]
			const size_t numIntervals = static_cast<size_t>(ceil(dRmax / dBinR));
			const size_t Dimsize_Phi_deg = 12;

			auto pinf_CI = make_PointInformation(64, PointInformation::Difference::Central0);
			auto pinf = make_PointInformation(numIntervals, PointInformation::Difference::Backward);
			
			Histo::up2CI = make_ScaledHistogram1D(pinf_CI);
			

			Histo::up2R = DCSHistogram<Conv_1000_1000>::make_1D_vs_R(numIntervals, dBinR);
			
			Histo::up2Theta_deg = DCSHistogram<Conv_1000_1000>::make_1D_vs_Theta_deg(numIntervals, dBinR);

			Histo::up2DetectorSensitivity = DetectorSensitivity1D_1000_1000::make(numIntervals, dBinR);


			Histo::up2R_Phi_deg = DCSHistogram<Conv_1000_1000>::make_2D_vs_R_Phi_deg(numIntervals, dBinR, Dimsize_Phi_deg);

			Histo::up2Theta_deg_VarPhi_deg = DCSHistogram<Conv_1000_1000>::make_2D_vs_Theta_deg_VarPhi_deg(numIntervals, dBinR, Dimsize_Phi_deg);

			Histo::up2DetectorSensitivity2D = DetectorSensitivity2D_1000_1000::make(numIntervals, dBinR, Dimsize_Phi_deg);

			Histo::isOK = true;
		}
		catch (std::exception& ex) {

			Histo::up2CI.reset();

			Histo::up2R.reset();
			Histo::up2Theta_deg.reset();
			Histo::up2DetectorSensitivity.reset();

			Histo::up2R_Phi_deg.reset();
			Histo::up2Theta_deg_VarPhi_deg.reset();
			Histo::up2DetectorSensitivity2D.reset();

			Histo::isOK = false;

			AfxMessageBox(ex.what());
		}




		//Conditionを使いたいので読み込む
		try {
			CppAddress_CCFileCoordinate::ClearAll();

			CppAddress_CCFileCoordinate::SetPr(_T("DeltaEventTime"), Crd::DeltaEventTime);
			CppAddress_CCFileCoordinate::SetPr(_T("EventCounter"), Crd::dEventCounter);
			CppAddress_CCFileCoordinate::SetPr(_T("ConsistencyIndicator"), Crd::ConsistencyIndicator);

			CppAddress_CCFileCoordinate::SetPr(_T("n1"), Crd::counts, 0);
			CppAddress_CCFileCoordinate::SetPr(_T("n2"), Crd::counts, 1);
			CppAddress_CCFileCoordinate::SetPr(_T("n3"), Crd::counts, 2);
			CppAddress_CCFileCoordinate::SetPr(_T("n4"), Crd::counts, 3);
			CppAddress_CCFileCoordinate::SetPr(_T("n5"), Crd::counts, 4);
			CppAddress_CCFileCoordinate::SetPr(_T("n6"), Crd::counts, 5);
			CppAddress_CCFileCoordinate::SetPr(_T("n7"), Crd::counts, 6);
			CppAddress_CCFileCoordinate::SetPr(_T("n8"), Crd::counts, 7);

			CppAddress_CCFileCoordinate::SetPr(_T("x1"), Crd::x1);
			CppAddress_CCFileCoordinate::SetPr(_T("x2"), Crd::x2);
			CppAddress_CCFileCoordinate::SetPr(_T("y1"), Crd::y1);
			CppAddress_CCFileCoordinate::SetPr(_T("y2"), Crd::y2);
			CppAddress_CCFileCoordinate::SetPr(_T("z1"), Crd::z1);
			CppAddress_CCFileCoordinate::SetPr(_T("z2"), Crd::z2);

			CppAddress_CCFileCoordinate::SetPr(_T("TOF"), Crd::TOF_ns);

			CppAddress_CCFileCoordinate::SetPr(_T("raw_x"), Crd::raw_x);
			CppAddress_CCFileCoordinate::SetPr(_T("raw_y"), Crd::raw_y);
			CppAddress_CCFileCoordinate::SetPr(_T("raw_w"), Crd::raw_w);

			CppAddress_CCFileCoordinate::SetPr(_T("raw_sumx"), Crd::raw_sumx);
			CppAddress_CCFileCoordinate::SetPr(_T("raw_sumy"), Crd::raw_sumy);
			CppAddress_CCFileCoordinate::SetPr(_T("raw_sumw"), Crd::raw_sumw);
			CppAddress_CCFileCoordinate::SetPr(_T("raw_sumxyw"), Crd::raw_sumxyw);
			CppAddress_CCFileCoordinate::SetPr(_T("raw_diffxy"), Crd::raw_diffxy);

			CppAddress_CCFileCoordinate::SetPr(_T("sumx"), Crd::sumx);
			CppAddress_CCFileCoordinate::SetPr(_T("sumy"), Crd::sumy);
			CppAddress_CCFileCoordinate::SetPr(_T("sumw"), Crd::sumw);
			CppAddress_CCFileCoordinate::SetPr(_T("sumxyw"), Crd::sumxyw);
			CppAddress_CCFileCoordinate::SetPr(_T("diffxy"), Crd::diffxy);

			CppAddress_CCFileCoordinate::SetPr(_T("PosX"), Crd::PosX);
			CppAddress_CCFileCoordinate::SetPr(_T("PosY"), Crd::PosY);

			CppAddress_CCFileCoordinate::SetPr(_T("r"), Crd::r);
			CppAddress_CCFileCoordinate::SetPr(_T("phi"), Crd::phi);

			CppAddress_CCFileCoordinate::SetPr(_T("Xuv"), Crd::Xuv);
			CppAddress_CCFileCoordinate::SetPr(_T("Yuv"), Crd::Yuv);
			CppAddress_CCFileCoordinate::SetPr(_T("Xuw"), Crd::Xuw);
			CppAddress_CCFileCoordinate::SetPr(_T("Yuw"), Crd::Yuw);
			CppAddress_CCFileCoordinate::SetPr(_T("Xvw"), Crd::Xuv);
			CppAddress_CCFileCoordinate::SetPr(_T("Yvw"), Crd::Yuv);

			CppAddress_CCFileCoordinate::SetPr(_T("dX"), Crd::dX);
			CppAddress_CCFileCoordinate::SetPr(_T("dY"), Crd::dY);

			CppAddress_CCFileCoordinate::SetPr(_T("M_ConsistencyIndicator"), Crd::M_ConsistencyIndicator);
			CppAddress_CCFileCoordinate::SetPr(_T("NextHitIndicator"), Crd::NextHitIndicator);
			CppAddress_CCFileCoordinate::SetPr(_T("M_NextHitIndicator"), Crd::M_NextHitIndicator);
			CppAddress_CCFileCoordinate::SetPr(_T("TOF_NoOffset"), Crd::TOF_ns_NoOffset);

			CppAddress_CCFileCoordinate::SetPr(_T("Theta_deg"), Crd::Theta_deg);
			CppAddress_CCFileCoordinate::SetPr(_T("VarPhi_deg"), Crd::VarPhi_deg);
			

			//オブジェクト作成
			up2CCFileCondition = make_CCFileCondition({ LibPrm::ReadConditionsFromCCF_FilePath }, CppAddress_CCFileCoordinate::Get());
		}
		catch (std::exception& ex) {
			up2CCFileCondition.reset();
			AfxMessageBox(ex.what());
		}

		//ファイル出力時に使うCondition文字列
		LibFlag::Condition_ToString(ConditionString_For_file_putput, static_cast<LibFlag::type_Flag>(i32WriteCoordinatesToFile_Condition));

		
		//データ書き込み先の外部ファイルを開く
		if (i32WriteCoordinatesToFile > 0) {
			if (!file_output.Open(LibPrm::WriteForEachEvents_FilePath, CFile::modeCreate | CFile::modeWrite | CFile::typeText)) {
				file_output.Abort();
				AfxMessageBox(_T("output fopen failed."), MB_OK | MB_ICONINFORMATION);
			}
			else {
				
				//表に書き出したいCoordinateを選択
				std::basic_string<TCHAR> tstr;

				try {
					const auto WriteCoordinatesToFile = i32WriteCoordinatesToFile > 0 ? static_cast<LibFlag::type_Flag>(i32WriteCoordinatesToFile) : 0;
					
					//書き出し用クラス
					up2StringForOutputFile = make_StringsForOutputFile(WriteCoordinatesToFile , _T(','));

					//冒頭の情報
					up2StringForOutputFile->GetHeaderString(tstr);
					file_output.WriteString(tstr.c_str());
					file_output.WriteString(_T("\n"));

					//Condition
					file_output.WriteString(ConditionString_For_file_putput.c_str());
					file_output.WriteString(_T("\n"));
					
					//Coordinate名称
					up2StringForOutputFile->GetCoordinatesString(tstr);
					file_output.WriteString(tstr.c_str());
					file_output.WriteString(_T("\n"));

				}
				catch (std::exception& ex) {
					//エラーならメッセージ
					up2StringForOutputFile.reset();
					AfxMessageBox(ex.what());
				}

				//ファイルは開いたままAnalysisProccessへ

			}
		}

		//確認
		//p2Elements->Conditions().ShowConditionsString(str);
		//AfxMessageBox(str);
		
		//AfxMessageBox(_T("wait"));


		return 0;
	}

	


	//解析
	__int32 AnalysisProcessEvent(CDoubleArray& EventData, CDoubleArray& Parameter, CDoubleArray& WeighParameter, LMFPreEventData& preEventData) {
		
		
		double AbsoluteEventTime = 0;			// ns since start
		double DeltaEventTime = 0;				// This Time - PreviousTime
		double True = true;					// always true
		__int32 ConsistencyIndicator;
		__int32 PLLStatusLocked;			// totally locked then true else false

		__int32 counts[32];
		double x1, x2, y1, y2, z1, z2;
		double TOF_ns = 0.;
		double raw_x, raw_y, raw_w;
		double sumx, sumy, sumw, sumxyw, diffxy;
		double raw_sumx, raw_sumy, raw_sumw, raw_sumxyw, raw_diffxy;
		double PosX, PosY;
		double r, phi;
		double Xuv, Yuv, Xuw, Yuw, Xvw, Yvw;
		double dX, dY;


		//  Example of how to use spectrum commands
		//	pDAnUserSP->AddOneAt(2,20);			// add one in spectrum 2 at channel 20
		//	pDAnUserSP->AddValueAt(3,20,0.5);		// add 0.5 in spectrum 3 at channel 20

		memset(counts, 0, 32 * sizeof(__int32));

		double dMCPChannelData = 0.;

		// get time information if present
		if (i32TimeStamp)
		{
			// AbsoluteEventTime
			AbsoluteEventTime = GetEventTime(preEventData, Parameter);
			// AbsoluteDeltaEventTime
			DeltaEventTime = GetDeltaEventTime(preEventData, Parameter);
			// dEventCounter
		}

		if (racpRateAveragingInstance)
			if (theDAnApp.SharedClassInstance)
			{
				racpRateAveragingInstance->ui32EventCounter += unsigned __int32(theDAnApp.SharedClassInstance->rate);
				if (i32TimeStamp)
					racpRateAveragingInstance->CheckTimestamp(AbsoluteEventTime);
			}

		// dEventCounter
		bool bdEventCounterReset = RoundToNearestInt32(Parameter[1040]) ? true : false;		// parameter 1040
		if (bdEventCounterReset)
		{
			dEventCounter = 0;
			Parameter[1040] = false;
		}
		dEventCounter += 1;


		//@警告を無視
#pragma warning(push)
#pragma warning(disable : 26451)

		// Get Status Information
		if (i32DAq_ID == DAQ_ID_HM1_ABM)
		{
			counts[0] = counts[1] = counts[2] = 1;
			PLLStatusLocked = false;
		}
		else if ((i32DAq_ID == DAQ_ID_HM1) || (i32DAq_ID == DAQ_ID_2HM1))
		{
			for (__int32 ch = 0; ch < 4; ++ch)
			{
				counts[ch] = (RoundToNearestInt32(EventData[(i32StartDAqTDCData + ch * (i32NumberOfHits + 1))]) & 0x0007) - 1;
				counts[ch] = counts[ch] < 0 ? 0 : counts[ch]; // correct negative (missed) hit
			}

			PLLStatusLocked = (RoundToNearestInt32(EventData[(i32StartDAqTDCData + 0 * (i32NumberOfHits + 1))]) & 0x0080) &
				(RoundToNearestInt32(EventData[(i32StartDAqTDCData + 1 * (i32NumberOfHits + 1))]) & 0x0080) &
				(RoundToNearestInt32(EventData[(i32StartDAqTDCData + 2 * (i32NumberOfHits + 1))]) & 0x0080) &
				(RoundToNearestInt32(EventData[(i32StartDAqTDCData + 3 * (i32NumberOfHits + 1))]) & 0x0080);
			PLLStatusLocked = PLLStatusLocked ? true : false;

		}
		else if ((i32DAq_ID == DAQ_ID_TDC8) || (i32DAq_ID == DAQ_ID_2TDC8 || (i32DAq_ID == DAQ_ID_HPTDC) || (i32DAq_ID == DAQ_ID_HPTDCRAW)))
		{
			for (__int32 ch = 0; ch < i32NumberOfChannels; ++ch)
				counts[ch] = RoundToNearestInt32(EventData[(i32StartDAqTDCData + ch * (i32NumberOfHits + 1))]);

			PLLStatusLocked = false;
		}
		else	// for all other... this information is not valid
			PLLStatusLocked = false;


		// ConsistencyIndicator
		ConsistencyIndicator = 0;
		ConsistencyIndicator += counts[i32Cx1] > 0 ? 1 : 0;
		ConsistencyIndicator += counts[i32Cx2] > 0 ? 2 : 0;
		ConsistencyIndicator += counts[i32Cy1] > 0 ? 4 : 0;
		ConsistencyIndicator += counts[i32Cy2] > 0 ? 8 : 0;
		if (i32HexAnode)
		{
			ConsistencyIndicator += counts[i32Cz1] > 0 ? 16 : 0;
			ConsistencyIndicator += counts[i32Cz2] > 0 ? 32 : 0;
		}

		// Get Raw-Data
		if (i32DAq_ID == DAQ_ID_HM1_ABM)
			x1 = x2 = y1 = y2 = z1 = z2 = 0;
		else
		{
			x1 = EventData[(i32StartDAqTDCData + i32Cx1 * (i32NumberOfHits + 1) + i32UseHit)];
			x2 = EventData[(i32StartDAqTDCData + i32Cx2 * (i32NumberOfHits + 1) + i32UseHit)];
			y1 = EventData[(i32StartDAqTDCData + i32Cy1 * (i32NumberOfHits + 1) + i32UseHit)];
			y2 = EventData[(i32StartDAqTDCData + i32Cy2 * (i32NumberOfHits + 1) + i32UseHit)];
			if (i32HexAnode)
			{
				z1 = EventData[(i32StartDAqTDCData + i32Cz1 * (i32NumberOfHits + 1) + i32UseHit)];
				z2 = EventData[(i32StartDAqTDCData + i32Cz2 * (i32NumberOfHits + 1) + i32UseHit)];
			}
			else
				z1 = z2 = 0.;

			if (i32TOFChannel >= 0)
			{
				if (counts[i32TOFChannel] > 0)
					TOF_ns = EventData[(i32StartDAqTDCData + i32TOFChannel * (i32NumberOfHits + 1) + 1)];
				else
					TOF_ns = 0.;
			}
			else
				TOF_ns = 0.;

			if (i32Cmcp != -1)
			{
				dMCPChannelData = EventData[(i32Cmcp * (i32NumberOfHits + 1) + i32UseHit)];
				if (i32DAq_ID == DAQ_ID_HPTDC && Parameter[88] > 0.5)
					TOF_ns = dMCPChannelData;
				if (!(i32DAq_ID == DAQ_ID_HPTDC && Parameter[88] > 0.5))
				{
					x1 -= dMCPChannelData;
					x2 -= dMCPChannelData;
					y1 -= dMCPChannelData;
					y2 -= dMCPChannelData;
					if (i32HexAnode)
					{
						z1 -= dMCPChannelData;
						z2 -= dMCPChannelData;
					}
					else
						z1 = z2 = 0.;
				}
			}
		}
		

		// correct DNL if
		if ((i32DAq_ID == DAQ_ID_HM1) && dDnlCorrection)
		{
			x1 = CorrectGP1NDL(x1, dDnlCorrection);
			x2 = CorrectGP1NDL(x2, dDnlCorrection);
			y1 = CorrectGP1NDL(y1, dDnlCorrection);
			y2 = CorrectGP1NDL(y2, dDnlCorrection);
		}

		if (bAntiMoire)
		{
			x1 += Rnd() - 0.5;
			x2 += Rnd() - 0.5;
			y1 += Rnd() - 0.5;
			y2 += Rnd() - 0.5;
			if (i32HexAnode)
			{
				z1 += Rnd() - 0.5;
				z2 += Rnd() - 0.5;
			}
			if (i32TOFChannel >= 0)
				if (counts[i32TOFChannel] > 0)
					TOF_ns += Rnd() - 0.5;
		}

		// now get the "real" position
		if (i32DAq_ID == DAQ_ID_HM1_ABM)
		{
			raw_x = RoundToNearestInt32(EventData[(i32StartDAqTDCData + 0)]);
			raw_y = RoundToNearestInt32(EventData[(i32StartDAqTDCData + 1)]);
			raw_w = RoundToNearestInt32(EventData[(i32StartDAqTDCData + 2)]);

			raw_sumx = raw_sumy = raw_sumw = raw_sumxyw = raw_diffxy = -1.e200;
		}
		else
		{
			raw_x = (x1 - x2);
			raw_y = (y1 - y2);
			raw_sumx = x1 + x2;
			raw_sumy = y1 + y2;
			raw_sumxyw = raw_sumx + raw_sumy;
			raw_diffxy = raw_sumx - raw_sumy;

			if (i32HexAnode)
			{
				raw_w = (z1 - z2);
				raw_sumw = z1 + z2;
				raw_sumxyw += raw_sumw;
			}
			else
			{
				raw_w = -1.e200;
				raw_sumw = -1.e200;
			}
		}

		// do conversion ? then first to time (ns)
		if (i32Conversion)
		{
			x1 *= dTDCResolution;
			x2 *= dTDCResolution;
			y1 *= dTDCResolution;
			y2 *= dTDCResolution;
			dMCPChannelData *= dTDCResolution;
			if (i32HexAnode)
			{
				z1 *= dTDCResolution;
				z2 *= dTDCResolution;
			}
		}
		TOF_ns *= dTDCResolution;

		// sums and differences
		// sums and differences in channels if parameter 1000 set to "channel"
		// sums and differences in ns (TIME) if parameter 1000 set to other than "channel"
		sumx = (x1 + x2) + dOSum;
		sumy = (y1 + y2) + dOSum;
		if (i32HexAnode)
			sumw = (z1 + z2) + dOSum;
		else
			sumw = 0;
		if (i32HexAnode)
			sumxyw = (sumx + sumy + sumw) - (2 * dOSum);		// only one OSum -> -(2*dOSum)!!!
		else
			sumxyw = (sumx + sumy) - dOSum;				// only one OSum -> -(dOSum)!!!
		diffxy = (sumx - sumy) + dOSum;

		// convert also to position? then to position (mm)
		if (i32Conversion == 2)
		{
			x1 *= dTPCalX;
			x2 *= dTPCalX;
			y1 *= dTPCalY;
			y2 *= dTPCalY;
			if (i32HexAnode)
			{
				z1 *= dTPCalZ;
				z2 *= dTPCalZ;
			}
		}

		
		//@Pos計算　下で代入するので無意味
		/*if (i32HexAnode)
		{
			double x = (x1 - x2) * 0.5;
			double y = (y1 - y2) * 0.5;
			PosX = x + dOPx;
			PosY = (x - 2. * y) / sqrt(3.) + dOPy;
		}
		else
		{
			double x = (x1 - x2) * 0.5;
			double y = (y1 - y2) * 0.5;
			PosX = x + dOPx;
			PosY = y + dOPy;
		}*/

		//@計算順序並び替え
		//Defaultでは	Pos計算 -> Pos回転 -> r-phiをPosから計算 -> HexならXuvとかを独立に計算
		//ここでは		HexならXuvとかを計算 -> Xuvなどよりレイヤ選択でPosへ代入 -> Pos回転 -> r-phiをPosから計算
		{
			if (i32HexAnode)
			{
				double x = (x1 - x2) * 0.5;
				double y = (y1 - y2) * 0.5;
				double w = (z1 - z2) * 0.5 + dOPw;
				Xuv = x + dOPx;
				Yuv = (x - 2. * y) / sqrt(3.) + dOPy;
				Xuw = Xuv;
				Yuw = (2. * w - x) / sqrt(3.) + dOPy;
				Xvw = (y + w) + dOPx;
				Yvw = (w - y) / sqrt(3.) + dOPy;
				dX = Xuv - Xvw;
				dY = Yuv - Yvw;
			}
			else
				Xuv = Yuv = Xuw = Yuw = Xvw = Yvw = dX = dY = DBL_MAX;		// not hex anode -> set to DBL_MAX 

			//@採用するレイヤペア
			switch (i32SelectLayerPair)
			{
			case LayerPair::UV:
				PosX = Xuv;
				PosY = Yuv;
				break;
			case LayerPair::VW:
				PosX = Xvw;
				PosY = Yvw;
				break;
			case LayerPair::UW:
				PosX = Xuw;
				PosY = Yuw;
				break;
			default:
				PosX = DBL_MAX;
				PosY = DBL_MAX;
				break;
			}

			// do rotation
			if (dRotA)
			{
				double xRot, yRot;
				RotateXY(PosX, PosY, dRotA, i32PhiConversion, dCOx, dCOy, xRot, yRot);
				PosX = xRot;
				PosY = yRot;
			}

			// convert to r-phi
			OrthoToRPhi(PosX, PosY, i32PhiConversion, dCRPhix, dCRPhiy, r, phi);
		}
		
		
		//@MCP信号も考慮したConsistency Indicator
		{
			__int32 M_ConsistencyIndicator = 0;
			if (i32Cmcp != -1) {//MCP信号を使用する？
				M_ConsistencyIndicator = ConsistencyIndicator + ((counts[i32Cmcp] > 0) ? 64 : 0);
			}
			else {
				M_ConsistencyIndicator = -1;
			}
			Crd::M_ConsistencyIndicator = M_ConsistencyIndicator;
		}

		//@NextHit CIs
		{
			__int32 NextHitIndicator = 0;
			__int32 M_NextHitIndicator = 0;
			if (i32UseHit + 1 <= i32NumberOfHits) {//NextHit番号が解析する信号数を超えない
				__int32 i32NextHit = i32UseHit + 1;
				NextHitIndicator += (counts[i32Cx1] >= i32NextHit) ? 1 : 0;
				NextHitIndicator += (counts[i32Cx2] >= i32NextHit) ? 2 : 0;
				NextHitIndicator += (counts[i32Cy1] >= i32NextHit) ? 4 : 0;
				NextHitIndicator += (counts[i32Cy2] >= i32NextHit) ? 8 : 0;
				if (i32HexAnode)
				{
					NextHitIndicator += (counts[i32Cz1] >= i32NextHit) ? 16 : 0;
					NextHitIndicator += (counts[i32Cz2] >= i32NextHit) ? 32 : 0;
				}
				if (i32Cmcp != -1)
				{
					M_NextHitIndicator = NextHitIndicator + ((counts[i32Cmcp] >= i32NextHit) ? 64 : 0);
				}
				else
				{
					M_NextHitIndicator = -1;
				}
			}
			else {
				NextHitIndicator = -1;
				M_NextHitIndicator = -1;
			}

			Crd::NextHitIndicator = NextHitIndicator;
			Crd::M_NextHitIndicator = M_NextHitIndicator;
		}

		//@オフセットなしTOF
		{
			double TOF_ns_NoOffset = 0;

			TOF_ns_NoOffset = TOF_ns - dTOF_Offset;

			Crd::TOF_ns_NoOffset = TOF_ns_NoOffset;
		}
		

		//@散乱後の極角と方位角と微分散乱断面積
		try
		{
			double Theta_deg=0;
			double VarPhi_deg=0;
			//分解能に基づく所属区間のTheta値をTheta_Graphなどと表記する
			
			Theta_deg = Conv_1000_1000::Theta_deg(r);
			//VarPhi_deg = Conv_1000_1000::VarPhi_deg_0_360(phi);
			VarPhi_deg = Conv_1000_1000::VarPhi(phi, static_cast<LibFlag::AzimuthFormat>(i32PhiConversion), i32VarPhiFormat == 0 ? LibFlag::AzimuthFormat::Deg0 : LibFlag::AzimuthFormat::Deg);
			
			Crd::Theta_deg = Theta_deg;
			Crd::VarPhi_deg = VarPhi_deg;
			
		}
		catch (std::exception&) {

		}
		
		
		


		//@デフォルトパラメータの代入
		{
			/*
			double AbsoluteEventTime = 0;			// ns since start
			double DeltaEventTime = 0;				// This Time - PreviousTime
			double True = true;					// always true
			__int32 ConsistencyIndicator;
			__int32 PLLStatusLocked;			// totally locked then true else false

			__int32 counts[32];
			double x1, x2, y1, y2, z1, z2;
			double TOF_ns = 0.;
			double raw_x, raw_y, raw_w;
			double sumx, sumy, sumw, sumxyw, diffxy;
			double raw_sumx, raw_sumy, raw_sumw, raw_sumxyw, raw_diffxy;
			double PosX, PosY;
			double r, phi;
			double Xuv, Yuv, Xuw, Yuw, Xvw, Yvw;
			double dX, dY;
			*/
			
			Crd::AbsoluteEventTime = AbsoluteEventTime;
			Crd::DeltaEventTime = DeltaEventTime;
			Crd::dEventCounter = dEventCounter;
			Crd::ConsistencyIndicator = ConsistencyIndicator;
			
			for (__int32 ch = 0; ch < 8; ++ch)						// hit counter of the first 9 channels
				Crd::counts[ch] = counts[ch];
			
			Crd::x1 = x1;
			Crd::x2 = x2;
			Crd::y1 = y1;
			Crd::y2 = y2;
			Crd::z1 = z1;
			Crd::z2 = z2;
			
			Crd::TOF_ns = TOF_ns;
			
			Crd::raw_x = raw_x;
			Crd::raw_y = raw_y;
			Crd::raw_w = raw_w;
			
			Crd::sumx = sumx;
			Crd::sumy = sumy;
			Crd::sumw = sumw;
			Crd::sumxyw = sumxyw;
			Crd::diffxy = diffxy;
			
			Crd::raw_sumx = raw_sumx;
			Crd::raw_sumy = raw_sumy;
			Crd::raw_sumw = raw_sumw;
			Crd::raw_sumxyw = raw_sumxyw;
			Crd::raw_diffxy = raw_diffxy;
			
			Crd::PosX = PosX;
			Crd::PosY = PosY;

			Crd::r = r;
			Crd::phi = phi;
			
			Crd::Xuv = Xuv;
			Crd::Yuv = Yuv;
			Crd::Xuw = Xuw;
			Crd::Yuw = Yuw;
			Crd::Xvw = Xvw;
			Crd::Yvw = Yvw;
			
			Crd::dX = dX;
			Crd::dY = dY;
		}





		//ヒストグラムへ加算
		try {
			if (Histo::isOK) {
				
				Histo::up2CI->AddValue(static_cast<double>(Crd::ConsistencyIndicator));

				if (up2CCFileCondition->IsTrue(ConditionString_For_file_putput)) {
					
					Histo::up2R->AddValue(Crd::r);
					
					Histo::up2Theta_deg->AddValue(Crd::Theta_deg);


					//[0,360)に収めておく
					double phi_0_360 = Crd::phi;
					if (phi_0_360 >= 360) {
						phi_0_360 -= 360;
					}
					else if (phi_0_360 < 0) {
						phi_0_360 += 360;
					}
					Histo::up2R_Phi_deg->AddValue(Crd::r, phi_0_360);

					Histo::up2Theta_deg_VarPhi_deg->AddValue(Crd::Theta_deg, Crd::VarPhi_deg);
				}
			}
		}
		catch (std::exception&) {
			//AfxMessageBox(ex.what());
		}


		

		//@アドレス登録したグローバル変数の更新が完了したので外部ファイルへ書き込み
		try
		{
			std::basic_string<TCHAR> tstr;

			if (up2CCFileCondition->IsTrue(ConditionString_For_file_putput)) {
				if (up2StringForOutputFile->GetValuesString(tstr) == 0) {
					file_output.WriteString(tstr.c_str());
					file_output.WriteString(_T("\n"));
				}
			}

		}
		catch (std::exception&) {
			//
		}



		/////////////////////////////////////
		// write all data back to coordinates
		/////////////////////////////////////
		__int32 address = 0;


		EventData[i32StartDAnData + address++] = AbsoluteEventTime;
		EventData[i32StartDAnData + address++] = DeltaEventTime;
		EventData[i32StartDAnData + address++] = dEventCounter;
		EventData[i32StartDAnData + address++] = True;
		EventData[i32StartDAnData + address++] = 0; // false
		EventData[i32StartDAnData + address++] = ConsistencyIndicator;
		EventData[i32StartDAnData + address++] = PLLStatusLocked;

		if (racpRateAveragingInstance)
			EventData[i32StartDAnData + address++] = racpRateAveragingInstance->ui32Rate;	// rate
		else
			EventData[i32StartDAnData + address++] = 0.; // rate

		EventData[i32StartDAnData + address++] = Parameter[1060];	// manually set Condition1

		for (__int32 ch = 0; ch < 8; ++ch)						// hit counter of the first 9 channels
			EventData[i32StartDAnData + address++] = counts[ch];

		EventData[i32StartDAnData + address++] = x1;
		EventData[i32StartDAnData + address++] = x2;
		EventData[i32StartDAnData + address++] = y1;
		EventData[i32StartDAnData + address++] = y2;
		EventData[i32StartDAnData + address++] = z1;
		EventData[i32StartDAnData + address++] = z2;

		EventData[i32StartDAnData + address++] = TOF_ns;

		EventData[i32StartDAnData + address++] = raw_x;
		EventData[i32StartDAnData + address++] = raw_y;
		EventData[i32StartDAnData + address++] = raw_w;

		EventData[i32StartDAnData + address++] = raw_sumx;
		EventData[i32StartDAnData + address++] = raw_sumy;
		EventData[i32StartDAnData + address++] = raw_sumw;
		EventData[i32StartDAnData + address++] = raw_sumxyw;
		EventData[i32StartDAnData + address++] = raw_diffxy;

		EventData[i32StartDAnData + address++] = sumx;
		EventData[i32StartDAnData + address++] = sumy;
		EventData[i32StartDAnData + address++] = sumw;
		EventData[i32StartDAnData + address++] = sumxyw;

		EventData[i32StartDAnData + address++] = diffxy;

		EventData[i32StartDAnData + address++] = PosX;
		EventData[i32StartDAnData + address++] = PosY;

		EventData[i32StartDAnData + address++] = r;
		EventData[i32StartDAnData + address++] = phi;

		EventData[i32StartDAnData + address++] = Xuv;
		EventData[i32StartDAnData + address++] = Yuv;
		EventData[i32StartDAnData + address++] = Xuw;
		EventData[i32StartDAnData + address++] = Yuw;
		EventData[i32StartDAnData + address++] = Xvw;
		EventData[i32StartDAnData + address++] = Yvw;

		EventData[i32StartDAnData + address++] = dX;
		EventData[i32StartDAnData + address++] = dY;

		double dMcpSignalDiff = -1.e200;
		if (i32NumberOfHits > 1)
		{
			if (i32Cmcp > -1)
			{
				if (counts[i32Cmcp] > 1)
				{
					dMcpSignalDiff = EventData[(i32StartDAqTDCData + i32Cmcp * (i32NumberOfHits + 1) + 2)] - EventData[(i32StartDAqTDCData + i32Cmcp * (i32NumberOfHits + 1) + 1)];
					dMcpSignalDiff *= dTDCResolution;
				}
			}
			else if (i32Cmcp == -1)
			{	// if Parameter 1025 is set to 0 then perform the calculations for Hit2-Hit1 on TDC Channel 8 (normally x1)
				__int32 _i32Cmcp = 7;
				if (counts[_i32Cmcp] > 1)
				{
					dMcpSignalDiff = EventData[(i32StartDAqTDCData + _i32Cmcp * (i32NumberOfHits + 1) + 2)] - EventData[(i32StartDAqTDCData + _i32Cmcp * (i32NumberOfHits + 1) + 1)];
					dMcpSignalDiff *= dTDCResolution;
				}
			}
		}
		EventData[i32StartDAnData + address++] = dMcpSignalDiff;

		EventData[i32StartDAnData + address++] = GetReflectionValue(i32Cx1, i32Cx2, counts, EventData);
		EventData[i32StartDAnData + address++] = GetReflectionValue(i32Cx2, i32Cx1, counts, EventData);
		EventData[i32StartDAnData + address++] = GetReflectionValue(i32Cy1, i32Cy2, counts, EventData);
		EventData[i32StartDAnData + address++] = GetReflectionValue(i32Cy2, i32Cy1, counts, EventData);
		if (i32HexAnode)
		{
			EventData[i32StartDAnData + address++] = GetReflectionValue(i32Cz1, i32Cz2, counts, EventData);
			EventData[i32StartDAnData + address++] = GetReflectionValue(i32Cz2, i32Cz1, counts, EventData);
		}
		else
		{
			EventData[i32StartDAnData + address++] = -1.e200;
			EventData[i32StartDAnData + address++] = -1.e210;
		}


		//@-------ここにccf宣言順にユーザー座標を代入--------//
		EventData[i32StartDAnData + address++] = Crd::M_ConsistencyIndicator;
		EventData[i32StartDAnData + address++] = Crd::NextHitIndicator;
		EventData[i32StartDAnData + address++] = Crd::M_NextHitIndicator;
		EventData[i32StartDAnData + address++] = Crd::TOF_ns_NoOffset;

		EventData[i32StartDAnData + address++] = Crd::Theta_deg;
		EventData[i32StartDAnData + address++] = Crd::VarPhi_deg;
		
		//---------------------------------------------------//
		


		for (__int32 ch = 0; ch < i32NumberOfChannels; ++ch)								// const 1-i32NumberOfChannels
			EventData[i32StartDAnData + address++] = double(ch + 1);						// must be the last coordinates


		//@警告を無視
#pragma warning(pop)


		CCoboldParser* pParser = GetCCoboldParser();
		pParser->execute_all_commands();

		return true;
	}


	//メモリ解放など
	__int32 AnalysisFinalize(CDoubleArray& EventData, CDoubleArray& Parameter, CDoubleArray& WeighParameter) {
		try {

			//イベント記録ファイルを閉じる
			file_output.Close();



			std::basic_string<TCHAR> tstr;


			///Condition 含んだファイル名
			tstr = _T("C:\\Program Files\\RoentDek Handels GmbH\\CoboldPC 2011 R5-2-x64 (Visual Studio .Net 2010 Compilation) V10.1.1412.2\\LAES2_TDC8PCI2_HEX\\Cobold_Shared1\\temp\\DCSHisto_") + ConditionString_For_file_putput + _T(".itx");


			//非線形ヒストグラム用出力ファイル
			if (!file_output.Open(tstr.c_str(), CFile::modeCreate | CFile::modeWrite | CFile::typeText)) {
				file_output.Abort();
				AfxMessageBox(_T("DCSHisto fopen failed."), MB_OK | MB_ICONINFORMATION);
			}
			else {


				if (Histo::isOK) {


					//スケール情報
					const auto& sinfoR = Histo::up2DetectorSensitivity->Hist_R().ScaleInfo();
					const auto& sinfoTheta_deg = Histo::up2DetectorSensitivity->Hist_Theta_deg().ScaleInfo();

					const auto& sinfoVarPhi_deg = Histo::up2DetectorSensitivity2D->ScaleInfo_VarPhi_deg();
					const auto& sinfoPhi_deg = Histo::up2DetectorSensitivity2D->ScaleInfo_Phi_deg();


					//DCSの理論曲線
					std::unique_ptr<std::vector <double>> DCS_Theory_vs_Theta_deg;
					std::unique_ptr<std::vector <double>> DCS_Theory_vs_R;
					
					std::unique_ptr<std::vector<std::vector <double>>> DCS_Theory_vs_Theta_deg_VarPhi_deg;
					std::unique_ptr<std::vector<std::vector <double>>> DCS_Theory_vs_R_Phi_deg;

					//理論値をプロット
					switch (static_cast<LibFlag::Sample>(i32SelectAtom))
					{
					case LibFlag::Sample::He:
						DCS_Theory_vs_Theta_deg = DCSTheoretical<Conv_1000_1000, Elastic32_He_1000eV>::make_1D_vs_Theta_deg(sinfoTheta_deg);
						DCS_Theory_vs_R = DCSTheoretical<Conv_1000_1000, Elastic32_He_1000eV>::make_1D_vs_R(sinfoR);
						DCS_Theory_vs_Theta_deg_VarPhi_deg = DCSTheoretical<Conv_1000_1000, Elastic32_He_1000eV>::make_2D_vs_Theta_deg_VarPhi_deg(sinfoTheta_deg, sinfoVarPhi_deg);
						DCS_Theory_vs_R_Phi_deg = DCSTheoretical<Conv_1000_1000, Elastic32_He_1000eV>::make_2D_vs_R_Phi_deg(sinfoR, sinfoPhi_deg);
						break;
					case LibFlag::Sample::C:
						DCS_Theory_vs_Theta_deg = DCSTheoretical<Conv_1000_1000, Elastic32_C_1000eV>::make_1D_vs_Theta_deg(sinfoTheta_deg);
						DCS_Theory_vs_R = DCSTheoretical<Conv_1000_1000, Elastic32_C_1000eV>::make_1D_vs_R(sinfoR);
						DCS_Theory_vs_Theta_deg_VarPhi_deg = DCSTheoretical<Conv_1000_1000, Elastic32_C_1000eV>::make_2D_vs_Theta_deg_VarPhi_deg(sinfoTheta_deg, sinfoVarPhi_deg);
						DCS_Theory_vs_R_Phi_deg = DCSTheoretical<Conv_1000_1000, Elastic32_C_1000eV>::make_2D_vs_R_Phi_deg(sinfoR, sinfoPhi_deg);
						break;
					case LibFlag::Sample::N:
						DCS_Theory_vs_Theta_deg = DCSTheoretical<Conv_1000_1000, Elastic32_N_1000eV>::make_1D_vs_Theta_deg(sinfoTheta_deg);
						DCS_Theory_vs_R = DCSTheoretical<Conv_1000_1000, Elastic32_N_1000eV>::make_1D_vs_R(sinfoR);
						DCS_Theory_vs_Theta_deg_VarPhi_deg = DCSTheoretical<Conv_1000_1000, Elastic32_N_1000eV>::make_2D_vs_Theta_deg_VarPhi_deg(sinfoTheta_deg, sinfoVarPhi_deg);
						DCS_Theory_vs_R_Phi_deg = DCSTheoretical<Conv_1000_1000, Elastic32_N_1000eV>::make_2D_vs_R_Phi_deg(sinfoR, sinfoPhi_deg);
						break;
					case LibFlag::Sample::Ne:
						DCS_Theory_vs_Theta_deg = DCSTheoretical<Conv_1000_1000, Elastic32_Ne_1000eV>::make_1D_vs_Theta_deg(sinfoTheta_deg);
						DCS_Theory_vs_R = DCSTheoretical<Conv_1000_1000, Elastic32_Ne_1000eV>::make_1D_vs_R(sinfoR);
						DCS_Theory_vs_Theta_deg_VarPhi_deg = DCSTheoretical<Conv_1000_1000, Elastic32_Ne_1000eV>::make_2D_vs_Theta_deg_VarPhi_deg(sinfoTheta_deg, sinfoVarPhi_deg);
						DCS_Theory_vs_R_Phi_deg = DCSTheoretical<Conv_1000_1000, Elastic32_Ne_1000eV>::make_2D_vs_R_Phi_deg(sinfoR, sinfoPhi_deg);
						break;
					case LibFlag::Sample::Cl:
						DCS_Theory_vs_Theta_deg = DCSTheoretical<Conv_1000_1000, Elastic32_Cl_1000eV>::make_1D_vs_Theta_deg(sinfoTheta_deg);
						DCS_Theory_vs_R = DCSTheoretical<Conv_1000_1000, Elastic32_Cl_1000eV>::make_1D_vs_R(sinfoR);
						DCS_Theory_vs_Theta_deg_VarPhi_deg = DCSTheoretical<Conv_1000_1000, Elastic32_Cl_1000eV>::make_2D_vs_Theta_deg_VarPhi_deg(sinfoTheta_deg, sinfoVarPhi_deg);
						DCS_Theory_vs_R_Phi_deg = DCSTheoretical<Conv_1000_1000, Elastic32_Cl_1000eV>::make_2D_vs_R_Phi_deg(sinfoR, sinfoPhi_deg);
						break;
					case LibFlag::Sample::Ar:
						DCS_Theory_vs_Theta_deg = DCSTheoretical<Conv_1000_1000, Elastic32_Ar_1000eV>::make_1D_vs_Theta_deg(sinfoTheta_deg);
						DCS_Theory_vs_R = DCSTheoretical<Conv_1000_1000, Elastic32_Ar_1000eV>::make_1D_vs_R(sinfoR);
						DCS_Theory_vs_Theta_deg_VarPhi_deg = DCSTheoretical<Conv_1000_1000, Elastic32_Ar_1000eV>::make_2D_vs_Theta_deg_VarPhi_deg(sinfoTheta_deg, sinfoVarPhi_deg);
						DCS_Theory_vs_R_Phi_deg = DCSTheoretical<Conv_1000_1000, Elastic32_Ar_1000eV>::make_2D_vs_R_Phi_deg(sinfoR, sinfoPhi_deg);
						break;
					case LibFlag::Sample::Xe:
						DCS_Theory_vs_Theta_deg = DCSTheoretical<Conv_1000_1000, Elastic32_Xe_1000eV>::make_1D_vs_Theta_deg(sinfoTheta_deg);
						DCS_Theory_vs_R = DCSTheoretical<Conv_1000_1000, Elastic32_Xe_1000eV>::make_1D_vs_R(sinfoR);
						DCS_Theory_vs_Theta_deg_VarPhi_deg = DCSTheoretical<Conv_1000_1000, Elastic32_Xe_1000eV>::make_2D_vs_Theta_deg_VarPhi_deg(sinfoTheta_deg, sinfoVarPhi_deg);
						DCS_Theory_vs_R_Phi_deg = DCSTheoretical<Conv_1000_1000, Elastic32_Xe_1000eV>::make_2D_vs_R_Phi_deg(sinfoR, sinfoPhi_deg);
						break;
					default:
						DCS_Theory_vs_Theta_deg.reset();
						DCS_Theory_vs_R.reset();
						DCS_Theory_vs_Theta_deg_VarPhi_deg.reset();
						DCS_Theory_vs_R_Phi_deg.reset();
						throw std::exception("Failure on making Unique ptr to DCSTheoretical curves");
					}


					
					//R, 積算が終わったらヒストグラムに感度補正割合を掛け算する
					//const auto& dsR = Histo::up2DetectorSensitivity->Ratio_vs_R();
					auto up2clb_R = std::make_unique<std::vector<double>>(Histo::up2DetectorSensitivity->Calibrate_Hist_R(*Histo::up2R));
					////誤差
					//const auto& dsR_e1 = Histo::up2DetectorSensitivity->Ratio_vs_R_Error1();
					//auto up2clb_R_e = std::make_unique<std::vector<double>>(Histo::up2DetectorSensitivity->ErrorOfCalibrated_Hist_R(*Histo::up2R));


					//Theta_deg, 積算が終わったらヒストグラムに感度補正割合を掛け算する
					//const auto& dsTheta_deg = Histo::up2DetectorSensitivity->Ratio_vs_Theta_deg();
					auto up2clb_Theta_deg = std::make_unique<std::vector<double>>(Histo::up2DetectorSensitivity->Calibrate_Hist_Theta_deg(*Histo::up2Theta_deg));
					auto up2clb_Theta_deg_e = std::make_unique<std::vector<double>>(Histo::up2DetectorSensitivity->ErrorOfCalibrated_Hist_Theta_deg(*Histo::up2Theta_deg));


					//R, Phi_deg, 積算が終わったらヒストグラムに感度補正割合を掛け算する
					//const auto& dsR_Phi_deg = Histo::up2DetectorSensitivity2D->Ratio_vs_R_Phi_deg();
					auto up2clb_R_Phi_deg = std::make_unique<std::vector<std::vector<double>>>(Histo::up2DetectorSensitivity2D->Calibrate_Hist_R_Phi_deg(*Histo::up2R_Phi_deg));
					////
					//const auto& dsR_Phi_deg_e1 = Histo::up2DetectorSensitivity2D->Ratio_vs_R_Phi_deg_Error1();
					//auto up2clb_R_Phi_deg_e = std::make_unique<std::vector<std::vector<double>>>(Vc::Op<double>(sqrt, dsR_Phi_deg_e1));


					//Theta_deg, VarPhi_deg, 積算が終わったらヒストグラムに感度補正割合を掛け算する
					//const auto& dsTheta_deg_VarPhi_deg = Histo::up2DetectorSensitivity2D->Ratio_vs_Theta_deg_VarPhi_deg();
					auto up2clb_Theta_deg_VarPhi_deg = std::make_unique<std::vector<std::vector<double>>>(Histo::up2DetectorSensitivity2D->Calibrate_Hist_Theta_deg_VarPhi_deg(*Histo::up2Theta_deg_VarPhi_deg));
					auto up2clb_Theta_deg_VarPhi_deg_e = std::make_unique<std::vector<std::vector<double>>>(Histo::up2DetectorSensitivity2D->ErrorOfCalibrated_Hist_Theta_deg_VarPhi_deg(*Histo::up2Theta_deg_VarPhi_deg));



					//itx へ出力
					file_output.WriteString(_T("IGOR\n"));



					//vs R, 感度補正なし
					StringsForIgorTextWave::WaveText(tstr, _T("hist_R_v"), _T("hist_R_h"), Histo::up2R->Get(), Histo::up2R->ScaleInfo());
					file_output.WriteString(tstr.c_str());

					//vs Theta_deg, 感度補正なし
					StringsForIgorTextWave::WaveText(tstr, _T("hist_Theta_deg_v"), _T("hist_Theta_deg_h"), Histo::up2Theta_deg->Get(), Histo::up2Theta_deg->ScaleInfo());
					file_output.WriteString(tstr.c_str());

					//vs R, 感度補正あり
					StringsForIgorTextWave::WaveText(tstr, _T("histc_R_v"), _T("histc_R_h"), *up2clb_R, sinfoR);
					file_output.WriteString(tstr.c_str());

					//vs R, 感度補正ありの誤差
					//StringsForIgorTextWave::WaveText(tstr, _T("histcE_R_v"), _T("histcE_R_h"), *up2clb_R_e, sinfoR);
					//file_output.WriteString(tstr.c_str());

					//vs Theta_deg, 感度補正あり
					StringsForIgorTextWave::WaveText(tstr, _T("histc_Theta_deg_v"), _T("histc_Theta_deg_h"), *up2clb_Theta_deg, sinfoTheta_deg);
					file_output.WriteString(tstr.c_str());

					//vs Theta_deg, 感度補正ありの誤差
					StringsForIgorTextWave::WaveText(tstr, _T("histcE_Theta_deg_v"), _T("histcE_Theta_deg_h"), *up2clb_Theta_deg_e, sinfoTheta_deg);
					file_output.WriteString(tstr.c_str());

					//vs R, DCS理論曲線
					StringsForIgorTextWave::WaveText(tstr, _T("DCS_R_v"), _T("DCS_R_h"), *DCS_Theory_vs_R, sinfoR);
					file_output.WriteString(tstr.c_str());

					//vs Theta_deg, DCS理論曲線
					StringsForIgorTextWave::WaveText(tstr, _T("DCS_Theta_deg_v"), _T("DCS_Theta_deg_h"), *DCS_Theory_vs_Theta_deg, sinfoTheta_deg);
					file_output.WriteString(tstr.c_str());

					//vs R, 検出器感度
					StringsForIgorTextWave::WaveText(tstr, _T("Ratio_R_v"), _T("Ratio_R_h"), Histo::up2DetectorSensitivity->Ratio_vs_R(), sinfoR);
					file_output.WriteString(tstr.c_str());
					
					//感度補正の元の相対誤差は感度補正係数の相対誤差に等しい
					const auto vvvv = Histo::up2DetectorSensitivity->Hist_Theta_deg().GetE1();
					//

					//vs Theta_deg, 検出器感度
					StringsForIgorTextWave::WaveText(tstr, _T("Ratio_Theta_deg_v"), _T("Ratio_Theta_deg_h"), Histo::up2DetectorSensitivity->Ratio_vs_Theta_deg(), sinfoTheta_deg);
					file_output.WriteString(tstr.c_str());









					//vs R, Phi_deg, 感度補正なし
					StringsForIgorTextWave::WaveText(tstr, _T("hist_R_Phi_deg_v"), _T("hist_R_Phi_deg_h0"), _T("hist_R_Phi_deg_h1"), Histo::up2R_Phi_deg->Get(), Histo::up2R_Phi_deg->ScaleInfo0(), Histo::up2R_Phi_deg->ScaleInfo1());
					file_output.WriteString(tstr.c_str());


					//vs R, Phi_deg, 感度補正あり
					StringsForIgorTextWave::WaveText(tstr, _T("histc_R_Phi_deg_v"), _T("histc_R_Phi_deg_h0"), _T("histc_R_Phi_deg_h1"), *up2clb_R_Phi_deg, sinfoR, sinfoPhi_deg);
					file_output.WriteString(tstr.c_str());


					//vs Theta_deg, VarPhi_deg, 感度補正なし
					StringsForIgorTextWave::WaveText(tstr, _T("hist_Theta_deg_VarPhi_deg_v"), _T("hist_Theta_deg_VarPhi_deg_h0"), _T("hist_Theta_deg_VarPhi_deg_h1"), Histo::up2Theta_deg_VarPhi_deg->Get(), Histo::up2Theta_deg_VarPhi_deg->ScaleInfo0(), Histo::up2Theta_deg_VarPhi_deg->ScaleInfo1());
					file_output.WriteString(tstr.c_str());

					
					//vs Theta_deg, VarPhi_deg, 感度補正あり
					StringsForIgorTextWave::WaveText(tstr, _T("histc_Theta_deg_VarPhi_deg_v"), _T("histc_Theta_deg_VarPhi_deg_h0"), _T("histc_Theta_deg_VarPhi_deg_h1"), *up2clb_Theta_deg_VarPhi_deg, sinfoTheta_deg, sinfoVarPhi_deg);
					file_output.WriteString(tstr.c_str());


					//vs Theta_deg, VarPhi_deg, 感度補正ありの誤差
					StringsForIgorTextWave::WaveText(tstr, _T("hcE_Theta_deg_VarPhi_deg_v"), _T("hcE_Theta_deg_VarPhi_deg_h0"), _T("hcE_Theta_deg_VarPhi_deg_h1"), *up2clb_Theta_deg_VarPhi_deg_e, sinfoTheta_deg, sinfoVarPhi_deg);
					file_output.WriteString(tstr.c_str());


					//vs R, rPhi_deg, DCS理論曲線
					StringsForIgorTextWave::WaveText(tstr, _T("DCS_R_Phi_deg_v"), _T("DCS_R_Phi_deg_h0"), _T("DCS_R_Phi_deg_h1"), *DCS_Theory_vs_R_Phi_deg, sinfoR, sinfoPhi_deg);
					file_output.WriteString(tstr.c_str());


					//vs Theta_deg, VarPhi_deg, DCS理論曲線
					StringsForIgorTextWave::WaveText(tstr, _T("DCS_Theta_deg_VarPhi_deg_v"), _T("DCS_Theta_deg_VarPhi_deg_h0"), _T("DCS_Theta_deg_VarPhi_deg_h1"), *DCS_Theory_vs_Theta_deg_VarPhi_deg, sinfoTheta_deg, sinfoVarPhi_deg);
					file_output.WriteString(tstr.c_str());

					
					//vs R, Phi_deg, 検出器感度
					StringsForIgorTextWave::WaveText(tstr, _T("Ratio_R_Phi_deg_v"), _T("Ratio_R_Phi_deg_h0"), _T("Ratio_R_Phi_deg_h1"), Histo::up2DetectorSensitivity2D->Ratio_vs_R_Phi_deg(), Histo::up2DetectorSensitivity2D->ScaleInfo_R(), Histo::up2DetectorSensitivity2D->ScaleInfo_Phi_deg());
					file_output.WriteString(tstr.c_str());


					//vs Theta_deg, VarPhi_deg, 検出器感度
					StringsForIgorTextWave::WaveText(tstr, _T("Ratio_Theta_deg_VarPhi_deg_v"), _T("Ratio_Theta_deg_VarPhi_deg_h0"), _T("Ratio_Theta_deg_VarPhi_deg_h1"), Histo::up2DetectorSensitivity2D->Ratio_vs_Theta_deg_VarPhi_deg(), Histo::up2DetectorSensitivity2D->ScaleInfo_Theta_deg(), Histo::up2DetectorSensitivity2D->ScaleInfo_VarPhi_deg());
					file_output.WriteString(tstr.c_str());


					const auto& raw = Histo::up2DetectorSensitivity->Hist_R().GetRaw();
					StringsForIgorTextWave::WaveText(tstr, _T("SEraw_R_v"), _T("hist_R_h"), raw, Histo::up2R->ScaleInfo());
					file_output.WriteString(tstr.c_str());



				} // isOK



				file_output.Close();
			}


		}
		catch (std::out_of_range& ex) {
			AfxMessageBox(ex.what());
		}
		catch (std::exception& ex) {
			AfxMessageBox(ex.what());
			
		}

		return 0;

	}

#endif // LAES2_ANALYSIS_DEFINED
#endif // LAES2_ANALYSIS_200313




//-----------200918------------//
#ifdef LAES2_ANALYSIS_200918
#ifndef LAES2_ANALYSIS_DEFINED
#define LAES2_ANALYSIS_DEFINED//二重定義防止



	//Parameter 1100 : どの層組をPosに選ぶ？
	__int32 i32SelectLayerPair;
	enum LayerPair { //switchの選択肢
		UV = 0,
		VW = 1,
		UW = 2,
	};



	//Parameter 1101 : 外部ファイルへ書き込む？
	__int32 i32WriteCoordinatesToFile;
	//Parameter 1102 : 外部ファイルへ書き込むときどのConditionを適用する？
	__int32 i32WriteCoordinatesToFile_Condition;


	//Parameter 1103 : 外部ファイルへ書き込む？
	__int32 i32WriteCoordinatesToFile1;
	//Parameter 1104 : 外部ファイルへ書き込むときどのConditionを適用する？もう一つのファイル
	__int32 i32WriteCoordinatesToFile_Condition1;


	//Parameter 1105 : 外部ファイルへ書き込む？
	__int32 i32WriteCoordinatesToFile2;
	//Parameter 1106 : 外部ファイルへ書き込むときどのConditionを適用する？もう一つのファイル
	__int32 i32WriteCoordinatesToFile_Condition2;


	//Parameter 1107 : 外部ファイルへ書き込む？
	__int32 i32WriteCoordinatesToFile3;
	//Parameter 1108 : 外部ファイルへ書き込むときどのConditionを適用する？もう一つのファイル
	__int32 i32WriteCoordinatesToFile_Condition3;


	//Parameter 1109 : 外部ファイルへ書き込む？
	__int32 i32WriteCoordinatesToFile4;
	//Parameter 1110 : 外部ファイルへ書き込むときどのConditionを適用する？もう一つのファイル
	__int32 i32WriteCoordinatesToFile_Condition4;





	//Parameter 1111 : 計算する気体原子（後で分子も）の種類
	__int32 i32SelectAtom;

	

	

	//Parameter 1114 : 検出器半径分布のbin[mm]
	double dBinR;

	//Parameter 1115 : 検出器半径R分布の最大値[mm]
	double dRmax;

	//Parameter 1116 : VarPhiのフォーマット
	__int32 i32VarPhiConversion;

	//Parameter 1117 : VarPhiの刻み数
	__int32 i32VarPhiNumSteps;


	//Parameter 1118 : 検出器PosXのbin[mm]
	double dBinPosX;

	//Parameter 1119 : 検出器PosYのbin[mm]
	double dBinPosY;

	

	//Parameter1120 : IR有りのときの遅延発生器DG645の値[ns]
	double dTOF_Offset_WithIR;

	//Parameter1121 : IR無しのときの遅延発生器DG645の値[ns]
	double dTOF_Offset_WithoutIR;

	//Parameter1125 : 外部へエネルギーシフト信号を書き出したかったら1，そうでないならそれ以外
	__int32 i32Output_HistogramsEvents_LAES;




	//C++プログラムでConditionを使う
	std::unique_ptr<CCFileCondition> up2CCFileCondition;


	

	//知りたいCoordinateをグローバル変数に代入
	namespace Crd {
		double AbsoluteEventTime;			// ns since start
		double DeltaEventTime;				// This Time - PreviousTime
		double dEventCounter;
		__int32 ConsistencyIndicator;

		__int32 counts[32];
		double x1, x2, y1, y2, z1, z2;
		double TOF_ns;
		double raw_x, raw_y, raw_w;
		double sumx, sumy, sumw, sumxyw, diffxy;
		double raw_sumx, raw_sumy, raw_sumw, raw_sumxyw, raw_diffxy;
		double PosX, PosY;
		double r, phi;
		double Xuv, Yuv, Xuw, Yuw, Xvw, Yvw;
		double dX, dY;


		//@------ここに変数を追加--------

		__int32 M_ConsistencyIndicator;		//MCP信号も考慮したConsistency Indecator
		__int32 NextHitIndicator;			//NextHitについてのConsistency Indecator
		__int32 M_NextHitIndicator;			//NextHitについてMCP信号も考慮したConsistency Indecator
		double TOF_ns_NoOffset;				//遅延発生器の時間を除いた飛行時間

		double Theta_deg;					//散乱後エネルギー1keVの極角
		double VarPhi_deg;					//散乱後エネルギー1keVの方位角

		double KineticEnergy;				//散乱後の運動エネルギー[eV]
		//-------------------------------
	}


	//ヒストグラムをまとめて作る
	class HistoProc {
	public:
		HistoProc()
			:
			up2CI(nullptr),
			up2DetectorSensitivity(nullptr),
			up2Theta_deg(nullptr),
			up2Theta_deg_VarPhi(nullptr),
			up2R(nullptr),
			up2R_Phi(nullptr),
			up2RawTheta_deg(nullptr),
			up2RawTheta_deg_VarPhi(nullptr),
			up2RawR(nullptr),
			up2RawR_Phi(nullptr),
			m_isOK(false)
		{
			try {
				//区間の個数
				//I0=(0,R1], ... , I_(n-1)=(R_(n-1), R_n], I_n=(R_n, R_max]
				const size_t numIntervals = static_cast<size_t>(ceil(dRmax / dBinR));
				const size_t Dimsize_Phi_deg = static_cast<size_t>(i32VarPhiNumSteps > 0 ? i32VarPhiNumSteps : 12);//Phiのサイズ

				auto pinf_CI = make_PointInformation(64, PointInformation::Difference::Central0);
				auto pinf = make_PointInformation(numIntervals, PointInformation::Difference::Backward);


				up2CI = make_ScaledHistogram1D(pinf_CI);

				up2DetectorSensitivity = DetectorSensitivity1_1000_1000::make(numIntervals, dBinR, Dimsize_Phi_deg, static_cast<LibFlag::AzimuthFormat>(i32PhiConversion));

				up2Theta_deg = DCSHistogram_1000_1000::make_1D_vs_Theta_deg(numIntervals, dBinR);
				up2Theta_deg_VarPhi = DCSHistogram_1000_1000::make_2D_vs_Theta_deg_VarPhi(numIntervals, dBinR, Dimsize_Phi_deg, static_cast<LibFlag::AzimuthFormat>(i32VarPhiConversion));

				up2R = DCSHistogram_1000_1000::make_1D_vs_R(numIntervals, dBinR);
				up2R_Phi = DCSHistogram_1000_1000::make_2D_vs_R_Phi(numIntervals, dBinR, Dimsize_Phi_deg, static_cast<LibFlag::AzimuthFormat>(i32PhiConversion));

				up2RawTheta_deg = DCSHistogram<Conv_1000_1000>::make_1D_vs_Theta_deg(numIntervals, dBinR);
				up2RawTheta_deg_VarPhi = DCSHistogram<Conv_1000_1000>::make_2D_vs_Theta_deg_VarPhi(numIntervals, dBinR, Dimsize_Phi_deg, static_cast<LibFlag::AzimuthFormat>(i32VarPhiConversion));

				up2RawR = DCSHistogram<Conv_1000_1000>::make_1D_vs_R(numIntervals, dBinR);
				up2RawR_Phi = DCSHistogram<Conv_1000_1000>::make_2D_vs_R_Phi(numIntervals, dBinR, Dimsize_Phi_deg, static_cast<LibFlag::AzimuthFormat>(i32PhiConversion));

				m_isOK = true;
			}
			catch (std::exception& ex) {

				up2CI.reset();

				up2Theta_deg.reset();
				up2Theta_deg_VarPhi.reset();

				up2R.reset();
				up2R_Phi.reset();

				up2RawTheta_deg.reset();
				up2RawTheta_deg_VarPhi.reset();

				up2RawR.reset();
				up2RawR_Phi.reset();

				up2DetectorSensitivity.reset();

				m_isOK = false;

				AfxMessageBox(ex.what());
			}
		}
		~HistoProc() = default;

		//Crdに全て書き終わってから実行する
		void AddToHist() {
			try {
				//コンストラクタでエラーなし
				if (m_isOK) {

					//座標計算でスペクトルレンジ外なら例外

					up2CI->AddValue(static_cast<double>(Crd::ConsistencyIndicator), 1, 1);


					//感度補正による正味のカウント数を加算
					auto w_we = up2DetectorSensitivity->Weight(Crd::r, Crd::phi);
					double we1 = w_we.second / w_we.first;
					double SignalAmount = w_we.first;
					double SignalError = w_we.first * sqrt(1 + we1 * we1);

					up2Theta_deg->AddValue(Crd::Theta_deg, SignalAmount, SignalError);
					up2Theta_deg_VarPhi->AddValue(Crd::Theta_deg, Crd::VarPhi_deg, SignalAmount, SignalError);

					up2R->AddValue(Crd::r, SignalAmount, SignalError);
					up2R_Phi->AddValue(Crd::r, Crd::phi, SignalAmount, SignalError);


					//感度補正なし
					up2RawTheta_deg->AddValue(Crd::Theta_deg, 1, 1);
					up2RawTheta_deg_VarPhi->AddValue(Crd::Theta_deg, Crd::VarPhi_deg, 1, 1);

					up2RawR->AddValue(Crd::r, 1, 1);
					up2RawR_Phi->AddValue(Crd::r, Crd::phi, 1, 1);

				}
			}
			catch (std::exception&) {
				throw;
			}
		}

		//全部積算終わったらファイルへ書き出す
		void WriteToIgorTextFile(CStdioFile& file_output, const bool Put_IGOR_First, const std::basic_string<TCHAR>& Wave_Comment, const std::basic_string<TCHAR>& WaveName_Suffix) {
			try {
				if (m_isOK) {

					std::basic_string<TCHAR> tstr;

					//スケール情報
					const auto& sinfoR = up2R_Phi->ScaleInfo0();
					const auto& sinfoTheta_deg = up2Theta_deg_VarPhi->ScaleInfo0();

					const auto& sinfoVarPhi = up2Theta_deg_VarPhi->ScaleInfo1();
					const auto& sinfoPhi = up2R_Phi->ScaleInfo1();


					//DCSの理論曲線
					std::unique_ptr<std::vector <double>> DCS_Theory_vs_Theta_deg;
					std::unique_ptr<std::vector <double>> DCS_Theory_vs_R;

					std::unique_ptr<std::vector<std::vector <double>>> DCS_Theory_vs_Theta_deg_VarPhi;
					std::unique_ptr<std::vector<std::vector <double>>> DCS_Theory_vs_R_Phi;

					//理論値をプロット
					switch (static_cast<LibFlag::Sample>(i32SelectAtom))
					{
					case LibFlag::Sample::He:
						DCS_Theory_vs_Theta_deg = DCSTheoretical<Conv_1000_1000, Elastic32_He_1000eV>::make_1D_vs_Theta_deg(sinfoTheta_deg);
						DCS_Theory_vs_R = DCSTheoretical<Conv_1000_1000, Elastic32_He_1000eV>::make_1D_vs_R(sinfoR);
						DCS_Theory_vs_Theta_deg_VarPhi = DCSTheoretical<Conv_1000_1000, Elastic32_He_1000eV>::make_2D_vs_Theta_deg_VarPhi(sinfoTheta_deg, sinfoVarPhi);
						DCS_Theory_vs_R_Phi = DCSTheoretical<Conv_1000_1000, Elastic32_He_1000eV>::make_2D_vs_R_Phi(sinfoR, sinfoPhi);
						break;
					case LibFlag::Sample::C:
						DCS_Theory_vs_Theta_deg = DCSTheoretical<Conv_1000_1000, Elastic32_C_1000eV>::make_1D_vs_Theta_deg(sinfoTheta_deg);
						DCS_Theory_vs_R = DCSTheoretical<Conv_1000_1000, Elastic32_C_1000eV>::make_1D_vs_R(sinfoR);
						DCS_Theory_vs_Theta_deg_VarPhi = DCSTheoretical<Conv_1000_1000, Elastic32_C_1000eV>::make_2D_vs_Theta_deg_VarPhi(sinfoTheta_deg, sinfoVarPhi);
						DCS_Theory_vs_R_Phi = DCSTheoretical<Conv_1000_1000, Elastic32_C_1000eV>::make_2D_vs_R_Phi(sinfoR, sinfoPhi);
						break;
					case LibFlag::Sample::N:
						DCS_Theory_vs_Theta_deg = DCSTheoretical<Conv_1000_1000, Elastic32_N_1000eV>::make_1D_vs_Theta_deg(sinfoTheta_deg);
						DCS_Theory_vs_R = DCSTheoretical<Conv_1000_1000, Elastic32_N_1000eV>::make_1D_vs_R(sinfoR);
						DCS_Theory_vs_Theta_deg_VarPhi = DCSTheoretical<Conv_1000_1000, Elastic32_N_1000eV>::make_2D_vs_Theta_deg_VarPhi(sinfoTheta_deg, sinfoVarPhi);
						DCS_Theory_vs_R_Phi = DCSTheoretical<Conv_1000_1000, Elastic32_N_1000eV>::make_2D_vs_R_Phi(sinfoR, sinfoPhi);
						break;
					case LibFlag::Sample::Ne:
						DCS_Theory_vs_Theta_deg = DCSTheoretical<Conv_1000_1000, Elastic32_Ne_1000eV>::make_1D_vs_Theta_deg(sinfoTheta_deg);
						DCS_Theory_vs_R = DCSTheoretical<Conv_1000_1000, Elastic32_Ne_1000eV>::make_1D_vs_R(sinfoR);
						DCS_Theory_vs_Theta_deg_VarPhi = DCSTheoretical<Conv_1000_1000, Elastic32_Ne_1000eV>::make_2D_vs_Theta_deg_VarPhi(sinfoTheta_deg, sinfoVarPhi);
						DCS_Theory_vs_R_Phi = DCSTheoretical<Conv_1000_1000, Elastic32_Ne_1000eV>::make_2D_vs_R_Phi(sinfoR, sinfoPhi);
						break;
					case LibFlag::Sample::Cl:
						DCS_Theory_vs_Theta_deg = DCSTheoretical<Conv_1000_1000, Elastic32_Cl_1000eV>::make_1D_vs_Theta_deg(sinfoTheta_deg);
						DCS_Theory_vs_R = DCSTheoretical<Conv_1000_1000, Elastic32_Cl_1000eV>::make_1D_vs_R(sinfoR);
						DCS_Theory_vs_Theta_deg_VarPhi = DCSTheoretical<Conv_1000_1000, Elastic32_Cl_1000eV>::make_2D_vs_Theta_deg_VarPhi(sinfoTheta_deg, sinfoVarPhi);
						DCS_Theory_vs_R_Phi = DCSTheoretical<Conv_1000_1000, Elastic32_Cl_1000eV>::make_2D_vs_R_Phi(sinfoR, sinfoPhi);
						break;
					case LibFlag::Sample::Ar:
						DCS_Theory_vs_Theta_deg = DCSTheoretical<Conv_1000_1000, Elastic32_Ar_1000eV>::make_1D_vs_Theta_deg(sinfoTheta_deg);
						DCS_Theory_vs_R = DCSTheoretical<Conv_1000_1000, Elastic32_Ar_1000eV>::make_1D_vs_R(sinfoR);
						DCS_Theory_vs_Theta_deg_VarPhi = DCSTheoretical<Conv_1000_1000, Elastic32_Ar_1000eV>::make_2D_vs_Theta_deg_VarPhi(sinfoTheta_deg, sinfoVarPhi);
						DCS_Theory_vs_R_Phi = DCSTheoretical<Conv_1000_1000, Elastic32_Ar_1000eV>::make_2D_vs_R_Phi(sinfoR, sinfoPhi);
						break;
					case LibFlag::Sample::Xe:
						DCS_Theory_vs_Theta_deg = DCSTheoretical<Conv_1000_1000, Elastic32_Xe_1000eV>::make_1D_vs_Theta_deg(sinfoTheta_deg);
						DCS_Theory_vs_R = DCSTheoretical<Conv_1000_1000, Elastic32_Xe_1000eV>::make_1D_vs_R(sinfoR);
						DCS_Theory_vs_Theta_deg_VarPhi = DCSTheoretical<Conv_1000_1000, Elastic32_Xe_1000eV>::make_2D_vs_Theta_deg_VarPhi(sinfoTheta_deg, sinfoVarPhi);
						DCS_Theory_vs_R_Phi = DCSTheoretical<Conv_1000_1000, Elastic32_Xe_1000eV>::make_2D_vs_R_Phi(sinfoR, sinfoPhi);
						break;
					default:
						DCS_Theory_vs_Theta_deg.reset();
						DCS_Theory_vs_R.reset();
						DCS_Theory_vs_Theta_deg_VarPhi.reset();
						DCS_Theory_vs_R_Phi.reset();
						throw std::exception("Failure on making Unique ptr to DCSTheoretical curves");
					}



					//横軸 bin の代表値を取得する関数　一次元グラフ用
					auto ToScales = [](const ScaleInformation& sci) {
						std::vector<double> v;
						v.reserve(sci.NumIntervals());
						for (size_t i = 0; i < sci.NumIntervals(); i++) {
							v.push_back(sci.IntervalIndex2XRepresentative(i));
						}
						return std::move(v);
					};
					//横軸 bin の境界を取得する関数　二次元イメージ用
					auto ToBoundaries = [](const ScaleInformation& sci) {
						std::vector<double> v;
						v.reserve(sci.NumIntervals() + 1);
						for (size_t i = 0; i < sci.NumIntervals(); i++) {
							v.push_back(sci.Interval(i).first);
						}
						v.push_back(sci.Interval(sci.NumIntervals() - 1).second);
						return std::move(v);
					};

					//ウェーブ名の先頭と末尾に文字列つける
					auto SetName = [&WaveName_Suffix](const std::basic_string<TCHAR>&& Name) {
						const auto Src = CurrentDAqInfo::SourceLMFileName();//ソースlmf名称を先頭に
						return Src + Name + WaveName_Suffix;
					};

					//itx へ出力
					if (Put_IGOR_First) {
						file_output.WriteString(_T("IGOR\n"));
					}

					//積算を終えたらCalibrateを実行して結果を記録する
					StringsForIgorTextWave::WaveText1(tstr,
						{
							{ToScales(sinfoR),SetName(_T("g_R")), _T("For 1D graphs")},//横軸スケール R
							{ToScales(sinfoTheta_deg),SetName(_T("g_Th")),_T("For 1D graphs")},//横軸スケール Theta_deg

							{up2RawR->Get(),SetName(_T("h_R")), Wave_Comment},//vs R, 感度補正なし
							{up2RawR->GetE(),SetName(_T("hE_R")), Wave_Comment},//vs R, 感度補正なし


							{up2RawTheta_deg->Get(), SetName(_T("h_Th")),Wave_Comment},//vs Theta_deg, 感度補正なし
							{up2RawTheta_deg->GetE(), SetName(_T("hE_Th")),Wave_Comment},//vs Theta_deg, 感度補正なし


							{std::move(*DCS_Theory_vs_R),_T("DCS_R"),_T("Elastic32, 1000 eV")},//vs R, DCS理論曲線
							{std::move(*DCS_Theory_vs_Theta_deg),_T("DCS_Th"),_T("Elastic32, 1000 eV")},//vs Theta_deg, DCS理論曲線


							{up2Theta_deg->Get(),SetName(_T("c_Th")),Wave_Comment},//vs Theta_deg, 感度補正あり
							{up2Theta_deg->GetE(),SetName(_T("cE_Th")),Wave_Comment},//vs Theta_deg, 感度補正ありの誤差

						}
					);
					file_output.WriteString(tstr.c_str());


					StringsForIgorTextWave::WaveText1(tstr,
						{
							{ToBoundaries(sinfoR),SetName(_T("i_R")),_T("For 2D images")},//横軸境界 R
							{ToBoundaries(sinfoTheta_deg),SetName(_T("i_Th")),_T("For 2D images")},//横軸境界 Theta_deg

							{ToBoundaries(sinfoPhi),SetName(_T("i_Ph")),_T("For 2D images")},//横軸境界 Phi_deg
							{ToBoundaries(sinfoVarPhi),SetName(_T("i_VPh")),_T("For 2D images")},//横軸境界 VarPhi_deg
						}
					);
					file_output.WriteString(tstr.c_str());


					StringsForIgorTextWave::WaveText2(tstr,
						{
							{up2RawR_Phi->Get(), SetName(_T("h_R_Ph")), Wave_Comment},//vs R, Phi_deg, 感度補正なし
							{up2RawR_Phi->GetE(), SetName(_T("hE_R_Ph")), Wave_Comment},//vs R, Phi_deg, 感度補正なし

							{up2RawTheta_deg_VarPhi->Get(),SetName(_T("h_Th_VPh")), Wave_Comment},//vs Theta_deg, VarPhi, 感度補正なし
							{up2RawTheta_deg_VarPhi->GetE(),SetName(_T("hE_Th_VPh")),Wave_Comment},//vs Theta_deg, VarPhi, 感度補正なし

							{std::move(*DCS_Theory_vs_R_Phi),_T("DCS_R_Ph"),_T("Elastic32, 1000 eV")},//vs R, rPhi_deg, DCS理論曲面
							{std::move(*DCS_Theory_vs_Theta_deg_VarPhi),_T("DCS_Th_VPh"),_T("Elastic32, 1000 eV")},//vs Theta_deg, VarPhi_deg, DCS理論曲面

							{up2DetectorSensitivity->DetectorSensitivityDistribution(),_T("Ratio_R_Ph"),_T("Ei = 1000 eV")},//vs R, Phi, 検出器感度
							{up2DetectorSensitivity->DetectorSensitivityDistributionE(),_T("RatioE_R_Ph"),_T("Ei = 1000 eV")},//vs R, Phi, 検出器感度の誤差


							{up2Theta_deg_VarPhi->Get(), SetName(_T("c_Th_VPh")), Wave_Comment},//vs Theta_deg, VarPhi, 感度補正あり
							{up2Theta_deg_VarPhi->GetE(), SetName(_T("cE_Th_VPh")), Wave_Comment},
						}
					);
					file_output.WriteString(tstr.c_str());


				} // isOK

			}
			catch (std::exception&) {
				throw;
			}
		}



		bool isOK()const noexcept {
			return m_isOK;
		}

		//CI
		std::unique_ptr<ScaledHistogram1D> up2CI;

		//1000 eVでの感度
		std::unique_ptr<DetectorSensitivity1_1000_1000> up2DetectorSensitivity;

		//感度補正有り
		std::unique_ptr<ScaledHistogram1D> up2Theta_deg;
		std::unique_ptr<ScaledHistogram2D> up2Theta_deg_VarPhi;
		std::unique_ptr<ScaledHistogram1D> up2R;
		std::unique_ptr<ScaledHistogram2D> up2R_Phi;

		//感度補正なし
		std::unique_ptr<ScaledHistogram1D> up2RawTheta_deg;
		std::unique_ptr<ScaledHistogram2D> up2RawTheta_deg_VarPhi;
		std::unique_ptr<ScaledHistogram1D> up2RawR;
		std::unique_ptr<ScaledHistogram2D> up2RawR_Phi;

	private:
		//状態
		bool m_isOK;
	};



	/// <summary>
	/// ヒストグラムをまとめて作る
	/// </summary>
	/// <typeparam name="class_DCSHistogram_Ei_Ef"> : 入射エネルギー Ei, 出射エネルギー Ef に対応する変換を用いたヒストグラム</typeparam>
	/// <typeparam name="class_DetectorSensitivity_Ei"> : 検出器衝突時エネルギー Ef のときの検出器感度</typeparam>
	template<class class_DCSHistogram_Ei_Ef, class class_DetectorSensitivity_Ef>
	class HistoProc1 {
	public:
		HistoProc1()
			:
			up2CI(nullptr),
			up2DetectorSensitivity(nullptr),
			up2Theta_deg(nullptr),
			up2Theta_deg_VarPhi(nullptr),
			up2R(nullptr),
			up2R_Phi(nullptr),
			up2RawTheta_deg(nullptr),
			up2RawTheta_deg_VarPhi(nullptr),
			up2RawR(nullptr),
			up2RawR_Phi(nullptr),
			m_isOK(false)
		{
			try {
				//区間の個数
				//I0=(0,R1], ... , I_(n-1)=(R_(n-1), R_n], I_n=(R_n, R_max]
				const size_t numIntervals = static_cast<size_t>(ceil(dRmax / dBinR));
				const size_t Dimsize_Phi_deg = static_cast<size_t>(i32VarPhiNumSteps > 0 ? i32VarPhiNumSteps : 12);//Phiのサイズ

				auto pinf_CI = make_PointInformation(64, PointInformation::Difference::Central0);
				auto pinf = make_PointInformation(numIntervals, PointInformation::Difference::Backward);


				up2CI = make_ScaledHistogram1D(pinf_CI);

				up2DetectorSensitivity = class_DetectorSensitivity_Ef::make(numIntervals, dBinR, Dimsize_Phi_deg, static_cast<LibFlag::AzimuthFormat>(i32PhiConversion));

				up2Theta_deg = class_DCSHistogram_Ei_Ef::make_1D_vs_Theta_deg(numIntervals, dBinR);
				up2Theta_deg_VarPhi = class_DCSHistogram_Ei_Ef::make_2D_vs_Theta_deg_VarPhi(numIntervals, dBinR, Dimsize_Phi_deg, static_cast<LibFlag::AzimuthFormat>(i32VarPhiConversion));

				up2R = class_DCSHistogram_Ei_Ef::make_1D_vs_R(numIntervals, dBinR);
				up2R_Phi = class_DCSHistogram_Ei_Ef::make_2D_vs_R_Phi(numIntervals, dBinR, Dimsize_Phi_deg, static_cast<LibFlag::AzimuthFormat>(i32PhiConversion));

				up2RawTheta_deg = class_DCSHistogram_Ei_Ef::make_1D_vs_Theta_deg(numIntervals, dBinR);
				up2RawTheta_deg_VarPhi = class_DCSHistogram_Ei_Ef::make_2D_vs_Theta_deg_VarPhi(numIntervals, dBinR, Dimsize_Phi_deg, static_cast<LibFlag::AzimuthFormat>(i32VarPhiConversion));

				up2RawR = class_DCSHistogram_Ei_Ef::make_1D_vs_R(numIntervals, dBinR);
				up2RawR_Phi = class_DCSHistogram_Ei_Ef::make_2D_vs_R_Phi(numIntervals, dBinR, Dimsize_Phi_deg, static_cast<LibFlag::AzimuthFormat>(i32PhiConversion));

				m_isOK = true;
			}
			catch (std::exception& ex) {

				up2CI.reset();

				up2Theta_deg.reset();
				up2Theta_deg_VarPhi.reset();

				up2R.reset();
				up2R_Phi.reset();

				up2RawTheta_deg.reset();
				up2RawTheta_deg_VarPhi.reset();

				up2RawR.reset();
				up2RawR_Phi.reset();

				up2DetectorSensitivity.reset();

				m_isOK = false;

				AfxMessageBox(ex.what());
			}
		}
		~HistoProc1() = default;

		//Crdに全て書き終わってから実行する
		void AddToHist() {
			try {
				//コンストラクタでエラーなし
				if (m_isOK) {

					//座標計算でスペクトルレンジ外なら例外

					up2CI->AddValue(static_cast<double>(Crd::ConsistencyIndicator), 1, 1);


					//感度補正による正味のカウント数を加算
					auto w_we = up2DetectorSensitivity->Weight(Crd::r, Crd::phi);
					double we1 = w_we.second / w_we.first;
					double SignalAmount = w_we.first;
					double SignalError = w_we.first * sqrt(1 + we1 * we1);

					up2Theta_deg->AddValue(Crd::Theta_deg, SignalAmount, SignalError);
					up2Theta_deg_VarPhi->AddValue(Crd::Theta_deg, Crd::VarPhi_deg, SignalAmount, SignalError);

					up2R->AddValue(Crd::r, SignalAmount, SignalError);
					up2R_Phi->AddValue(Crd::r, Crd::phi, SignalAmount, SignalError);


					//感度補正なし
					up2RawTheta_deg->AddValue(Crd::Theta_deg, 1, 1);
					up2RawTheta_deg_VarPhi->AddValue(Crd::Theta_deg, Crd::VarPhi_deg, 1, 1);

					up2RawR->AddValue(Crd::r, 1, 1);
					up2RawR_Phi->AddValue(Crd::r, Crd::phi, 1, 1);

				}
			}
			catch (std::exception&) {
				throw;
			}
		}

		//全部積算終わったらファイルへ書き出す
		void WriteToIgorTextFile(CStdioFile& file_output, const bool Put_IGOR_First, const std::basic_string<TCHAR>& Wave_Comment, const std::basic_string<TCHAR>& WaveName_Preffix, const std::basic_string<TCHAR>& WaveName_Suffix,const bool bOutputTheoreticalDCSCurves) {
			try {
				if (m_isOK) {

					std::basic_string<TCHAR> tstr;

					//スケール情報
					const auto& sinfoR = up2R_Phi->ScaleInfo0();
					const auto& sinfoTheta_deg = up2Theta_deg_VarPhi->ScaleInfo0();

					const auto& sinfoVarPhi = up2Theta_deg_VarPhi->ScaleInfo1();
					const auto& sinfoPhi = up2R_Phi->ScaleInfo1();


					//横軸 bin の代表値を取得する関数　一次元グラフ用
					auto ToScales = [](const ScaleInformation& sci) {
						std::vector<double> v;
						v.reserve(sci.NumIntervals());
						for (size_t i = 0; i < sci.NumIntervals(); i++) {
							v.push_back(sci.IntervalIndex2XRepresentative(i));
						}
						return std::move(v);
					};
					//横軸 bin の境界を取得する関数　二次元イメージ用
					auto ToBoundaries = [](const ScaleInformation& sci) {
						std::vector<double> v;
						v.reserve(sci.NumIntervals() + 1);
						for (size_t i = 0; i < sci.NumIntervals(); i++) {
							v.push_back(sci.Interval(i).first);
						}
						v.push_back(sci.Interval(sci.NumIntervals() - 1).second);
						return std::move(v);
					};


					//ウェーブ名の先頭と末尾に文字列つける
					auto SetName = [&WaveName_Preffix,&WaveName_Suffix](const std::basic_string<TCHAR>&& Name) {
						return WaveName_Preffix + Name + WaveName_Suffix;
					};


					//itx へ出力
					if (Put_IGOR_First) {
						file_output.WriteString(_T("IGOR\n"));
					}

					//積算を終えたらCalibrateを実行して結果を記録する
					StringsForIgorTextWave::WaveText1(tstr,
						{
							{ToScales(sinfoR),SetName(_T("g_R")), _T("For 1D graphs")},//横軸スケール R
							{ToScales(sinfoTheta_deg),SetName(_T("g_Th")),_T("For 1D graphs")},//横軸スケール Theta_deg

							{up2RawR->Get(),SetName(_T("h_R")), Wave_Comment},//vs R, 感度補正なし
							{up2RawR->GetE(),SetName(_T("hE_R")), Wave_Comment},//vs R, 感度補正なし


							{up2RawTheta_deg->Get(), SetName(_T("h_Th")),Wave_Comment},//vs Theta_deg, 感度補正なし
							{up2RawTheta_deg->GetE(), SetName(_T("hE_Th")),Wave_Comment},//vs Theta_deg, 感度補正なし

							
							{up2Theta_deg->Get(),SetName(_T("c_Th")),Wave_Comment},//vs Theta_deg, 感度補正あり
							{up2Theta_deg->GetE(),SetName(_T("cE_Th")),Wave_Comment},//vs Theta_deg, 感度補正ありの誤差

						}
					);
					file_output.WriteString(tstr.c_str());


					StringsForIgorTextWave::WaveText1(tstr,
						{
							{ToBoundaries(sinfoR),SetName(_T("i_R")),_T("For 2D images")},//横軸境界 R
							{ToBoundaries(sinfoTheta_deg),SetName(_T("i_Th")),_T("For 2D images")},//横軸境界 Theta_deg

							{ToBoundaries(sinfoPhi),SetName(_T("i_Ph")),_T("For 2D images")},//横軸境界 Phi
							{ToBoundaries(sinfoVarPhi),SetName(_T("i_VPh")),_T("For 2D images")},//横軸境界 VarPhi
						}
					);
					file_output.WriteString(tstr.c_str());


					StringsForIgorTextWave::WaveText2(tstr,
						{
							{up2RawR_Phi->Get(), SetName(_T("h_R_Ph")), Wave_Comment},//vs R, Phi, 感度補正なし
							{up2RawR_Phi->GetE(), SetName(_T("hE_R_Ph")), Wave_Comment},//vs R, Phi, 感度補正なし

							{up2RawTheta_deg_VarPhi->Get(),SetName(_T("h_Th_VPh")), Wave_Comment},//vs Theta_deg, VarPhi, 感度補正なし
							{up2RawTheta_deg_VarPhi->GetE(),SetName(_T("hE_Th_VPh")),Wave_Comment},//vs Theta_deg, VarPhi, 感度補正なし

							
							{up2DetectorSensitivity->DetectorSensitivityDistribution(),_T("Ratio_R_Ph"),_T("Ei = 1000 eV")},//vs R, Phi, 検出器感度
							{up2DetectorSensitivity->DetectorSensitivityDistributionE(),_T("RatioE_R_Ph"),_T("Ei = 1000 eV")},//vs R, Phi, 検出器感度の誤差


							{up2Theta_deg_VarPhi->Get(), SetName(_T("c_Th_VPh")), Wave_Comment},//vs Theta_deg, VarPhi, 感度補正あり
							{up2Theta_deg_VarPhi->GetE(), SetName(_T("cE_Th_VPh")), Wave_Comment},
						}
					);
					file_output.WriteString(tstr.c_str());


					//DCSの理論曲線も描く？
					if (bOutputTheoreticalDCSCurves) {

						std::unique_ptr<std::vector <double>> DCS_Theory_vs_Theta_deg;
						std::unique_ptr<std::vector <double>> DCS_Theory_vs_R;

						std::unique_ptr<std::vector<std::vector <double>>> DCS_Theory_vs_Theta_deg_VarPhi;
						std::unique_ptr<std::vector<std::vector <double>>> DCS_Theory_vs_R_Phi;

						//理論値をプロット
						switch (static_cast<LibFlag::Sample>(i32SelectAtom))
						{
						case LibFlag::Sample::He:
							DCS_Theory_vs_Theta_deg = DCSTheoretical<Conv_1000_1000, Elastic32_He_1000eV>::make_1D_vs_Theta_deg(sinfoTheta_deg);
							DCS_Theory_vs_R = DCSTheoretical<Conv_1000_1000, Elastic32_He_1000eV>::make_1D_vs_R(sinfoR);
							DCS_Theory_vs_Theta_deg_VarPhi = DCSTheoretical<Conv_1000_1000, Elastic32_He_1000eV>::make_2D_vs_Theta_deg_VarPhi(sinfoTheta_deg, sinfoVarPhi);
							DCS_Theory_vs_R_Phi = DCSTheoretical<Conv_1000_1000, Elastic32_He_1000eV>::make_2D_vs_R_Phi(sinfoR, sinfoPhi);
							break;
						case LibFlag::Sample::C:
							DCS_Theory_vs_Theta_deg = DCSTheoretical<Conv_1000_1000, Elastic32_C_1000eV>::make_1D_vs_Theta_deg(sinfoTheta_deg);
							DCS_Theory_vs_R = DCSTheoretical<Conv_1000_1000, Elastic32_C_1000eV>::make_1D_vs_R(sinfoR);
							DCS_Theory_vs_Theta_deg_VarPhi = DCSTheoretical<Conv_1000_1000, Elastic32_C_1000eV>::make_2D_vs_Theta_deg_VarPhi(sinfoTheta_deg, sinfoVarPhi);
							DCS_Theory_vs_R_Phi = DCSTheoretical<Conv_1000_1000, Elastic32_C_1000eV>::make_2D_vs_R_Phi(sinfoR, sinfoPhi);
							break;
						case LibFlag::Sample::N:
							DCS_Theory_vs_Theta_deg = DCSTheoretical<Conv_1000_1000, Elastic32_N_1000eV>::make_1D_vs_Theta_deg(sinfoTheta_deg);
							DCS_Theory_vs_R = DCSTheoretical<Conv_1000_1000, Elastic32_N_1000eV>::make_1D_vs_R(sinfoR);
							DCS_Theory_vs_Theta_deg_VarPhi = DCSTheoretical<Conv_1000_1000, Elastic32_N_1000eV>::make_2D_vs_Theta_deg_VarPhi(sinfoTheta_deg, sinfoVarPhi);
							DCS_Theory_vs_R_Phi = DCSTheoretical<Conv_1000_1000, Elastic32_N_1000eV>::make_2D_vs_R_Phi(sinfoR, sinfoPhi);
							break;
						case LibFlag::Sample::Ne:
							DCS_Theory_vs_Theta_deg = DCSTheoretical<Conv_1000_1000, Elastic32_Ne_1000eV>::make_1D_vs_Theta_deg(sinfoTheta_deg);
							DCS_Theory_vs_R = DCSTheoretical<Conv_1000_1000, Elastic32_Ne_1000eV>::make_1D_vs_R(sinfoR);
							DCS_Theory_vs_Theta_deg_VarPhi = DCSTheoretical<Conv_1000_1000, Elastic32_Ne_1000eV>::make_2D_vs_Theta_deg_VarPhi(sinfoTheta_deg, sinfoVarPhi);
							DCS_Theory_vs_R_Phi = DCSTheoretical<Conv_1000_1000, Elastic32_Ne_1000eV>::make_2D_vs_R_Phi(sinfoR, sinfoPhi);
							break;
						case LibFlag::Sample::Cl:
							DCS_Theory_vs_Theta_deg = DCSTheoretical<Conv_1000_1000, Elastic32_Cl_1000eV>::make_1D_vs_Theta_deg(sinfoTheta_deg);
							DCS_Theory_vs_R = DCSTheoretical<Conv_1000_1000, Elastic32_Cl_1000eV>::make_1D_vs_R(sinfoR);
							DCS_Theory_vs_Theta_deg_VarPhi = DCSTheoretical<Conv_1000_1000, Elastic32_Cl_1000eV>::make_2D_vs_Theta_deg_VarPhi(sinfoTheta_deg, sinfoVarPhi);
							DCS_Theory_vs_R_Phi = DCSTheoretical<Conv_1000_1000, Elastic32_Cl_1000eV>::make_2D_vs_R_Phi(sinfoR, sinfoPhi);
							break;
						case LibFlag::Sample::Ar:
							DCS_Theory_vs_Theta_deg = DCSTheoretical<Conv_1000_1000, Elastic32_Ar_1000eV>::make_1D_vs_Theta_deg(sinfoTheta_deg);
							DCS_Theory_vs_R = DCSTheoretical<Conv_1000_1000, Elastic32_Ar_1000eV>::make_1D_vs_R(sinfoR);
							DCS_Theory_vs_Theta_deg_VarPhi = DCSTheoretical<Conv_1000_1000, Elastic32_Ar_1000eV>::make_2D_vs_Theta_deg_VarPhi(sinfoTheta_deg, sinfoVarPhi);
							DCS_Theory_vs_R_Phi = DCSTheoretical<Conv_1000_1000, Elastic32_Ar_1000eV>::make_2D_vs_R_Phi(sinfoR, sinfoPhi);
							break;
						case LibFlag::Sample::Xe:
							DCS_Theory_vs_Theta_deg = DCSTheoretical<Conv_1000_1000, Elastic32_Xe_1000eV>::make_1D_vs_Theta_deg(sinfoTheta_deg);
							DCS_Theory_vs_R = DCSTheoretical<Conv_1000_1000, Elastic32_Xe_1000eV>::make_1D_vs_R(sinfoR);
							DCS_Theory_vs_Theta_deg_VarPhi = DCSTheoretical<Conv_1000_1000, Elastic32_Xe_1000eV>::make_2D_vs_Theta_deg_VarPhi(sinfoTheta_deg, sinfoVarPhi);
							DCS_Theory_vs_R_Phi = DCSTheoretical<Conv_1000_1000, Elastic32_Xe_1000eV>::make_2D_vs_R_Phi(sinfoR, sinfoPhi);
							break;
						default:
							DCS_Theory_vs_Theta_deg.reset();
							DCS_Theory_vs_R.reset();
							DCS_Theory_vs_Theta_deg_VarPhi.reset();
							DCS_Theory_vs_R_Phi.reset();
							throw std::exception("Failure on making Unique ptr to DCSTheoretical curves");
						}


						StringsForIgorTextWave::WaveText1(tstr,
							{
								{ std::move(*DCS_Theory_vs_R),_T("DCS_R"),_T("Elastic32, 1000 eV") },//vs R, DCS理論曲線
								{ std::move(*DCS_Theory_vs_Theta_deg),_T("DCS_Th"),_T("Elastic32, 1000 eV") },//vs Theta_deg, DCS理論曲線
							}
							);
						file_output.WriteString(tstr.c_str());

						StringsForIgorTextWave::WaveText2(tstr,
							{
								{std::move(*DCS_Theory_vs_R_Phi),_T("DCS_R_Ph"),_T("Elastic32, 1000 eV")},//vs R, Phi, DCS理論曲面
								{std::move(*DCS_Theory_vs_Theta_deg_VarPhi),_T("DCS_Th_VPh"),_T("Elastic32, 1000 eV")},//vs Theta_deg, VarPhi, DCS理論曲面
							}
						);
						file_output.WriteString(tstr.c_str());

					}


				} // isOK

			}
			catch (std::exception&) {
				throw;
			}
		}



		bool isOK()const noexcept {
			return m_isOK;
		}

		//CI
		std::unique_ptr<ScaledHistogram1D> up2CI;

		//1000 eVでの感度
		std::unique_ptr<class_DetectorSensitivity_Ef> up2DetectorSensitivity;

		//感度補正有り
		std::unique_ptr<ScaledHistogram1D> up2Theta_deg;
		std::unique_ptr<ScaledHistogram2D> up2Theta_deg_VarPhi;
		std::unique_ptr<ScaledHistogram1D> up2R;
		std::unique_ptr<ScaledHistogram2D> up2R_Phi;

		//感度補正なし
		std::unique_ptr<ScaledHistogram1D> up2RawTheta_deg;
		std::unique_ptr<ScaledHistogram2D> up2RawTheta_deg_VarPhi;
		std::unique_ptr<ScaledHistogram1D> up2RawR;
		std::unique_ptr<ScaledHistogram2D> up2RawR_Phi;

	private:
		//状態
		bool m_isOK;
	};



	//個別イベントごとにCoordinateを数表で出力する
	class EventProc {
	public:
		EventProc() = delete;
		EventProc(const __int32 WriteCoordinate, const TCHAR sep, const std::basic_string<TCHAR>& ConditionStr)
			:
			m_up2StringForOutputFile(nullptr),
			m_ConditionStr(ConditionStr),
			m_isOK(false),
			tstr()
		{
			try {
				auto coord = static_cast<LibFlag::type_Flag>(WriteCoordinate > 0 ? WriteCoordinate : 0);
				m_up2StringForOutputFile = make_StringsForOutputFile(coord, sep);
				if (!m_up2StringForOutputFile) {
					throw std::exception("EventProc::EventProc : Failure in making StringsForOutputFile");
				}
				m_isOK = true;
			}
			catch (std::exception&ex) {
				m_up2StringForOutputFile.reset();
				AfxMessageBox(ex.what());
				m_isOK = false;
			}
		}
		~EventProc() = default;

		void WriteHeaderString(CStdioFile& file_output) {
			try {
				if (m_isOK) {
					
					//冒頭の情報
					m_up2StringForOutputFile->GetHeaderString(tstr);
					file_output.WriteString(tstr.c_str());
					file_output.WriteString(_T("\n"));

					//Condition
					file_output.WriteString(m_ConditionStr.c_str());
					file_output.WriteString(_T("\n"));

					//Coordinate名称
					m_up2StringForOutputFile->GetCoordinatesString(tstr);
					file_output.WriteString(tstr.c_str());
					file_output.WriteString(_T("\n"));
				}
			}
			catch (std::exception&) {
				throw;
			}
		}

		void WriteCoordinateValueString(CStdioFile& file_output) {
			try {
				if (m_isOK) {
					if (up2CCFileCondition->IsTrue(m_ConditionStr)) {
						if (m_up2StringForOutputFile->GetValuesString(tstr) == 0) {
							file_output.WriteString(tstr.c_str());
							file_output.WriteString(_T("\n"));
						}
					}
				}
			}
			catch (std::exception&) {
				throw;
			}
		}

		bool isOK()const noexcept { return m_isOK; }

		//書き出し用クラス
		std::unique_ptr<StringsForOutputFile> m_up2StringForOutputFile;

		//採用するCondition
		const std::basic_string<TCHAR> m_ConditionStr;

	private:
		bool m_isOK;
		std::basic_string<TCHAR> tstr;
	};

	//CCFで設定したCoordinate, Conditionごとに外部ファイルへ書き込む作業をまとめたクラス
	class HistEventProc {
	public:
		HistEventProc() = delete;
		~HistEventProc() = default;
		
		HistEventProc(std::basic_string<TCHAR>& FileName_Histo_ITX, std::basic_string<TCHAR>& FileName_Event_TXT, const __int32 WriteCoordinate, const TCHAR sep, const __int32 WriteCoordinatesToFile_Condition)
			:
			m_FileName_Histo_ITX(FileName_Histo_ITX),
			m_FileName_Event_TXT(FileName_Event_TXT),
			m_File(),
			m_ConditionStr(),
			m_HistoP(nullptr),
			m_EventP(nullptr),
			m_isOK(false)
		{
			try {

				//Condition文字列を取得
				LibFlag::Condition_ToString(m_ConditionStr, static_cast<LibFlag::type_Flag>(WriteCoordinatesToFile_Condition));
				
				//ファイル書き込みプロシージャを作成
				m_HistoP = std::make_unique<HistoProc>();
				m_EventP = std::make_unique<EventProc>(WriteCoordinate, sep, m_ConditionStr);

				if (!m_HistoP) {
					throw std::exception("HistEventProc::HistEventProc : m_HistP 1");
				}
				if (!m_HistoP->isOK()) {
					throw std::exception("HistEventProc::HistEventProc : m_HistP 2");
				}
				if (!m_EventP) {
					throw std::exception("HistEventProc::HistEventProc : m_EventP 1");
				}
				if (!m_EventP->isOK()) {
					throw std::exception("HistEventProc::HistEventProc : m_EventP 2");
				}

				//ファイルを開く
				if (!m_File.Open(m_FileName_Event_TXT.c_str(), CFile::modeCreate | CFile::modeWrite | CFile::typeText)) {
					m_File.Abort();
					throw std::exception("HistEventProc::HistEventProc : file for event cannot be opened.");
				}
				else {
					//Event冒頭
					m_EventP->WriteHeaderString(m_File);
				}
				
				//開いたまま次へ

				m_isOK = true;
			}
			catch (std::exception& ex) {
				AfxMessageBox(ex.what());
				m_isOK = false;
			}			
		}

		void AppendEvent() {
			try {
				if (m_isOK) {
					bool MyCondition = up2CCFileCondition->IsTrue(m_ConditionStr);
					if (MyCondition) {

						//ヒストグラムへ加算
						m_HistoP->AddToHist();

						//イベントファイルへ書き込み
						m_EventP->WriteCoordinateValueString(m_File);

					}
				}
			}
			catch (std::exception&) {
				throw;
			}
		}


		void Finalize() {
			try {
				if (m_isOK) {
					//イベントファイルを閉じる
					m_File.Close();

					//ヒストグラムをitxへ書き込む
					//非線形ヒストグラム用出力ファイルを開く
					if (!m_File.Open(m_FileName_Histo_ITX.c_str(), CFile::modeCreate | CFile::modeWrite | CFile::typeText)) {
						m_File.Abort();
						throw std::exception("HistEventProc::Finalize : file for histograms cannot be opened.");
					}
					else {
						std::basic_string<TCHAR> Comment = m_ConditionStr;
						m_HistoP->WriteToIgorTextFile(m_File, true, Comment, _T(""));//末尾に文字列つけない
						m_File.Close();
					}
				}
			}
			catch (std::exception&) {
				throw;
			}
		}


		void CloseFile() { m_File.Close(); }

		bool isOK()const noexcept {
			return m_isOK;
		}

		
		const std::basic_string<TCHAR> m_FileName_Histo_ITX;
		const std::basic_string<TCHAR> m_FileName_Event_TXT;

		CStdioFile m_File;
		std::basic_string<TCHAR> m_ConditionStr;
		std::unique_ptr<HistoProc> m_HistoP;
		std::unique_ptr<EventProc> m_EventP;

	private:
		bool m_isOK;
	};

	std::unique_ptr<HistEventProc> HEP;
	std::unique_ptr<HistEventProc> HEP1;
	std::unique_ptr<HistEventProc> HEP2;
	std::unique_ptr<HistEventProc> HEP3;
	std::unique_ptr<HistEventProc> HEP4;



	//LAESのlmfからファイルをまとめて出力
	//CCFでのCnodition設定に関係なくエネルギーシフト信号をファイルへ書き出す
	class HistEventProc_LAES {
	public:
		HistEventProc_LAES()
			:
			m_F_Histo_ITX(),
			m_F_Event_wp2(),
			m_F_Event_op2(),
			m_F_Event_wp1(),
			m_F_Event_op1(),
			m_HistoP_wp2(nullptr),
			m_HistoP_op2(nullptr),
			m_HistoP_wp1(nullptr),
			m_HistoP_op1(nullptr),
			m_EventP_wp2(nullptr),
			m_EventP_op2(nullptr),
			m_EventP_wp1(nullptr),
			m_EventP_op1(nullptr),
			m_str_wp2(),
			m_str_op2(),
			m_str_wp1(),
			m_str_op1(),
			m_isOK(false)
		{
			try {
				//Condition文字列を取得
				LibFlag::Condition_ToString(m_str_wp2, static_cast<LibFlag::type_Flag>(m_wp2));
				LibFlag::Condition_ToString(m_str_op2, static_cast<LibFlag::type_Flag>(m_op2));
				LibFlag::Condition_ToString(m_str_wp1, static_cast<LibFlag::type_Flag>(m_wp1));
				LibFlag::Condition_ToString(m_str_op1, static_cast<LibFlag::type_Flag>(m_op1));

				//ファイル書き込みプロシージャを作成
				m_HistoP_wp2 = std::make_unique<HistoProc1<DCSHistogram_1000_1003p10, DetectorSensitivity1_1000_1000>>();
				m_HistoP_op2 = std::make_unique<HistoProc1<DCSHistogram_1000_1003p10, DetectorSensitivity1_1000_1000>>();
				m_HistoP_wp1 = std::make_unique<HistoProc1<DCSHistogram_1000_1001p55, DetectorSensitivity1_1000_1000>>();
				m_HistoP_op1 = std::make_unique<HistoProc1<DCSHistogram_1000_1001p55, DetectorSensitivity1_1000_1000>>();

				m_EventP_wp2 = std::make_unique<EventProc>(m_WriteCoordinate, m_sep, m_str_wp2);
				m_EventP_op2 = std::make_unique<EventProc>(m_WriteCoordinate, m_sep, m_str_op2);
				m_EventP_wp1 = std::make_unique<EventProc>(m_WriteCoordinate, m_sep, m_str_wp1);
				m_EventP_op1 = std::make_unique<EventProc>(m_WriteCoordinate, m_sep, m_str_op1);

				//ファイルを開く
				if (!m_F_Event_wp2.Open(m_FileName_Event_TXT_wp2.c_str(), CFile::modeCreate | CFile::modeWrite | CFile::typeText)) {
					m_F_Event_wp2.Abort();
					throw std::exception("HistEventProc_LAES::HistEventProc_LAES : file for event cannot be opened.");
				}
				else {
					//Event冒頭
					m_EventP_wp2->WriteHeaderString(m_F_Event_wp2);
				}

				//ファイルを開く
				if (!m_F_Event_op2.Open(m_FileName_Event_TXT_op2.c_str(), CFile::modeCreate | CFile::modeWrite | CFile::typeText)) {
					m_F_Event_op2.Abort();
					throw std::exception("HistEventProc_LAES::HistEventProc_LAES : file for event cannot be opened.");
				}
				else {
					//Event冒頭
					m_EventP_op2->WriteHeaderString(m_F_Event_op2);
				}

				//ファイルを開く
				if (!m_F_Event_wp1.Open(m_FileName_Event_TXT_wp1.c_str(), CFile::modeCreate | CFile::modeWrite | CFile::typeText)) {
					m_F_Event_wp1.Abort();
					throw std::exception("HistEventProc_LAES::HistEventProc_LAES : file for event cannot be opened.");
				}
				else {
					//Event冒頭
					m_EventP_wp1->WriteHeaderString(m_F_Event_wp1);
				}

				//ファイルを開く
				if (!m_F_Event_op1.Open(m_FileName_Event_TXT_op1.c_str(), CFile::modeCreate | CFile::modeWrite | CFile::typeText)) {
					m_F_Event_op1.Abort();
					throw std::exception("HistEventProc_LAES::HistEventProc_LAES : file for event cannot be opened.");
				}
				else {
					//Event冒頭
					m_EventP_op1->WriteHeaderString(m_F_Event_op1);
				}

				m_isOK = true;

				//ファイル開いたままイベント書き込みへ

			}
			catch (std::exception& ex) {
				AfxMessageBox(ex.what());
				m_isOK = false;
			}
		}
		~HistEventProc_LAES() = default;

		


		void AppendEvent() {
			try {
				if (m_isOK) {

					//Cnodition満たしていたら
					if (up2CCFileCondition->IsTrue(m_wp2)) {
						//ヒストグラムに加算
						m_HistoP_wp2->AddToHist();
						//イベントを書き込む
						m_EventP_wp2->WriteCoordinateValueString(m_F_Event_wp2);
					}

					//Cnodition満たしていたら
					if (up2CCFileCondition->IsTrue(m_op2)) {
						//ヒストグラムに加算
						m_HistoP_op2->AddToHist();
						//イベントを書き込む
						m_EventP_op2->WriteCoordinateValueString(m_F_Event_op2);
					}

					//Cnodition満たしていたら
					if (up2CCFileCondition->IsTrue(m_wp1)) {
						//ヒストグラムに加算
						m_HistoP_wp1->AddToHist();
						//イベントを書き込む
						m_EventP_wp1->WriteCoordinateValueString(m_F_Event_wp1);
					}

					//Cnodition満たしていたら
					if (up2CCFileCondition->IsTrue(m_op1)) {
						//ヒストグラムに加算
						m_HistoP_op1->AddToHist();
						//イベントを書き込む
						m_EventP_op1->WriteCoordinateValueString(m_F_Event_op1);
					}

				}
			}
			catch (std::exception&) {
				throw;
			}
		}




		void Finalize() {
			try {
				if (m_isOK) {

					//イベントファイルを閉じる
					m_F_Event_wp2.Close();
					m_F_Event_op2.Close();
					m_F_Event_wp1.Close();
					m_F_Event_op1.Close();

					//ヒストグラムをまとめてitxへ書き込む
					//非線形ヒストグラム用出力ファイルを開く
					if (!m_F_Histo_ITX.Open(m_FileName_Histo_ITX.c_str(), CFile::modeCreate | CFile::modeWrite | CFile::typeText)) {
						m_F_Histo_ITX.Abort();
						throw std::exception("HistEventProc::Finalize : file for histograms cannot be opened.");
					}
					else {
						//ウェーブ名冒頭はソースlmf名称
						std::basic_string<TCHAR> preff = CurrentDAqInfo::SourceLMFileName();
						
						m_HistoP_wp2->WriteToIgorTextFile(m_F_Histo_ITX, true, m_str_wp2, preff, m_str_wp2, false);
						m_HistoP_op2->WriteToIgorTextFile(m_F_Histo_ITX, false, m_str_op2, preff, m_str_op2, false);
						m_HistoP_wp1->WriteToIgorTextFile(m_F_Histo_ITX, false, m_str_wp1, preff, m_str_wp1, false);
						m_HistoP_op1->WriteToIgorTextFile(m_F_Histo_ITX, false, m_str_op1, preff, m_str_op1, false);

						m_F_Histo_ITX.Close();
					}

				}
			}
			catch (std::exception&) {
				throw;
			}
		}



		bool isOK()const noexcept {
			return m_isOK;
		}


		//ファイル
		CStdioFile m_F_Histo_ITX;
		CStdioFile m_F_Event_wp2;
		CStdioFile m_F_Event_op2;
		CStdioFile m_F_Event_wp1;
		CStdioFile m_F_Event_op1;

		//ヒストグラムに書き出す作業まとめ
		std::unique_ptr<HistoProc1<DCSHistogram_1000_1003p10, DetectorSensitivity1_1000_1000>> m_HistoP_wp2;
		std::unique_ptr<HistoProc1<DCSHistogram_1000_1003p10, DetectorSensitivity1_1000_1000>> m_HistoP_op2;
		std::unique_ptr<HistoProc1<DCSHistogram_1000_1001p55, DetectorSensitivity1_1000_1000>> m_HistoP_wp1;
		std::unique_ptr<HistoProc1<DCSHistogram_1000_1001p55, DetectorSensitivity1_1000_1000>> m_HistoP_op1;

		//イベントを書き出す作業まとめ
		std::unique_ptr<EventProc> m_EventP_wp2;
		std::unique_ptr<EventProc> m_EventP_op2;
		std::unique_ptr<EventProc> m_EventP_wp1;
		std::unique_ptr<EventProc> m_EventP_op1;
		
		//COndition文字列
		std::basic_string<TCHAR> m_str_wp2;
		std::basic_string<TCHAR> m_str_op2;
		std::basic_string<TCHAR> m_str_wp1;
		std::basic_string<TCHAR> m_str_op1;
		
	private:
		//書き出したいCoordinteは共通
		const __int32 m_WriteCoordinate = 47;
		
		//書き出すときのセパレータ
		const TCHAR m_sep = _T(',');

		//採用するCondition
		//w : WithIR, o : WithoutIR
		//p2 : n=+2, p1 : n=+1
		const LibFlag::Condition m_wp2 = LibFlag::Condition::mcleanT_WithIR_KESp2R;
		const LibFlag::Condition m_op2 = LibFlag::Condition::mcleanT_WithoutIR_KESp2R;

		const LibFlag::Condition m_wp1 = LibFlag::Condition::mcleanT_WithIR_KESp1R;
		const LibFlag::Condition m_op1 = LibFlag::Condition::mcleanT_WithoutIR_KESp1R;

		//出力先
		const std::basic_string<TCHAR> m_FileName_Histo_ITX = _T("C:\\Program Files\\RoentDek Handels GmbH\\CoboldPC 2011 R5-2-x64 (Visual Studio .Net 2010 Compilation) V10.1.1412.2\\LAES2_TDC8PCI2_HEX\\Cobold_Shared1\\temp\\DCSHisto_LAES.itx");
		
		const std::basic_string<TCHAR> m_FileName_Event_TXT_wp2 = _T("C:\\Program Files\\RoentDek Handels GmbH\\CoboldPC 2011 R5-2-x64 (Visual Studio .Net 2010 Compilation) V10.1.1412.2\\LAES2_TDC8PCI2_HEX\\Cobold_Shared1\\temp\\Events_LAES_mcleanT_WithIR_KESp2R.txt");
		const std::basic_string<TCHAR> m_FileName_Event_TXT_op2 = _T("C:\\Program Files\\RoentDek Handels GmbH\\CoboldPC 2011 R5-2-x64 (Visual Studio .Net 2010 Compilation) V10.1.1412.2\\LAES2_TDC8PCI2_HEX\\Cobold_Shared1\\temp\\Events_LAES_mcleanT_WithoutIR_KESp2R.txt");

		const std::basic_string<TCHAR> m_FileName_Event_TXT_wp1 = _T("C:\\Program Files\\RoentDek Handels GmbH\\CoboldPC 2011 R5-2-x64 (Visual Studio .Net 2010 Compilation) V10.1.1412.2\\LAES2_TDC8PCI2_HEX\\Cobold_Shared1\\temp\\Events_LAES_mcleanT_WithIR_KESp1R.txt");
		const std::basic_string<TCHAR> m_FileName_Event_TXT_op1 = _T("C:\\Program Files\\RoentDek Handels GmbH\\CoboldPC 2011 R5-2-x64 (Visual Studio .Net 2010 Compilation) V10.1.1412.2\\LAES2_TDC8PCI2_HEX\\Cobold_Shared1\\temp\\Events_LAES_mcleanT_WithoutIR_KESp1R.txt");

	private:
		bool m_isOK;
	};

	std::unique_ptr<HistEventProc_LAES> HEP_LAES;




	//解析名称
	TCHAR cAnalysisName[64];
	//ヘルプで表示する解析名
	TCHAR* GetAnalysisName() {
		strcpy_s(cAnalysisName, CString("200918").GetString());
		return cAnalysisName;
	}


	__int32 AnalysisInitialize(CDoubleArray& EventData, CDoubleArray& Parameter, CDoubleArray& WeighParameter) {

		//追加のグローバル変数を初期化
		i32SelectLayerPair = RoundToNearestInt32(Parameter[1100]);



		i32WriteCoordinatesToFile = RoundToNearestInt32(Parameter[1101]);
		i32WriteCoordinatesToFile_Condition = RoundToNearestInt32(Parameter[1102]);


		i32WriteCoordinatesToFile1 = RoundToNearestInt32(Parameter[1103]);
		i32WriteCoordinatesToFile_Condition1 = RoundToNearestInt32(Parameter[1104]);


		i32WriteCoordinatesToFile2 = RoundToNearestInt32(Parameter[1105]);
		i32WriteCoordinatesToFile_Condition2 = RoundToNearestInt32(Parameter[1106]);


		i32WriteCoordinatesToFile3 = RoundToNearestInt32(Parameter[1107]);
		i32WriteCoordinatesToFile_Condition3 = RoundToNearestInt32(Parameter[1108]);


		i32WriteCoordinatesToFile4 = RoundToNearestInt32(Parameter[1109]);
		i32WriteCoordinatesToFile_Condition4 = RoundToNearestInt32(Parameter[1110]);


		i32SelectAtom = RoundToNearestInt32(Parameter[1111]);


		dTOF_Offset_WithIR = Parameter[1120];
		dTOF_Offset_WithoutIR = Parameter[1121];

		dBinR = Parameter[1114];
		dRmax = Parameter[1115];
		

		i32VarPhiConversion = RoundToNearestInt32(Parameter[1116]);
		i32VarPhiNumSteps = RoundToNearestInt32(Parameter[1117]);

		dBinPosX = Parameter[1118];
		dBinPosY = Parameter[1119];

		i32Output_HistogramsEvents_LAES = RoundToNearestInt32(Parameter[1125]);




		//Conditionを使いたいので読み込む
		try {
			CppAddress_CCFileCoordinate::ClearAll();

			CppAddress_CCFileCoordinate::SetPr(_T("DeltaEventTime"), Crd::DeltaEventTime);
			CppAddress_CCFileCoordinate::SetPr(_T("EventCounter"), Crd::dEventCounter);
			CppAddress_CCFileCoordinate::SetPr(_T("ConsistencyIndicator"), Crd::ConsistencyIndicator);

			CppAddress_CCFileCoordinate::SetPr(_T("n1"), Crd::counts, 0);
			CppAddress_CCFileCoordinate::SetPr(_T("n2"), Crd::counts, 1);
			CppAddress_CCFileCoordinate::SetPr(_T("n3"), Crd::counts, 2);
			CppAddress_CCFileCoordinate::SetPr(_T("n4"), Crd::counts, 3);
			CppAddress_CCFileCoordinate::SetPr(_T("n5"), Crd::counts, 4);
			CppAddress_CCFileCoordinate::SetPr(_T("n6"), Crd::counts, 5);
			CppAddress_CCFileCoordinate::SetPr(_T("n7"), Crd::counts, 6);
			CppAddress_CCFileCoordinate::SetPr(_T("n8"), Crd::counts, 7);

			CppAddress_CCFileCoordinate::SetPr(_T("x1"), Crd::x1);
			CppAddress_CCFileCoordinate::SetPr(_T("x2"), Crd::x2);
			CppAddress_CCFileCoordinate::SetPr(_T("y1"), Crd::y1);
			CppAddress_CCFileCoordinate::SetPr(_T("y2"), Crd::y2);
			CppAddress_CCFileCoordinate::SetPr(_T("z1"), Crd::z1);
			CppAddress_CCFileCoordinate::SetPr(_T("z2"), Crd::z2);

			CppAddress_CCFileCoordinate::SetPr(_T("TOF"), Crd::TOF_ns);

			CppAddress_CCFileCoordinate::SetPr(_T("raw_x"), Crd::raw_x);
			CppAddress_CCFileCoordinate::SetPr(_T("raw_y"), Crd::raw_y);
			CppAddress_CCFileCoordinate::SetPr(_T("raw_w"), Crd::raw_w);

			CppAddress_CCFileCoordinate::SetPr(_T("raw_sumx"), Crd::raw_sumx);
			CppAddress_CCFileCoordinate::SetPr(_T("raw_sumy"), Crd::raw_sumy);
			CppAddress_CCFileCoordinate::SetPr(_T("raw_sumw"), Crd::raw_sumw);
			CppAddress_CCFileCoordinate::SetPr(_T("raw_sumxyw"), Crd::raw_sumxyw);
			CppAddress_CCFileCoordinate::SetPr(_T("raw_diffxy"), Crd::raw_diffxy);

			CppAddress_CCFileCoordinate::SetPr(_T("sumx"), Crd::sumx);
			CppAddress_CCFileCoordinate::SetPr(_T("sumy"), Crd::sumy);
			CppAddress_CCFileCoordinate::SetPr(_T("sumw"), Crd::sumw);
			CppAddress_CCFileCoordinate::SetPr(_T("sumxyw"), Crd::sumxyw);
			CppAddress_CCFileCoordinate::SetPr(_T("diffxy"), Crd::diffxy);

			CppAddress_CCFileCoordinate::SetPr(_T("PosX"), Crd::PosX);
			CppAddress_CCFileCoordinate::SetPr(_T("PosY"), Crd::PosY);

			CppAddress_CCFileCoordinate::SetPr(_T("r"), Crd::r);
			CppAddress_CCFileCoordinate::SetPr(_T("phi"), Crd::phi);

			CppAddress_CCFileCoordinate::SetPr(_T("Xuv"), Crd::Xuv);
			CppAddress_CCFileCoordinate::SetPr(_T("Yuv"), Crd::Yuv);
			CppAddress_CCFileCoordinate::SetPr(_T("Xuw"), Crd::Xuw);
			CppAddress_CCFileCoordinate::SetPr(_T("Yuw"), Crd::Yuw);
			CppAddress_CCFileCoordinate::SetPr(_T("Xvw"), Crd::Xuv);
			CppAddress_CCFileCoordinate::SetPr(_T("Yvw"), Crd::Yuv);

			CppAddress_CCFileCoordinate::SetPr(_T("dX"), Crd::dX);
			CppAddress_CCFileCoordinate::SetPr(_T("dY"), Crd::dY);

			CppAddress_CCFileCoordinate::SetPr(_T("M_ConsistencyIndicator"), Crd::M_ConsistencyIndicator);
			CppAddress_CCFileCoordinate::SetPr(_T("NextHitIndicator"), Crd::NextHitIndicator);
			CppAddress_CCFileCoordinate::SetPr(_T("M_NextHitIndicator"), Crd::M_NextHitIndicator);
			CppAddress_CCFileCoordinate::SetPr(_T("TOF_NoOffset"), Crd::TOF_ns_NoOffset);

			CppAddress_CCFileCoordinate::SetPr(_T("Theta_deg"), Crd::Theta_deg);
			CppAddress_CCFileCoordinate::SetPr(_T("VarPhi_deg"), Crd::VarPhi_deg);

			CppAddress_CCFileCoordinate::SetPr(_T("KineticEnergy"), Crd::KineticEnergy);

			//オブジェクト作成
			up2CCFileCondition = make_CCFileCondition({ LibPrm::ReadConditionsFromCCF_FilePath }, CppAddress_CCFileCoordinate::Get());
		}
		catch (std::exception& ex) {
			up2CCFileCondition.reset();
			AfxMessageBox(ex.what());
		}

		//ヒストグラムとイベントの書き込み用ファイルを作成する
		HEP = std::make_unique<HistEventProc>(std::basic_string<TCHAR>(LibPrm::WriteDCSHisto_FilePath), std::basic_string<TCHAR>(LibPrm::WriteForEachEvents_FilePath), i32WriteCoordinatesToFile, _T(','), i32WriteCoordinatesToFile_Condition);
		HEP1 = std::make_unique<HistEventProc>(std::basic_string<TCHAR>(LibPrm::WriteDCSHisto_FilePath1), std::basic_string<TCHAR>(LibPrm::WriteForEachEvents_FilePath1), i32WriteCoordinatesToFile1, _T(','), i32WriteCoordinatesToFile_Condition1);
		HEP2 = std::make_unique<HistEventProc>(std::basic_string<TCHAR>(LibPrm::WriteDCSHisto_FilePath2), std::basic_string<TCHAR>(LibPrm::WriteForEachEvents_FilePath2), i32WriteCoordinatesToFile2, _T(','), i32WriteCoordinatesToFile_Condition2);
		HEP3 = std::make_unique<HistEventProc>(std::basic_string<TCHAR>(LibPrm::WriteDCSHisto_FilePath3), std::basic_string<TCHAR>(LibPrm::WriteForEachEvents_FilePath3), i32WriteCoordinatesToFile3, _T(','), i32WriteCoordinatesToFile_Condition3);
		HEP4 = std::make_unique<HistEventProc>(std::basic_string<TCHAR>(LibPrm::WriteDCSHisto_FilePath4), std::basic_string<TCHAR>(LibPrm::WriteForEachEvents_FilePath4), i32WriteCoordinatesToFile4, _T(','), i32WriteCoordinatesToFile_Condition4);


		//LAES用
		if (i32Output_HistogramsEvents_LAES == 1) {
			HEP_LAES = std::make_unique<HistEventProc_LAES>();
		}
		else {
			HEP_LAES.reset();
		}



		//確認
		//AfxMessageBox(up2CCFileCondition->Show().c_str());

		//AfxMessageBox(_T("wait"));


		return 0;
	}




	//解析
	__int32 AnalysisProcessEvent(CDoubleArray& EventData, CDoubleArray& Parameter, CDoubleArray& WeighParameter, LMFPreEventData& preEventData) {


		double AbsoluteEventTime = 0;			// ns since start
		double DeltaEventTime = 0;				// This Time - PreviousTime
		double True = true;					// always true
		__int32 ConsistencyIndicator;
		__int32 PLLStatusLocked;			// totally locked then true else false

		__int32 counts[32];
		double x1, x2, y1, y2, z1, z2;
		double TOF_ns = 0.;
		double raw_x, raw_y, raw_w;
		double sumx, sumy, sumw, sumxyw, diffxy;
		double raw_sumx, raw_sumy, raw_sumw, raw_sumxyw, raw_diffxy;
		double PosX, PosY;
		double r, phi;
		double Xuv, Yuv, Xuw, Yuw, Xvw, Yvw;
		double dX, dY;


		//  Example of how to use spectrum commands
		//	pDAnUserSP->AddOneAt(2,20);			// add one in spectrum 2 at channel 20
		//	pDAnUserSP->AddValueAt(3,20,0.5);		// add 0.5 in spectrum 3 at channel 20

		memset(counts, 0, 32 * sizeof(__int32));

		double dMCPChannelData = 0.;

		// get time information if present
		if (i32TimeStamp)
		{
			// AbsoluteEventTime
			AbsoluteEventTime = GetEventTime(preEventData, Parameter);
			// AbsoluteDeltaEventTime
			DeltaEventTime = GetDeltaEventTime(preEventData, Parameter);
			// dEventCounter
		}

		if (racpRateAveragingInstance)
			if (theDAnApp.SharedClassInstance)
			{
				racpRateAveragingInstance->ui32EventCounter += unsigned __int32(theDAnApp.SharedClassInstance->rate);
				if (i32TimeStamp)
					racpRateAveragingInstance->CheckTimestamp(AbsoluteEventTime);
			}

		// dEventCounter
		bool bdEventCounterReset = RoundToNearestInt32(Parameter[1040]) ? true : false;		// parameter 1040
		if (bdEventCounterReset)
		{
			dEventCounter = 0;
			Parameter[1040] = false;
		}
		dEventCounter += 1;


		//@警告を無視
#pragma warning(push)
#pragma warning(disable : 26451)

		// Get Status Information
		if (i32DAq_ID == DAQ_ID_HM1_ABM)
		{
			counts[0] = counts[1] = counts[2] = 1;
			PLLStatusLocked = false;
		}
		else if ((i32DAq_ID == DAQ_ID_HM1) || (i32DAq_ID == DAQ_ID_2HM1))
		{
			for (__int32 ch = 0; ch < 4; ++ch)
			{
				counts[ch] = (RoundToNearestInt32(EventData[(i32StartDAqTDCData + ch * (i32NumberOfHits + 1))]) & 0x0007) - 1;
				counts[ch] = counts[ch] < 0 ? 0 : counts[ch]; // correct negative (missed) hit
			}

			PLLStatusLocked = (RoundToNearestInt32(EventData[(i32StartDAqTDCData + 0 * (i32NumberOfHits + 1))]) & 0x0080) &
				(RoundToNearestInt32(EventData[(i32StartDAqTDCData + 1 * (i32NumberOfHits + 1))]) & 0x0080) &
				(RoundToNearestInt32(EventData[(i32StartDAqTDCData + 2 * (i32NumberOfHits + 1))]) & 0x0080) &
				(RoundToNearestInt32(EventData[(i32StartDAqTDCData + 3 * (i32NumberOfHits + 1))]) & 0x0080);
			PLLStatusLocked = PLLStatusLocked ? true : false;

		}
		else if ((i32DAq_ID == DAQ_ID_TDC8) || (i32DAq_ID == DAQ_ID_2TDC8 || (i32DAq_ID == DAQ_ID_HPTDC) || (i32DAq_ID == DAQ_ID_HPTDCRAW)))
		{
			for (__int32 ch = 0; ch < i32NumberOfChannels; ++ch)
				counts[ch] = RoundToNearestInt32(EventData[(i32StartDAqTDCData + ch * (i32NumberOfHits + 1))]);

			PLLStatusLocked = false;
		}
		else	// for all other... this information is not valid
			PLLStatusLocked = false;


		// ConsistencyIndicator
		ConsistencyIndicator = 0;
		ConsistencyIndicator += counts[i32Cx1] > 0 ? 1 : 0;
		ConsistencyIndicator += counts[i32Cx2] > 0 ? 2 : 0;
		ConsistencyIndicator += counts[i32Cy1] > 0 ? 4 : 0;
		ConsistencyIndicator += counts[i32Cy2] > 0 ? 8 : 0;
		if (i32HexAnode)
		{
			ConsistencyIndicator += counts[i32Cz1] > 0 ? 16 : 0;
			ConsistencyIndicator += counts[i32Cz2] > 0 ? 32 : 0;
		}

		// Get Raw-Data
		if (i32DAq_ID == DAQ_ID_HM1_ABM)
			x1 = x2 = y1 = y2 = z1 = z2 = 0;
		else
		{
			x1 = EventData[(i32StartDAqTDCData + i32Cx1 * (i32NumberOfHits + 1) + i32UseHit)];
			x2 = EventData[(i32StartDAqTDCData + i32Cx2 * (i32NumberOfHits + 1) + i32UseHit)];
			y1 = EventData[(i32StartDAqTDCData + i32Cy1 * (i32NumberOfHits + 1) + i32UseHit)];
			y2 = EventData[(i32StartDAqTDCData + i32Cy2 * (i32NumberOfHits + 1) + i32UseHit)];
			if (i32HexAnode)
			{
				z1 = EventData[(i32StartDAqTDCData + i32Cz1 * (i32NumberOfHits + 1) + i32UseHit)];
				z2 = EventData[(i32StartDAqTDCData + i32Cz2 * (i32NumberOfHits + 1) + i32UseHit)];
			}
			else
				z1 = z2 = 0.;

			if (i32TOFChannel >= 0)
			{
				if (counts[i32TOFChannel] > 0)
					TOF_ns = EventData[(i32StartDAqTDCData + i32TOFChannel * (i32NumberOfHits + 1) + 1)];
				else
					TOF_ns = 0.;
			}
			else
				TOF_ns = 0.;

			if (i32Cmcp != -1)
			{
				dMCPChannelData = EventData[(i32Cmcp * (i32NumberOfHits + 1) + i32UseHit)];
				if (i32DAq_ID == DAQ_ID_HPTDC && Parameter[88] > 0.5)
					TOF_ns = dMCPChannelData;
				if (!(i32DAq_ID == DAQ_ID_HPTDC && Parameter[88] > 0.5))
				{
					x1 -= dMCPChannelData;
					x2 -= dMCPChannelData;
					y1 -= dMCPChannelData;
					y2 -= dMCPChannelData;
					if (i32HexAnode)
					{
						z1 -= dMCPChannelData;
						z2 -= dMCPChannelData;
					}
					else
						z1 = z2 = 0.;
				}
			}
		}


		// correct DNL if
		if ((i32DAq_ID == DAQ_ID_HM1) && dDnlCorrection)
		{
			x1 = CorrectGP1NDL(x1, dDnlCorrection);
			x2 = CorrectGP1NDL(x2, dDnlCorrection);
			y1 = CorrectGP1NDL(y1, dDnlCorrection);
			y2 = CorrectGP1NDL(y2, dDnlCorrection);
		}

		if (bAntiMoire)
		{
			x1 += Rnd() - 0.5;
			x2 += Rnd() - 0.5;
			y1 += Rnd() - 0.5;
			y2 += Rnd() - 0.5;
			if (i32HexAnode)
			{
				z1 += Rnd() - 0.5;
				z2 += Rnd() - 0.5;
			}
			if (i32TOFChannel >= 0)
				if (counts[i32TOFChannel] > 0)
					TOF_ns += Rnd() - 0.5;
		}

		// now get the "real" position
		if (i32DAq_ID == DAQ_ID_HM1_ABM)
		{
			raw_x = RoundToNearestInt32(EventData[(i32StartDAqTDCData + 0)]);
			raw_y = RoundToNearestInt32(EventData[(i32StartDAqTDCData + 1)]);
			raw_w = RoundToNearestInt32(EventData[(i32StartDAqTDCData + 2)]);

			raw_sumx = raw_sumy = raw_sumw = raw_sumxyw = raw_diffxy = -1.e200;
		}
		else
		{
			raw_x = (x1 - x2);
			raw_y = (y1 - y2);
			raw_sumx = x1 + x2;
			raw_sumy = y1 + y2;
			raw_sumxyw = raw_sumx + raw_sumy;
			raw_diffxy = raw_sumx - raw_sumy;

			if (i32HexAnode)
			{
				raw_w = (z1 - z2);
				raw_sumw = z1 + z2;
				raw_sumxyw += raw_sumw;
			}
			else
			{
				raw_w = -1.e200;
				raw_sumw = -1.e200;
			}
		}

		// do conversion ? then first to time (ns)
		if (i32Conversion)
		{
			x1 *= dTDCResolution;
			x2 *= dTDCResolution;
			y1 *= dTDCResolution;
			y2 *= dTDCResolution;
			dMCPChannelData *= dTDCResolution;
			if (i32HexAnode)
			{
				z1 *= dTDCResolution;
				z2 *= dTDCResolution;
			}
		}
		TOF_ns *= dTDCResolution;

		// sums and differences
		// sums and differences in channels if parameter 1000 set to "channel"
		// sums and differences in ns (TIME) if parameter 1000 set to other than "channel"
		sumx = (x1 + x2) + dOSum;
		sumy = (y1 + y2) + dOSum;
		if (i32HexAnode)
			sumw = (z1 + z2) + dOSum;
		else
			sumw = 0;
		if (i32HexAnode)
			sumxyw = (sumx + sumy + sumw) - (2 * dOSum);		// only one OSum -> -(2*dOSum)!!!
		else
			sumxyw = (sumx + sumy) - dOSum;				// only one OSum -> -(dOSum)!!!
		diffxy = (sumx - sumy) + dOSum;

		// convert also to position? then to position (mm)
		if (i32Conversion == 2)
		{
			x1 *= dTPCalX;
			x2 *= dTPCalX;
			y1 *= dTPCalY;
			y2 *= dTPCalY;
			if (i32HexAnode)
			{
				z1 *= dTPCalZ;
				z2 *= dTPCalZ;
			}
		}


		//@Pos計算　下で代入するので無意味
		/*if (i32HexAnode)
		{
			double x = (x1 - x2) * 0.5;
			double y = (y1 - y2) * 0.5;
			PosX = x + dOPx;
			PosY = (x - 2. * y) / sqrt(3.) + dOPy;
		}
		else
		{
			double x = (x1 - x2) * 0.5;
			double y = (y1 - y2) * 0.5;
			PosX = x + dOPx;
			PosY = y + dOPy;
		}*/

		//@計算順序並び替え
		//Defaultでは	Pos計算 -> Pos回転 -> r-phiをPosから計算 -> HexならXuvとかを独立に計算
		//ここでは		HexならXuvとかを計算 -> Xuvなどよりレイヤ選択でPosへ代入 -> Pos回転 -> r-phiをPosから計算
		{
			if (i32HexAnode)
			{
				double x = (x1 - x2) * 0.5;
				double y = (y1 - y2) * 0.5;
				double w = (z1 - z2) * 0.5 + dOPw;
				Xuv = x + dOPx;
				Yuv = (x - 2. * y) / sqrt(3.) + dOPy;
				Xuw = Xuv;
				Yuw = (2. * w - x) / sqrt(3.) + dOPy;
				Xvw = (y + w) + dOPx;
				Yvw = (w - y) / sqrt(3.) + dOPy;
				dX = Xuv - Xvw;
				dY = Yuv - Yvw;
			}
			else
				Xuv = Yuv = Xuw = Yuw = Xvw = Yvw = dX = dY = DBL_MAX;		// not hex anode -> set to DBL_MAX 

			//@採用するレイヤペア
			switch (i32SelectLayerPair)
			{
			case LayerPair::UV:
				PosX = Xuv;
				PosY = Yuv;
				break;
			case LayerPair::VW:
				PosX = Xvw;
				PosY = Yvw;
				break;
			case LayerPair::UW:
				PosX = Xuw;
				PosY = Yuw;
				break;
			default:
				PosX = DBL_MAX;
				PosY = DBL_MAX;
				break;
			}

			// do rotation
			if (dRotA)
			{
				double xRot, yRot;
				RotateXY(PosX, PosY, dRotA, i32PhiConversion, dCOx, dCOy, xRot, yRot);
				PosX = xRot;
				PosY = yRot;
			}

			// convert to r-phi
			//@フォーマット変更反映されない？  自作OrthoToRPhiへ変更
			LibFlag::OrthoToRPhi(PosX, PosY, i32PhiConversion, dCRPhix, dCRPhiy, r, phi);
		}




		//@デフォルトパラメータの代入
		{
			/*
			double AbsoluteEventTime = 0;			// ns since start
			double DeltaEventTime = 0;				// This Time - PreviousTime
			double True = true;					// always true
			__int32 ConsistencyIndicator;
			__int32 PLLStatusLocked;			// totally locked then true else false

			__int32 counts[32];
			double x1, x2, y1, y2, z1, z2;
			double TOF_ns = 0.;
			double raw_x, raw_y, raw_w;
			double sumx, sumy, sumw, sumxyw, diffxy;
			double raw_sumx, raw_sumy, raw_sumw, raw_sumxyw, raw_diffxy;
			double PosX, PosY;
			double r, phi;
			double Xuv, Yuv, Xuw, Yuw, Xvw, Yvw;
			double dX, dY;
			*/

			Crd::AbsoluteEventTime = AbsoluteEventTime;
			Crd::DeltaEventTime = DeltaEventTime;
			Crd::dEventCounter = dEventCounter;
			Crd::ConsistencyIndicator = ConsistencyIndicator;

			for (__int32 ch = 0; ch < 8; ++ch)						// hit counter of the first 9 channels
				Crd::counts[ch] = counts[ch];

			Crd::x1 = x1;
			Crd::x2 = x2;
			Crd::y1 = y1;
			Crd::y2 = y2;
			Crd::z1 = z1;
			Crd::z2 = z2;

			Crd::TOF_ns = TOF_ns;

			Crd::raw_x = raw_x;
			Crd::raw_y = raw_y;
			Crd::raw_w = raw_w;

			Crd::sumx = sumx;
			Crd::sumy = sumy;
			Crd::sumw = sumw;
			Crd::sumxyw = sumxyw;
			Crd::diffxy = diffxy;

			Crd::raw_sumx = raw_sumx;
			Crd::raw_sumy = raw_sumy;
			Crd::raw_sumw = raw_sumw;
			Crd::raw_sumxyw = raw_sumxyw;
			Crd::raw_diffxy = raw_diffxy;

			Crd::PosX = PosX;
			Crd::PosY = PosY;

			Crd::r = r;
			Crd::phi = phi;

			Crd::Xuv = Xuv;
			Crd::Yuv = Yuv;
			Crd::Xuw = Xuw;
			Crd::Yuw = Yuw;
			Crd::Xvw = Xvw;
			Crd::Yvw = Yvw;

			Crd::dX = dX;
			Crd::dY = dY;
		}




		//@MCP信号も考慮したConsistency Indicator
		{
			__int32 M_ConsistencyIndicator = 0;
			if (i32Cmcp != -1) {//MCP信号を使用する？
				M_ConsistencyIndicator = ConsistencyIndicator + ((counts[i32Cmcp] > 0) ? 64 : 0);
			}
			else {
				M_ConsistencyIndicator = -1;
			}
			Crd::M_ConsistencyIndicator = M_ConsistencyIndicator;
		}

		//@NextHit CIs
		{
			__int32 NextHitIndicator = 0;
			__int32 M_NextHitIndicator = 0;
			if (i32UseHit + 1 <= i32NumberOfHits) {//NextHit番号が解析する信号数を超えない
				__int32 i32NextHit = i32UseHit + 1;
				NextHitIndicator += (counts[i32Cx1] >= i32NextHit) ? 1 : 0;
				NextHitIndicator += (counts[i32Cx2] >= i32NextHit) ? 2 : 0;
				NextHitIndicator += (counts[i32Cy1] >= i32NextHit) ? 4 : 0;
				NextHitIndicator += (counts[i32Cy2] >= i32NextHit) ? 8 : 0;
				if (i32HexAnode)
				{
					NextHitIndicator += (counts[i32Cz1] >= i32NextHit) ? 16 : 0;
					NextHitIndicator += (counts[i32Cz2] >= i32NextHit) ? 32 : 0;
				}
				if (i32Cmcp != -1)
				{
					M_NextHitIndicator = NextHitIndicator + ((counts[i32Cmcp] >= i32NextHit) ? 64 : 0);
				}
				else
				{
					M_NextHitIndicator = -1;
				}
			}
			else {
				NextHitIndicator = -1;
				M_NextHitIndicator = -1;
			}

			Crd::NextHitIndicator = NextHitIndicator;
			Crd::M_NextHitIndicator = M_NextHitIndicator;
		}

		
		
		//@変換座標の計算
		bool bEx = false;//座標計算中に例外発生した？
		try
		{
			//@オフセットなしTOF
			double TOF_ns_NoOffset = 0;
			if (up2CCFileCondition->IsTrue(LibFlag::Condition::WithIR)) {
				TOF_ns_NoOffset = TOF_ns - dTOF_Offset_WithIR;
			}
			else if (up2CCFileCondition->IsTrue(LibFlag::Condition::WithoutIR)) {
				TOF_ns_NoOffset = TOF_ns - dTOF_Offset_WithoutIR;
			}
			Crd::TOF_ns_NoOffset = TOF_ns_NoOffset;
			

			//@散乱後の極角と方位角
			double Theta_deg = 0;
			double VarPhi_deg = 0;
			Theta_deg = Conv_1000_1000::Theta_deg(r);
			VarPhi_deg = Conv_1000_1000::VarPhi(phi, static_cast<LibFlag::AzimuthFormat>(i32PhiConversion), static_cast<LibFlag::AzimuthFormat>(i32VarPhiConversion));
			Crd::Theta_deg = Theta_deg;
			Crd::VarPhi_deg = VarPhi_deg;

			//@エネルギー
			double KineticEnergy = 0;
			KineticEnergy = ConversionToKES_1000::KineticEnergy(Crd::PosX, Crd::PosY, Crd::TOF_ns_NoOffset);
			Crd::KineticEnergy = KineticEnergy;


			bEx = false;
		}
		catch (std::exception&) {
			Crd::TOF_ns_NoOffset = -1;
			Crd::Theta_deg = -1;
			Crd::VarPhi_deg = -361;
			Crd::KineticEnergy = -1;

			//変な信号の場合は判別式は負になることがある
			/*if (!bErrorInAnalysisProcess_Conv) {
				AfxMessageBox(ex.what());
			}
			bErrorInAnalysisProcess_Conv = true;*/

			bEx = true;
		}



		//エラーがなかったらイベントを追加
		if (!bEx) {
			try {
				HEP->AppendEvent();
				HEP1->AppendEvent();
				HEP2->AppendEvent();
				HEP3->AppendEvent();
				HEP4->AppendEvent();


				if (HEP_LAES) {
					HEP_LAES->AppendEvent();
				}


			}
			catch (std::exception&) {
				//
			}
		}





		/////////////////////////////////////
		// write all data back to coordinates
		/////////////////////////////////////
		__int32 address = 0;


		EventData[i32StartDAnData + address++] = AbsoluteEventTime;
		EventData[i32StartDAnData + address++] = DeltaEventTime;
		EventData[i32StartDAnData + address++] = dEventCounter;
		EventData[i32StartDAnData + address++] = True;
		EventData[i32StartDAnData + address++] = 0; // false
		EventData[i32StartDAnData + address++] = ConsistencyIndicator;
		EventData[i32StartDAnData + address++] = PLLStatusLocked;

		if (racpRateAveragingInstance)
			EventData[i32StartDAnData + address++] = racpRateAveragingInstance->ui32Rate;	// rate
		else
			EventData[i32StartDAnData + address++] = 0.; // rate

		EventData[i32StartDAnData + address++] = Parameter[1060];	// manually set Condition1

		for (__int32 ch = 0; ch < 8; ++ch)						// hit counter of the first 9 channels
			EventData[i32StartDAnData + address++] = counts[ch];

		EventData[i32StartDAnData + address++] = x1;
		EventData[i32StartDAnData + address++] = x2;
		EventData[i32StartDAnData + address++] = y1;
		EventData[i32StartDAnData + address++] = y2;
		EventData[i32StartDAnData + address++] = z1;
		EventData[i32StartDAnData + address++] = z2;

		EventData[i32StartDAnData + address++] = TOF_ns;

		EventData[i32StartDAnData + address++] = raw_x;
		EventData[i32StartDAnData + address++] = raw_y;
		EventData[i32StartDAnData + address++] = raw_w;

		EventData[i32StartDAnData + address++] = raw_sumx;
		EventData[i32StartDAnData + address++] = raw_sumy;
		EventData[i32StartDAnData + address++] = raw_sumw;
		EventData[i32StartDAnData + address++] = raw_sumxyw;
		EventData[i32StartDAnData + address++] = raw_diffxy;

		EventData[i32StartDAnData + address++] = sumx;
		EventData[i32StartDAnData + address++] = sumy;
		EventData[i32StartDAnData + address++] = sumw;
		EventData[i32StartDAnData + address++] = sumxyw;

		EventData[i32StartDAnData + address++] = diffxy;

		EventData[i32StartDAnData + address++] = PosX;
		EventData[i32StartDAnData + address++] = PosY;

		EventData[i32StartDAnData + address++] = r;
		EventData[i32StartDAnData + address++] = phi;

		EventData[i32StartDAnData + address++] = Xuv;
		EventData[i32StartDAnData + address++] = Yuv;
		EventData[i32StartDAnData + address++] = Xuw;
		EventData[i32StartDAnData + address++] = Yuw;
		EventData[i32StartDAnData + address++] = Xvw;
		EventData[i32StartDAnData + address++] = Yvw;

		EventData[i32StartDAnData + address++] = dX;
		EventData[i32StartDAnData + address++] = dY;

		double dMcpSignalDiff = -1.e200;
		if (i32NumberOfHits > 1)
		{
			if (i32Cmcp > -1)
			{
				if (counts[i32Cmcp] > 1)
				{
					dMcpSignalDiff = EventData[(i32StartDAqTDCData + i32Cmcp * (i32NumberOfHits + 1) + 2)] - EventData[(i32StartDAqTDCData + i32Cmcp * (i32NumberOfHits + 1) + 1)];
					dMcpSignalDiff *= dTDCResolution;
				}
			}
			else if (i32Cmcp == -1)
			{	// if Parameter 1025 is set to 0 then perform the calculations for Hit2-Hit1 on TDC Channel 8 (normally x1)
				__int32 _i32Cmcp = 7;
				if (counts[_i32Cmcp] > 1)
				{
					dMcpSignalDiff = EventData[(i32StartDAqTDCData + _i32Cmcp * (i32NumberOfHits + 1) + 2)] - EventData[(i32StartDAqTDCData + _i32Cmcp * (i32NumberOfHits + 1) + 1)];
					dMcpSignalDiff *= dTDCResolution;
				}
			}
		}
		EventData[i32StartDAnData + address++] = dMcpSignalDiff;

		EventData[i32StartDAnData + address++] = GetReflectionValue(i32Cx1, i32Cx2, counts, EventData);
		EventData[i32StartDAnData + address++] = GetReflectionValue(i32Cx2, i32Cx1, counts, EventData);
		EventData[i32StartDAnData + address++] = GetReflectionValue(i32Cy1, i32Cy2, counts, EventData);
		EventData[i32StartDAnData + address++] = GetReflectionValue(i32Cy2, i32Cy1, counts, EventData);
		if (i32HexAnode)
		{
			EventData[i32StartDAnData + address++] = GetReflectionValue(i32Cz1, i32Cz2, counts, EventData);
			EventData[i32StartDAnData + address++] = GetReflectionValue(i32Cz2, i32Cz1, counts, EventData);
		}
		else
		{
			EventData[i32StartDAnData + address++] = -1.e200;
			EventData[i32StartDAnData + address++] = -1.e210;
		}


		//@-------ここにccf宣言順にユーザー座標を代入--------//
		EventData[i32StartDAnData + address++] = Crd::M_ConsistencyIndicator;
		EventData[i32StartDAnData + address++] = Crd::NextHitIndicator;
		EventData[i32StartDAnData + address++] = Crd::M_NextHitIndicator;
		EventData[i32StartDAnData + address++] = Crd::TOF_ns_NoOffset;

		EventData[i32StartDAnData + address++] = Crd::Theta_deg;
		EventData[i32StartDAnData + address++] = Crd::VarPhi_deg;

		EventData[i32StartDAnData + address++] = Crd::KineticEnergy;
		//---------------------------------------------------//



		for (__int32 ch = 0; ch < i32NumberOfChannels; ++ch)								// const 1-i32NumberOfChannels
			EventData[i32StartDAnData + address++] = double(ch + 1);						// must be the last coordinates


		//@警告を無視
#pragma warning(pop)


		CCoboldParser* pParser = GetCCoboldParser();
		pParser->execute_all_commands();

		return true;
	}


	//メモリ解放など
	__int32 AnalysisFinalize(CDoubleArray& EventData, CDoubleArray& Parameter, CDoubleArray& WeighParameter) {
		try {

			//最後の作業
			HEP->Finalize();
			HEP1->Finalize();
			HEP2->Finalize();
			HEP3->Finalize();
			HEP4->Finalize();

			if (HEP_LAES) {
				HEP_LAES->Finalize();
			}

		}
		catch (std::out_of_range& ex) {
			AfxMessageBox(ex.what());
		}
		catch (std::exception& ex) {
			AfxMessageBox(ex.what());

		}

		return 0;

	}

#endif // LAES2_ANALYSIS_DEFINED
#endif // LAES2_ANALYSIS_200918




} // namespace LAES2