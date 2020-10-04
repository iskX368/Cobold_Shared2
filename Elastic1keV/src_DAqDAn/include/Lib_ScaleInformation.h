#pragma once

#include <memory>
#include <stdexcept>
#include <functional>
#include <vector>
#include <tuple>


//--------------------------------------------------------------------------------------------------------------------------------------------------------
//�s�ψ�ȃO���t�ڐ�����������߂̃N���X
//PointInformation �N���X�ɂ���āC�[���n�܂萮���C���f�b�N�X�Ƌ�Ԓ[�̊J���w�肷��
//ScaleInformation �N���X�� PointInformation �Ɛ����C���f�b�N�X�Ɩڐ��萔�l�Ƃ̑Ή��֐� f ��n���āC�ڐ��萔�l f(0), f(1), ... �Ƃ��̑Ή���Ԃ��쐬����
//���ʂ� PointInformation ��n�����ƂŁC�����֐��ȂǕ����p�����[�^��bin�̋�Ԓ[�̈����𓝈ꂵ�����ꍇ�ɑΉ�����
//--------------------------------------------------------------------------------------------------------------------------------------------------------







//��Ԃ̊J�� { = ( or [, } = ) or ] �Ƃ���
//I_n = { k, k+1 } |-> F_n = { f(k), f(k+1) }
//�̑Ή��ɂ�����
//
//1. �P�ʕ������� : I_n(1) - I_n(0) == 1
//2. �[�Ōq�����Ă��� : I_(n+1)(0) == I_n(1)
//
//���̂悤�ȋ�Ԃ̏W�� I_n (n=0, ... , N-1) �����
//�e��� I_n �͂��ꂼ���ԑ�\�l n ������
struct PointInformation
{
	virtual ~PointInformation();

	enum class Difference {
		Forward,	//	�O�i���� : I_n = [ n , n-1 ) ( n = 0, ... ,N-1 )
		Backward,	//	��ލ��� : I_n = ( n-1 , n ] ( n = 0, ... ,N-1 )
		Central0,	//	���S�������̂O : I_n = [ n-1/2 , n+1/2 ) ( n = 0, ... ,N-1 )
		Central1,	//	���S�������̂P : I_n = ( n-1/2 , n+1/2 ] ( n = 0, ... ,N-1 )

		Central2,	//	���S�������̂Q : I_n = [ n-1/2 , n+1/2 ] ( n = 0, ... ,N-1 )

	};

	/// <summary>
	/// �|�C���g�̌�
	/// </summary>
	///<exception cref="�Ȃ�"></exception>
	/// <returns></returns>
	virtual size_t NumPoints()const noexcept = 0;

	/// <summary>
	/// ��Ԏn�܂肪�܂܂ꂢ�Ă���̂Ȃ�true
	/// </summary>
	///<exception cref="�Ȃ�"></exception>
	/// <returns></returns>
	virtual bool IsClosed0()const noexcept = 0;

	/// <summary>
	/// ��ԏI��肪�܂܂ꂢ�Ă���̂Ȃ�true
	/// </summary>
	///<exception cref="�Ȃ�"></exception>
	/// <returns></returns>
	virtual bool IsClosed1()const noexcept = 0;

	/// <summary>
	/// �o�^���������̎��
	/// </summary>
	///<exception cref="�Ȃ�"></exception>
	/// <returns></returns>
	virtual Difference Enum_Difference()const noexcept = 0;

	/// <summary>
	/// ��ԑ�\�l�����v������ I ���擾����
	/// </summary>
	/// <param name="Point"></param>
	/// <exception cref = "std::out_of_range"></exception>
	/// <returns></returns>
	virtual std::pair<double, double> Point2PointInterval(const size_t Point)const = 0;


	/// <summary>
	/// �S��Ԃ̎n�܂��Ԃ�
	/// </summary>
	/// <exception cref = "std::out_of_range"></exception>
	/// <returns></returns>
	virtual double PointRange0()const = 0;

	/// <summary>
	/// �S��Ԃ̏I����Ԃ�
	/// </summary>
	/// <exception cref = "std::out_of_range"></exception>
	/// <returns></returns>
	virtual double PointRange1()const = 0;


	/// <summary>
	/// ��ԑ�\�l 0, 1, ..., (NumberOfPoints-1) ���獷�� dif �ɏ]���� I ��Ԃ̏W���N���X PointInformation �ւ� shared_ptr ���쐬����
	/// </summary>
	/// <param name="NumberOfPoints"> : ��ԁi��\�|�C���g�j�̌�</param>
	/// <param name="dif"> : ����</param>
	/// <exception cref = "std::invalid_argument"></exception>
	/// <exception cref = "std::exception"></exception>
	/// <returns></returns>
	static std::shared_ptr<PointInformation> make(const size_t NumberOfPoints, PointInformation::Difference dif);
};
bool operator==(const PointInformation& t1, const PointInformation& t2) noexcept;
bool operator!=(const PointInformation& t1, const PointInformation& t2) noexcept;

//�G���g���[�|�C���g
#define make_PointInformation PointInformation::make





//��Ԃ̊J�� { = ( or [, } = ) or ] �Ƃ���
//I_n = { p, p+1 } |-> F_n = { f(p), f(p+1) }
//�̑Ή��ɂ����āC�Ԃ�l�̋�Ԃ̏W�� F_n (n=0, ... , N-1) �����
//f(p) < f(p+1) �ł���Ƃ͌���Ȃ�
//�S�Ă� n �ɂ��Č�������v�����s�����ł���K�v������
//�e��� F_n �͂��ꂼ���ԑ�\�l f(n) ������
struct ScaleInformation {
	virtual ~ScaleInformation();


	/// <summary>
	/// I ��ԏW���N���X�ւ̃|�C���^���擾����
	/// </summary>
	///<exception cref = "std::invalid_argument"></exception>
	/// <returns></returns>
	virtual const std::shared_ptr<PointInformation>& PointInfo()const = 0;


	/// <summary>
	/// ���ʂ� I ��ԏW���N���X�����Ȃ�true, �����łȂ��Ȃ�false
	/// </summary>
	/// <param name="scaleInformation"></param>
	///<exception cref = "�Ȃ�"></exception>
	/// <returns></returns>
	virtual bool HasCommonPointInfo(const ScaleInformation& scaleInformation)const noexcept = 0;


	/// <summary>
	/// ���ʂ� I ��Ԃ����ڐ���N���X�̑Ή���\�l�����߂�
	/// </summary>
	/// <param name="another_ScaleInformation_Sharing_PntInfo"></param>
	/// <param name="Xvalue_Of_ThisScale"></param>
	///<exception cref = "std::invalid_argument"></exception>
	///<exception cref = "std::out_of_range"></exception>
	/// <returns></returns>
	virtual double Convert2AnotherRepresentative(const ScaleInformation& another_ScaleInformation_Sharing_PntInfo, const double Xvalue_Of_ThisScale)const = 0;


	/// <summary>
	/// /F ��Ԃ̎n�[���܂ނȂ�true
	/// </summary>
	/// <exception cref = "�Ȃ�"></exception>
	/// <returns></returns>
	virtual bool IsClosed0()const noexcept = 0;


	/// <summary>
	/// F ��Ԃ̏I�[���܂ނȂ�true
	/// </summary>
	/// <exception cref = "�Ȃ�"></exception>
	/// <returns></returns>
	virtual bool IsClosed1()const noexcept = 0;


	/// <summary>
	/// <para>���`�P�������Ȃ�true�C���`�P�������Ȃ�false</para>
	/// <para>���`�łȂ��P�[�X�͎��O�ɂ͂����Ă���</para>
	/// </summary>
	/// <exception cref = "�Ȃ�"></exception>
	/// <returns></returns>
	virtual bool IsMonotonicallyIncreasing()const noexcept = 0;


	/// <summary>
	/// F ��Ԃ̌����擾����
	/// </summary>
	///<exception cref="�Ȃ�"></exception>
	virtual size_t NumIntervals()const noexcept = 0;


	/// <summary>
	/// F ��Ԃ��擾����
	/// </summary>
	///<exception cref="std::out_of_range">��ԃC���f�b�N�X���͈͊O</exception>
	/// <param name="Interval_Index">��ԃC���f�b�N�X : [0, NumIntervals ) </param>
	/// <returns>��Ԏn�܂�� first, ��ԏI���� second �Ƃ��� pair</returns>
	virtual const std::pair<double, double>& Interval(const size_t Interval_Index)const = 0;


	/// <summary>
	/// F ��Ԃ̋�ԏI���Ƌ�Ԏn�܂�̍����擾����
	/// </summary>
	///<exception cref="std::out_of_range">��ԃC���f�b�N�X���͈͊O</exception>
	/// <param name="Interval_Index">��ԃC���f�b�N�X : [0, NumIntervals ) </param>
	/// <returns>�i��ԏI���j-�i��Ԏn�܂�j</returns>
	virtual double IntervalWidth(const size_t Interval_Index)const = 0;


	/// <summary>
	/// �S��Ԃ��J�o�[����͈͂̎n�܂�ƏI�����擾����
	/// </summary>
	///<exception cref="std::out_of_range">�R���X�g���N�^�G���[��Range����������Ȃ�����</exception>
	/// <returns>�͈͎n�܂�� first, �͈͏I���� second �Ƃ��� pair</returns>
	virtual const std::pair<double, double>& Range()const = 0;


	/// <summary>
	/// ���� Xvalue ���S F ��Ԃ��J�o�[����͈͂ɑ����Ă���Ȃ� true, �����łȂ��Ȃ� false
	/// </summary>
	///<exception cref="�Ȃ�"></exception>
	virtual bool IsInRange(const double Xvalue)const = 0;


	/// <summary>
	/// ���� Xvalue ����ԃC���f�b�N�X Interval_Index �Ŏw�肳��� F ��Ԃɑ�����Ȃ� true, �����łȂ��Ȃ� false
	/// </summary>
	/// <param name="Interval_Index">��ԃC���f�b�N�X : [0, NumIntervals ) </param>
	///<exception cref="�Ȃ�"></exception>
	virtual bool IsInInterval(const double Xvalue, const size_t Interval_Index)const = 0;


	/// <summary>
	/// <para>���� Xvalue �������� F ��Ԃ̃C���f�b�N�X���擾����</para>
	/// <para>��O����̂��� IsInRange �Ŏ��O�Ɋm�F���邱��</para>
	/// </summary>
	///<exception cref="std::out_of_range">XValue ���������Ԃ�������Ȃ�����</exception>
	virtual size_t X2IntervalIndex(const double Xvalue)const = 0;


	/// <summary>
	/// <para>���� Xvalue ������ F ��Ԃ���\����ڐ��萔�l���擾����</para>
	/// <para>��O����̂��� IsInRange �Ŏ��O�Ɋm�F���邱��</para>
	/// </summary>
	///<exception cref="std::out_of_range">XValue ���������Ԃ�������Ȃ�����</exception>
	/// <returns></returns>
	virtual double X2XRepresentative(const double Xvalue)const = 0;


	/// <summary>
	/// <para>��ԃC���f�b�N�X Interval_Index ���� F ��Ԃ���\����ڐ��萔�l���擾����</para>
	/// <para>��O����̂��� NumIntervals �Ŏ��O�Ɋm�F���邱��</para>
	/// </summary>
	/// <param name="Interval_Index">  ��ԃC���f�b�N�X : [0, NumIntervals ) </param>
	///<exception cref="std::out_of_range">��ԃC���f�b�N�X���͈͊O</exception>
	/// <returns></returns>
	virtual double IntervalIndex2XRepresentative(const size_t Interval_Index)const = 0;



	/// <summary>
	/// ScaleInformation���쐬����
	/// </summary>
	/// <param name="sppointInformation"> : I ��Ԃ̏W���ւ�shared_ptr�D���L�J�E���^���P���₷</param>
	/// <param name="f"><para> : I ��Ԃ��� F ��Ԃ֑Ή�������֐��DPointInformation �̑S�͈͂ɂ킽���ċ��`�P�������܂��͋��`�P�������łȂ���΂Ȃ�Ȃ�</para><para>   ���S�������g�p����ꍇ�͐��`�Ȋ֐��Ƃ��邱��</para></param>
	/// <exception cref = "std::invalid_argument"></exception>
	/// <exception cref = "std::out_of_range"></exception>
	/// <exception cref = "std::exception"></exception>
	/// <returns></returns>
	static std::unique_ptr<ScaleInformation> make(const std::shared_ptr<PointInformation>& sppointInformation, const std::function<double(const double)>& f);


	/// <summary>
	/// ScaleInformation���쐬����
	/// </summary>
	/// <param name="sppointInformation"> : I ��Ԃ̏W���ւ�shared_ptr�D���L�J�E���^���P���₷</param>
	/// <param name="Pnt2LinearScale_Ratio"> : I ��Ԃ��� F ��Ԃ֑Ή���������`�֐��̌X��</param>
	/// <param name="Pnt2LinearScale_PntOffset"> : I ��Ԃ��� F ��Ԃ֑Ή���������`�֐��̃I�t�Z�b�g</param>
	/// <exception cref = "std::invalid_argument"></exception>
	/// <exception cref = "std::out_of_range"></exception>
	/// <exception cref = "std::exception"></exception>
	/// <returns></returns>
	static std::unique_ptr<ScaleInformation> make(const std::shared_ptr<PointInformation>& sppointInformation, const double Pnt2LinearScale_Ratio = 1.0, const double Pnt2LinearScale_PntOffset = 0.0);

};
bool operator==(const ScaleInformation& t1, const ScaleInformation& t2);
bool operator!=(const ScaleInformation& t1, const ScaleInformation& t2);


#define make_ScaleInformation ScaleInformation::make





//
//class ScaledHistogram1D {
//public:
//
//	virtual ~ScaledHistogram1D();
//
//	/// <summary>
//	/// �q�X�g�O�����ɐ��l�����Z����
//	/// </summary>
//	/// <param name="Xvalue"> : ���Z���鉡���̐��l  �ł��߂��ڐ���l���I�΂��</param>
//	/// <param name="Weight"> : ���Z�����  ������d�݊���Z�֐��Ŋ������l����������</param>
//	/// <exception cref="std::out_of_range"></exception>
//	/// <exception cref="std::exception"></exception>
//	/// <returns></returns>
//	virtual void AddValue(const double Xvalue, const double Weight = 1.0) = 0;
//
//
//	/// <summary>
//	/// �q�X�g�O�����ɐ��l�����̂܂܃Z�b�g����
//	/// </summary>
//	/// <param name="Xvalue"> : ���Z���鉡���̐��l  �ł��߂��ڐ���l���I�΂��</param>
//	/// <param name="Intensity"> : �Z�b�g����l</param>
//	/// <exception cref="std::out_of_range"></exception>
//	/// <exception cref="std::exception"></exception>
//	/// <returns></returns>
//	virtual void SetValue(const double Xvalue, const double Intensity) = 0;
//
//
//	/// <summary>
//	/// �ꎟ���q�X�g�O�����̐��l�����ׂă[���ɂ���
//	/// </summary>
//	/// <returns></returns>
//	virtual void ClearHistogram() noexcept = 0;
//
//	/// <summary>
//	///	�ꎟ���q�X�g�O�������擾����
//	/// </summary>
//	/// <returns></returns>
//	virtual const std::vector<double>& Get() const noexcept = 0;
//
//	
//	virtual double GetAt(const double Xvalue) const = 0;
//
//	virtual double GetAt(const size_t Pnt) const = 0;
//
//
//
//	/// <summary>
//	/// �N���X ScaleInformation ���擾����
//	/// </summary>
//	/// <returns></returns>
//	virtual const ScaleInformation& ScaleInfo()const noexcept= 0;
//
//	/// <summary>
//	/// �N���X ScaledHistogram1D ���쐬����
//	/// </summary>
//	/// <param name="sppointInformation">�N���X PointInformation ��shared_ptr</param>
//	/// <param name="f_Pnt2HorizontalScale"> : �[���ȏ�̐����i�|�C���g���j���牡���ڐ���l�ւ̕ϊ��֐�</param>
//	/// <param name="f_WeightDivisionFactor"> : �q�X�g�O�������Z�� (Weight) �͂��̊֐��l�Ŋ���ꂽ��ɉ�������</param>
//	/// <exception cref="std::invalid_argument"></exception>
//	/// <exception cref="std::out_of_range"></exception>
//	/// <exception cref = "Exceptions from std::make_unique"></exception>
//	/// <returns></returns>
//	static std::unique_ptr<ScaledHistogram1D> make(const std::shared_ptr<PointInformation>& sppointInformation, std::function<double(const double)> f_Pnt2HorizontalScale, std::function<double(const double)> f_WeightDivisionFactor);
//	
//	/// <summary>
//	/// <para>�N���X ScaledHistogram1D ���쐬����</para><para>�����ڐ��肪�[���ȏ�̐����i�|�C���g���j�ɑ΂��Đ��`�Ƃ���</para>
//	/// </summary>
//	/// <param name="sppointInformation">�N���X PointInformation ��shared_ptr</param>
//	/// <param name="Pnt2LinearScale_Ratio"> : �|�C���g�����牡���ڐ���l�ւ̐��`�ϊ��̌X��</param>
//	/// <param name="Pnt2LinearScale_PntOffset"> : �|�C���g�����牡���ڐ���l�ւ̐��`�ϊ��̃|�C���g���I�t�Z�b�g</param>
//	/// <returns></returns>
//	static std::unique_ptr<ScaledHistogram1D> make(const std::shared_ptr<PointInformation>& sppointInformation, const double Pnt2LinearScale_Ratio = 1.0, const double Pnt2LinearScale_PntOffset = 0.0);
//
//
//};
//
//#define make_ScaledHistogram1D ScaledHistogram1D::make







class ScaledHistogram1D {
public:

	virtual ~ScaledHistogram1D();

	/// <summary>
	/// �q�X�g�O�����ɐ��l�����Z����
	/// </summary>
	/// <param name="Xvalue"> : ���Z���鉡���̐��l  �ł��߂��ڐ���l���I�΂��</param>
	/// <param name="Weight"> : ���Z�����  ������d�݊���Z�֐��Ŋ������l����������</param>
	/// <exception cref="std::out_of_range"></exception>
	/// <exception cref="std::exception"></exception>
	/// <returns></returns>
	//virtual void AddValue(const double Xvalue, const double Weight = 1.0) = 0;



	virtual void AddValue(const double Xvalue, const double Weight, const double AbsoluteErrorOfWeight) = 0;


	/// <summary>
	/// �ꎟ���q�X�g�O�����̐��l�����ׂă[���ɂ���
	/// </summary>
	/// <returns></returns>
	virtual void ClearHistogram() noexcept = 0;

	//Get����O�Ɏ��s���邱��
	//virtual void RefineHistogram() = 0;


	/// <summary>
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
	virtual const ScaleInformation& ScaleInfo()const noexcept = 0;

	/// <summary>
	/// �����Ōv�Z
	/// </summary>
	/// <param name="sppointInformation"></param>
	/// <param name="f_Pnt2HorizontalScale"></param>
	/// <returns></returns>
	static std::unique_ptr<ScaledHistogram1D> make(const std::shared_ptr<PointInformation>& sppointInformation, std::function<double(const double)> f_Pnt2HorizontalScale);

	static std::unique_ptr<ScaledHistogram1D> make(const std::shared_ptr<PointInformation>& sppointInformation, const double Pnt2LinearScale_Ratio = 1.0, const double Pnt2LinearScale_PntOffset = 0.0);
};

#define make_ScaledHistogram1D ScaledHistogram1D::make








class ScaledHistogram2D {
public:

	virtual ~ScaledHistogram2D();

	/// <summary>
	/// �q�X�g�O�����ɐ��l�����Z����  ����������[����Ԃ�
	/// </summary>
	/// <param name="Xvalue"> : ���Z���鉡���̐��l���̂P  �ł��߂��ڐ���l���I�΂��</param>
	/// <param name="Yvalue"> : ���Z���鉡���̐��l���̂Q  �ł��߂��ڐ���l���I�΂��</param>
	/// <param name="Weight"> : ���Z�����  ������d�݊���Z�֐��Ŋ������l����������</param>
	/// <exception cref="std::out_of_range"></exception>
	/// <exception cref="std::exception"></exception>
	/// <returns></returns>
	//virtual void AddValue(const double Xvalue, const double Yvalue, const double Weight = 1.0) = 0;


	virtual void AddValue(const double Xvalue, const double Yvalue, const double Weight, const double AbsoluteErrorOfWeight) = 0;


	/// <summary>
	/// �񎟌��q�X�g�O�����̐��l�����ׂă[���ɂ���
	/// </summary>
	/// <exception cref="�Ȃ�"></exception>
	/// <returns></returns>
	virtual void ClearHistogram() noexcept = 0;

	/// <summary>
	/// <para>�񎟌��q�X�g�O�������擾����
	/// </summary>
	/// <exception cref="std::out_of_range"></exception>
	/// <returns></returns>
	virtual std::vector<std::vector<double>> Get()const = 0;


	/// <summary>
	/// <para>�񎟌��q�X�g�O�����̐�Ό덷���擾����
	/// </summary>
	/// <exception cref="std::out_of_range"></exception>
	/// <returns></returns>
	virtual std::vector<std::vector<double>> GetE()const = 0;

	/// <summary>
	/// <para>�񎟌��q�X�g�O�����̑��Ό덷���擾����
	/// </summary>
	/// <exception cref="std::out_of_range"></exception>
	/// <returns></returns>
	virtual std::vector<std::vector<double>> GetE1()const = 0;

	/// <summary>
	/// �񎟌��q�X�g�O�����̉����ɂ����� bin �����擾����@�m�F�p
	/// </summary>
	/// <returns></returns>
	virtual const std::vector<std::vector<double>>& GetWidth()const noexcept = 0;

	/// <summary>
	/// �񎟌��q�X�g�O�����̉����X�P�[���ɂ��C���O�̒l���擾����@�m�F�p
	/// </summary>
	/// <returns></returns>
	virtual const std::vector<std::vector<double>>& GetRaw()const noexcept = 0;

	/// <summary>
	/// �񎟌��q�X�g�O�����̐��l���擾����@��ԋ߂��ڐ���̐��l��Ԃ�
	/// </summary>
	/// <param name="Xvalue"></param>
	/// <param name="Yvalue"></param>
	/// <exception cref="std::out_of_range"></exception>
	/// <returns></returns>
	virtual double GetAt(const double Xvalue, const double Yvalue)const = 0;

	/// <summary>
	/// �񎟌��q�X�g�O�����̐��l���擾����
	/// </summary>
	/// <param name="Pnt0"></param>
	/// <param name="Pnt1"></param>
	/// <exception cref="std::out_of_range"></exception>
	/// <returns></returns>
	virtual double GetAt(const size_t Pnt0, const size_t Pnt1)const = 0;

	/// <summary>
	/// �񎟌��q�X�g�O�����̐�Ό덷���擾����@��ԋ߂��ڐ���̐��l��Ԃ�
	/// </summary>
	/// <param name="Xvalue"></param>
	/// <param name="Yvalue"></param>
	/// <exception cref="std::out_of_range"></exception>
	/// <returns></returns>
	virtual double GetEAt(const double Xvalue, const double Yvalue)const = 0;

	/// <summary>
	/// �񎟌��q�X�g�O�����̐�Ό덷���擾����
	/// </summary>
	/// <param name="Pnt0"></param>
	/// <param name="Pnt1"></param>
	/// <exception cref="std::out_of_range"></exception>
	/// <returns></returns>
	virtual double GetEAt(const size_t Pnt0, const size_t Pnt1)const = 0;

	/// <summary>
	/// �񎟌��q�X�g�O�����̑��Ό덷���擾����@��ԋ߂��ڐ���̐��l��Ԃ�
	/// </summary>
	/// <param name="Xvalue"></param>
	/// <param name="Yvalue"></param>
	/// <exception cref="std::out_of_range"></exception>
	/// <returns></returns>
	virtual double GetE1At(const double Xvalue, const double Yvalue)const = 0;

	/// <summary>
	/// �񎟌��q�X�g�O�����̑��Ό덷���擾����
	/// </summary>
	/// <param name="Pnt0"></param>
	/// <param name="Pnt1"></param>
	/// <exception cref="std::out_of_range"></exception>
	/// <returns></returns>
	virtual double GetE1At(const size_t Pnt0, const size_t Pnt1)const = 0;


	/// <summary>
	/// �������̂O��S������ ScaleInformation ���擾����
	/// </summary>
	/// <exception cref="std::exception">�Ԓl�̎��̂����݂��Ȃ�</exception>
	/// <returns></returns>
	virtual const ScaleInformation& ScaleInfo0()const noexcept = 0;

	/// <summary>
	/// �������̂P��S������ ScaleInformation ���擾����
	/// </summary>
	/// <exception cref="std::exception">�Ԓl�̎��̂����݂��Ȃ�</exception>
	/// <returns></returns>
	virtual const ScaleInformation& ScaleInfo1()const noexcept = 0;



	/// <summary>
	/// �N���X ScaledHistogram2D ���쐬����
	/// </summary>
	/// <param name="sppointInformation0"> : �������̂O��\���N���X PointInformation ��shared_ptr</param>
	/// <param name="sppointInformation1"> : �������̂P��\���N���X PointInformation ��shared_ptr</param>
	/// <param name="f_Pnt2HorizontalScale0"> : �[���ȏ�̐����i�|�C���g���j���牡�����̂O�ڐ���l�ւ̕ϊ��֐�</param>
	/// <param name="f_Pnt2HorizontalScale1"> : �[���ȏ�̐����i�|�C���g���j���牡�����̂P�ڐ���l�ւ̕ϊ��֐�</param>
	/// /// <exception cref="std::invalid_argument"></exception>
	/// <exception cref="std::out_of_range"></exception>
	/// <exception cref = "Exceptions from std::make_unique"></exception>
	/// <returns></returns>
	static std::unique_ptr<ScaledHistogram2D> make(const std::shared_ptr<PointInformation>& sppointInformation0, const std::shared_ptr<PointInformation>& sppointInformation1, std::function<double(const double)> f_Pnt2HorizontalScale0, std::function<double(const double)> f_Pnt2HorizontalScale1);


};


#define make_ScaledHistogram2D ScaledHistogram2D::make