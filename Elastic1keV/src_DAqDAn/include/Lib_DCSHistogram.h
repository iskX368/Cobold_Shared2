#pragma once


#include <memory>
#include <stdexcept>
#include <vector>
#include "Lib_ScaleInformation.h"
#include "Lib_ConversionToDCS.h"

#include "Lib_Inputs.h"


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



	static std::unique_ptr<std::vector<std::vector<double>>> make_2D_vs_Theta_deg_VarPhi(const ScaleInformation& ScaleInfo_Theta_deg, const ScaleInformation& ScaleInfo_VarPhi);



	static std::unique_ptr<std::vector<std::vector<double>>> make_2D_vs_R_Phi(const ScaleInformation& ScaleInfo_R, const ScaleInformation& ScaleInfo_Phi);

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




	static std::unique_ptr<ScaledHistogram2D> make_2D_vs_Theta_deg_VarPhi(const size_t DimSize_R, const double DimDelta_R, const size_t DimSize_VarPhi, const LibFlag::AzimuthFormat Format_VarPhi);



	static std::unique_ptr<ScaledHistogram2D> make_2D_vs_R_Phi(const size_t DimSize_R, const double DimDelta_R, const size_t DimSize_Phi, const LibFlag::AzimuthFormat Format_Phi);


};

using DCSHistogram_1000_1000 = DCSHistogram<Conv_1000_1000>;




//template<class class_ConversionToKES>
//class KESHistogram {
//public:
//
//	static std::unique_ptr<ScaledHistogram1D> make_1D_vs_TOF(const size_t DimSize_TOF, const double DimDelta_TOF, const double DimOffset_TOF);
//
//
//	//位置ごとのKEスペクトル
//	static std::unique_ptr<ScaledHistogram1D> make_1D_vs_KineticEnergy(const size_t DimSize_TOF, const double DimDelta_TOF, const double DimOffset_TOF, const double PosX, const double PosY);
//
//
//
//};





/// <summary>
/// TOFやKEのための一次元スペクトル
/// </summary>
/// <typeparam name="class_ConversionToKES"></typeparam>
template<class class_ConversionToKES>
class TOF_KEHistogram {
public:
	virtual ~TOF_KEHistogram();



	virtual void AddValue(const double PosX, const double PosY, const double HorizontalValue, const double Weight, const double AbsoluteErrorOfWeight) = 0;


	/// <summary>
	/// 一次元ヒストグラムの数値をすべてゼロにする
	/// </summary>
	/// <returns></returns>
	virtual void ClearHistogram() noexcept = 0;




	/*/// <summary>
	///	一次元ヒストグラムを取得する
	/// </summary>
	/// <exception cref="std::out_of_range"></exception>
	/// <returns></returns>
	virtual std::vector<double> Get()const = 0;


	/// <summary>
	/// 一次元ヒストグラムの絶対誤差を取得する
	/// </summary>
	/// <exception cref="std::out_of_range"></exception>
	/// <returns></returns>
	virtual std::vector<double> GetE() const = 0;


	/// <summary>
	/// 一次元ヒストグラムの相対誤差を取得する
	/// </summary>
	/// <exception cref="std::out_of_range"></exception>
	/// <returns></returns>
	virtual std::vector<double> GetE1() const = 0;


	/// <summary>
	/// 一次元ヒストグラムの横軸における bin 幅を取得する　確認用
	/// </summary>
	/// <returns></returns>
	virtual const std::vector<double>& GetWidth()const noexcept = 0;

	/// <summary>
	/// 一次元ヒストグラムの横軸スケールによる修正前の値を取得する　確認用
	/// </summary>
	/// <returns></returns>
	virtual const std::vector<double>& GetRaw()const noexcept = 0;

	/// <summary>
	/// 一次元ヒストグラムの数値を取得する　一番近い目盛りの数値を返す
	/// </summary>
	/// <param name="Xvalue"></param>
	/// <exception cref="std::out_of_range"></exception>
	/// <returns></returns>
	virtual double GetAt(const double Xvalue)const = 0;

	/// <summary>
	/// 一次元ヒストグラムの数値を取得する
	/// </summary>
	/// <param name="Pnt"></param>
	/// <exception cref="std::out_of_range"></exception>
	/// <returns></returns>
	virtual double GetAt(const size_t Pnt)const = 0;

	/// <summary>
	/// 一次元ヒストグラムの絶対誤差を取得する　一番近い目盛りの数値を返す
	/// </summary>
	/// <param name="Xvalue"></param>
	/// <exception cref="std::out_of_range"></exception>
	/// <returns></returns>
	virtual double GetEAt(const double Xvalue)const = 0;

	/// <summary>
	/// 一次元ヒストグラムの絶対誤差を取得する
	/// </summary>
	/// <param name="Pnt"></param>
	/// <exception cref="std::out_of_range"></exception>
	/// <returns></returns>
	virtual double GetEAt(const size_t Pnt)const = 0;

	/// <summary>
	/// 一次元ヒストグラムの絶対誤差を取得する　一番近い目盛りの数値を返す
	/// </summary>
	/// <param name="Xvalue"></param>
	/// <exception cref="std::out_of_range"></exception>
	/// <returns></returns>
	virtual double GetE1At(const double Xvalue)const = 0;

	/// <summary>
	/// 一次元ヒストグラムの相対誤差を取得する
	/// </summary>
	/// <param name="Pnt"></param>
	/// <exception cref="std::out_of_range"></exception>
	/// <returns></returns>
	virtual double GetE1At(const size_t Pnt)const = 0;

	/// <summary>
	/// クラス ScaleInformation を取得する
	/// </summary>
	/// <returns></returns>
	virtual const ScaleInformation& ScaleInfo()const noexcept = 0;*/



	static std::unique_ptr<TOF_KEHistogram<class_ConversionToKES>> make(const size_t DimSize, const double DimDelta, const double DimOffset, const bool mode_KE);

};














//
//
///// <summary>
///// 検出器感度補正
///// </summary>
///// <typeparam name="class_ConversionToDCS"></typeparam>
///// <typeparam name="class_TheoreticalAtomicDCS_Elastic32"></typeparam>
///// <typeparam name="class_SourceEvents"></typeparam>
//template<class class_ConversionToDCS, class class_TheoreticalAtomicDCS_Elastic32, class class_SourceEvents>
//class DetectorSensitivityDistribution1D {
//public:
//	virtual ~DetectorSensitivityDistribution1D();
//
//
//	virtual std::vector<double> Calibrate_Hist_R(ScaledHistogram1D& Hist_R) = 0;
//
//	virtual std::vector<double> Calibrate_Hist_Theta_deg(ScaledHistogram1D& Hist_Theta_deg) = 0;
//
//	//virtual std::vector<double> ErrorOfCalibrated_Hist_R(ScaledHistogram1D& Hist_R) = 0;
//
//	virtual std::vector<double> ErrorOfCalibrated_Hist_Theta_deg(ScaledHistogram1D& Hist_Theta_deg) = 0;
//
//
//	
//	//ここから class_SourceEvents のヒストグラム
//
//
//	virtual const ScaledHistogram1D& Hist_R()const noexcept = 0;
//
//	virtual const ScaleInformation& ScaleInfo_R()const noexcept = 0;
//
//
//	virtual const ScaledHistogram1D& Hist_Theta_deg()const noexcept = 0;
//
//	virtual const ScaleInformation& ScaleInfo_Theta_deg()const noexcept = 0;
//
//
//	virtual const std::vector<double>& DCS_vs_Theta_deg()const noexcept = 0;
//
//	/// <summary>
//	/// <para>感度補正用割合値を取得する</para><para>感度補正前の散乱極角 (deg) ヒストグラムをプロットし終えたら対応する bin にこれを掛け算する</para>
//	/// </summary>
//	/// <returns>strad/a0^2</returns>
//	virtual const std::vector<double>& Ratio_vs_Theta_deg()const noexcept = 0;
//
//	/// <summary>
//	/// <para>感度補正用割合値の相対誤差 dx/x を取得する</para><para>感度補正前の散乱極角 (deg) ヒストグラムをプロットし終えたら対応する bin の誤差自乗にこれの自乗を足して平方根をとる</para>
//	/// </summary>
//	/// <returns></returns>
//	//virtual const std::vector<double>& Ratio_vs_Theta_deg_Error1()const noexcept = 0;
//
//
//	virtual const std::vector<double>& DCS_vs_R()const noexcept = 0;
//
//	/// <summary>
//	/// <para>感度補正用割合値を取得する</para><para>感度補正前の検出器半径 (mm) ヒストグラムをプロットし終えたら対応する bin にこれを掛け算する</para>
//	/// </summary>
//	/// <returns></returns>
//	virtual const std::vector<double>& Ratio_vs_R()const noexcept = 0;
//
//	/// <summary>
//	/// <para>感度補正用割合値の相対誤差 dx/x を取得する</para><para>感度補正前の検出器半径 (mm) ヒストグラムをプロットし終えたら対応する bin の誤差自乗にこれの自乗を足して平方根をとる</para>
//	/// </summary>
//	/// <returns></returns>
//	//virtual const std::vector<double>& Ratio_vs_R_Error1()const noexcept = 0;
//
//	/// <summary>
//	/// クラス DetectorSensitivityDistribution1D を作成する
//	/// </summary>
//	/// <param name="DimSize_R"> : </param>
//	/// <param name="DimDelta_R"> : </param>
//	/// <returns></returns>
//	static std::unique_ptr<DetectorSensitivityDistribution1D<class_ConversionToDCS, class_TheoreticalAtomicDCS_Elastic32, class_SourceEvents>> make(const size_t DimSize_R, const double DimDelta_R);
//};
//
////class SourceEvents_DetectorSensitivity;
//using DetectorSensitivity1D_1000_1000 = DetectorSensitivityDistribution1D<Conv_1000_1000, Elastic32_He_1000eV, SourceEvents_DetectorSensitivity_19122536>;
//
//
//
//
//template<class class_ConversionToDCS, class class_TheoreticalAtomicDCS_Elastic32, class class_SourceEvents>
//class DetectorSensitivityDistribution2D {
//public:
//	virtual ~DetectorSensitivityDistribution2D();
//
//	virtual std::vector<std::vector<double>> Calibrate_Hist_R_Phi_deg(ScaledHistogram2D& Hist_R_Phi_deg) = 0;
//
//	virtual std::vector<std::vector<double>> Calibrate_Hist_Theta_deg_VarPhi_deg(ScaledHistogram2D& Hist_Theta_deg_VarPhi_deg) = 0;
//
//	//virtual std::vector<std::vector<double>> ErrorOfCalibrated_Hist_R_Phi_deg(ScaledHistogram2D& Hist_R_Phi_deg) = 0;
//
//	virtual std::vector<std::vector<double>> ErrorOfCalibrated_Hist_Theta_deg_VarPhi_deg(ScaledHistogram2D& Hist_Theta_deg_VarPhi_deg) = 0;
//
//
//
//
//	//ここから class_SourceEvents のヒストグラム
//
//
//	virtual const ScaledHistogram2D& Hist_R_Phi_deg()const noexcept = 0;
//
//	virtual const ScaleInformation& ScaleInfo_R()const noexcept = 0;
//
//	virtual const ScaleInformation& ScaleInfo_Phi_deg()const noexcept = 0;
//
//	
//	virtual const ScaledHistogram2D& Hist_Theta_deg_VarPhi_deg()const noexcept = 0;
//
//	virtual const ScaleInformation& ScaleInfo_Theta_deg()const noexcept = 0;
//
//	virtual const ScaleInformation& ScaleInfo_VarPhi_deg()const noexcept = 0;
//
//
//	virtual const std::vector<std::vector<double>>& DCS_vs_Theta_deg_VarPhi_deg()const noexcept = 0;
//
//	/// <summary>
//	/// <para>感度補正用割合値を取得する</para><para>感度補正前の散乱極角 (deg) ヒストグラムをプロットし終えたら対応する bin にこれを掛け算する</para>
//	/// </summary>
//	/// <returns>strad/a0^2</returns>
//	virtual const std::vector<std::vector<double>>& Ratio_vs_Theta_deg_VarPhi_deg()const noexcept = 0;
//
//	/// <summary>
//	/// <para>感度補正用割合値の相対誤差 dx/x を取得する</para><para>感度補正前の散乱極角 (deg) ヒストグラムをプロットし終えたら対応する bin の誤差自乗にこれの自乗を足して平方根をとる</para>
//	/// </summary>
//	/// <returns></returns>
//	virtual const std::vector<std::vector<double>>& Ratio_vs_Theta_deg_VarPhi_deg_Error1()const noexcept = 0;
//
//
//
//	virtual const std::vector<std::vector<double>>& DCS_vs_R_Phi_deg()const noexcept = 0;
//
//	/// <summary>
//	/// <para>感度補正用割合値を取得する</para><para>感度補正前の検出器半径 (mm) ヒストグラムをプロットし終えたら対応する bin にこれを掛け算する</para>
//	/// </summary>
//	/// <returns></returns>
//	virtual const std::vector<std::vector<double>>& Ratio_vs_R_Phi_deg()const noexcept = 0;
//
//	/// <summary>
//	/// <para>感度補正用割合値の相対誤差 dx/x を取得する</para><para>感度補正前の検出器半径 (mm) ヒストグラムをプロットし終えたら対応する bin の誤差自乗にこれの自乗を足して平方根をとる</para>
//	/// </summary>
//	/// <returns></returns>
//	virtual const std::vector<std::vector<double>>& Ratio_vs_R_Phi_deg_Error1()const noexcept = 0;
//
//
//	static std::unique_ptr<DetectorSensitivityDistribution2D<class_ConversionToDCS, class_TheoreticalAtomicDCS_Elastic32, class_SourceEvents>> make(const size_t DimSize_R, const double DimDelta_R, const size_t DimSize_Phi, const LibFlag::AzimuthFormat Format_Phi, const LibFlag::AzimuthFormat Format_VarPhi);
//};
//
//
//using DetectorSensitivity2D_1000_1000 = DetectorSensitivityDistribution2D<Conv_1000_1000, Elastic32_He_1000eV, SourceEvents_DetectorSensitivity_19122536>;



/// <summary>
/// 検出器感度を取得するクラス
/// </summary>
/// <typeparam name="class_ConversionToDCS"> : 変換関数</typeparam>
/// <typeparam name="class_TheoreticalAtomicDCS_Elastic32"> : 実験条件に対応する理論DCS</typeparam>
/// <typeparam name="class_SourceEvents"> : 感度補正に採用する実験イベントデータ</typeparam>
template<class class_ConversionToDCS, class class_TheoreticalAtomicDCS_Elastic32, class class_SourceEvents>
class DetectorSensitivityDistribution1 {
public:
	virtual ~DetectorSensitivityDistribution1();

	/// <summary>
	/// 検出器感度とその絶対誤差のペアを取得する
	/// </summary>
	/// <param name="r"> : 検出器半径</param>
	/// <param name="phi"> : 検出器方位角</param>
	/// <returns></returns>
	virtual std::pair<double, double> Weight(const double r, const double phi)const = 0;

	/// <summary>
	/// 検出器方位角のフォーマットを取得する
	/// </summary>
	/// <returns></returns>
	virtual LibFlag::AzimuthFormat Format_Phi()const noexcept = 0;



	/// <summary>
	/// 検出器感度分布
	/// </summary>
	/// <exception cref="std::exception">uptrメンバに異常．コンストラクタ要確認</exception>
	/// <returns></returns>
	virtual std::vector<std::vector<double>> DetectorSensitivityDistribution()const = 0;

	/// <summary>
	/// 検出器感度誤差分布
	/// </summary>
	/// <exception cref="std::exception">uptrメンバに異常．コンストラクタ要確認</exception>
	/// <returns></returns>
	virtual std::vector<std::vector<double>> DetectorSensitivityDistributionE()const = 0;



	static std::unique_ptr<DetectorSensitivityDistribution1<class_ConversionToDCS, class_TheoreticalAtomicDCS_Elastic32, class_SourceEvents>> make(const size_t DimSize_R, const double DimDelta_R, const size_t DimSize_Phi, const LibFlag::AzimuthFormat Format_Phi);

};

using DetectorSensitivity1_1000_1000 = DetectorSensitivityDistribution1<Conv_1000_1000, Elastic32_He_1000eV, SourceEvents_DetectorSensitivity_19122536>;




