#pragma once

#include <stdexcept>

#define _USE_MATH_DEFINES
#include <math.h>


#include "Lib_Inputs.h"


//---------------------------------------------------------------
//artof���u�ϊ��֐��܂Ƃ�
//---------------------------------------------------------------



/// <summary>
/// <para>R : ���o�ʒu�̌��o�풆�S����̋��� (mm)</para>
/// <para>Theta : �U���Ɋp (rad)</para>
/// <para>Theta_deg : �U���Ɋp (deg)</para>
/// <para>VarPhi : �U�����ʊp (rad)</para>
/// <para>VarPhi_deg : �U�����ʊp (deg)</para>
/// <para>Phi : ���o�ʒu�̌��o�풆�S�ɂ��Ă̕��ʊp (rad)</para>
/// <para>Phi_deg : ���o�ʒu�̌��o�풆�S�ɂ��Ă̕��ʊp (deg)</para>
/// <para>Omega : ���̊p (strad)</para>
/// <para>dpdq : �p�����[�^ p �̃p�����[�^ q �Ɋւ����K����</para>
/// </summary>
template<class Params>
class ConversionToDCS1 {
public:

	/// <summary>
	/// �U���Ɋp (deg) ���猟�o�ʒu���S����̋��� (mm) �֕ϊ�����
	/// </summary>
	/// <param name="Theta_deg"></param>
	/// <exception cref="�Ȃ�"></exception>
	/// <returns></returns>
	static double R(const double Theta_deg)noexcept;


	/// <summary>
	/// �U���Ɋp (deg) ���猟�o�ʒu���S����̋��� (mm) �ւ̕ϊ����̈�K����
	/// </summary>
	/// <param name="Theta_deg"></param>
	/// <exception cref="�Ȃ�"></exception>
	/// <returns></returns>
	static double dRdTheta_deg(const double Theta_deg)noexcept;


	/// <summary>
	/// ���o�ʒu���S����̋��� (mm) ����U���Ɋp (deg) �֕ϊ�����
	/// </summary>
	/// <param name="Param"> : �ϊ��p�p�����[�^</param>
	/// <param name="R"> : ���o�ʒu�̌��o�ʒu���S����̋��� (mm)</param>
	/// <exception cref="std::invalid_argument"></exception>
	/// <returns>�U���Ɋp (deg)</returns>
	static double Theta_deg(const double R);


	/// <summary>
	/// ���o�ʒu���S����̋��� (mm) ����U�����ʊp (deg) �֕ϊ�����
	/// </summary>
	/// <param name="Param"> : �ϊ��p�p�����[�^</param>
	/// <param name="AzimuthAngle_Detector_deg"> : ���o�풆�S�Ɋւ��錟�o�ʒu�̕��ʊp (deg)</param>
	/// <exception cref="std::invalid_argument"></exception>
	/// <returns>�U�����ʊp (deg)</returns>
	//static double VarPhi_deg(const double Phi_deg) noexcept;


	/// <summary>
	/// ���o���̕��ʊp���U�����ʊp�֎w�肵���t�H�[�}�b�g�ŕϊ�����
	/// </summary>
	/// <param name="Phi"> : ���o�풆�S�Ɋւ��錟�o�ʒu�̕��ʊp  �P�ʂƒ�`��͎��̈����Ŏw��</param>
	/// <param name="PhiFormat"> : �����̕��ʊp�̃t�H�[�}�b�g</param>
	/// <param name="VarPhiFormat"> : �Ԓl�̎U�����ʊp�̃t�H�[�}�b�g</param>
	/// <exception cref="std::invalid_argument"></exception>
	/// <exception cref="std::out_of_range"></exception>
	/// <returns></returns>
	static double VarPhi(const double Phi, const LibFlag::AzimuthFormat PhiFormat, const LibFlag::AzimuthFormat VarPhiFormat);


	

	/// <summary>
	/// ���o�ʒu���S����̋��� (mm) ����U�����ʊp (rad) �ւ̕ϊ����̈�K���� dTheta/dR
	/// </summary>
	/// <param name="Param"> : �ϊ��p�p�����[�^</param>
	/// <param name="R"> : ���o�ʒu�̌��o�ʒu���S����̋��� (mm)</param>
	/// <exception cref="std::invalid_argument"></exception>
	/// <returns>(rad/mm)</returns>
	static double dThetadR(const double R);


	/// <summary>
	/// <para>�U�����ʊp (rad) �ɂ��Đϕ��ς݂̗��̊p�֐��̎U���Ɋp (rad) �ɂ��Ă̈�K���� dOmega/dTheta�@��</para><para>�U���Ɋp�̌��o�ʒu���S����̋��� (mm) �ɂ��Ă̈�K���� dTheta/dR �̐�</para><para>dOmega/dR �ɓ�����</para>
	/// </summary>
	/// <param name="Param"> : �ϊ��p�p�����[�^</param>
	/// <param name="R"> : ���o�ʒu�̌��o�ʒu���S����̋��� (mm)</param>
	/// <exception cref="std::invalid_argument"></exception>
	/// <returns>(strad/mm)</returns>
	static double dOmegadTheta_dThetadR(const double R);

};

using Conv_1000_1000 = ConversionToDCS1<Parameters_ConversionToDCS_e19111401>;//���˃G�l���M�[ 1000 eV, �o�˃G�l���M�[ 1000 eV
using Conv_1000_1001p55 = ConversionToDCS1<Parameters_ConversionToDCS_e19111402>;//���˃G�l���M�[ 1000 eV, �o�˃G�l���M�[ 1001.55 eV
using Conv_1000_1003p10 = ConversionToDCS1<Parameters_ConversionToDCS_e19111403>;//���˃G�l���M�[ 1000 eV, �o�˃G�l���M�[ 1001.55 eV




/// <summary>
/// ���q�̒e�������U���f�ʐς̗��_�l��Elastic32�̃t�B�b�e�B���O���ʂ����Ɍv�Z����
/// </summary>
/// <typeparam name="Params"></typeparam>
template<class Params>
class TheoreticalAtomicDCS_Elastic32 {
public:
	/// <summary>
	/// �����U���f�ʐς��擾����
	/// </summary>
	/// <param name="Theta_deg"> : �U���p�x (deg)</param>
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

