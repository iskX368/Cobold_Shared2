#pragma once

#include <stdexcept>

#define _USE_MATH_DEFINES
#include <math.h>


#include "Lib_Inputs.h"


//---------------------------------------------------------------
//artof装置変換関数まとめ
//---------------------------------------------------------------



/// <summary>
/// <para>R : 検出位置の検出器中心からの距離 (mm)</para>
/// <para>Theta : 散乱極角 (rad)</para>
/// <para>Theta_deg : 散乱極角 (deg)</para>
/// <para>VarPhi : 散乱方位角 (rad)</para>
/// <para>VarPhi_deg : 散乱方位角 (deg)</para>
/// <para>Phi : 検出位置の検出器中心についての方位角 (rad)</para>
/// <para>Phi_deg : 検出位置の検出器中心についての方位角 (deg)</para>
/// <para>Omega : 立体角 (strad)</para>
/// <para>dpdq : パラメータ p のパラメータ q に関する一階微分</para>
/// </summary>
template<class Params>
class ConversionToDCS1 {
public:

	/// <summary>
	/// 散乱極角 (deg) から検出位置中心からの距離 (mm) へ変換する
	/// </summary>
	/// <param name="Theta_deg"></param>
	/// <exception cref="なし"></exception>
	/// <returns></returns>
	static double R(const double Theta_deg)noexcept;


	/// <summary>
	/// 散乱極角 (deg) から検出位置中心からの距離 (mm) への変換式の一階微分
	/// </summary>
	/// <param name="Theta_deg"></param>
	/// <exception cref="なし"></exception>
	/// <returns></returns>
	static double dRdTheta_deg(const double Theta_deg)noexcept;


	/// <summary>
	/// 検出位置中心からの距離 (mm) から散乱極角 (deg) へ変換する
	/// </summary>
	/// <param name="Param"> : 変換用パラメータ</param>
	/// <param name="R"> : 検出位置の検出位置中心からの距離 (mm)</param>
	/// <exception cref="std::invalid_argument"></exception>
	/// <returns>散乱極角 (deg)</returns>
	static double Theta_deg(const double R);


	/// <summary>
	/// 検出位置中心からの距離 (mm) から散乱方位角 (deg) へ変換する
	/// </summary>
	/// <param name="Param"> : 変換用パラメータ</param>
	/// <param name="AzimuthAngle_Detector_deg"> : 検出器中心に関する検出位置の方位角 (deg)</param>
	/// <exception cref="std::invalid_argument"></exception>
	/// <returns>散乱方位角 (deg)</returns>
	//static double VarPhi_deg(const double Phi_deg) noexcept;


	/// <summary>
	/// 検出器上の方位角を散乱方位角へ指定したフォーマットで変換する
	/// </summary>
	/// <param name="Phi"> : 検出器中心に関する検出位置の方位角  単位と定義域は次の引数で指定</param>
	/// <param name="PhiFormat"> : 引数の方位角のフォーマット</param>
	/// <param name="VarPhiFormat"> : 返値の散乱方位角のフォーマット</param>
	/// <exception cref="std::invalid_argument"></exception>
	/// <exception cref="std::out_of_range"></exception>
	/// <returns></returns>
	static double VarPhi(const double Phi, const LibFlag::AzimuthFormat PhiFormat, const LibFlag::AzimuthFormat VarPhiFormat);


	

	/// <summary>
	/// 検出位置中心からの距離 (mm) から散乱方位角 (rad) への変換式の一階微分 dTheta/dR
	/// </summary>
	/// <param name="Param"> : 変換用パラメータ</param>
	/// <param name="R"> : 検出位置の検出位置中心からの距離 (mm)</param>
	/// <exception cref="std::invalid_argument"></exception>
	/// <returns>(rad/mm)</returns>
	static double dThetadR(const double R);


	/// <summary>
	/// <para>散乱方位角 (rad) について積分済みの立体角関数の散乱極角 (rad) についての一階微分 dOmega/dTheta　と</para><para>散乱極角の検出位置中心からの距離 (mm) についての一階微分 dTheta/dR の積</para><para>dOmega/dR に等しい</para>
	/// </summary>
	/// <param name="Param"> : 変換用パラメータ</param>
	/// <param name="R"> : 検出位置の検出位置中心からの距離 (mm)</param>
	/// <exception cref="std::invalid_argument"></exception>
	/// <returns>(strad/mm)</returns>
	static double dOmegadTheta_dThetadR(const double R);

};

using Conv_1000_1000 = ConversionToDCS1<Parameters_ConversionToDCS_e19111401>;//入射エネルギー 1000 eV, 出射エネルギー 1000 eV
using Conv_1000_1001p55 = ConversionToDCS1<Parameters_ConversionToDCS_e19111402>;//入射エネルギー 1000 eV, 出射エネルギー 1001.55 eV
using Conv_1000_1003p10 = ConversionToDCS1<Parameters_ConversionToDCS_e19111403>;//入射エネルギー 1000 eV, 出射エネルギー 1001.55 eV




/// <summary>
/// 原子の弾性微分散乱断面積の理論値をElastic32のフィッティング結果を元に計算する
/// </summary>
/// <typeparam name="Params"></typeparam>
template<class Params>
class TheoreticalAtomicDCS_Elastic32 {
public:
	/// <summary>
	/// 微分散乱断面積を取得する
	/// </summary>
	/// <param name="Theta_deg"> : 散乱角度 (deg)</param>
	/// <returns>(ao**2/sterad), ao**2 = 2.8002852E-21 m**2</returns>
	static double dsigmadOmega(const double Theta_deg)noexcept;
};
using Elastic32_He_1000eV = TheoreticalAtomicDCS_Elastic32<Elastic32_FittedByDblGauss::He_1000eV>;
using Elastic32_C_1000eV = TheoreticalAtomicDCS_Elastic32<Elastic32_FittedByDblGauss::C_1000eV>;
using Elastic32_N_1000eV = TheoreticalAtomicDCS_Elastic32<Elastic32_FittedByDblGauss::N_1000eV>;
using Elastic32_Ne_1000eV = TheoreticalAtomicDCS_Elastic32<Elastic32_FittedByDblGauss::Ne_1000eV>;
using Elastic32_Cl_1000eV = TheoreticalAtomicDCS_Elastic32<Elastic32_FittedByDblGauss::Cl_1000eV>;
using Elastic32_Ar_1000eV = TheoreticalAtomicDCS_Elastic32<Elastic32_FittedByDblGauss::Ar_1000eV>;
using Elastic32_Xe_1000eV = TheoreticalAtomicDCS_Elastic32<Elastic32_FittedByDblGauss::Xe_1000eV>;

