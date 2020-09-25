#pragma once


#include <memory>
#include <vector>
#include <stdexcept>


/// <summary>
/// <para>���Ԋu�ڐ���̓񎟌��J�E���g�����z</para>
/// <para>[i][j]�̋��x�͒����`�̈� [ x(i)-binx/2 , x(i)+binx/2 ) * [ y(j)-biny/2 , y(j)+biny/2 ) �Ɋ܂܂��J�E���g����\��</para>
/// </summary>
class XYPairTableToXYDist
{
public:
	virtual ~XYPairTableToXYDist();

	//�񎟌����z�̗v�f�̌^
	using type_Elm_XYDist = size_t;

	//�񎟌����z�̌^
	using type_XYDist = std::vector<std::vector<type_Elm_XYDist>>;

	/// <summary>
	/// �񎟌����z�̃J�E���g���N���A����
	/// </summary>
	/// <returns></returns>
	virtual void ClearCounts() noexcept = 0;

	/// <summary>
	/// �񎟌����z���擾����
	/// </summary>
	/// <returns></returns>
	virtual const type_XYDist& Get()const = 0;

	/// <summary>
	/// �񎟌����z�̓���|�C���g�ł̒l��Ԃ�
	/// </summary>
	/// <param name="i"> : X�����̃|�C���g��</param>
	/// <param name="j"> : Y�����̃|�C���g��</param>
	/// <exception cref="std::out_of_range"></exception>
	/// <returns>�J�E���g��</returns>
	virtual type_Elm_XYDist At(const size_t i, const size_t j)const = 0;

	/// <summary>
	/// <para>�e�C�x���g��XY���W���L�^�����ꎟ���z�� { X0, Y0, X1, Y1, ..., Xn, Yn } ����ǂݍ���œ񎟌����z�֒ǉ�����</para>
	/// <para>��O�ɂ�蒆�f�����ꍇ�C�f�[�^�͕ύX����Ȃ�</para>
	/// </summary>
	/// <param name="XYPairTable"> : XY���W���L�^�����ꎟ���z��</param>
	/// <exception cref="std::out_of_range"></exception>
	/// <exception cref="std::invalid_argument">�R���X�g���N�^�łُ̈�܂��͈����̗v�f���̖��</exception>
	/// <returns></returns>
	virtual void Appned_XY1DTable_Events_To_XYDist(const std::vector<double>& XYPairTable) = 0;

	/// <summary>
	/// <para>�e�C�x���g��XY���W���L�^�����ꎟ���z�� { X0, Y0, X1, Y1, ..., Xn, Yn } ����ǂݍ���œ񎟌����z�֒ǉ�����</para>
	/// <para>��O�ɂ�蒆�f�����ꍇ�C�f�[�^�͕ύX����Ȃ�</para>
	/// </summary>
	/// <param name="XYPairTable"> : XY���W���L�^�����ꎟ���z��</param>
	/// <param name="NumElements"> : �ꎟ���z��̗v�f���D�C�x���g���� (n+1) �̓�{�ɓ������D</param>
	/// <exception cref="std::out_of_range"></exception>
	/// <exception cref="std::invalid_argument">�R���X�g���N�^�łُ̈�܂��͈����̗v�f���̖��</exception>
	/// <returns></returns>
	virtual void Appned_XY1DTable_Events_To_XYDist(const double* XYPairTable, const size_t NumElements) = 0;



	struct UniformScale {
		virtual ~UniformScale();
		virtual explicit operator bool()const noexcept = 0;

		/// <summary>
		/// <para>��Ԃ̌� N</para>
		/// <para>��ԃC���f�b�N�X�iIgor Pro �̃|�C���g�Ɠ��`�j�� 0 ���� (N-1) �܂ł̐���</para>
		/// </summary>
		/// <returns></returns>
		virtual size_t NumBins()const noexcept = 0;

		/// <summary>
		/// <para>��Ԃ̎n�[ x(0)-binx/2</para>
		/// <para>��ԑ�\�l x(0) �Ƃ͈قȂ�</para>
		/// </summary>
		/// <returns></returns>
		virtual double Range_Beg()const noexcept = 0;

		/// <summary>
		/// <para>��Ԃ̏I�[ x(N-1)+binx/2</para>
		/// <para>��ԑ�\�l x(N-1) �Ƃ͈قȂ�</para>
		/// </summary>
		/// <returns></returns>
		virtual double Range_Last()const noexcept = 0;

		/// <summary>
		/// ��ԕ� binx
		/// </summary>
		/// <returns></returns>
		virtual double Bin()const noexcept = 0;

		/// <summary>
		/// ��ԃC���f�b�N�X�����ԑ�\�l���擾����
		/// </summary>
		/// <param name="Pnt"> : ��ԃC���f�b�N�X�iIgor Pro �̃|�C���g�Ɠ��`�j�D0 ���� (NumBins()-1) �܂ł̐���</param>
		/// <exception cref="std::out_of_range">��ԃC���f�b�N�X���͈͊O</exception>
		/// <returns>��Ԃ��\����l</returns>
		virtual double Pnt2Scale(const size_t Pnt)const = 0;
	};

	/// <summary>
	/// �ڐ������ X ����
	/// </summary>
	/// <exception cref="std::invalid_argument">�R���X�g���N�^�łُ̈�ɂ��I�u�W�F�N�g�擾�Ɏ��s</exception>
	/// <returns> X �����Ɋւ��� UniformScale �ւ̎Q��</returns>
	virtual const XYPairTableToXYDist::UniformScale& X()const = 0;

	/// <summary>
	/// �ڐ������ Y ����
	/// </summary>
	/// <exception cref="std::invalid_argument">�R���X�g���N�^�łُ̈�ɂ��I�u�W�F�N�g�擾�Ɏ��s</exception>
	/// <returns> Y �����Ɋւ��� UniformScale �ւ̎Q��</returns>
	virtual const XYPairTableToXYDist::UniformScale& Y()const = 0;

	/// <summary>
	/// �R���X�g���N�^���������Ă����� true, ���s���Ă����� false ��Ԃ�
	/// </summary>
	/// <returns></returns>
	virtual bool Exists()const noexcept = 0;


	/// <summary>
	/// <para>�N���X XYPairTableToXYDist ���쐬���Ă��̃|�C���^��Ԃ�</para>
	/// <para>�����ɖ������������ꍇ��O�𓊂���</para>
	/// </summary>
	/// <exception cref = "std::invalid_argument">NumBin�����łȂ��CBin�����łȂ�</exception>
	/// <exception cref = "std::out_of_range"></exception>
	/// <exception cref = "Exceptions of std::make_unique or std::make_share"></exception>
	/// <returns></returns>
	std::unique_ptr<XYPairTableToXYDist> make(const size_t NumBins_X, const double Offset_X, const double Bin_X, const size_t NumBins_Y, const double Offset_Y, const double Bin_Y);
};


#define make_XYPairTableToXYDist XYPairTableToXYDist::make