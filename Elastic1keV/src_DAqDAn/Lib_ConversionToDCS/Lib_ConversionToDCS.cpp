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
template<class Params>
double ConversionToDCS1<Params>::VarPhi_deg(const double Phi_deg) noexcept {
	return Phi_deg - Params::Phi_Offset_deg;
}



template<class Params>
double ConversionToDCS1<Params>::VarPhi(const double Phi, const LibFlag::AzimuthFormat PhiFormat, const LibFlag::AzimuthFormat VarPhiFormat) {
	try {
		double ans = 0;
		double vphi0 = 0;

		//オフセット
		const double Phi_Offset_deg = Params::Phi_Offset_deg;
		const double Phi_Offset_rad = Params::Phi_Offset_deg * gv_deg2rad;
		
		//一周
		const double Turn_deg = 360;
		const double Turn_rad = M_PI + M_PI;

		switch (PhiFormat)
		{
		case LibFlag::AzimuthFormat::Deg:
			if (Phi < -180 || Phi>180) {
				throw std::out_of_range("ConversionToDCS1<Params>::VarPhi : Phi range ?");
			}
			
			switch (VarPhiFormat)
			{
			case LibFlag::AzimuthFormat::Deg:
				vphi0 = Phi - Phi_Offset_deg;
				if (vphi0<-180) {
					vphi0 += Turn_deg;
				}
				else if (vphi0 > 180) {
					vphi0 -= Turn_deg;
				}
				ans = vphi0;
				break;
			case LibFlag::AzimuthFormat::Deg0:
				vphi0 = Phi + Turn_deg - Phi_Offset_deg;
				if (vphi0 < 0) {
					vphi0 += Turn_deg;
				}
				else if (vphi0 > 360) {
					vphi0 -= Turn_deg;
				}
				ans = vphi0;
				break;
			case LibFlag::AzimuthFormat::Rad:
				vphi0 = Phi * gv_deg2rad - Phi_Offset_rad;
				if (vphi0 < 0 - M_PI) {
					vphi0 += Turn_rad;
				}
				else if (vphi0 > M_PI) {
					vphi0 -= Turn_rad;
				}
				ans = vphi0;
				break;
			case LibFlag::AzimuthFormat::Rad0:
				vphi0 = Phi * gv_deg2rad + Turn_rad - Phi_Offset_rad;
				if (vphi0 < 0) {
					vphi0 += Turn_rad;
				}
				else if (vphi0 > M_PI + M_PI) {
					vphi0 -= Turn_rad;
				}
				ans = vphi0;
				break;
			default:
				throw std::invalid_argument("ConversionToDCS1<Params>::VarPhi : VarPhiFormat ?");
			}

			break;
		case LibFlag::AzimuthFormat::Deg0:
			if (Phi < 0 || Phi>360) {
				throw std::out_of_range("ConversionToDCS1<Params>::VarPhi : Phi range ?");
			}
			
			switch (VarPhiFormat)
			{
			case LibFlag::AzimuthFormat::Deg:
				vphi0 = Phi - Turn_deg - Phi_Offset_deg;
				if (vphi0 < -180) {
					vphi0 += Turn_deg;
				}
				else if (vphi0 > 180) {
					vphi0 -= Turn_deg;
				}
				ans = vphi0;
				break;
			case LibFlag::AzimuthFormat::Deg0:
				vphi0 = Phi - Phi_Offset_deg;
				if (vphi0 < 0) {
					vphi0 += Turn_deg;
				}
				else if (vphi0 > 360) {
					vphi0 -= Turn_deg;
				}
				ans = vphi0;
				break;
			case LibFlag::AzimuthFormat::Rad:
				vphi0 = Phi * gv_deg2rad - Turn_rad - Phi_Offset_rad;
				if (vphi0 < 0 - M_PI) {
					vphi0 += Turn_rad;
				}
				else if (vphi0 > M_PI) {
					vphi0 -= Turn_rad;
				}
				ans = vphi0;
				break;
			case LibFlag::AzimuthFormat::Rad0:
				vphi0 = Phi * gv_deg2rad - Phi_Offset_rad;
				if (vphi0 < 0) {
					vphi0 += Turn_rad;
				}
				else if (vphi0 > M_PI + M_PI) {
					vphi0 -= Turn_rad;
				}
				ans = vphi0;
				break;
			default:
				throw std::invalid_argument("ConversionToDCS1<Params>::VarPhi : VarPhiFormat ?");
			}
			
			break;
		case LibFlag::AzimuthFormat::Rad:
			if (Phi < -M_PI || Phi>M_PI) {
				throw std::out_of_range("ConversionToDCS1<Params>::VarPhi : Phi range ?");
			}
			
			switch (VarPhiFormat)
			{
			case LibFlag::AzimuthFormat::Deg:
				vphi0 = Phi * gv_rad2deg - Phi_Offset_deg;
				if (vphi0 < -180) {
					vphi0 += Turn_deg;
				}
				else if (vphi0 > 180) {
					vphi0 -= Turn_deg;
				}
				ans = vphi0;
				break;
			case LibFlag::AzimuthFormat::Deg0:
				vphi0 = Phi * gv_rad2deg + Turn_deg - Phi_Offset_deg;
				if (vphi0 < 0) {
					vphi0 += Turn_deg;
				}
				else if (vphi0 > 360) {
					vphi0 -= Turn_deg;
				}
				ans = vphi0;
				break;
			case LibFlag::AzimuthFormat::Rad:
				vphi0 = Phi - Phi_Offset_rad;
				if (vphi0 < 0 - M_PI) {
					vphi0 += Turn_rad;
				}
				else if (vphi0 > M_PI) {
					vphi0 -= Turn_rad;
				}
				ans = vphi0;
				break;
			case LibFlag::AzimuthFormat::Rad0:
				vphi0 = Phi + Turn_rad - Phi_Offset_rad;
				if (vphi0 < 0) {
					vphi0 += Turn_rad;
				}
				else if (vphi0 > M_PI + M_PI) {
					vphi0 -= Turn_rad;
				}
				ans = vphi0;
				break;
			default:
				throw std::invalid_argument("ConversionToDCS1<Params>::VarPhi : VarPhiFormat ?");
			}
			
			break;
		case LibFlag::AzimuthFormat::Rad0:
			if (Phi < 0 || Phi > M_PI + M_PI) {
				throw std::out_of_range("ConversionToDCS1<Params>::VarPhi : Phi range ?");
			}
			
			switch (VarPhiFormat)
			{
			case LibFlag::AzimuthFormat::Deg:
				vphi0 = Phi * gv_rad2deg - Turn_deg - Phi_Offset_deg;
				if (vphi0 < -180) {
					vphi0 += Turn_deg;
				}
				else if (vphi0 > 180) {
					vphi0 -= Turn_deg;
				}
				ans = vphi0;
				break;
			case LibFlag::AzimuthFormat::Deg0:
				vphi0 = Phi * gv_rad2deg - Phi_Offset_deg;
				if (vphi0 < 0) {
					vphi0 += Turn_deg;
				}
				else if (vphi0 > 360) {
					vphi0 -= Turn_deg;
				}
				ans = vphi0;
				break;
			case LibFlag::AzimuthFormat::Rad:
				vphi0 = Phi - Turn_rad - Phi_Offset_rad;
				if (vphi0 < 0 - M_PI) {
					vphi0 += Turn_rad;
				}
				else if (vphi0 > M_PI) {
					vphi0 -= Turn_rad;
				}
				ans = vphi0;
				break;
			case LibFlag::AzimuthFormat::Rad0:
				vphi0 = Phi - Phi_Offset_rad;
				if (vphi0 < 0) {
					vphi0 += Turn_rad;
				}
				else if (vphi0 > M_PI + M_PI) {
					vphi0 -= Turn_rad;
				}
				ans = vphi0;
				break;
			default:
				throw std::invalid_argument("ConversionToDCS1<Params>::VarPhi : VarPhiFormat ?");
			}
			
			break;
		default:
			throw std::invalid_argument("ConversionToDCS1<Params>::VarPhi : PhiFormat ?");
		}

		return ans;
	}
	catch (std::invalid_argument&) {
		throw;
	}
	catch (std::out_of_range&) {
		throw;
	}
}


template<class Params>
double ConversionToDCS1<Params>::VarPhi_deg_0_360(const double Phi_deg) noexcept {
	double vp0 = Phi_deg - Params::Phi_Offset_deg;
	if (vp0 >= 360) {
		return vp0 - 360;
	}
	else if (vp0 < 0) {
		return vp0 + 360;
	}
	else {
		return vp0;
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

//e19111401
//入射エネルギー 1000 eV, 出射エネルギー 1000 eV
struct Parameters_ConversionToDCS_e19111401 {
	static constexpr double K0 = 0;//散乱極角 Theta_deg から検出位置中心からの距離 R_mm への変換二次多項式 R_mm = F(theta_deg) の定数項
	static constexpr double K1 = 0.834573;//散乱極角 Theta_deg から検出位置中心からの距離 R_mm への変換二次多項式 R_mm = F(theta_deg) の第一係数
	static constexpr double K2 = 0.0405143;//散乱極角 Theta_deg から検出位置中心からの距離 R_mm への変換二次多項式 R_mm = F(theta_deg) の第二係数

	static constexpr double Phi_Offset_deg = 0;//検出器中心に関する方位角 Phi_deg からこれを引くと散乱方位角 varPhi_deg が求められる
};
template class ConversionToDCS1<Parameters_ConversionToDCS_e19111401>;

//e19111402
//入射エネルギー 1000 eV, 出射エネルギー 1000 + 1.55 eV
struct Parameters_ConversionToDCS_e19111402 {
	static constexpr double K0 = 0;//散乱極角 Theta_deg から検出位置中心からの距離 R_mm への変換二次多項式 R_mm = F(theta_deg) の定数項
	static constexpr double K1 = 0.67572;//散乱極角 Theta_deg から検出位置中心からの距離 R_mm への変換二次多項式 R_mm = F(theta_deg) の第一係数
	static constexpr double K2 = 0.04862;//散乱極角 Theta_deg から検出位置中心からの距離 R_mm への変換二次多項式 R_mm = F(theta_deg) の第二係数

	static constexpr double Phi_Offset_deg = 0;//検出器中心に関する方位角 Phi_deg からこれを引くと散乱方位角 varPhi_deg が求められる
};
template class ConversionToDCS1<Parameters_ConversionToDCS_e19111402>;


//ヘリウム原子
//1000 eV, 弾性散乱
struct Parameters_TheoreticalAtomicDCS_Elastic32_He_1000eV {
	static constexpr double y0 = 0;

	static constexpr double A1 = 0.2061254115058485;
	static constexpr double x1 = 0;
	static constexpr double width1 = 22.21560539908612;

	static constexpr double A2 = 0.4594778733989648;
	static constexpr double x2 = 0;
	static constexpr double width2 = 10.51057480365862;

};

//炭素原子
//1000 eV, 弾性散乱
struct Parameters_TheoreticalAtomicDCS_Elastic32_C_1000eV {
	static constexpr double y0 = 0;

	static constexpr double A1 = 4.940528216909887;
	static constexpr double x1 = 0;
	static constexpr double width1 = 14.89060294791608;

	static constexpr double A2 = 14.65720057784397;
	static constexpr double x2 = 0;
	static constexpr double width2 = 7.000591626682274;

};

//窒素原子
//1000 eV, 弾性散乱
struct Parameters_TheoreticalAtomicDCS_Elastic32_N_1000eV {
	static constexpr double y0 = 0;

	static constexpr double A1 = 3.657996227826588;
	static constexpr double x1 = 0;
	static constexpr double width1 = 17.44280067582442;

	static constexpr double A2 = 11.18558859617259;
	static constexpr double x2 = 0;
	static constexpr double width2 = 8.060781092185545;

};

//ネオン原子
//1000 eV, 弾性散乱
struct Parameters_TheoreticalAtomicDCS_Elastic32_Ne_1000eV {
	static constexpr double y0 = 0;

	static constexpr double A1 = 1.934591395945878;
	static constexpr double x1 = 0;
	static constexpr double width1 = 23.07055198894904;

	static constexpr double A2 = 5.240446505025217;
	static constexpr double x2 = 0;
	static constexpr double width2 = 10.73669485959234;

};

//塩素原子
//1000 eV, 弾性散乱
struct Parameters_TheoreticalAtomicDCS_Elastic32_Cl_1000eV {
	static constexpr double y0 = 0;

	static constexpr double A1 = 9.129344975875995;
	static constexpr double x1 = 0;
	static constexpr double width1 = 16.01129868089606;

	static constexpr double A2 = 42.53411080123093;
	static constexpr double x2 = 0;
	static constexpr double width2 = 7.163525875790771;

};

//アルゴン原子
//1000 eV, 弾性散乱
struct Parameters_TheoreticalAtomicDCS_Elastic32_Ar_1000eV {
	static constexpr double y0 = 0;

	static constexpr double A1 = 35.99857082703646;
	static constexpr double x1 = 0;
	static constexpr double width1 = 7.789543887983262;

	static constexpr double A2 = 7.226501412028382;
	static constexpr double x2 = 0;
	static constexpr double width2 = 17.58360677041105;

};

//キセノン原子
//1000 eV, 弾性散乱
struct Parameters_TheoreticalAtomicDCS_Elastic32_Xe_1000eV {
	static constexpr double y0 = 0;

	static constexpr double A1 = 116.6179148528798;
	static constexpr double x1 = 0;
	static constexpr double width1 = 6.099721784422442;

	static constexpr double A2 = 18.96236804214671;
	static constexpr double x2 = 0;
	static constexpr double width2 = 14.57429917691583;

};




template class TheoreticalAtomicDCS_Elastic32<Parameters_TheoreticalAtomicDCS_Elastic32_He_1000eV>;
template class TheoreticalAtomicDCS_Elastic32<Parameters_TheoreticalAtomicDCS_Elastic32_C_1000eV>;
template class TheoreticalAtomicDCS_Elastic32<Parameters_TheoreticalAtomicDCS_Elastic32_N_1000eV>;
template class TheoreticalAtomicDCS_Elastic32<Parameters_TheoreticalAtomicDCS_Elastic32_Ne_1000eV>;
template class TheoreticalAtomicDCS_Elastic32<Parameters_TheoreticalAtomicDCS_Elastic32_Cl_1000eV>;
template class TheoreticalAtomicDCS_Elastic32<Parameters_TheoreticalAtomicDCS_Elastic32_Ar_1000eV>;
template class TheoreticalAtomicDCS_Elastic32<Parameters_TheoreticalAtomicDCS_Elastic32_Xe_1000eV>;
