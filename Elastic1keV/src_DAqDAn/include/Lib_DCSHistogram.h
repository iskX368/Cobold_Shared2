#pragma once


#include <memory>
#include <stdexcept>
#include <vector>
#include "Lib_ScaleInformation.h"
#include "Lib_ConversionToDCS.h"


//
//DCS�����l�Ɨ��_�l�̕`��
//���o�튴�x�␳
//���s���N���X
//
//���ʊp�t�H�[�}�b�g�ݒ�̒ǉ��͌�ŁD
//







/// <summary>
/// �����U���f�ʐς̗��_�l�q�X�g�O�������쐬����N���X
/// </summary>
/// <typeparam name="class_TheoreticalAtomicDCS_Elastic32"></typeparam>
/// <typeparam name="class_ConversionToDCS"></typeparam>
template<class class_ConversionToDCS, class class_TheoreticalAtomicDCS_Elastic32>
class DCSTheoretical {
public:

	/// <summary>
	/// �������U���Ɋp (deg) �Ƃ����Ƃ��ɔ����U���f�ʐϗ��_�l���c���Ƃ����ꎟ���O���t�̒l���擾����
	/// </summary>
	/// <param name="ScaleInfo_Theta_deg"> : �U���Ɋp (deg) �Ɋւ��鎲���</param>
	/// <exception cref="std::invalid_argument"></exception>
	/// <exception cref="std::out_of_range"></exception>
	/// <exception cref = "std::exception"></exception>
	/// <returns></returns>
	static std::unique_ptr<std::vector<double>> make_1D_vs_Theta_deg(const ScaleInformation& ScaleInfo_Theta_deg);

	/// <summary>
	/// ���������o�피�a (mm) �Ƃ����Ƃ��ɔ����U���f�ʐϗ��_�l���c���Ƃ����ꎟ���O���t�̒l���擾����
	/// </summary>
	/// <param name="ScaleInfo_R"> : ���o�피�a (mm) �Ɋւ��鎲���</param>
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
/// �����U���f�ʐς̎����l�q�X�g�O�������쐬����N���X
/// </summary>
/// <typeparam name="class_ConversionToDCS"> : �ϊ��֐����܂Ƃ߂��N���X</typeparam>
template<class class_ConversionToDCS>
class DCSHistogram {
public:

	/// <summary>
	/// �������U���Ɋp (deg) �Ƃ����q�X�g�O���������
	/// </summary>
	/// <param name="DimSize_R"> : �����̌��ƂȂ錟�o�피�a (mm) ���W���ɂ����� bin �̌�</param>
	/// <param name="DimDelta_R"> : �����̌��ƂȂ錟�o�피�a (mm) ���W���ɂ����� bin �̑����� (mm)</param>
	/// <exception cref="std::invalid_argument"></exception>
	/// <exception cref="std::out_of_range"></exception>
	/// <exception cref = "std::exception"></exception>
	/// <returns></returns>
	static std::unique_ptr<ScaledHistogram1D> make_1D_vs_Theta_deg(const size_t DimSize_R, const double DimDelta_R);
	
	/// <summary>
	/// ���������o�피�a (mm) �Ƃ����q�X�g�O���������
	/// </summary>
	/// <param name="DimSize_R"> : bin �̌�</param>
	/// <param name="DimDelta_R"> : bin �̑����� (mm)</param>
	/// <exception cref="std::invalid_argument"></exception>
	/// <exception cref="std::out_of_range"></exception>
	/// <exception cref = "std::exception"></exception>
	/// <returns></returns>
	static std::unique_ptr<ScaledHistogram1D> make_1D_vs_R(const size_t DimSize_R, const double DimDelta_R);




	static std::unique_ptr<ScaledHistogram2D> make_2D_vs_Theta_deg_VarPhi_deg(const size_t DimSize_R, const double DimDelta_R, const size_t DimSize_Phi_deg);



	static std::unique_ptr<ScaledHistogram2D> make_2D_vs_R_Phi_deg(const size_t DimSize_R, const double DimDelta_R, const size_t DimSize_Phi_deg);
};





/// <summary>
/// ���o�튴�x�␳
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


	
	//�������� class_SourceEvents �̃q�X�g�O����


	virtual const ScaledHistogram1D& Hist_R()const noexcept = 0;

	virtual const ScaleInformation& ScaleInfo_R()const noexcept = 0;


	virtual const ScaledHistogram1D& Hist_Theta_deg()const noexcept = 0;

	virtual const ScaleInformation& ScaleInfo_Theta_deg()const noexcept = 0;


	virtual const std::vector<double>& DCS_vs_Theta_deg()const noexcept = 0;

	/// <summary>
	/// <para>���x�␳�p�����l���擾����</para><para>���x�␳�O�̎U���Ɋp (deg) �q�X�g�O�������v���b�g���I������Ή����� bin �ɂ�����|���Z����</para>
	/// </summary>
	/// <returns>strad/a0^2</returns>
	virtual const std::vector<double>& Ratio_vs_Theta_deg()const noexcept = 0;

	/// <summary>
	/// <para>���x�␳�p�����l�̑��Ό덷 dx/x ���擾����</para><para>���x�␳�O�̎U���Ɋp (deg) �q�X�g�O�������v���b�g���I������Ή����� bin �̌덷����ɂ���̎���𑫂��ĕ��������Ƃ�</para>
	/// </summary>
	/// <returns></returns>
	//virtual const std::vector<double>& Ratio_vs_Theta_deg_Error1()const noexcept = 0;


	virtual const std::vector<double>& DCS_vs_R()const noexcept = 0;

	/// <summary>
	/// <para>���x�␳�p�����l���擾����</para><para>���x�␳�O�̌��o�피�a (mm) �q�X�g�O�������v���b�g���I������Ή����� bin �ɂ�����|���Z����</para>
	/// </summary>
	/// <returns></returns>
	virtual const std::vector<double>& Ratio_vs_R()const noexcept = 0;

	/// <summary>
	/// <para>���x�␳�p�����l�̑��Ό덷 dx/x ���擾����</para><para>���x�␳�O�̌��o�피�a (mm) �q�X�g�O�������v���b�g���I������Ή����� bin �̌덷����ɂ���̎���𑫂��ĕ��������Ƃ�</para>
	/// </summary>
	/// <returns></returns>
	//virtual const std::vector<double>& Ratio_vs_R_Error1()const noexcept = 0;

	/// <summary>
	/// �N���X DetectorSensitivityDistribution1D ���쐬����
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
	/// <para>���x�␳�p�����l���擾����</para><para>���x�␳�O�̎U���Ɋp (deg) �q�X�g�O�������v���b�g���I������Ή����� bin �ɂ�����|���Z����</para>
	/// </summary>
	/// <returns>strad/a0^2</returns>
	virtual const std::vector<std::vector<double>>& Ratio_vs_Theta_deg_VarPhi_deg()const noexcept = 0;

	/// <summary>
	/// <para>���x�␳�p�����l�̑��Ό덷 dx/x ���擾����</para><para>���x�␳�O�̎U���Ɋp (deg) �q�X�g�O�������v���b�g���I������Ή����� bin �̌덷����ɂ���̎���𑫂��ĕ��������Ƃ�</para>
	/// </summary>
	/// <returns></returns>
	virtual const std::vector<std::vector<double>>& Ratio_vs_Theta_deg_VarPhi_deg_Error1()const noexcept = 0;



	virtual const std::vector<std::vector<double>>& DCS_vs_R_Phi_deg()const noexcept = 0;

	/// <summary>
	/// <para>���x�␳�p�����l���擾����</para><para>���x�␳�O�̌��o�피�a (mm) �q�X�g�O�������v���b�g���I������Ή����� bin �ɂ�����|���Z����</para>
	/// </summary>
	/// <returns></returns>
	virtual const std::vector<std::vector<double>>& Ratio_vs_R_Phi_deg()const noexcept = 0;

	/// <summary>
	/// <para>���x�␳�p�����l�̑��Ό덷 dx/x ���擾����</para><para>���x�␳�O�̌��o�피�a (mm) �q�X�g�O�������v���b�g���I������Ή����� bin �̌덷����ɂ���̎���𑫂��ĕ��������Ƃ�</para>
	/// </summary>
	/// <returns></returns>
	virtual const std::vector<std::vector<double>>& Ratio_vs_R_Phi_deg_Error1()const noexcept = 0;


	static std::unique_ptr<DetectorSensitivityDistribution2D<class_ConversionToDCS, class_TheoreticalAtomicDCS_Elastic32, class_SourceEvents>> make(const size_t DimSize_R, const double DimDelta_R, const size_t DimSize_Phi_deg);
};


using DetectorSensitivity2D_1000_1000 = DetectorSensitivityDistribution2D<Conv_1000_1000, Elastic32_He_1000eV, SourceEvents_DetectorSensitivity>;



