#pragma once
#include <string>


#define _USE_MATH_DEFINES
#include <math.h>



//�悭�g���O���[�o���ϐ�

static constexpr double gv_deg2rad = M_PI / 180;
static constexpr double gv_rad2deg = 180 / M_PI;



//���C�u�����Ŏg���t���O�̂܂Ƃ�
class LibFlag {
public:
	//��
	using type_Flag = unsigned;


	//�O���t�@�C���֏������݂����C�x���g��Coordinate���r�b�g�t���O�ŕ����I��
	//CCF��Coordinate������ƈ�v������
	enum class Coordinate :type_Flag {
		PosX,
		PosY,
		r,
		phi,
		TOF,
		TOF_NoOffset,
		Theta_deg,
		VarPhi_deg,
		ConsistencyIndicator,

	};
	//�ǉ������炱����X�V
	static constexpr type_Flag Coordinate_Min = 0;
	static constexpr type_Flag Coordinate_Max = 8;
	//enum�̗v�f�𕶎���֕ϊ�����
	static __int32 Coordinate_ToString(std::basic_string<char>& Dst, const type_Flag FlagValue, const Coordinate Default = static_cast<Coordinate>(0U))noexcept;



	//�O���t�@�C���֏������݂����C�x���g���t�B���^�����O����Condition�𐔒l�ň�I��
	//CCF��Condition������ƈ�v������
	enum class Condition :type_Flag
	{
		always,
		clean,
		mclean,
		mcleanT,
		mcleanT_elastic,
	};
	//�ǉ������炱���ύX
	static constexpr type_Flag Condition_Min = 0;
	static constexpr type_Flag Condition_Max = 4;
	//enum�̗v�f�𕶎���֕ϊ�����
	static __int32 Condition_ToString(std::basic_string<char>& Dst, const type_Flag FlagValue, const Condition Default = Condition::always)noexcept;



	//��͑Ώۂ̎����𐔒l�ň�I��
	enum class Sample :type_Flag
	{
		He,
		C,
		N,
		Ne,
		Cl,
		Ar,
		Xe,
	};



	//���ʊp�̃t�H�[�}�b�g
	//Parameter1003�Ɛݒ�𑵂��邱��
	enum class AzimuthFormat :type_Flag
	{
		Rad,//RAD [-pi..pi]
		Rad0,//RAD [0..2pi]
		Deg,//DEG [-180..180]
		Deg0,//DEG [0..360]
	};
	


};


//�t�@�C���p�X�Ȃǂ̃p�����[�^
class LibPrm {
public:
	//CurrentDAqInfo�ɂĎg�p
	static constexpr char DAqInfo_FilePath[] = "C:\\Program Files\\RoentDek Handels GmbH\\CoboldPC 2011 R5-2-x64 (Visual Studio .Net 2010 Compilation) V10.1.1412.2\\LAES2_TDC8PCI2_HEX\\Cobold_Shared1\\temp\\DAqinfo.dat";
	static constexpr char DAqInfo_COBOLD_online = '1';
	static constexpr char DAqInfo_COBOLD_offline = '0';


	//���̃t�@�C���փC�x���g���Ƃ�Coordinate�l����������
	static constexpr char WriteForEachEvents_FilePath[] = "C:\\Program Files\\RoentDek Handels GmbH\\CoboldPC 2011 R5-2-x64 (Visual Studio .Net 2010 Compilation) V10.1.1412.2\\LAES2_TDC8PCI2_HEX\\Cobold_Shared1\\temp\\Events.txt";


	//���̃t�@�C������Condition��ǂݍ���
	static constexpr char ReadConditionsFromCCF_FilePath[] = "C:\\Program Files\\RoentDek Handels GmbH\\CoboldPC 2011 R5-2-x64 (Visual Studio .Net 2010 Compilation) V10.1.1412.2\\LAES2_TDC8PCI2_HEX\\Cobold_Shared1\\Elastic1keV\\200313\\DAn_Condition.ccf";





};

