#pragma once

#include <stdexcept>
#include "Lib_Inputs.h"


//(PosX, PosY, TOF_NoOffset)����U����̉^���G�l���M�[ E_f ���v�Z����



template<class class_CoefDist_TOFNoOffset2KES>
class ConversionToKES {
public:

	/// <summary>
	/// �e���v���[�g�����փA�N�Z�X����
	/// </summary>
	/// <returns></returns>
	//static const class_CoefDist_TOFNoOffset2KES& CoefDist_TOFNoOffset2KES()noexcept;

	/// <summary>
	/// �^���G�l���M�[���擾����
	/// </summary>
	/// <param name="PosX"> : ���o�ʒu X [mm]</param>
	/// <param name="PosY"> : ���o�ʒu Y [mm]</param>
	/// <param name="TOF_NoOffset"> : ��s���Ԃ���x��������ɂ��f�B���C�������Z�����l [ns]</param>
	/// <exception cref="std::invalid_argument"> : ���ʎ������i�����̂����ꂩ���傫���O��Ă���j</exception>
	/// <exception cref="std::out_of_range"> : �W���擾�Ɏ��s�iPosX��PosY���W���\�͈̔͂���O��Ă���j</exception>
	/// <returns></returns>
	static double KineticEnergy(const double PosX, const double PosY, const double TOF_NoOffset);

};

using ConversionToKES_1000 = ConversionToKES<CoefDist_TOFNoOffset2KES_191121>;

