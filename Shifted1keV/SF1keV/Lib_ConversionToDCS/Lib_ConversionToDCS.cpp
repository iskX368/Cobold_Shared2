// Lib_ConversionToDCS.cpp : スタティック ライブラリ用の関数を定義します。
//

#include "pch.h"
#include "framework.h"

#include "Lib_ConversionToDCS.h"






//判別式
template<class Params>
static double Hanbetsu(const double R_mm) noexcept {
	return Params::K1 * Params::K1 - 4 * Params::K2 * (Params::K0 - R_mm);
}

//判別式の平方根
template<class Params>
static double SqrtHanbetsu(const double R_mm) {
	double hanbetsu = Hanbetsu<Params>(R_mm);
	if (hanbetsu < 0) {
		throw std::invalid_argument("SqrtHanbetsu : D < 0");
	}
	return sqrt(hanbetsu);
}


template<class Params>
double ConversionToDCS1<Params>::R(const double Theta_deg)noexcept {
	return Params::K0 + Params::K1 * Theta_deg + Params::K2 * Theta_deg * Theta_deg;
}


template<class Params>
double ConversionToDCS1<Params>::dRdTheta_deg(const double Theta_deg)noexcept {
	return Params::K1 + 2 * Params::K2 * Theta_deg;
}


//極角(deg)への変換
template<class Params>
double ConversionToDCS1<Params>::Theta_deg(const double R) {
	try
	{
		if (Params::K2 == 0) {
			throw std::invalid_argument("ConversionToDCS::R_mm_2_Theta_deg : K2 == 0");
		}

		return (SqrtHanbetsu<Params>(R) - Params::K1) / (2 * Params::K2);
	}
	catch (std::invalid_argument&)
	{
		throw;
	}
}

//方位角(deg)への変換
//template<class Params>
//double ConversionToDCS1<Params>::VarPhi_deg(const double Phi_deg) noexcept {
//	return Phi_deg - Params::Phi_Offset;
//}



template<class Params>
double ConversionToDCS1<Params>::VarPhi(const double Phi, const LibFlag::AzimuthFormat PhiFormat, const LibFlag::AzimuthFormat VarPhiFormat) {
	try {
		
		//フォーマット揃えて引き算
		double a = LibFlag::ConvertAzimuthFormat(Phi, PhiFormat, VarPhiFormat) - LibFlag::ConvertAzimuthFormat(Params::Phi_Offset, Params::Format_Phi_Offset, VarPhiFormat);
		
		//レンジに収める
		double b = LibFlag::AzimuthIntoRnge(a, VarPhiFormat);

		return b;

	}
	catch (std::invalid_argument&) {
		throw;
	}
	catch (std::out_of_range&) {
		throw;
	}
}


//極角への変換の一階微分(rad/mm)
template<class Params>
double ConversionToDCS1<Params>::dThetadR(const double R) {
	try
	{
		return gv_deg2rad / SqrtHanbetsu<Params>(R);
	}
	catch (std::invalid_argument&)
	{
		throw;
	}
}

// (dOmg/dTheta) * (dTheta/dR)
template<class Params>
double ConversionToDCS1<Params>::dOmegadTheta_dThetadR(const double R) {
	try {
		return (M_PI + M_PI) * sin(Theta_deg(R) * gv_deg2rad) * dThetadR(R);
	}
	catch (std::invalid_argument&) {
		throw;
	}
}










//Igor Proでフィッテイングに使った関数
template<class Params>
static double DblGauss(const double x)noexcept {
	double p1 = (x - Params::x1) / Params::width1;
	double p2 = (x - Params::x2) / Params::width2;
	return Params::y0 + Params::A1 * exp(-p1 * p1) + Params::A2 * exp(-p2 * p2);
}

template<class Params>
double TheoreticalAtomicDCS_Elastic32<Params>::dsigmadOmega(const double Theta_deg)noexcept {
	return DblGauss<Params>(Theta_deg);
}




//////////////////////////////////////////////
//明示的なインスタンス化をソースファイルで要求
//////////////////////////////////////////////
template class ConversionToDCS1<Parameters_ConversionToDCS_e19111401>;
template class ConversionToDCS1<Parameters_ConversionToDCS_e19111402>;
template class ConversionToDCS1<Parameters_ConversionToDCS_e19111403>;

template class TheoreticalAtomicDCS_Elastic32<Elastic32_FittedByDblGauss::He_1000eV>;
template class TheoreticalAtomicDCS_Elastic32<Elastic32_FittedByDblGauss::C_1000eV>;
template class TheoreticalAtomicDCS_Elastic32<Elastic32_FittedByDblGauss::N_1000eV>;
template class TheoreticalAtomicDCS_Elastic32<Elastic32_FittedByDblGauss::Ne_1000eV>;
template class TheoreticalAtomicDCS_Elastic32<Elastic32_FittedByDblGauss::Cl_1000eV>;
template class TheoreticalAtomicDCS_Elastic32<Elastic32_FittedByDblGauss::Ar_1000eV>;
template class TheoreticalAtomicDCS_Elastic32<Elastic32_FittedByDblGauss::Xe_1000eV>;


