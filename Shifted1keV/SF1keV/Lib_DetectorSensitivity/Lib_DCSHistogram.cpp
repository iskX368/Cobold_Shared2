// Lib_DetectorSensitivity.cpp : スタティック ライブラリ用の関数を定義します。
//

#include "pch.h"
#include "framework.h"
#include "Lib_DCSHistogram.h"

//#include "Source_XYTable.h"
//#include "SourceEvents_DetectorSensitivity.h"



#include <vector>
#include <math.h>
#include <functional>



static const double gv_d = 83;//散乱点とワイヤーの距離
static const double gv_del = 0.5;//ビームブロック半径
static const double gv_d_e = 5;//散乱点とワイヤーの距離の測定誤差
static const double gv_del_e = 0.2;//ビームブロック半径の誤差




//ポイントから数値を求める  DimDeltaずれた位置を0ポイントとして (0, DimDelta] を始まりの区間とする
//ヒストグラム横軸変換時に原点でのゼロ割を回避する
static inline double Pnt2X_Backward(const double dPnt, const double delta, const double offset) {
	return offset + (dPnt + 1) * delta;
}


//ポイントから数値を求める  DimDelta/2ずれた位置を0ポイントとして [0, DimDelta) を始まりの区間とする
static inline double Pnt2X_Central0(const double dPnt, const double delta, const double offset) {
	return offset + (dPnt + 0.5) * delta;
}



//DCS理論値
template<class class_ConversionToDCS, class class_TheoreticalAtomicDCS_Elastic32>
std::unique_ptr<std::vector<double>> DCSTheoretical<class_ConversionToDCS, class_TheoreticalAtomicDCS_Elastic32>::make_1D_vs_Theta_deg(const ScaleInformation& ScaleInfo_Theta_deg) {
	try {
		auto vec = std::make_unique<std::vector<double>>();
		vec->reserve(ScaleInfo_Theta_deg.NumIntervals());

		double DCS, Theta_deg;
		for (size_t i = 0; i < ScaleInfo_Theta_deg.NumIntervals(); i++) {
			Theta_deg = ScaleInfo_Theta_deg.IntervalIndex2XRepresentative(i);
			DCS = class_TheoreticalAtomicDCS_Elastic32::dsigmadOmega(Theta_deg);//横軸に対してそのまま代入
			vec->push_back(DCS);
		}

		return std::move(vec);
	}
	catch (std::invalid_argument&) {
		throw;
	}
	catch (std::out_of_range&) {
		throw;
	}
	catch (std::exception&) {
		throw;
	}
}
template<class class_ConversionToDCS, class class_TheoreticalAtomicDCS_Elastic32>
std::unique_ptr<std::vector<double>> DCSTheoretical<class_ConversionToDCS, class_TheoreticalAtomicDCS_Elastic32>::make_1D_vs_R(const ScaleInformation& ScaleInfo_R) {
	try {
		auto vec = std::make_unique<std::vector<double>>();
		vec->reserve(ScaleInfo_R.NumIntervals());

		double DCS, R, Theta_deg, dTheta_degdR;
		double delTheta_degdelR;
		for (size_t i = 0; i < ScaleInfo_R.NumIntervals(); i++) {
			R = ScaleInfo_R.IntervalIndex2XRepresentative(i);
			Theta_deg = class_ConversionToDCS::Theta_deg(R);
			DCS = class_TheoreticalAtomicDCS_Elastic32::dsigmadOmega(Theta_deg);
			
			//微分で修正　bin 幅が小さいなら
			dTheta_degdR = class_ConversionToDCS::dThetadR(R) * gv_rad2deg;
			
			//差分で修正
			const auto& itvl = ScaleInfo_R.Interval(i);
			delTheta_degdelR = (class_ConversionToDCS::Theta_deg(itvl.second) - class_ConversionToDCS::Theta_deg(itvl.first)) / (itvl.second - itvl.first);
			

			//vec->push_back(DCS * dTheta_degdR);//微分
			vec->push_back(DCS * delTheta_degdelR);//差分

		}

		return std::move(vec);
	}
	catch (std::invalid_argument&) {
		throw;
	}
	catch (std::out_of_range&) {
		throw;
	}
	catch (std::exception&) {
		throw;
	}
}



template<class class_ConversionToDCS, class class_TheoreticalAtomicDCS_Elastic32>
std::unique_ptr<std::vector<std::vector<double>>> DCSTheoretical<class_ConversionToDCS, class_TheoreticalAtomicDCS_Elastic32>::make_2D_vs_Theta_deg_VarPhi(const ScaleInformation& ScaleInfo_Theta_deg, const ScaleInformation& ScaleInfo_VarPhi) {
	try {
		auto vec = std::make_unique<std::vector<std::vector<double>>>();

		const size_t im = ScaleInfo_Theta_deg.NumIntervals();
		const size_t jm = ScaleInfo_VarPhi.NumIntervals();

		//方位角に依存しない
		auto DCS1D_vs_Theta_deg = make_1D_vs_Theta_deg(ScaleInfo_Theta_deg);//一次元バージョン

		vec->reserve(im);
		double DCS;
		for (size_t i = 0; i < im; i++) {
			DCS = DCS1D_vs_Theta_deg->at(i);

			auto v = std::vector<double>();
			v.reserve(jm);
			for (size_t j = 0; j < jm; j++) {
				v.push_back(DCS);//phi非依存なのでそのまま
			}

			vec->push_back(std::move(v));

		}

		return std::move(vec);
	}
	catch (std::invalid_argument&) {
		throw;
	}
	catch (std::out_of_range&) {
		throw;
	}
	catch (std::exception&) {
		throw;
	}
}
template<class class_ConversionToDCS, class class_TheoreticalAtomicDCS_Elastic32>
std::unique_ptr<std::vector<std::vector<double>>> DCSTheoretical<class_ConversionToDCS, class_TheoreticalAtomicDCS_Elastic32>::make_2D_vs_R_Phi(const ScaleInformation& ScaleInfo_R, const ScaleInformation& ScaleInfo_Phi) {
	try {
		auto vec = std::make_unique<std::vector<std::vector<double>>>();

		const size_t im = ScaleInfo_R.NumIntervals();
		const size_t jm = ScaleInfo_Phi.NumIntervals();

		//方位角に依存しない
		auto DCS1D_vs_R= make_1D_vs_R(ScaleInfo_R);//一次元バージョン

		vec->reserve(im);

		double DCS;
		for (size_t i = 0; i < im; i++) {
			DCS = DCS1D_vs_R->at(i);

			auto v = std::vector<double>();
			v.reserve(jm);
			for (size_t j = 0; j < jm; j++) {

				v.push_back(DCS);//phi非依存なのでそのまま

			}

			vec->push_back(std::move(v));

		}

		return std::move(vec);
	}
	catch (std::invalid_argument&) {
		throw;
	}
	catch (std::out_of_range&) {
		throw;
	}
	catch (std::exception&) {
		throw;
	}
}








template<class class_ConversionToDCS>
std::unique_ptr<ScaledHistogram1D> DCSHistogram<class_ConversionToDCS>::make_1D_vs_Theta_deg(const size_t DimSize_R, const double DimDelta_R) {
	try {
		//ヒストグラム横軸の基本情報
		auto pinf = make_PointInformation(DimSize_R, PointInformation::Difference::Backward);

		//引数をポイント数にした関数を定義する
		auto f_pnt2Theta_deg = [=](const double dPnt)->double {return class_ConversionToDCS::Theta_deg(Pnt2X_Backward(dPnt, DimDelta_R, 0)); };

		//ヒストグラムを作る
		//return make_ScaledHistogram1D(pinf, f_pnt2Theta_deg, df_pnt2Theta_deg);
		return make_ScaledHistogram1D(pinf, f_pnt2Theta_deg);

	}
	catch (std::invalid_argument&) {
		throw;
	}
	catch (std::out_of_range&) {
		throw;
	}
	catch (std::exception&) {
		throw;
	}
}

template<class class_ConversionToDCS>
std::unique_ptr<ScaledHistogram1D> DCSHistogram<class_ConversionToDCS>::make_1D_vs_R(const size_t DimSize_R, const double DimDelta_R) {
	try {
		//ヒストグラム横軸の基本情報
		auto pinf = make_PointInformation(DimSize_R, PointInformation::Difference::Backward);

		//bin幅で関数を特殊化
		auto f_pnt2R = [=](const double dPnt)->double {return Pnt2X_Backward(dPnt, DimDelta_R, 0); };

		//ヒストグラムを作る
		//return make_ScaledHistogram1D(pinf, f_pnt2R, df_pnt2R);
		return make_ScaledHistogram1D(pinf, f_pnt2R);

	}
	catch (std::invalid_argument&) {
		throw;
	}
	catch (std::out_of_range&) {
		throw;
	}
	catch (std::exception&) {
		throw;
	}
}


template<class class_ConversionToDCS>
std::unique_ptr<ScaledHistogram2D> DCSHistogram<class_ConversionToDCS>::make_2D_vs_Theta_deg_VarPhi(const size_t DimSize_R, const double DimDelta_R, const size_t DimSize_VarPhi, const LibFlag::AzimuthFormat Format_VarPhi) {
	try {
		
		const auto range = LibFlag::AzimuthRange(Format_VarPhi);
		double DimDelta_VarPhi = (range.second - range.first) / static_cast<double>(DimSize_VarPhi);
		double DimOffset_VarPhi = range.first;
		

		//ヒストグラム横軸の基本情報
		auto pinf_Theta_deg = make_PointInformation(DimSize_R, PointInformation::Difference::Backward);
		auto pinf_VarPhi = make_PointInformation(DimSize_VarPhi, PointInformation::Difference::Central0);

		//ポイントから散乱角度を作る
		auto f_pnt2Theta_deg = [=](const double dPnt)->double {
			return class_ConversionToDCS::Theta_deg(Pnt2X_Backward(dPnt, DimDelta_R, 0));
		};
		auto f_pnt2VarPhi = [=](const double dPnt)->double {
			return Pnt2X_Central0(dPnt, DimDelta_VarPhi, DimOffset_VarPhi);
		};
		
		//ヒストグラムを作る
		return make_ScaledHistogram2D(pinf_Theta_deg, pinf_VarPhi, f_pnt2Theta_deg, f_pnt2VarPhi);
	}
	catch (std::invalid_argument&) {
		throw;
	}
	catch (std::out_of_range&) {
		throw;
	}
	catch (std::exception&) {
		throw;
	}
}



template<class class_ConversionToDCS>
std::unique_ptr<ScaledHistogram2D> DCSHistogram<class_ConversionToDCS>::make_2D_vs_R_Phi(const size_t DimSize_R, const double DimDelta_R, const size_t DimSize_Phi, const LibFlag::AzimuthFormat Format_Phi) {
	try {

		const auto range = LibFlag::AzimuthRange(Format_Phi);
		double DimDelta_Phi = (range.second - range.first) / static_cast<double>(DimSize_Phi);
		double DimOffset_Phi = range.first;


		//ヒストグラム横軸の基本情報
		auto pinf_R = make_PointInformation(DimSize_R, PointInformation::Difference::Backward);
		auto pinf_Phi = make_PointInformation(DimSize_Phi, PointInformation::Difference::Central0);

		//ポイントから検出器上の極座標を作る
		auto f_pnt2R = [=](const double dPnt)->double {return Pnt2X_Backward(dPnt, DimDelta_R, 0); };
		auto f_pnt2Phi = [=](const double dPnt)->double {return Pnt2X_Central0(dPnt, DimDelta_Phi, DimOffset_Phi); };

		//ヒストグラムを作る
		return make_ScaledHistogram2D(pinf_R, pinf_Phi, f_pnt2R, f_pnt2Phi);
	}
	catch (std::invalid_argument&) {
		throw;
	}
	catch (std::out_of_range&) {
		throw;
	}
	catch (std::exception&) {
		throw;
	}
}










//template<class class_ConversionToKES>
//std::unique_ptr<ScaledHistogram1D> KESHistogram<class_ConversionToKES>::make_1D_vs_TOF(const size_t DimSize_TOF, const double DimDelta_TOF, const double DimOffset_TOF) {
//	try {
//		
//		//ヒストグラム横軸の基本情報
//		auto pinf = make_PointInformation(DimSize_TOF, PointInformation::Difference::Backward);
//
//		//引数をポイント数にした関数を定義する
//		auto f_pnt2TOF = [](const dPnt)->double {return Pnt2X_Central0(dPnt, DimDelta_TOF, DimOffset_TOF); };
//
//		//ヒストグラムを作る
//		return make_ScaledHistogram1D(pinf, f_pnt2TOF);
//		
//	}
//	catch (std::invalid_argument&) {
//		throw;
//	}
//	catch (std::out_of_range&) {
//		throw;
//	}
//	catch (std::exception&) {
//		throw;
//	}
//}
//template<class class_ConversionToKES>
//static std::unique_ptr<ScaledHistogram1D> KESHistogram<class_ConversionToKES>::make_1D_vs_KineticEnergy(const size_t DimSize_TOF, const double DimDelta_TOF, const double DimOffset_TOF, const double PosX, const double PosY) {
//	try {
//		
//		//ヒストグラム横軸の基本情報
//		auto pinf = make_PointInformation(DimSize_TOF, PointInformation::Difference::Backward);
//
//		//引数をポイント数にした関数を定義する
//		auto f_pnt2KineticEnergy = [=](const dPnt)->double {return class_ConversionToKES::KineticEnergy(PosX, PosY, Pnt2X_Central0(dPnt, DimDelta_TOF, DimOffset_TOF)); };
//
//		//ヒストグラムを作る
//		return make_ScaledHistogram1D(pinf, f_pnt2TOF);
//
//	}
//	catch (std::invalid_argument&) {
//		throw;
//	}
//	catch (std::out_of_range&) {
//		throw;
//	}
//	catch (std::exception&) {
//		throw;
//	}
//}
//






template<class class_ConversionToKES>
TOF_KEHistogram<class_ConversionToKES>::~TOF_KEHistogram() = default;

template<class class_ConversionToKES>
class TOF_KEHistogram_impl :public TOF_KEHistogram<class_ConversionToKES> {
public:
	TOF_KEHistogram_impl() = delete;
	virtual ~TOF_KEHistogram_impl() = default;


	TOF_KEHistogram_impl(std::unique_ptr<std::vector<double>>&& up2Histogram, std::unique_ptr<std::vector<double>>&& up2HistogramErrorSq, const std::shared_ptr<PointInformation>& sp2pinf,const std::function<double(double, double, double)>& f_Pnt2Value)
		:
		m_up2Histogram(std::forward<std::unique_ptr<std::vector<double>>>(up2Histogram)),
		m_up2HistogramErrorSq(std::forward<std::unique_ptr<std::vector<double>>>(up2HistogramErrorSq)),
		m_sp2pinf(sp2pinf),
		m_f_Pnt2Value(f_Pnt2Value)
	{}
	

	virtual void AddValue(const double PosX, const double PosY, const double HorizontalValue, const double Weight, const double AbsoluteErrorOfWeight) {
		try {

			//HorizontalはTOF_NoOffstかKEかは自分で気を付ける

			//位置によって変換を特殊化
			auto f_Pnt2Value = [=](const double dPnt)->double {return m_f_Pnt2Value(dPnt, PosX, PosY); };
			//スケール情報を動的に生成
			std::unique_ptr<ScaleInformation> sci = make_ScaleInformation(m_sp2pinf, f_Pnt2Value);

			//所属する bin
			const size_t ind = sci->X2IntervalIndex(HorizontalValue);

			//検出位置によって横軸目盛り幅 width は異なる
			const double width = sci->Interval(ind).second - sci->Interval(ind).first;

			//カウントの量はスケールされる
			m_up2Histogram->at(ind) += Weight / width;
			m_up2HistogramErrorSq->at(ind) += AbsoluteErrorOfWeight * AbsoluteErrorOfWeight / width;
			

		}
		catch (std::out_of_range&) {
			throw;
		}
		catch (std::exception&) {
			throw;
		}
	}



	virtual void ClearHistogram()noexcept {
		for (auto& elm : *m_up2Histogram) {
			elm = 0;
		}
		for (auto& elm : *m_up2HistogramErrorSq) {
			elm = 0;
		}
	}



	//virtual const std::vector<double>& Get() {
	//	try {
	//		//RefineHistogram();
	//		return *m_up2Histogram;
	//	}
	//	catch (std::out_of_range&) {
	//		throw;
	//	}
	//}

	//virtual std::vector<double> Get() const {
	//	try {
	//		size_t im = m_sp2pinf->NumPoints();

	//		std::vector<double> v;
	//		v.reserve(im);
	//		for (size_t i = 0; i < im; i++) {
	//			v.push_back(GetAt(i));
	//		}

	//		return std::move(v);
	//	}
	//	catch (std::out_of_range&) {
	//		throw;
	//	}
	//}

	//virtual std::vector<double> GetE() const {
	//	try {
	//		size_t im = m_sp2pinf->NumPoints();

	//		std::vector<double> v;
	//		v.reserve(im);
	//		for (size_t i = 0; i < im; i++) {
	//			v.push_back(GetEAt(i));
	//		}

	//		return std::move(v);
	//	}
	//	catch (std::out_of_range&) {
	//		throw;
	//	}
	//}

	//virtual std::vector<double> GetE1()const {
	//	try {
	//		size_t im = m_sp2pinf->NumPoints();

	//		std::vector<double> v;
	//		v.reserve(im);
	//		for (size_t i = 0; i < im; i++) {
	//			v.push_back(GetE1At(i));
	//		}

	//		return std::move(v);
	//	}
	//	catch (std::out_of_range&) {
	//		throw;
	//	}
	//}

	//virtual const std::vector<double>& GetWidth()const noexcept {
	//	return *m_up2Width;
	//}


	//virtual const std::vector<double>& GetRaw()const noexcept {
	//	return *m_up2RawHistogram;
	//}

	//virtual double GetAt(const double Xvalue)const {
	//	try {
	//		return GetAt(m_up2ScaleInfo->X2IntervalIndex(Xvalue));
	//	}
	//	catch (std::out_of_range&) {
	//		throw;
	//	}
	//}

	////
	//virtual double GetAt(const size_t Pnt)const {
	//	try {
	//		return m_up2RawHistogram->at(Pnt) / m_up2Width->at(Pnt);
	//	}
	//	catch (std::out_of_range&) {
	//		throw;
	//	}
	//}

	//virtual double GetEAt(const double Xvalue)const {
	//	try {
	//		return GetEAt(m_up2ScaleInfo->X2IntervalIndex(Xvalue));
	//	}
	//	catch (std::out_of_range&) {
	//		throw;
	//	}
	//}

	////
	//virtual double GetEAt(const size_t Pnt)const {
	//	try {
	//		return sqrt(m_up2RawHistogramErrorSq->at(Pnt)) / m_up2Width->at(Pnt);
	//	}
	//	catch (std::out_of_range&) {
	//		throw;
	//	}
	//}

	//virtual double GetE1At(const double Xvalue)const {
	//	try {
	//		return GetE1At(m_up2ScaleInfo->X2IntervalIndex(Xvalue));
	//	}
	//	catch (std::out_of_range&) {
	//		throw;
	//	}
	//}

	////
	//virtual double GetE1At(const size_t Pnt)const {
	//	try {
	//		return sqrt(m_up2RawHistogramErrorSq->at(Pnt)) / m_up2RawHistogram->at(Pnt);
	//	}
	//	catch (std::out_of_range&) {
	//		throw;
	//	}
	//}

	//virtual const ScaleInformation& ScaleInfo()const noexcept {
	//	return *m_up2ScaleInfo;
	//}




private:

	std::unique_ptr<std::vector<double>> m_up2Histogram;
	std::unique_ptr<std::vector<double>> m_up2HistogramErrorSq;

	std::shared_ptr<PointInformation> m_sp2pinf;
	std::function<double(double, double, double)> m_f_Pnt2Value;

};



template<class class_ConversionToKES>
std::unique_ptr<TOF_KEHistogram<class_ConversionToKES>> TOF_KEHistogram<class_ConversionToKES>::make(const size_t DimSize, const double DimDelta, const double DimOffset, const bool mode_KE) {
	try {

		//ヒストグラム横軸の基本情報
		auto pinf = make_PointInformation(DimSize, PointInformation::Difference::Central0);

		//位置によって目盛りの刻みは異なる
		//十分な bin 幅をもった統一した刻みを用いる

		auto Histogram = std::vector<double>();
		auto HistogramErrorSq = std::vector<double>();
		Histogram.reserve(DimSize);
		HistogramErrorSq.reserve(DimSize);

		for (size_t i = 0; i < DimSize; i++) {
			Histogram.push_back(0);
			HistogramErrorSq.push_back(0);
		}


		//横軸のモードによって変換関数は異なる
		auto f_pnt2TOF_NoOffset = [](const double dPnt, const double PosX, const double PosY)->double {
			//pnt -> TOF_NoOffset
			return Pnt2X_Central0(dPnt, DimDelta, DimOffset);
		};


		if (mode_KE) {
			//KE

			auto f_pnt2KE = [=](const double dPnt, const double PosX, const double PosY)->double {
				//pnt -> TOF_NoOffset
				double TOF_NoOffset = f_pnt2TOF_NoOffset(dPnt, PosX, PosY);
				
				//TOF_NoOffset -> KE
				return class_ConversionToKES::KineticEnergy(PosX, PosY, TOF_NoOffset);
			};

			//TOF_KEHistogram_impl(std::unique_ptr<std::vector<double>>&& up2Histogram, std::unique_ptr<std::vector<double>>&& up2HistogramErrorSq, const std::shared_ptr<PointInformation>& sp2pinf,const std::function<double(double, double, double)>& f_Pnt2Value)
			return std::make_unique<TOF_KEHistogram_impl<class_ConversionToKES>>(std::unique_ptr<std::vector<double>>(Histogram), std::unique_ptr<std::vector<double>>(HistogramErrorSq), pinf, f_pnt2KE);

		}
		else {
			//TOF_NoOffset
			return std::make_unique<TOF_KEHistogram_impl<class_ConversionToKES>>(std::unique_ptr<std::vector<double>>(Histogram), std::unique_ptr<std::vector<double>>(HistogramErrorSq), pinf, f_pnt2TOF_NoOffset);

		}





	}
	catch (std::invalid_argument&) {
		throw;
	}
	catch (std::exception&) {
		throw;
	}
}













//
//
//
//template<class class_ConversionToDCS, class class_TheoreticalAtomicDCS_Elastic32, class class_SourceEvents>
//DetectorSensitivityDistribution1D<class_ConversionToDCS, class_TheoreticalAtomicDCS_Elastic32, class_SourceEvents>::~DetectorSensitivityDistribution1D() = default;
//
//
//template<class class_ConversionToDCS, class class_TheoreticalAtomicDCS_Elastic32, class class_SourceEvents>
//class DetectorSensitivityDistribution1D_impl 
//	:
//	public DetectorSensitivityDistribution1D<class_ConversionToDCS, class_TheoreticalAtomicDCS_Elastic32, class_SourceEvents>
//{
//public:
//	DetectorSensitivityDistribution1D_impl() = delete;
//	virtual ~DetectorSensitivityDistribution1D_impl() = default;
//
//	
//	DetectorSensitivityDistribution1D_impl(std::unique_ptr<ScaledHistogram1D>&& up2Hist1D_R, std::unique_ptr<ScaledHistogram1D>&& up2Hist1D_Theta_deg, 
//		std::unique_ptr<std::vector<double>>&& up2Curve_DCS_vs_Theta_deg, std::unique_ptr<std::vector<double>>&& up2Curve_Ratio_vs_Theta_deg, std::unique_ptr<std::vector<double>>&& up2Curve_DCS_vs_R, std::unique_ptr<std::vector<double>>&& up2Curve_Ratio_vs_R
//		//std::unique_ptr<std::vector<double>>&& up2Curve_Ratio_vs_Theta_deg_Error1, std::unique_ptr<std::vector<double>>&& up2Curve_Ratio_vs_R_Error1
//	)
//		:
//		m_up2Hist1D_R(std::forward<std::unique_ptr<ScaledHistogram1D>>(up2Hist1D_R)),
//		m_up2Hist1D_Theta_deg(std::forward<std::unique_ptr<ScaledHistogram1D>>(up2Hist1D_Theta_deg)),
//		m_up2Curve_DCS_vs_Theta_deg(std::forward<std::unique_ptr<std::vector<double>>>(up2Curve_DCS_vs_Theta_deg)),
//		m_up2Curve_DCS_vs_R(std::forward<std::unique_ptr<std::vector<double>>>(up2Curve_DCS_vs_R)),
//		m_up2Curve_Ratio_vs_Theta_deg(std::forward<std::unique_ptr<std::vector<double>>>(up2Curve_Ratio_vs_Theta_deg)),
//		m_up2Curve_Ratio_vs_R(std::forward<std::unique_ptr<std::vector<double>>>(up2Curve_Ratio_vs_R))
//		//m_up2Curve_Ratio_vs_Theta_deg_Error1(std::forward<std::unique_ptr<std::vector<double>>>(up2Curve_Ratio_vs_Theta_deg_Error1)),
//		//m_up2Curve_Ratio_vs_R_Error1(std::forward<std::unique_ptr<std::vector<double>>>(up2Curve_Ratio_vs_R_Error1))
//	{}
//
//
//	virtual std::vector<double> Calibrate_Hist_R(ScaledHistogram1D& Hist_R) {
//		try {
//
//			//両者のScaleInfoが一致している？
//			size_t im = Hist_R.ScaleInfo().NumIntervals();
//			if (Hist_R.ScaleInfo() != m_up2Hist1D_R->ScaleInfo()) {
//				throw std::invalid_argument("DetectorSensitivityDistribution1D::Calibrate_Hist_R : ScaleInformation");
//			}
//
//			auto v = std::vector<double>();
//			v.reserve(im);
//
//			//掛け算する
//			for (size_t i = 0; i < im; i++) {
//				v.push_back(Hist_R.GetAt(i) * m_up2Curve_DCS_vs_R->at(i) / m_up2Hist1D_R->GetAt(i));
//			}
//
//			return std::move(v);
//		}
//		catch (std::invalid_argument&) {
//			throw;
//		}
//		catch (std::out_of_range&) {
//			throw;
//		}
//		catch (std::exception&) {
//			throw;
//		}
//	}
//
//	virtual std::vector<double> Calibrate_Hist_Theta_deg(ScaledHistogram1D& Hist_Theta_deg) {
//		try {
//
//			//両者のScaleInfoが一致している？
//			size_t im = Hist_Theta_deg.ScaleInfo().NumIntervals();
//			if (Hist_Theta_deg.ScaleInfo() != m_up2Hist1D_Theta_deg->ScaleInfo()) {
//				throw std::invalid_argument("DetectorSensitivityDistribution1D::Calibrate_Hist_Theta_deg : ScaleIformation");
//			}
//
//			auto v = std::vector<double>();
//			v.reserve(im);
//
//			
//
//			//掛け算する
//			for (size_t i = 0; i < im; i++) {
//				v.push_back(Hist_Theta_deg.GetAt(i) * m_up2Curve_DCS_vs_Theta_deg->at(i) / m_up2Hist1D_Theta_deg->GetAt(i));
//			}
//
//			return std::move(v);
//		}
//		catch (std::invalid_argument&) {
//			throw;
//		}
//		catch (std::out_of_range&) {
//			throw;
//		}
//		catch (std::exception&) {
//			throw;
//		}
//	}
//
//	//virtual std::vector<double> ErrorOfCalibrated_Hist_R(ScaledHistogram1D& Hist_R) {
//	//	try {
//
//	//		//両者のScaleInfoが一致している？
//	//		size_t im = Hist_R.ScaleInfo().NumIntervals();
//	//		if (Hist_R.ScaleInfo() != m_up2Hist1D_R->ScaleInfo()) {
//	//			throw std::invalid_argument("DetectorSensitivityDistribution1D::ErrorOfCalibrated_Hist_R : ScaleInformation");
//	//		}
//
//	//		auto v = std::vector<double>();
//	//		v.reserve(im);
//
//	//		//誤差計算する
//	//		double err1 = 0;
//	//		double hr_err1 = 0, hr_standard_err1 = 0;
//	//		for (size_t i = 0; i < im; i++) {
//	//			hr_err1 = Hist_R.GetE1At(i);
//	//			hr_standard_err1 = m_up2Hist1D_R->GetE1At(i);
//
//	//			err1 = sqrt(hr_err1 * hr_err1 + hr_standard_err1 * hr_standard_err1);
//	//			
//	//			v.push_back(Hist_R.GetAt(i) / m_up2Hist1D_R->GetAt(i) * err1 * m_up2Curve_DCS_vs_R->at(i));
//	//		}
//
//	//		return std::move(v);
//	//	}
//	//	catch (std::invalid_argument&) {
//	//		throw;
//	//	}
//	//	catch (std::out_of_range&) {
//	//		throw;
//	//	}
//	//	catch (std::exception&) {
//	//		throw;
//	//	}
//	//}
//
//	virtual std::vector<double> ErrorOfCalibrated_Hist_Theta_deg(ScaledHistogram1D& Hist_Theta_deg) {
//		try {
//
//			//両者のScaleInfoが一致している？
//			size_t im = Hist_Theta_deg.ScaleInfo().NumIntervals();
//			if (Hist_Theta_deg.ScaleInfo() != m_up2Hist1D_Theta_deg->ScaleInfo()) {
//				throw std::invalid_argument("DetectorSensitivityDistribution1D::ErrorByCalib_Hist_Theta_deg : ScaleIformation");
//			}
//
//			auto v = std::vector<double>();
//			v.reserve(im);
//
//
//			//パラメータ
//			const double d = gv_d;//散乱点とワイヤーの距離
//			const double del = gv_del;//ビームブロック半径
//
//			//絶対誤差 e
//			const double d_e = gv_d_e;//測定するときの手振れ
//			const double del_e = gv_del_e;//BBの裏側へ回り込む信号がある？
//
//			//割合誤差 e1
//			const double d_e1 = d_e / d;//散乱点とワイヤーの距離
//			const double del_e1 = del_e / del;//ビームブロック半径
//
//			double bb_e1sq = 0;//ビームブロックによる感度補正係数の割合誤差自乗
//			double standard_e1 = 0;//感度補正用データの割合誤差
//			double arg_e1 = 0;//引数ヒストグラムの割合誤差
//			double e1 = 0;//感度補正後の割合誤差
//
//
//			//仲介用
//			double sin_alpha = 0;//ビームブロック半径によって隠れる方位角の正弦
//			double alpha = 0;
//			double alpha_esq = 0;//絶対誤差自乗
//			double bb = 0;//ビームブロックによる補正割合
//			
//
//
//
//			//誤差計算する
//			for (size_t i = 0; i < im; i++) {
//				sin_alpha = del / (d * tan(Hist_Theta_deg.ScaleInfo().IntervalIndex2XRepresentative(i) * gv_deg2rad));
//				alpha = asin(sin_alpha);
//				alpha_esq = (1 / (1 - sin_alpha * sin_alpha) - 1) * (d_e1 * d_e1 + del_e1 * del_e1);
//				bb = 1 - M_2_PI * alpha;
//				bb_e1sq = M_2_PI * M_2_PI * alpha_esq / (bb * bb);
//
//
//				standard_e1 = m_up2Hist1D_Theta_deg->GetE1At(i);
//
//
//				arg_e1 = Hist_Theta_deg.GetE1At(i);
//
//
//				e1 = sqrt(bb_e1sq + standard_e1 * standard_e1 + arg_e1 * arg_e1);
//
//
//				v.push_back(Hist_Theta_deg.GetAt(i) * m_up2Curve_DCS_vs_Theta_deg->at(i) / m_up2Hist1D_Theta_deg->GetAt(i) * e1);
//			}
//
//			return std::move(v);
//		}
//		catch (std::invalid_argument&) {
//			throw;
//		}
//		catch (std::out_of_range&) {
//			throw;
//		}
//		catch (std::exception&) {
//			throw;
//		}
//	}
//
//
//
//	////分かりやすくするため，以下はネストクラスに入れるかプレフィックスつけたい
//
//
//	virtual const ScaledHistogram1D& Hist_R()const noexcept {
//		return *m_up2Hist1D_R;
//	}
//
//	virtual const ScaleInformation& ScaleInfo_R()const noexcept {
//		return m_up2Hist1D_R->ScaleInfo();
//	}
//
//	virtual const ScaledHistogram1D& Hist_Theta_deg()const noexcept {
//		return *m_up2Hist1D_Theta_deg;
//	}
//
//	virtual const ScaleInformation& ScaleInfo_Theta_deg()const noexcept {
//		return m_up2Hist1D_Theta_deg->ScaleInfo();
//	}
//
//	virtual const std::vector<double>& DCS_vs_Theta_deg()const noexcept {
//		return *m_up2Curve_DCS_vs_Theta_deg;
//	}
//
//	virtual const std::vector<double>& Ratio_vs_Theta_deg()const noexcept {
//		return *m_up2Curve_Ratio_vs_Theta_deg;
//	}
//
//	/*virtual const std::vector<double>& Ratio_vs_Theta_deg_Error1()const noexcept {
//		return *m_up2Curve_Ratio_vs_Theta_deg_Error1;
//	}*/
//
//	virtual const std::vector<double>& DCS_vs_R()const noexcept {
//		return *m_up2Curve_DCS_vs_R;
//	}
//
//	virtual const std::vector<double>& Ratio_vs_R()const noexcept {
//		return *m_up2Curve_Ratio_vs_R;
//	}
//
//	/*virtual const std::vector<double>& Ratio_vs_R_Error1()const noexcept {
//		return *m_up2Curve_Ratio_vs_R_Error1;
//	}*/
//
//private:
//
//	std::unique_ptr<ScaledHistogram1D> m_up2Hist1D_R;//ソースイベントデータから読み込んだ生スペクトル
//	std::unique_ptr<ScaledHistogram1D> m_up2Hist1D_Theta_deg;//ソースイベントデータから読み込んだ生スペクトル
//
//	std::unique_ptr<std::vector<double>> m_up2Curve_DCS_vs_Theta_deg;//Elastic32に基づく理論値
//	std::unique_ptr<std::vector<double>> m_up2Curve_Ratio_vs_Theta_deg;//検出器感度
//	std::unique_ptr<std::vector<double>> m_up2Curve_DCS_vs_R;//Elastic32に基づく理論値
//	std::unique_ptr<std::vector<double>> m_up2Curve_Ratio_vs_R;//検出器感度
//	
//	//std::unique_ptr<std::vector<double>> m_up2Curve_Ratio_vs_Theta_deg_Error1;//相対誤差は1つける
//	//std::unique_ptr<std::vector<double>> m_up2Curve_Ratio_vs_R_Error1;//相対誤差は1つける
//
//};
//
//
//template<class class_ConversionToDCS, class class_TheoreticalAtomicDCS_Elastic32, class class_SourceEvents>
//std::unique_ptr<DetectorSensitivityDistribution1D<class_ConversionToDCS, class_TheoreticalAtomicDCS_Elastic32, class_SourceEvents>> DetectorSensitivityDistribution1D<class_ConversionToDCS, class_TheoreticalAtomicDCS_Elastic32, class_SourceEvents>::make(const size_t DimSize_R, const double DimDelta_R) {
//	try {
//
//		//カウントデータを累積する
//		auto hist_R = DCSHistogram<class_ConversionToDCS>::make_1D_vs_R(DimSize_R, DimDelta_R);
//		auto hist_Theta_deg = DCSHistogram<class_ConversionToDCS>::make_1D_vs_Theta_deg(DimSize_R, DimDelta_R);
//
//		const auto& sinfo_R = hist_R->ScaleInfo();
//		//for (const auto& r : class_SourceEvents::r) {//イベントデータ
//		//	if (sinfo_R.IsInRange(r)) {
//		//		hist_R->AddValue(r);
//		//		hist_Theta_deg->AddValue(class_ConversionToDCS::Theta_deg(r));
//		//	}
//		//}
//		for (size_t i = 0; i < class_SourceEvents::NumEvents; i++) {//イベントデータ
//			const auto tpl = class_SourceEvents::Event(i);
//			const double r = std::get<0>(tpl);
//			if (sinfo_R.IsInRange(r)) {
//				hist_R->AddValue(r);
//				hist_Theta_deg->AddValue(class_ConversionToDCS::Theta_deg(r));
//			}
//		}
//
//
//		////列を判別
//		//int cr = 0;
//
//		//double vr, vphi0, vphi;
//		//for (const auto& v : class_SourceEvents::r_phi) {//イベントデータ
//
//		//	if (cr == 0) {
//		//		vr = v;
//		//		cr++;
//		//	}
//		//	else {
//		//		//vphi0 = v;
//		//		//vphi = class_ConversionToDCS::ConvertAzimuthFormat(vphi0, class_SourceEvents::Format_Phi, Format_Phi);
//
//		//		//加算
//		//		if (sinfo_R.IsInRange(vr)) {
//		//			hist_R->AddValue(vr);
//		//			hist_Theta_deg->AddValue(class_ConversionToDCS::Theta_deg(vr));
//		//		}
//
//		//		cr = 0;
//		//	}
//
//		//}
//
//
//
//		//式から計算される
//		auto Curve_DCS_vs_Theta_deg = DCSTheoretical<class_ConversionToDCS, class_TheoreticalAtomicDCS_Elastic32>::make_1D_vs_Theta_deg(hist_Theta_deg->ScaleInfo());
//		auto Curve_DCS_vs_R = DCSTheoretical<class_ConversionToDCS, class_TheoreticalAtomicDCS_Elastic32>::make_1D_vs_R(hist_R->ScaleInfo());
//
//
//		//強度比が感度に相当する
//		auto Curve_Ratio_vs_Theta_deg = std::make_unique<std::vector<double>>();
//		auto Curve_Ratio_vs_R = std::make_unique<std::vector<double>>();
//		Curve_Ratio_vs_Theta_deg->reserve(DimSize_R);
//		Curve_Ratio_vs_R->reserve(DimSize_R);
//		//感度の誤差
//		//auto Curve_Ratio_vs_Theta_deg_Error1 = std::make_unique<std::vector<double>>();
//		//auto Curve_Ratio_vs_R_Error1 = std::make_unique<std::vector<double>>();
//		//Curve_Ratio_vs_Theta_deg_Error1->reserve(DimSize_R);
//		//Curve_Ratio_vs_R_Error1->reserve(DimSize_R);
//		for (size_t i = 0; i < DimSize_R; i++) {
//			Curve_Ratio_vs_Theta_deg->push_back(Curve_DCS_vs_Theta_deg->at(i) / hist_Theta_deg->GetAt(i));
//			Curve_Ratio_vs_R->push_back(Curve_DCS_vs_R->at(i) / hist_R->GetAt(i));
//			
//			//Curve_Ratio_vs_Theta_deg_Error1->push_back(Curve_DCS_vs_Theta_deg->at(i) * hist_Theta_deg->GetE1At(i));//相対誤差
//			//Curve_Ratio_vs_R_Error1->push_back(Curve_DCS_vs_R->at(i) * hist_R->GetE1At(i));//相対誤差
//		}
//
//
//		return std::make_unique<DetectorSensitivityDistribution1D_impl<class_ConversionToDCS, class_TheoreticalAtomicDCS_Elastic32, class_SourceEvents>>(std::move(hist_R), std::move(hist_Theta_deg), std::move(Curve_DCS_vs_Theta_deg), std::move(Curve_Ratio_vs_Theta_deg), std::move(Curve_DCS_vs_R), std::move(Curve_Ratio_vs_R)
//			
//			);
//
//	}
//	catch (std::invalid_argument&) {
//		throw;
//	}
//	catch (std::out_of_range&) {
//		throw;
//	}
//	catch (std::exception&) {
//		throw;
//	}
//}
//
//
//
//
//
//
//
//
//
//template<class class_ConversionToDCS, class class_TheoreticalAtomicDCS_Elastic32, class class_SourceEvents>
//DetectorSensitivityDistribution2D<class_ConversionToDCS, class_TheoreticalAtomicDCS_Elastic32, class_SourceEvents>::~DetectorSensitivityDistribution2D() = default;
//
//template<class class_ConversionToDCS, class class_TheoreticalAtomicDCS_Elastic32, class class_SourceEvents>
//class DetectorSensitivityDistribution2D_impl
//	:
//	public DetectorSensitivityDistribution2D<class_ConversionToDCS, class_TheoreticalAtomicDCS_Elastic32, class_SourceEvents>
//{
//public:
//	DetectorSensitivityDistribution2D_impl() = delete;
//	virtual ~DetectorSensitivityDistribution2D_impl() = default;
//
//
//	DetectorSensitivityDistribution2D_impl(
//		std::unique_ptr<ScaledHistogram2D>&& up2Hist2D_R_Phi_deg, std::unique_ptr<ScaledHistogram2D>&& up2Hist2D_Theta_deg_VarPhi_deg,
//		std::unique_ptr<std::vector<std::vector<double>>>&& up2Curve_DCS_vs_Theta_deg_VarPhi_deg, std::unique_ptr<std::vector<std::vector<double>>>&& up2Curve_Ratio_vs_Theta_deg_VarPhi_deg,
//		std::unique_ptr<std::vector<std::vector<double>>>&& up2Curve_DCS_vs_R_Phi_deg, std::unique_ptr<std::vector<std::vector<double>>>&& up2Curve_Ratio_vs_R_Phi_deg,
//		std::unique_ptr<std::vector<std::vector<double>>>&& up2Curve_Ratio_vs_Theta_deg_VarPhi_deg_Error1, std::unique_ptr<std::vector<std::vector<double>>>&& up2Curve_Ratio_vs_R_Phi_deg_Error1
//	)
//		:
//		m_up2Hist2D_R_Phi_deg(std::forward<std::unique_ptr<ScaledHistogram2D>>(up2Hist2D_R_Phi_deg)),
//		m_up2Hist2D_Theta_deg_VarPhi_deg(std::forward<std::unique_ptr<ScaledHistogram2D>>(up2Hist2D_Theta_deg_VarPhi_deg)),
//		m_up2Curve_DCS_vs_Theta_deg_VarPhi_deg(std::forward<std::unique_ptr<std::vector<std::vector<double>>>>(up2Curve_DCS_vs_Theta_deg_VarPhi_deg)),
//		m_up2Curve_Ratio_vs_Theta_deg_VarPhi_deg(std::forward<std::unique_ptr<std::vector<std::vector<double>>>>(up2Curve_Ratio_vs_Theta_deg_VarPhi_deg)),
//		m_up2Curve_DCS_vs_R_Phi_deg(std::forward<std::unique_ptr<std::vector<std::vector<double>>>>(up2Curve_DCS_vs_R_Phi_deg)),
//		m_up2Curve_Ratio_vs_R_Phi_deg(std::forward<std::unique_ptr<std::vector<std::vector<double>>>>(up2Curve_Ratio_vs_R_Phi_deg)),
//		m_up2Curve_Ratio_vs_Theta_deg_VarPhi_deg_Error1(std::forward<std::unique_ptr<std::vector<std::vector<double>>>>(up2Curve_Ratio_vs_Theta_deg_VarPhi_deg_Error1)),
//		m_up2Curve_Ratio_vs_R_Phi_deg_Error1(std::forward<std::unique_ptr<std::vector<std::vector<double>>>>(up2Curve_Ratio_vs_R_Phi_deg_Error1))
//	{}
//
//	virtual std::vector<std::vector<double>> Calibrate_Hist_R_Phi_deg(ScaledHistogram2D& Hist_R_Phi_deg) {
//		try {
//
//			if (Hist_R_Phi_deg.ScaleInfo0() != ScaleInfo_R()) {
//				throw std::invalid_argument("DetectorSensitivityDistribution2D::Calibrate_Hist_R_Phi_deg : ScaleInformation0");
//			}
//			if (Hist_R_Phi_deg.ScaleInfo1() != ScaleInfo_Phi_deg()) {
//				throw std::invalid_argument("DetectorSensitivityDistribution2D::Calibrate_Hist_R_Phi_deg : ScaleInformation1");
//			}
//
//			size_t im = Hist_R_Phi_deg.ScaleInfo0().NumIntervals();
//			size_t jm = Hist_R_Phi_deg.ScaleInfo1().NumIntervals();
//
//			auto v = std::vector<std::vector<double>>();
//			v.reserve(im);
//
//			//掛け算する
//			for (size_t i = 0; i < im; i++) {
//
//				auto vv = std::vector<double>();
//				vv.reserve(jm);
//
//				for (size_t j = 0; j < jm; j++) {
//					vv.push_back(Hist_R_Phi_deg.GetAt(i, j) * m_up2Curve_DCS_vs_R_Phi_deg->at(i).at(j) / m_up2Hist2D_R_Phi_deg->GetAt(i, j));
//				}
//				
//				v.push_back(std::move(vv));
//			}
//
//			return std::move(v);
//		}
//		catch (std::invalid_argument&) {
//			throw;
//		}
//		catch (std::out_of_range&) {
//			throw;
//		}
//		catch (std::exception&) {
//			throw;
//		}
//	}
//
//	virtual std::vector<std::vector<double>> Calibrate_Hist_Theta_deg_VarPhi_deg(ScaledHistogram2D& Hist_Theta_deg_VarPhi_deg) {
//		try {
//
//			if (Hist_Theta_deg_VarPhi_deg.ScaleInfo0() != ScaleInfo_Theta_deg()) {
//				throw std::invalid_argument("DetectorSensitivityDistribution2D::Calibrate_Hist_Theta_deg_VarPhi_deg : ScaleInformation0");
//			}
//			if (Hist_Theta_deg_VarPhi_deg.ScaleInfo1() != ScaleInfo_VarPhi_deg()) {
//				throw std::invalid_argument("DetectorSensitivityDistribution2D::Calibrate_Hist_Theta_deg_VarPhi_deg : ScaleInformation1");
//			}
//
//			size_t im = Hist_Theta_deg_VarPhi_deg.ScaleInfo0().NumIntervals();
//			size_t jm = Hist_Theta_deg_VarPhi_deg.ScaleInfo1().NumIntervals();
//
//			auto v = std::vector<std::vector<double>>();
//			v.reserve(im);
//
//			//掛け算する
//			for (size_t i = 0; i < im; i++) {
//				
//				auto vv = std::vector<double>();
//				vv.reserve(jm);
//
//				for (size_t j = 0; j < jm; j++) {
//					vv.push_back(Hist_Theta_deg_VarPhi_deg.GetAt(i, j) * m_up2Curve_DCS_vs_Theta_deg_VarPhi_deg->at(i).at(j) / m_up2Hist2D_Theta_deg_VarPhi_deg->GetAt(i, j));
//				}
//
//				v.push_back(std::move(vv));
//			}
//
//			return std::move(v);
//		}
//		catch (std::invalid_argument&) {
//			throw;
//		}
//		catch (std::out_of_range&) {
//			throw;
//		}
//		catch (std::exception&) {
//			throw;
//		}
//	}
//
//	//virtual std::vector<std::vector<double>> ErrorOfCalibrated_Hist_R_Phi_deg(ScaledHistogram2D& Hist_R_Phi_deg) = 0;
//
//
//	virtual std::vector<std::vector<double>> ErrorOfCalibrated_Hist_Theta_deg_VarPhi_deg(ScaledHistogram2D& Hist_Theta_deg_VarPhi_deg) {
//		try {
//
//			if (Hist_Theta_deg_VarPhi_deg.ScaleInfo0() != ScaleInfo_Theta_deg()) {
//				throw std::invalid_argument("DetectorSensitivityDistribution2D::ErrorOfCalibrated_Hist_Theta_deg_VarPhi_deg : ScaleInformation0");
//			}
//			if (Hist_Theta_deg_VarPhi_deg.ScaleInfo1() != ScaleInfo_VarPhi_deg()) {
//				throw std::invalid_argument("DetectorSensitivityDistribution2D::ErrorOfCalibrated_Hist_Theta_deg_VarPhi_deg : ScaleInformation1");
//			}
//
//			size_t im = Hist_Theta_deg_VarPhi_deg.ScaleInfo0().NumIntervals();
//			size_t jm = Hist_Theta_deg_VarPhi_deg.ScaleInfo1().NumIntervals();
//
//			auto v = std::vector<std::vector<double>>();
//			v.reserve(im);
//
//
//			//パラメータ
//			const double d = gv_d;//散乱点とワイヤーの距離
//			const double del = gv_del;//ビームブロック半径
//
//			//絶対誤差 e
//			const double d_e = gv_d_e;//測定するときの手振れ
//			const double del_e = gv_del_e;//BBの裏側へ回り込む信号がある？
//
//			//割合誤差 e1
//			const double d_e1 = d_e / d;//散乱点とワイヤーの距離
//			const double del_e1 = del_e / del;//ビームブロック半径
//
//			double bb_e1sq = 0;//ビームブロックによる感度補正係数の割合誤差自乗
//			double standard_e1 = 0;//感度補正用データの割合誤差
//			double arg_e1 = 0;//引数ヒストグラムの割合誤差
//			double e1 = 0;//感度補正後の割合誤差
//
//
//			//仲介用
//			double sin_alpha = 0;//ビームブロック半径によって隠れる方位角の正弦
//			double alpha = 0;
//			double alpha_esq = 0;//絶対誤差自乗
//			double bb = 0;//ビームブロックによる補正割合
//
//			for (size_t i = 0; i < im; i++) {
//
//				auto vv = std::vector<double>();
//				vv.reserve(jm);
//
//				for (size_t j = 0; j < jm; j++) {
//					
//					standard_e1 = m_up2Hist2D_Theta_deg_VarPhi_deg->GetE1At(i, j);
//
//					arg_e1 = Hist_Theta_deg_VarPhi_deg.GetE1At(i, j);
//
//					
//					e1 = sqrt(standard_e1 * standard_e1 + arg_e1 * arg_e1);
//
//
//					vv.push_back(Hist_Theta_deg_VarPhi_deg.GetAt(i, j) * m_up2Curve_DCS_vs_Theta_deg_VarPhi_deg->at(i).at(j) / m_up2Hist2D_Theta_deg_VarPhi_deg->GetAt(i, j) * e1);
//				}
//
//				v.push_back(std::move(vv));
//			}
//
//			return std::move(v);
//		}
//		catch (std::invalid_argument&) {
//			throw;
//		}
//		catch (std::out_of_range&) {
//			throw;
//		}
//		catch (std::exception&) {
//			throw;
//		}
//	}
//
//
//
//
//
//
//	
//	virtual const ScaledHistogram2D& Hist_R_Phi_deg()const noexcept {
//		return *m_up2Hist2D_R_Phi_deg;
//	}
//
//	virtual const ScaleInformation& ScaleInfo_R()const noexcept {
//		return m_up2Hist2D_R_Phi_deg->ScaleInfo0();
//	}
//
//	virtual const ScaleInformation& ScaleInfo_Phi_deg()const noexcept {
//		return m_up2Hist2D_R_Phi_deg->ScaleInfo1();
//	}
//
//
//
//	virtual const ScaledHistogram2D& Hist_Theta_deg_VarPhi_deg()const noexcept {
//		return *m_up2Hist2D_Theta_deg_VarPhi_deg;
//	}
//
//	virtual const ScaleInformation& ScaleInfo_Theta_deg()const noexcept {
//		return m_up2Hist2D_Theta_deg_VarPhi_deg->ScaleInfo0();
//	}
//
//	virtual const ScaleInformation& ScaleInfo_VarPhi_deg()const noexcept {
//		return m_up2Hist2D_Theta_deg_VarPhi_deg->ScaleInfo1();
//	}
//
//
//
//	virtual const std::vector<std::vector<double>>& DCS_vs_Theta_deg_VarPhi_deg()const noexcept {
//		return *m_up2Curve_DCS_vs_Theta_deg_VarPhi_deg;
//	}
//
//
//	virtual const std::vector<std::vector<double>>& Ratio_vs_Theta_deg_VarPhi_deg()const noexcept {
//		return *m_up2Curve_Ratio_vs_Theta_deg_VarPhi_deg;
//	}
//
//	virtual const std::vector<std::vector<double>>& Ratio_vs_Theta_deg_VarPhi_deg_Error1()const noexcept {
//		return *m_up2Curve_Ratio_vs_Theta_deg_VarPhi_deg_Error1;
//	}
//
//
//
//	virtual const std::vector<std::vector<double>>& DCS_vs_R_Phi_deg()const noexcept {
//		return *m_up2Curve_DCS_vs_R_Phi_deg;
//	}
//
//
//	virtual const std::vector<std::vector<double>>& Ratio_vs_R_Phi_deg()const noexcept {
//		return *m_up2Curve_Ratio_vs_R_Phi_deg;
//	}
//
//	virtual const std::vector<std::vector<double>>& Ratio_vs_R_Phi_deg_Error1()const noexcept {
//		return *m_up2Curve_Ratio_vs_R_Phi_deg_Error1;
//	}
//
//
//private:
//
//	std::unique_ptr<ScaledHistogram2D> m_up2Hist2D_R_Phi_deg;
//	std::unique_ptr<ScaledHistogram2D> m_up2Hist2D_Theta_deg_VarPhi_deg;
//
//	std::unique_ptr<std::vector<std::vector<double>>> m_up2Curve_DCS_vs_Theta_deg_VarPhi_deg;
//	std::unique_ptr<std::vector<std::vector<double>>> m_up2Curve_Ratio_vs_Theta_deg_VarPhi_deg;
//	
//	std::unique_ptr<std::vector<std::vector<double>>> m_up2Curve_DCS_vs_R_Phi_deg;
//	std::unique_ptr<std::vector<std::vector<double>>> m_up2Curve_Ratio_vs_R_Phi_deg;
//
//
//	std::unique_ptr<std::vector<std::vector<double>>> m_up2Curve_Ratio_vs_Theta_deg_VarPhi_deg_Error1;//相対誤差は1つける
//	std::unique_ptr<std::vector<std::vector<double>>> m_up2Curve_Ratio_vs_R_Phi_deg_Error1;//相対誤差は1つける
//};
//
//template<class class_ConversionToDCS, class class_TheoreticalAtomicDCS_Elastic32, class class_SourceEvents>
//std::unique_ptr<DetectorSensitivityDistribution2D<class_ConversionToDCS, class_TheoreticalAtomicDCS_Elastic32, class_SourceEvents>> DetectorSensitivityDistribution2D<class_ConversionToDCS, class_TheoreticalAtomicDCS_Elastic32, class_SourceEvents>::make(const size_t DimSize_R, const double DimDelta_R, const size_t DimSize_Phi, const LibFlag::AzimuthFormat Format_Phi, const LibFlag::AzimuthFormat Format_VarPhi) {
//	try {
//		
//		//カウントデータを累積する
//		std::unique_ptr<ScaledHistogram2D> hist_R_Phi_deg = DCSHistogram<class_ConversionToDCS>::make_2D_vs_R_Phi(DimSize_R, DimDelta_R, DimSize_Phi, Format_Phi);
//		std::unique_ptr<ScaledHistogram2D> hist_Theta_deg_VarPhi_deg = DCSHistogram<class_ConversionToDCS>::make_2D_vs_Theta_deg_VarPhi(DimSize_R, DimDelta_R, DimSize_Phi, Format_VarPhi);
//
//		const auto& sinfo_R = hist_R_Phi_deg->ScaleInfo0();
//		const auto& sinfo_Phi_deg = hist_R_Phi_deg->ScaleInfo1();
//
//		double r, phi0, phi, varphi;
//		for (size_t i = 0; i < class_SourceEvents::NumEvents; i++) {//イベントデータ
//			const auto tpl = class_SourceEvents::Event(i);
//			r = std::get<0>(tpl);
//			phi0 = std::get<1>(tpl);
//
//			phi = LibFlag::ConvertAzimuthFormat(phi0, class_SourceEvents::Format_Phi, Format_Phi);
//			varphi = class_ConversionToDCS::VarPhi(phi, Format_Phi, Format_VarPhi);
//
//			if (sinfo_R.IsInRange(r) && sinfo_Phi_deg.IsInRange(phi)) {//範囲内なら加算
//				hist_R_Phi_deg->AddValue(r, phi);
//				hist_Theta_deg_VarPhi_deg->AddValue(class_ConversionToDCS::Theta_deg(r), varphi);
//			}
//		}
//
//		
//		//式から計算される
//		std::unique_ptr<std::vector<std::vector<double>>> Curve_DCS_vs_R_Phi_deg = DCSTheoretical<class_ConversionToDCS, class_TheoreticalAtomicDCS_Elastic32>::make_2D_vs_R_Phi(hist_R_Phi_deg->ScaleInfo0(), hist_R_Phi_deg->ScaleInfo1());
//		std::unique_ptr<std::vector<std::vector<double>>> Curve_DCS_vs_Theta_deg_VarPhi_deg = DCSTheoretical<class_ConversionToDCS, class_TheoreticalAtomicDCS_Elastic32>::make_2D_vs_Theta_deg_VarPhi(hist_Theta_deg_VarPhi_deg->ScaleInfo0(), hist_Theta_deg_VarPhi_deg->ScaleInfo1());
//		
//
//		//強度比が感度に相当する
//		auto Curve_Ratio_vs_Theta_deg_VarPhi_deg = std::make_unique<std::vector<std::vector<double>>>();
//		auto Curve_Ratio_vs_R_Phi_deg = std::make_unique<std::vector<std::vector<double>>>();
//		Curve_Ratio_vs_Theta_deg_VarPhi_deg->reserve(DimSize_R);
//		Curve_Ratio_vs_R_Phi_deg->reserve(DimSize_R);
//		//
//		auto Curve_Ratio_vs_Theta_deg_VarPhi_deg_Error1 = std::make_unique<std::vector<std::vector<double>>>();
//		auto Curve_Ratio_vs_R_Phi_deg_Error1 = std::make_unique<std::vector<std::vector<double>>>();
//		Curve_Ratio_vs_Theta_deg_VarPhi_deg_Error1->reserve(DimSize_R);
//		Curve_Ratio_vs_R_Phi_deg_Error1->reserve(DimSize_R);
//		for (size_t i = 0; i < DimSize_R; i++) {
//
//			auto v_tvp = std::vector<double>();
//			auto v_rp = std::vector<double>();
//			v_tvp.reserve(DimSize_Phi);
//			v_rp.reserve(DimSize_Phi);
//
//			auto v_tvp_err = std::vector<double>();
//			auto v_rp_err = std::vector<double>();
//			v_tvp_err.reserve(DimSize_Phi);
//			v_rp_err.reserve(DimSize_Phi);
//
//			for (size_t j = 0; j < DimSize_Phi; j++) {
//				v_tvp.push_back(Curve_DCS_vs_Theta_deg_VarPhi_deg->at(i).at(j) / hist_Theta_deg_VarPhi_deg->GetAt(i, j));
//				v_rp.push_back(Curve_DCS_vs_R_Phi_deg->at(i).at(j) / hist_R_Phi_deg->GetAt(i, j));
//
//				v_tvp_err.push_back(Curve_DCS_vs_Theta_deg_VarPhi_deg->at(i).at(j) / hist_Theta_deg_VarPhi_deg->GetEAt(i, j));//相対誤差
//				v_rp_err.push_back(Curve_DCS_vs_R_Phi_deg->at(i).at(j) / hist_R_Phi_deg->GetEAt(i, j));//相対誤差
//			}
//
//			Curve_Ratio_vs_Theta_deg_VarPhi_deg->push_back(std::move(v_tvp));
//			Curve_Ratio_vs_R_Phi_deg->push_back(std::move(v_rp));
//
//			Curve_Ratio_vs_Theta_deg_VarPhi_deg_Error1->push_back(std::move(v_tvp_err));
//			Curve_Ratio_vs_R_Phi_deg_Error1->push_back(std::move(v_rp_err));
//		}
//
//
//		return std::make_unique<DetectorSensitivityDistribution2D_impl<class_ConversionToDCS, class_TheoreticalAtomicDCS_Elastic32, class_SourceEvents>>(
//			std::move(hist_R_Phi_deg), std::move(hist_Theta_deg_VarPhi_deg),
//			std::move(Curve_DCS_vs_Theta_deg_VarPhi_deg), std::move(Curve_Ratio_vs_Theta_deg_VarPhi_deg),
//			std::move(Curve_DCS_vs_R_Phi_deg), std::move(Curve_Ratio_vs_R_Phi_deg),
//			std::move(Curve_Ratio_vs_Theta_deg_VarPhi_deg_Error1), std::move(Curve_Ratio_vs_R_Phi_deg_Error1)
//			);
//
//	}
//	catch (std::invalid_argument&) {
//		throw;
//	}
//	catch (std::out_of_range&) {
//		throw;
//	}
//	catch (std::exception&) {
//		throw;
//	}
//}
//
//








template<class class_ConversionToDCS, class class_TheoreticalAtomicDCS_Elastic32, class class_SourceEvents>
DetectorSensitivityDistribution1<class_ConversionToDCS, class_TheoreticalAtomicDCS_Elastic32, class_SourceEvents>::~DetectorSensitivityDistribution1() = default;

template<class class_ConversionToDCS, class class_TheoreticalAtomicDCS_Elastic32, class class_SourceEvents>
class DetectorSensitivityDistribution1_impl:public DetectorSensitivityDistribution1<class_ConversionToDCS, class_TheoreticalAtomicDCS_Elastic32, class_SourceEvents> {
public:

	DetectorSensitivityDistribution1_impl() = delete;
	virtual ~DetectorSensitivityDistribution1_impl() = default;


	DetectorSensitivityDistribution1_impl(const LibFlag::AzimuthFormat Format_Phi, 
		std::unique_ptr<std::vector<std::vector<double>>>&& Ratio_vs_R_Phi, std::unique_ptr<std::vector<std::vector<double>>>&& Ratio_vs_R_Phi_Error,
		std::unique_ptr<ScaledHistogram2D>&& SourceEvents_hist_R_Phi, std::unique_ptr<std::vector<std::vector<double>>>&& Curve_DCS_vs_R_Phi
	)
		:
		m_Format_Phi(Format_Phi),
		m_Ratio_vs_R_Phi(std::forward<std::unique_ptr<std::vector<std::vector<double>>>>(Ratio_vs_R_Phi)),
		m_Ratio_vs_R_Phi_Error(std::forward<std::unique_ptr<std::vector<std::vector<double>>>>(Ratio_vs_R_Phi_Error)),
		m_SourceEvents_hist_R_Phi(std::forward<std::unique_ptr<ScaledHistogram2D>>(SourceEvents_hist_R_Phi)),
		m_Curve_DCS_vs_R_Phi(std::forward<std::unique_ptr<std::vector<std::vector<double>>>>(Curve_DCS_vs_R_Phi))
	{}



	virtual std::pair<double, double> Weight(const double r, const double phi)const {
		try {
			
			size_t ii = m_SourceEvents_hist_R_Phi->ScaleInfo0().X2IntervalIndex(r);
			size_t jj = m_SourceEvents_hist_R_Phi->ScaleInfo1().X2IntervalIndex(phi);
			return std::pair<double, double>(m_Ratio_vs_R_Phi->at(ii).at(jj), m_Ratio_vs_R_Phi_Error->at(ii).at(jj));
		}
		catch (std::out_of_range&) {
			throw;
		}
	}

	virtual LibFlag::AzimuthFormat Format_Phi()const noexcept {
		return m_Format_Phi;
	}






	virtual std::vector<std::vector<double>> DetectorSensitivityDistribution()const {
		try {
			if (!m_Ratio_vs_R_Phi) {
				throw std::exception("DetectorSensitivityDistribution1::DetectorSensitivityDistribution : uptr ?");
			}

			size_t im, jm;
			im = m_Ratio_vs_R_Phi->size();

			auto v = std::vector<std::vector<double>>();
			v.reserve(im);

			for (size_t i = 0; i < im; i++) {

				jm = m_Ratio_vs_R_Phi->at(i).size();
				auto vv = std::vector<double>();
				vv.reserve(jm);

				for (size_t j = 0; j < jm; j++) {
					vv.push_back(m_Ratio_vs_R_Phi->at(i).at(j));
				}

				v.push_back(std::move(vv));
			}

			return std::move(v);

		}
		catch (std::exception&) {
			throw;
		}
	}

	virtual std::vector<std::vector<double>> DetectorSensitivityDistributionE()const {
		try {
			if (!m_Ratio_vs_R_Phi_Error) {
				throw std::exception("DetectorSensitivityDistribution1::DetectorSensitivityDistributionE : uptr ?");
			}

			size_t im, jm;
			im = m_Ratio_vs_R_Phi_Error->size();

			auto v = std::vector<std::vector<double>>();
			v.reserve(im);

			for (size_t i = 0; i < im; i++) {

				jm = m_Ratio_vs_R_Phi_Error->at(i).size();
				auto vv = std::vector<double>();
				vv.reserve(jm);

				for (size_t j = 0; j < jm; j++) {
					vv.push_back(m_Ratio_vs_R_Phi_Error->at(i).at(j));
				}

				v.push_back(std::move(vv));
			}

			return std::move(v);

		}
		catch (std::exception&) {
			throw;
		}
	}





private:
	//二次元補間
	//static double interp2D(const std::vector<std::vector<double>>& Array2D, const ScaleInformation& sci0, const ScaleInformation& sci1, const double x, const double y) {
	//
	//	/*
	//	*z = a + b(x-x0) + c(y-y0) + d(x-x0)(y-y0)
	//	*で補間
	//	*
	//	*  2+-----------+3
	//	*	|			|
	//	* dy| +(x,y)	|
	//	*	|			|
	//	*  0+-----------+1
	//	*		 dx
	//	*
	//	* 4点を通る条件から係数を求めると任意の点(x,y)に対する補間値は
	//	*
	//	*	z = z0 + z10*dX + z20*dY + (z30-z10-z20)*dX*dY
	//	*	  = z0(1-dX-dY) + z1(dX-dX*dY) + z2(dY-dX*dY) + z3(dX*dY)
	//	*
	//	* ただし
	//	*	dX=(x-x0)/dx
	//	*	dY=(y-y0)/dy
	//	* など
	//	*/
	//
	//	size_t Size0 = Array2D.size();
	//	size_t Size1 = Array2D.at(0).size();
	//
	//	size_t ppp = sci0.X2IntervalIndex(x);
	//	size_t qqq = sci1.X2IntervalIndex(y);
	//	
	//	double dx = sci0.IntervalWidth(ppp);
	//	double dy = sci1.IntervalWidth(qqq);
	//
	//	double _dx = 1 / dx;
	//	double _dy = 1 / dy;
	//	double _dxdy = 1 / (dx * dy);
	//
	//	
	//
	//	double x0 = offsetx + fdp * dx;
	//	double y0 = offsety + fdq * dy;
	//	size_t p0 = static_cast<size_t>(fdp);
	//	size_t q0 = static_cast<size_t>(fdq);
	//
	//	double xx = (x - x0) * _dx;
	//	double yy = (y - y0) * _dy;
	//	double xxyy = xx * yy;
	//
	//	double V0 = (1 - xx - yy + xxyy) * Array2D[p0][q0];
	//	double V1 = (xx - xxyy) * Array2D[p0 + 1][q0];
	//	double V2 = (yy - xxyy) * Array2D[p0][q0 + 1];
	//	double V3 = xxyy * Array2D[p0 + 1][q0 + 1];
	//
	//	return V0 + V1 + V2 + V3;
	//}



	LibFlag::AzimuthFormat m_Format_Phi;

	std::unique_ptr<std::vector<std::vector<double>>> m_Ratio_vs_R_Phi;
	std::unique_ptr<std::vector<std::vector<double>>> m_Ratio_vs_R_Phi_Error;

	std::unique_ptr<ScaledHistogram2D> m_SourceEvents_hist_R_Phi;
	std::unique_ptr<std::vector<std::vector<double>>> m_Curve_DCS_vs_R_Phi;

};

template<class class_ConversionToDCS, class class_TheoreticalAtomicDCS_Elastic32, class class_SourceEvents>
std::unique_ptr<DetectorSensitivityDistribution1<class_ConversionToDCS, class_TheoreticalAtomicDCS_Elastic32, class_SourceEvents>> DetectorSensitivityDistribution1<class_ConversionToDCS, class_TheoreticalAtomicDCS_Elastic32, class_SourceEvents>::make(const size_t DimSize_R, const double DimDelta_R, const size_t DimSize_Phi, const LibFlag::AzimuthFormat Format_Phi) {
	try {

		//カウントデータを累積する
		std::unique_ptr<ScaledHistogram2D> hist_R_Phi = DCSHistogram<class_ConversionToDCS>::make_2D_vs_R_Phi(DimSize_R, DimDelta_R, DimSize_Phi, Format_Phi);
		
		const auto& sinfo_R = hist_R_Phi->ScaleInfo0();
		const auto& sinfo_Phi = hist_R_Phi->ScaleInfo1();

		double r, phi0, phi;
		for (size_t i = 0; i < class_SourceEvents::NumEvents; i++) {//イベントデータ
			const auto tpl = class_SourceEvents::Event(i);
			r = std::get<0>(tpl);
			phi0 = std::get<1>(tpl);

			phi = LibFlag::ConvertAzimuthFormat(phi0, class_SourceEvents::Format_Phi, Format_Phi);

			if (sinfo_R.IsInRange(r) && sinfo_Phi.IsInRange(phi)) {//範囲内なら加算
				hist_R_Phi->AddValue(r, phi, 1, 1);
			}
		}

		//DCS理論値
		std::unique_ptr<std::vector<std::vector<double>>> Curve_DCS_vs_R_Phi = DCSTheoretical<class_ConversionToDCS, class_TheoreticalAtomicDCS_Elastic32>::make_2D_vs_R_Phi(sinfo_R, sinfo_Phi);
		

		//感度補正値
		auto Ratio_vs_R_Phi = std::make_unique<std::vector<std::vector<double>>>();
		//感度補正値の絶対誤差
		auto Ratio_vs_R_Phi_Error = std::make_unique<std::vector<std::vector<double>>>();
		
		Ratio_vs_R_Phi->reserve(DimSize_R);
		Ratio_vs_R_Phi_Error->reserve(DimSize_R);

		for (size_t i = 0; i < DimSize_R; i++) {

			auto v = std::vector<double>();
			auto ve = std::vector<double>();
			v.reserve(DimSize_Phi);
			ve.reserve(DimSize_Phi);

			for (size_t j = 0; j < DimSize_Phi; j++) {
				
				v.push_back(Curve_DCS_vs_R_Phi->at(i).at(j) / hist_R_Phi->GetAt(i, j));
				ve.push_back(Curve_DCS_vs_R_Phi->at(i).at(j) * hist_R_Phi->GetE1At(i, j) / hist_R_Phi->GetAt(i, j));

			}

			Ratio_vs_R_Phi->push_back(std::move(v));
			Ratio_vs_R_Phi_Error->push_back(std::move(ve));

		}

		//もれ
		Ratio_vs_R_Phi->at(2).at(12) = 0.25 * (Ratio_vs_R_Phi->at(1).at(12) + Ratio_vs_R_Phi->at(3).at(12) + Ratio_vs_R_Phi->at(2).at(11) + Ratio_vs_R_Phi->at(2).at(13));
		Ratio_vs_R_Phi_Error->at(2).at(12) = 0.25 * (Ratio_vs_R_Phi_Error->at(1).at(12) + Ratio_vs_R_Phi_Error->at(3).at(12) + Ratio_vs_R_Phi_Error->at(2).at(11) + Ratio_vs_R_Phi_Error->at(2).at(13));


		//作成
		return std::make_unique<DetectorSensitivityDistribution1_impl<class_ConversionToDCS, class_TheoreticalAtomicDCS_Elastic32, class_SourceEvents>>(Format_Phi,
			std::move(Ratio_vs_R_Phi), std::move(Ratio_vs_R_Phi_Error),
			std::move(hist_R_Phi), std::move(Curve_DCS_vs_R_Phi)
			);


	}
	catch (std::invalid_argument&) {
		throw;
	}
	catch (std::out_of_range&) {
		throw;
	}
	catch (std::exception&) {
		throw;
	}
}













//--------------------------------
//テンプレートの特殊化
//--------------------------------


template class DCSTheoretical<Conv_1000_1000, Elastic32_He_1000eV>;
template class DCSTheoretical<Conv_1000_1000, Elastic32_C_1000eV>;
template class DCSTheoretical<Conv_1000_1000, Elastic32_N_1000eV>;
template class DCSTheoretical<Conv_1000_1000, Elastic32_Ne_1000eV>;
template class DCSTheoretical<Conv_1000_1000, Elastic32_Cl_1000eV>;
template class DCSTheoretical<Conv_1000_1000, Elastic32_Ar_1000eV>;
template class DCSTheoretical<Conv_1000_1000, Elastic32_Xe_1000eV>;


template class DCSHistogram<Conv_1000_1000>;
template class DCSHistogram<Conv_1000_1001p55>;
template class DCSHistogram<Conv_1000_1003p10>;


//template class  DetectorSensitivityDistribution1D<Conv_1000_1000, Elastic32_He_1000eV, SourceEvents_Elastic_He_1000eV>;
//template class  DetectorSensitivityDistribution1D<Conv_1000_1000, Elastic32_He_1000eV, SourceEvents_DetectorSensitivity_19122536>;


//template class  DetectorSensitivityDistribution2D<Conv_1000_1000, Elastic32_He_1000eV, SourceEvents_DetectorSensitivity_19122536>;


template class DetectorSensitivityDistribution1<Conv_1000_1000, Elastic32_He_1000eV, SourceEvents_DetectorSensitivity_19122536>;

