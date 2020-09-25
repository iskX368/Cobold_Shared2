#pragma once

#include <stdexcept>
#include <memory>

#include "Lib_ScaleInformation.h"

//(x,y)�󂯎���ĕϊ�
//���ʂ̓q�X�g�O����
//�Z�b�g�ɕK�v�ȏ��


struct Parameter_RadiusToScatteringAngles
{
	double K0;
	double K1;
	double K2;
	double Phi_Offset_deg;
};

//���ʎ�
double f_Hanbetsu(const Parameter_RadiusToScatteringAngles& Param, const double R_mm) noexcept;

//���ʎ��̕�����
double f_SqrtHanbetsu(const Parameter_RadiusToScatteringAngles& Param, const double R_mm);

//�Ɋp(deg)�ւ̕ϊ�
double f_R_mm_2_Theta_deg(const Parameter_RadiusToScatteringAngles& Param, const double R_mm);

//���ʊp(deg)�ւ̕ϊ�
double f_Phi_deg_2_varPhi_deg(const Parameter_RadiusToScatteringAngles& Param, const double Phi_deg) noexcept;

//�Ɋp�ւ̕ϊ��̈�K����(rad/mm)
double f_DTheta_rad_DR_mm(const Parameter_RadiusToScatteringAngles& Param, const double R_mm);

//�ꎟ��DCS  int_0^(2pi)(dVarPhi dsigma/dOmega) ���擾����  �P�ʂ�arb
//�C�x���g���Ɖ��Z�ʂɑ���
double f_R_mm_2_DsigmaDOmega_arb(const Parameter_RadiusToScatteringAngles& Param, const double R_mm);


double f_X(const Parameter_RadiusToScatteringAngles& Param, const double R_mm);


double f_pnt_2_R_mm(const double BinR_mm, const double dPnt)noexcept;

double f_pnt_2_Theta_deg(const Parameter_RadiusToScatteringAngles& Param, const double BinR_mm, const double dPnt);

double f_DiffOf_pnt_2_Theta_deg(const Parameter_RadiusToScatteringAngles& Param, const double BinR_mm, const double dPnt);






class XYPosToDCSHistogram {
public:
	virtual ~XYPosToDCSHistogram() = default;

	/// <summary>
	/// �q�X�g�O�������擾����
	/// </summary>
	/// <exception cdef="std::exception"></exception>
	/// <returns></returns>
	virtual ScaledHistogram1D& Histogram() = 0;

	/// <summary>
	/// ���a�����q�X�g�O�������擾����
	/// </summary>
	/// <exception cdef="std::exception"></exception>
	/// <returns></returns>
	virtual ScaledHistogram1D& Hist_R() = 0;



	/// <summary>
	/// �N���X XYPosToDCSHistogram ���쐬����
	/// </summary>
	/// <param name="K0"> : �ϊ��Ɏg�p����񎟑����� Radius_mm = F(Polar_deg) �̑�[���W��</param>
	/// <param name="K1"> : �ϊ��Ɏg�p����񎟑����� Radius_mm = F(Polar_deg) �̑��W��</param>
	/// <param name="K2"> : �ϊ��Ɏg�p����񎟑����� Radius_mm = F(Polar_deg) �̑��W��</param>
	/// <param name="Phi_Offset_deg"> : </param>
	/// <param name="NumBins_Radius"> : ���a�Ɋւ���X�y�N�g���̖ڐ����</param>
	/// <param name="Bin_Radius_mm"> : ���a�Ɋւ���X�y�N�g���̈�l�ڐ��蕝</param>
	/// <returns></returns>
	static std::unique_ptr<XYPosToDCSHistogram> make(const double K0, const double K1, const double K2, const double Phi_Offset_deg, const size_t NumBins_Radius, const double Bin_Radius_mm);

};

/// <summary>
/// �N���X XYPosToDCSHistogram ���쐬����
/// </summary>
/// <param name="K0"> : �ϊ��Ɏg�p����񎟑����� Radius_mm = F(Polar_deg) �̑�[���W��</param>
/// <param name="K1"> : �ϊ��Ɏg�p����񎟑����� Radius_mm = F(Polar_deg) �̑��W��</param>
/// <param name="K2"> : �ϊ��Ɏg�p����񎟑����� Radius_mm = F(Polar_deg) �̑��W��</param>
/// <param name="Phi_Offset_deg"> : </param>
/// <param name="NumBins_Radius"> : ���a�Ɋւ���X�y�N�g���̖ڐ����</param>
/// <param name="Bin_Radius_mm"> : ���a�Ɋւ���X�y�N�g���̈�l�ڐ��蕝</param>
/// <returns></returns>
#define make_XYPosToDCSHistogram XYPosToDCSHistogram::make

