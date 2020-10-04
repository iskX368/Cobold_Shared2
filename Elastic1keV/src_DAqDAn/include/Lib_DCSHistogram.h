#pragma once


#include <memory>
#include <stdexcept>
#include <vector>
#include "Lib_ScaleInformation.h"
#include "Lib_ConversionToDCS.h"

#include "Lib_Inputs.h"


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
//	//�ʒu���Ƃ�KE�X�y�N�g��
//	static std::unique_ptr<ScaledHistogram1D> make_1D_vs_KineticEnergy(const size_t DimSize_TOF, const double DimDelta_TOF, const double DimOffset_TOF, const double PosX, const double PosY);
//
//
//
//};





/// <summary>
/// TOF��KE�̂��߂̈ꎟ���X�y�N�g��
/// </summary>
/// <typeparam name="class_ConversionToKES"></typeparam>
template<class class_ConversionToKES>
class TOF_KEHistogram {
public:
	virtual ~TOF_KEHistogram();



	virtual void AddValue(const double PosX, const double PosY, const double HorizontalValue, const double Weight, const double AbsoluteErrorOfWeight) = 0;


	/// <summary>
	/// �ꎟ���q�X�g�O�����̐��l�����ׂă[���ɂ���
	/// </summary>
	/// <returns></returns>
	virtual void ClearHistogram() noexcept = 0;




	/*/// <summary>
	///	�ꎟ���q�X�g�O�������擾����
	/// </summary>
	/// <exception cref="std::out_of_range"></exception>
	/// <returns></returns>
	virtual std::vector<double> Get()const = 0;


	/// <summary>
	/// �ꎟ���q�X�g�O�����̐�Ό덷���擾����
	/// </summary>
	/// <exception cref="std::out_of_range"></exception>
	/// <returns></returns>
	virtual std::vector<double> GetE() const = 0;


	/// <summary>
	/// �ꎟ���q�X�g�O�����̑��Ό덷���擾����
	/// </summary>
	/// <exception cref="std::out_of_range"></exception>
	/// <returns></returns>
	virtual std::vector<double> GetE1() const = 0;


	/// <summary>
	/// �ꎟ���q�X�g�O�����̉����ɂ����� bin �����擾����@�m�F�p
	/// </summary>
	/// <returns></returns>
	virtual const std::vector<double>& GetWidth()const noexcept = 0;

	/// <summary>
	/// �ꎟ���q�X�g�O�����̉����X�P�[���ɂ��C���O�̒l���擾����@�m�F�p
	/// </summary>
	/// <returns></returns>
	virtual const std::vector<double>& GetRaw()const noexcept = 0;

	/// <summary>
	/// �ꎟ���q�X�g�O�����̐��l���擾����@��ԋ߂��ڐ���̐��l��Ԃ�
	/// </summary>
	/// <param name="Xvalue"></param>
	/// <exception cref="std::out_of_range"></exception>
	/// <returns></returns>
	virtual double GetAt(const double Xvalue)const = 0;

	/// <summary>
	/// �ꎟ���q�X�g�O�����̐��l���擾����
	/// </summary>
	/// <param name="Pnt"></param>
	/// <exception cref="std::out_of_range"></exception>
	/// <returns></returns>
	virtual double GetAt(const size_t Pnt)const = 0;

	/// <summary>
	/// �ꎟ���q�X�g�O�����̐�Ό덷���擾����@��ԋ߂��ڐ���̐��l��Ԃ�
	/// </summary>
	/// <param name="Xvalue"></param>
	/// <exception cref="std::out_of_range"></exception>
	/// <returns></returns>
	virtual double GetEAt(const double Xvalue)const = 0;

	/// <summary>
	/// �ꎟ���q�X�g�O�����̐�Ό덷���擾����
	/// </summary>
	/// <param name="Pnt"></param>
	/// <exception cref="std::out_of_range"></exception>
	/// <returns></returns>
	virtual double GetEAt(const size_t Pnt)const = 0;

	/// <summary>
	/// �ꎟ���q�X�g�O�����̐�Ό덷���擾����@��ԋ߂��ڐ���̐��l��Ԃ�
	/// </summary>
	/// <param name="Xvalue"></param>
	/// <exception cref="std::out_of_range"></exception>
	/// <returns></returns>
	virtual double GetE1At(const double Xvalue)const = 0;

	/// <summary>
	/// �ꎟ���q�X�g�O�����̑��Ό덷���擾����
	/// </summary>
	/// <param name="Pnt"></param>
	/// <exception cref="std::out_of_range"></exception>
	/// <returns></returns>
	virtual double GetE1At(const size_t Pnt)const = 0;

	/// <summary>
	/// �N���X ScaleInformation ���擾����
	/// </summary>
	/// <returns></returns>
	virtual const ScaleInformation& ScaleInfo()const noexcept = 0;*/



	static std::unique_ptr<TOF_KEHistogram<class_ConversionToKES>> make(const size_t DimSize, const double DimDelta, const double DimOffset, const bool mode_KE);

};














//
//
///// <summary>
///// ���o�튴�x�␳
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
//	//�������� class_SourceEvents �̃q�X�g�O����
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
//	/// <para>���x�␳�p�����l���擾����</para><para>���x�␳�O�̎U���Ɋp (deg) �q�X�g�O�������v���b�g���I������Ή����� bin �ɂ�����|���Z����</para>
//	/// </summary>
//	/// <returns>strad/a0^2</returns>
//	virtual const std::vector<double>& Ratio_vs_Theta_deg()const noexcept = 0;
//
//	/// <summary>
//	/// <para>���x�␳�p�����l�̑��Ό덷 dx/x ���擾����</para><para>���x�␳�O�̎U���Ɋp (deg) �q�X�g�O�������v���b�g���I������Ή����� bin �̌덷����ɂ���̎���𑫂��ĕ��������Ƃ�</para>
//	/// </summary>
//	/// <returns></returns>
//	//virtual const std::vector<double>& Ratio_vs_Theta_deg_Error1()const noexcept = 0;
//
//
//	virtual const std::vector<double>& DCS_vs_R()const noexcept = 0;
//
//	/// <summary>
//	/// <para>���x�␳�p�����l���擾����</para><para>���x�␳�O�̌��o�피�a (mm) �q�X�g�O�������v���b�g���I������Ή����� bin �ɂ�����|���Z����</para>
//	/// </summary>
//	/// <returns></returns>
//	virtual const std::vector<double>& Ratio_vs_R()const noexcept = 0;
//
//	/// <summary>
//	/// <para>���x�␳�p�����l�̑��Ό덷 dx/x ���擾����</para><para>���x�␳�O�̌��o�피�a (mm) �q�X�g�O�������v���b�g���I������Ή����� bin �̌덷����ɂ���̎���𑫂��ĕ��������Ƃ�</para>
//	/// </summary>
//	/// <returns></returns>
//	//virtual const std::vector<double>& Ratio_vs_R_Error1()const noexcept = 0;
//
//	/// <summary>
//	/// �N���X DetectorSensitivityDistribution1D ���쐬����
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
//	//�������� class_SourceEvents �̃q�X�g�O����
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
//	/// <para>���x�␳�p�����l���擾����</para><para>���x�␳�O�̎U���Ɋp (deg) �q�X�g�O�������v���b�g���I������Ή����� bin �ɂ�����|���Z����</para>
//	/// </summary>
//	/// <returns>strad/a0^2</returns>
//	virtual const std::vector<std::vector<double>>& Ratio_vs_Theta_deg_VarPhi_deg()const noexcept = 0;
//
//	/// <summary>
//	/// <para>���x�␳�p�����l�̑��Ό덷 dx/x ���擾����</para><para>���x�␳�O�̎U���Ɋp (deg) �q�X�g�O�������v���b�g���I������Ή����� bin �̌덷����ɂ���̎���𑫂��ĕ��������Ƃ�</para>
//	/// </summary>
//	/// <returns></returns>
//	virtual const std::vector<std::vector<double>>& Ratio_vs_Theta_deg_VarPhi_deg_Error1()const noexcept = 0;
//
//
//
//	virtual const std::vector<std::vector<double>>& DCS_vs_R_Phi_deg()const noexcept = 0;
//
//	/// <summary>
//	/// <para>���x�␳�p�����l���擾����</para><para>���x�␳�O�̌��o�피�a (mm) �q�X�g�O�������v���b�g���I������Ή����� bin �ɂ�����|���Z����</para>
//	/// </summary>
//	/// <returns></returns>
//	virtual const std::vector<std::vector<double>>& Ratio_vs_R_Phi_deg()const noexcept = 0;
//
//	/// <summary>
//	/// <para>���x�␳�p�����l�̑��Ό덷 dx/x ���擾����</para><para>���x�␳�O�̌��o�피�a (mm) �q�X�g�O�������v���b�g���I������Ή����� bin �̌덷����ɂ���̎���𑫂��ĕ��������Ƃ�</para>
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
/// ���o�튴�x���擾����N���X
/// </summary>
/// <typeparam name="class_ConversionToDCS"> : �ϊ��֐�</typeparam>
/// <typeparam name="class_TheoreticalAtomicDCS_Elastic32"> : ���������ɑΉ����闝�_DCS</typeparam>
/// <typeparam name="class_SourceEvents"> : ���x�␳�ɍ̗p��������C�x���g�f�[�^</typeparam>
template<class class_ConversionToDCS, class class_TheoreticalAtomicDCS_Elastic32, class class_SourceEvents>
class DetectorSensitivityDistribution1 {
public:
	virtual ~DetectorSensitivityDistribution1();

	/// <summary>
	/// ���o�튴�x�Ƃ��̐�Ό덷�̃y�A���擾����
	/// </summary>
	/// <param name="r"> : ���o�피�a</param>
	/// <param name="phi"> : ���o����ʊp</param>
	/// <returns></returns>
	virtual std::pair<double, double> Weight(const double r, const double phi)const = 0;

	/// <summary>
	/// ���o����ʊp�̃t�H�[�}�b�g���擾����
	/// </summary>
	/// <returns></returns>
	virtual LibFlag::AzimuthFormat Format_Phi()const noexcept = 0;



	/// <summary>
	/// ���o�튴�x���z
	/// </summary>
	/// <exception cref="std::exception">uptr�����o�Ɉُ�D�R���X�g���N�^�v�m�F</exception>
	/// <returns></returns>
	virtual std::vector<std::vector<double>> DetectorSensitivityDistribution()const = 0;

	/// <summary>
	/// ���o�튴�x�덷���z
	/// </summary>
	/// <exception cref="std::exception">uptr�����o�Ɉُ�D�R���X�g���N�^�v�m�F</exception>
	/// <returns></returns>
	virtual std::vector<std::vector<double>> DetectorSensitivityDistributionE()const = 0;



	static std::unique_ptr<DetectorSensitivityDistribution1<class_ConversionToDCS, class_TheoreticalAtomicDCS_Elastic32, class_SourceEvents>> make(const size_t DimSize_R, const double DimDelta_R, const size_t DimSize_Phi, const LibFlag::AzimuthFormat Format_Phi);

};

using DetectorSensitivity1_1000_1000 = DetectorSensitivityDistribution1<Conv_1000_1000, Elastic32_He_1000eV, SourceEvents_DetectorSensitivity_19122536>;




