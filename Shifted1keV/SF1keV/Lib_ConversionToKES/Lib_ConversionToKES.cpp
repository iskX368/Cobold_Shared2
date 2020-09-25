// Lib_ConversionToKES.cpp : スタティック ライブラリ用の関数を定義します。
//

#include "pch.h"
#include "framework.h"

#include "Lib_ConversionToKES.h"


//template<class class_CoefDist_TOFNoOffset2KES>
//const class_CoefDist_TOFNoOffset2KES& ConversionToKES<class_CoefDist_TOFNoOffset2KES>::CoefDist_TOFNoOffset2KES()noexcept {
//	return class_CoefDist_TOFNoOffset2KES;
//}


template<class class_CoefDist_TOFNoOffset2KES>
double ConversionToKES<class_CoefDist_TOFNoOffset2KES>::KineticEnergy(const double PosX, const double PosY, const double TOF_NoOffset) {
	try {

		//入射エネルギー
		double KE0 = class_CoefDist_TOFNoOffset2KES::KE0;

		//二次多項式の係数
		double c = class_CoefDist_TOFNoOffset2KES::K0(PosX, PosY);
		double b = class_CoefDist_TOFNoOffset2KES::K1(PosX, PosY);
		double a = class_CoefDist_TOFNoOffset2KES::K2(PosX, PosY);

		//判別式
		double hanbetsu = b * b - 4 * a * (c - TOF_NoOffset);
		if (hanbetsu < 0) {
			throw std::invalid_argument("ConversionToKES_1000::KineticEnergy : hanbetsu < 0");
		}

		//解はエネルギーシフト
		double KES = (-b + sqrt(hanbetsu)) / (2 * a);

		return KES + KE0;
	}
	catch (std::invalid_argument&) {
		throw;
	}
	catch (std::out_of_range&) {
		throw;
	}
}



//
//テンプレートを特殊化
//

template class ConversionToKES<CoefDist_TOFNoOffset2KES_191121>;
