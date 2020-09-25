#pragma once


#include <memory>
#include <stdexcept>
#include <vector>
#include "Lib_ScaleInformation.h"
#include "Lib_ConversionToDCS.h"


//
//DCS実験値と理論値の描画
//検出器感度補正
//を行うクラス
//
//方位角フォーマット設定の追加は後で．
//







/// <summary>
/// 微分散乱断面積の理論値ヒストグラムを作成するクラス
/// </summary>
/// <typeparam name="class_TheoreticalAtomicDCS_Elastic32"></typeparam>
/// <typeparam name="class_ConversionToDCS"></typeparam>
template<class class_ConversionToDCS, class class_TheoreticalAtomicDCS_Elastic32>
class DCSTheoretical {
public:

	/// <summary>
	/// 横軸を散乱極角 (deg) としたときに微分散乱断面積理論値を縦軸とした一次元グラフの値を取得する
	/// </summary>
	/// <param name="ScaleInfo_Theta_deg"> : 散乱極角 (deg) に関する軸情報</param>
	/// <exception cref="std::invalid_argument"></exception>
	/// <exception cref="std::out_of_range"></exception>
	/// <exception cref = "std::exception"></exception>
	/// <returns></returns>
	static std::unique_ptr<std::vector<double>> make_1D_vs_Theta_deg(const ScaleInformation& ScaleInfo_Theta_deg);

	/// <summary>
	/// 横軸を検出器半径 (mm) としたときに微分散乱断面積理論値を縦軸とした一次元グラフの値を取得する
	/// </summary>
	/// <param name="ScaleInfo_R"> : 検出器半径 (mm) に関する軸情報</param>
	/// <returns></returns>
	static std::unique_ptr<std::vector<double>> make_1D_vs_R(const ScaleInformation& ScaleInfo_R);


	
	static std::unique_ptr<std::vector<std::vector<double>>> make_2D_vs_Theta_deg_VarPhi_deg(const ScaleInformation& ScaleInfo_Theta_deg, const ScaleInformation& ScaleInfo_VarPhi_deg);



	static std::unique_ptr<std::vector<std::vector<double>>> make_2D_vs_R_Phi_deg(const ScaleInformation& ScaleInfo_R, const ScaleInformation& ScaleInfo_Phi_deg);

};

using DCSTheoretical_1000_He = DCSTheoretical<Conv_1000_1000, Elastic32_He_1000eV>;
using DCSTheoretical_1000_C = DCSTheoretical<Conv_1000_1000, Elastic32_C_1000eV>;
using DCSTheoretical_1000_N = DCSTheoretical<Conv_1000_1000, Elastic32_N_1000eV>;
using DCSTheoretical_1000_Ne = DCSTheoretical<Conv_1000_1000, Elastic32_Ne_1000eV>;
using DCSTheoretical_1000_Cl = DCSTheoretical<Conv_1000_1000, Elastic32_Cl_1000eV>;
using DCSTheoretical_1000_Ar = DCSTheoretical<Conv_1000_1000, Elastic32_Ar_1000eV>;
using DCSTheoretical_1000_Xe = DCSTheoretical<Conv_1000_1000, Elastic32_Xe_1000eV>;




/// <summary>
/// 微分散乱断面積の実験値ヒストグラムを作成するクラス
/// </summary>
/// <typeparam name="class_ConversionToDCS"> : 変換関数をまとめたクラス</typeparam>
template<class class_ConversionToDCS>
class DCSHistogram {
public:

	/// <summary>
	/// 横軸を散乱極角 (deg) としたヒストグラムを作る
	/// </summary>
	/// <param name="DimSize_R"> : 横軸の元となる検出器半径 (mm) 座標軸における bin の個数</param>
	/// <param name="DimDelta_R"> : 横軸の元となる検出器半径 (mm) 座標軸における bin の増加量 (mm)</param>
	/// <exception cref="std::invalid_argument"></exception>
	/// <exception cref="std::out_of_range"></exception>
	/// <exception cref = "std::exception"></exception>
	/// <returns></returns>
	static std::unique_ptr<ScaledHistogram1D> make_1D_vs_Theta_deg(const size_t DimSize_R, const double DimDelta_R);
	
	/// <summary>
	/// 横軸を検出器半径 (mm) としたヒストグラムを作る
	/// </summary>
	/// <param name="DimSize_R"> : bin の個数</param>
	/// <param name="DimDelta_R"> : bin の増加量 (mm)</param>
	/// <exception cref="std::invalid_argument"></exception>
	/// <exception cref="std::out_of_range"></exception>
	/// <exception cref = "std::exception"></exception>
	/// <returns></returns>
	static std::unique_ptr<ScaledHistogram1D> make_1D_vs_R(const size_t DimSize_R, const double DimDelta_R);




	static std::unique_ptr<ScaledHistogram2D> make_2D_vs_Theta_deg_VarPhi_deg(const size_t DimSize_R, const double DimDelta_R, const size_t DimSize_Phi_deg);



	static std::unique_ptr<ScaledHistogram2D> make_2D_vs_R_Phi_deg(const size_t DimSize_R, const double DimDelta_R, const size_t DimSize_Phi_deg);
};





/// <summary>
/// 検出器感度補正
/// </summary>
/// <typeparam name="class_ConversionToDCS"></typeparam>
/// <typeparam name="class_TheoreticalAtomicDCS_Elastic32"></typeparam>
/// <typeparam name="class_SourceEvents"></typeparam>
template<class class_ConversionToDCS, class class_TheoreticalAtomicDCS_Elastic32, class class_SourceEvents>
class DetectorSensitivityDistribution1D {
public:
	~DetectorSensitivityDistribution1D();


	virtual std::vector<double> Calibrate_Hist_R(ScaledHistogram1D& Hist_R) = 0;

	virtual std::vector<double> Calibrate_Hist_Theta_deg(ScaledHistogram1D& Hist_Theta_deg) = 0;

	//virtual std::vector<double> ErrorOfCalibrated_Hist_R(ScaledHistogram1D& Hist_R) = 0;

	virtual std::vector<double> ErrorOfCalibrated_Hist_Theta_deg(ScaledHistogram1D& Hist_Theta_deg) = 0;


	
	//ここから class_SourceEvents のヒストグラム


	virtual const ScaledHistogram1D& Hist_R()const noexcept = 0;

	virtual const ScaleInformation& ScaleInfo_R()const noexcept = 0;


	virtual const ScaledHistogram1D& Hist_Theta_deg()const noexcept = 0;

	virtual const ScaleInformation& ScaleInfo_Theta_deg()const noexcept = 0;


	virtual const std::vector<double>& DCS_vs_Theta_deg()const noexcept = 0;

	/// <summary>
	/// <para>感度補正用割合値を取得する</para><para>感度補正前の散乱極角 (deg) ヒストグラムをプロットし終えたら対応する bin にこれを掛け算する</para>
	/// </summary>
	/// <returns>strad/a0^2</returns>
	virtual const std::vector<double>& Ratio_vs_Theta_deg()const noexcept = 0;

	/// <summary>
	/// <para>感度補正用割合値の相対誤差 dx/x を取得する</para><para>感度補正前の散乱極角 (deg) ヒストグラムをプロットし終えたら対応する bin の誤差自乗にこれの自乗を足して平方根をとる</para>
	/// </summary>
	/// <returns></returns>
	//virtual const std::vector<double>& Ratio_vs_Theta_deg_Error1()const noexcept = 0;


	virtual const std::vector<double>& DCS_vs_R()const noexcept = 0;

	/// <summary>
	/// <para>感度補正用割合値を取得する</para><para>感度補正前の検出器半径 (mm) ヒストグラムをプロットし終えたら対応する bin にこれを掛け算する</para>
	/// </summary>
	/// <returns></returns>
	virtual const std::vector<double>& Ratio_vs_R()const noexcept = 0;

	/// <summary>
	/// <para>感度補正用割合値の相対誤差 dx/x を取得する</para><para>感度補正前の検出器半径 (mm) ヒストグラムをプロットし終えたら対応する bin の誤差自乗にこれの自乗を足して平方根をとる</para>
	/// </summary>
	/// <returns></returns>
	//virtual const std::vector<double>& Ratio_vs_R_Error1()const noexcept = 0;

	/// <summary>
	/// クラス DetectorSensitivityDistribution1D を作成する
	/// </summary>
	/// <param name="DimSize_R"> : </param>
	/// <param name="DimDelta_R"> : </param>
	/// <returns></returns>
	static std::unique_ptr<DetectorSensitivityDistribution1D<class_ConversionToDCS, class_TheoreticalAtomicDCS_Elastic32, class_SourceEvents>> make(const size_t DimSize_R, const double DimDelta_R);
};

class SourceEvents_DetectorSensitivity;
using DetectorSensitivity1D_1000_1000 = DetectorSensitivityDistribution1D<Conv_1000_1000, Elastic32_He_1000eV, SourceEvents_DetectorSensitivity>;




template<class class_ConversionToDCS, class class_TheoreticalAtomicDCS_Elastic32, class class_SourceEvents>
class DetectorSensitivityDistribution2D {
public:
	~DetectorSensitivityDistribution2D();

	virtual std::vector<std::vector<double>> Calibrate_Hist_R_Phi_deg(ScaledHistogram2D& Hist_R_Phi_deg) = 0;

	virtual std::vector<std::vector<double>> Calibrate_Hist_Theta_deg_VarPhi_deg(ScaledHistogram2D& Hist_Theta_deg_VarPhi_deg) = 0;

	//virtual std::vector<std::vector<double>> ErrorOfCalibrated_Hist_R_Phi_deg(ScaledHistogram2D& Hist_R_Phi_deg) = 0;

	virtual std::vector<std::vector<double>> ErrorOfCalibrated_Hist_Theta_deg_VarPhi_deg(ScaledHistogram2D& Hist_Theta_deg_VarPhi_deg) = 0;






	virtual const ScaledHistogram2D& Hist_R_Phi_deg()const noexcept = 0;

	virtual const ScaleInformation& ScaleInfo_R()const noexcept = 0;

	virtual const ScaleInformation& ScaleInfo_Phi_deg()const noexcept = 0;

	
	virtual const ScaledHistogram2D& Hist_Theta_deg_VarPhi_deg()const noexcept = 0;

	virtual const ScaleInformation& ScaleInfo_Theta_deg()const noexcept = 0;

	virtual const ScaleInformation& ScaleInfo_VarPhi_deg()const noexcept = 0;


	virtual const std::vector<std::vector<double>>& DCS_vs_Theta_deg_VarPhi_deg()const noexcept = 0;

	/// <summary>
	/// <para>感度補正用割合値を取得する</para><para>感度補正前の散乱極角 (deg) ヒストグラムをプロットし終えたら対応する bin にこれを掛け算する</para>
	/// </summary>
	/// <returns>strad/a0^2</returns>
	virtual const std::vector<std::vector<double>>& Ratio_vs_Theta_deg_VarPhi_deg()const noexcept = 0;

	/// <summary>
	/// <para>感度補正用割合値の相対誤差 dx/x を取得する</para><para>感度補正前の散乱極角 (deg) ヒストグラムをプロットし終えたら対応する bin の誤差自乗にこれの自乗を足して平方根をとる</para>
	/// </summary>
	/// <returns></returns>
	virtual const std::vector<std::vector<double>>& Ratio_vs_Theta_deg_VarPhi_deg_Error1()const noexcept = 0;



	virtual const std::vector<std::vector<double>>& DCS_vs_R_Phi_deg()const noexcept = 0;

	/// <summary>
	/// <para>感度補正用割合値を取得する</para><para>感度補正前の検出器半径 (mm) ヒストグラムをプロットし終えたら対応する bin にこれを掛け算する</para>
	/// </summary>
	/// <returns></returns>
	virtual const std::vector<std::vector<double>>& Ratio_vs_R_Phi_deg()const noexcept = 0;

	/// <summary>
	/// <para>感度補正用割合値の相対誤差 dx/x を取得する</para><para>感度補正前の検出器半径 (mm) ヒストグラムをプロットし終えたら対応する bin の誤差自乗にこれの自乗を足して平方根をとる</para>
	/// </summary>
	/// <returns></returns>
	virtual const std::vector<std::vector<double>>& Ratio_vs_R_Phi_deg_Error1()const noexcept = 0;


	static std::unique_ptr<DetectorSensitivityDistribution2D<class_ConversionToDCS, class_TheoreticalAtomicDCS_Elastic32, class_SourceEvents>> make(const size_t DimSize_R, const double DimDelta_R, const size_t DimSize_Phi_deg);
};


using DetectorSensitivity2D_1000_1000 = DetectorSensitivityDistribution2D<Conv_1000_1000, Elastic32_He_1000eV, SourceEvents_DetectorSensitivity>;



