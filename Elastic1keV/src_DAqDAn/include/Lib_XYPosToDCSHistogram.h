#pragma once

#include <stdexcept>
#include <memory>

#include "Lib_ScaleInformation.h"

//(x,y)受け取って変換
//結果はヒストグラム
//セットに必要な情報


struct Parameter_RadiusToScatteringAngles
{
	double K0;
	double K1;
	double K2;
	double Phi_Offset_deg;
};

//判別式
double f_Hanbetsu(const Parameter_RadiusToScatteringAngles& Param, const double R_mm) noexcept;

//判別式の平方根
double f_SqrtHanbetsu(const Parameter_RadiusToScatteringAngles& Param, const double R_mm);

//極角(deg)への変換
double f_R_mm_2_Theta_deg(const Parameter_RadiusToScatteringAngles& Param, const double R_mm);

//方位角(deg)への変換
double f_Phi_deg_2_varPhi_deg(const Parameter_RadiusToScatteringAngles& Param, const double Phi_deg) noexcept;

//極角への変換の一階微分(rad/mm)
double f_DTheta_rad_DR_mm(const Parameter_RadiusToScatteringAngles& Param, const double R_mm);

//一次元DCS  int_0^(2pi)(dVarPhi dsigma/dOmega) を取得する  単位はarb
//イベントごと加算量に相当
double f_R_mm_2_DsigmaDOmega_arb(const Parameter_RadiusToScatteringAngles& Param, const double R_mm);


double f_X(const Parameter_RadiusToScatteringAngles& Param, const double R_mm);


double f_pnt_2_R_mm(const double BinR_mm, const double dPnt)noexcept;

double f_pnt_2_Theta_deg(const Parameter_RadiusToScatteringAngles& Param, const double BinR_mm, const double dPnt);

double f_DiffOf_pnt_2_Theta_deg(const Parameter_RadiusToScatteringAngles& Param, const double BinR_mm, const double dPnt);






class XYPosToDCSHistogram {
public:
	virtual ~XYPosToDCSHistogram() = default;

	/// <summary>
	/// ヒストグラムを取得する
	/// </summary>
	/// <exception cdef="std::exception"></exception>
	/// <returns></returns>
	virtual ScaledHistogram1D& Histogram() = 0;

	/// <summary>
	/// 半径方向ヒストグラムを取得する
	/// </summary>
	/// <exception cdef="std::exception"></exception>
	/// <returns></returns>
	virtual ScaledHistogram1D& Hist_R() = 0;



	/// <summary>
	/// クラス XYPosToDCSHistogram を作成する
	/// </summary>
	/// <param name="K0"> : 変換に使用する二次多項式 Radius_mm = F(Polar_deg) の第ゼロ係数</param>
	/// <param name="K1"> : 変換に使用する二次多項式 Radius_mm = F(Polar_deg) の第一係数</param>
	/// <param name="K2"> : 変換に使用する二次多項式 Radius_mm = F(Polar_deg) の第二係数</param>
	/// <param name="Phi_Offset_deg"> : </param>
	/// <param name="NumBins_Radius"> : 半径に関するスペクトルの目盛り個数</param>
	/// <param name="Bin_Radius_mm"> : 半径に関するスペクトルの一様目盛り幅</param>
	/// <returns></returns>
	static std::unique_ptr<XYPosToDCSHistogram> make(const double K0, const double K1, const double K2, const double Phi_Offset_deg, const size_t NumBins_Radius, const double Bin_Radius_mm);

};

/// <summary>
/// クラス XYPosToDCSHistogram を作成する
/// </summary>
/// <param name="K0"> : 変換に使用する二次多項式 Radius_mm = F(Polar_deg) の第ゼロ係数</param>
/// <param name="K1"> : 変換に使用する二次多項式 Radius_mm = F(Polar_deg) の第一係数</param>
/// <param name="K2"> : 変換に使用する二次多項式 Radius_mm = F(Polar_deg) の第二係数</param>
/// <param name="Phi_Offset_deg"> : </param>
/// <param name="NumBins_Radius"> : 半径に関するスペクトルの目盛り個数</param>
/// <param name="Bin_Radius_mm"> : 半径に関するスペクトルの一様目盛り幅</param>
/// <returns></returns>
#define make_XYPosToDCSHistogram XYPosToDCSHistogram::make

