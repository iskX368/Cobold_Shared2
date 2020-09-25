#pragma once

#include <stdexcept>
#include "Lib_Inputs.h"


//(PosX, PosY, TOF_NoOffset)から散乱後の運動エネルギー E_f を計算する



template<class class_CoefDist_TOFNoOffset2KES>
class ConversionToKES {
public:

	/// <summary>
	/// テンプレート引数へアクセスする
	/// </summary>
	/// <returns></returns>
	//static const class_CoefDist_TOFNoOffset2KES& CoefDist_TOFNoOffset2KES()noexcept;

	/// <summary>
	/// 運動エネルギーを取得する
	/// </summary>
	/// <param name="PosX"> : 検出位置 X [mm]</param>
	/// <param name="PosY"> : 検出位置 Y [mm]</param>
	/// <param name="TOF_NoOffset"> : 飛行時間から遅延発生器によるディレイを引き算した値 [ns]</param>
	/// <exception cref="std::invalid_argument"> : 判別式が負（引数のいずれかが大きく外れている）</exception>
	/// <exception cref="std::out_of_range"> : 係数取得に失敗（PosXかPosYが係数表の範囲から外れている）</exception>
	/// <returns></returns>
	static double KineticEnergy(const double PosX, const double PosY, const double TOF_NoOffset);

};

using ConversionToKES_1000 = ConversionToKES<CoefDist_TOFNoOffset2KES_191121>;

