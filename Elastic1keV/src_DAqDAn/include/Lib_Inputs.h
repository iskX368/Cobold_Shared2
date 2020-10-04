#pragma once
#include <string>
#include <stdexcept>


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
	//�͈͂��擾����
	static std::pair<double, double> AzimuthRange(const AzimuthFormat Format);
	//�͈͓��֎��܂�悤�ɃV�t�g����
	static double AzimuthIntoRnge(const double Azimuth, const AzimuthFormat Format);
	//�t�H�[�}�b�g��ύX����
	static double ConvertAzimuthFormat(const double Azimuth, const LibFlag::AzimuthFormat Format_Before, const LibFlag::AzimuthFormat Format_After);
	//r,phi���擾����
	static void OrthoToRPhi(double dX, double dY, __int32 iAFormat, double dCX, double dCY, double& dR, double& dPhi);



};


//�t�@�C���p�X�Ȃǂ̃p�����[�^
class LibPrm {
public:
	
	//DAq�����ꎞ�I�ɕۑ�����t�@�C��  CurrentDAqInfo�ɂĎg�p
	static constexpr char DAqInfo_FilePath[] = "C:\\Program Files\\RoentDek Handels GmbH\\CoboldPC 2011 R5-2-x64 (Visual Studio .Net 2010 Compilation) V10.1.1412.2\\LAES2_TDC8PCI2_HEX\\Cobold_Shared2\\temp\\DAqinfo.dat";
	//��L�Ƀt�@�C���֏������܂��t���O
	static constexpr char DAqInfo_COBOLD_online = '1';
	static constexpr char DAqInfo_COBOLD_offline = '0';
	//�n�[�h�E�F�A��荞�݂̏ꍇ��L�ꎞ�t�@�C���֏������ޕ�����
	static constexpr char SourceLMFilePath_InCase_Hardwave[] = "HARDWARE";


	//�C�x���g���Ƃ�Coordinate�l�����\�ŏo�͂����
	static constexpr char WriteForEachEvents_FilePath[] = "C:\\Program Files\\RoentDek Handels GmbH\\CoboldPC 2011 R5-2-x64 (Visual Studio .Net 2010 Compilation) V10.1.1412.2\\LAES2_TDC8PCI2_HEX\\Cobold_Shared2\\temp\\Events.txt";

	//DCS�q�X�g�O������Igor Text �`���ŏo�͂����
	static constexpr char WriteDCSHisto_FilePath[] = "C:\\Program Files\\RoentDek Handels GmbH\\CoboldPC 2011 R5-2-x64 (Visual Studio .Net 2010 Compilation) V10.1.1412.2\\LAES2_TDC8PCI2_HEX\\Cobold_Shared2\\temp\\DCSHisto.itx";




	//���̃t�@�C������Condition��ǂݍ���
	static constexpr char ReadConditionsFromCCF_FilePath[] = "C:\\Program Files\\RoentDek Handels GmbH\\CoboldPC 2011 R5-2-x64 (Visual Studio .Net 2010 Compilation) V10.1.1412.2\\LAES2_TDC8PCI2_HEX\\Cobold_Shared2\\Elastic1keV\\200313\\DAn_Condition.ccf";


};





//Elastic32�ɂ��e���U����DCS��DblGauss�֐�
//y=f1(x)+f2(x), f(x)=y0 + A*exp(-((x-x0)/width)^2)
//�ɂ���ăt�B�b�e�C���O�������ʂ̌W��
class Elastic32_FittedByDblGauss {
public:

	//�w���E�����q, 1000 eV
	class He_1000eV {
	public:
		static constexpr double y0 = 0;

		static constexpr double A1 = 0.2061254115058485;
		static constexpr double x1 = 0;
		static constexpr double width1 = 22.21560539908612;

		static constexpr double A2 = 0.4594778733989648;
		static constexpr double x2 = 0;
		static constexpr double width2 = 10.51057480365862;
	};

	//�Y�f���q, 1000 eV
	class C_1000eV {
	public:
		static constexpr double y0 = 0;

		static constexpr double A1 = 4.940528216909887;
		static constexpr double x1 = 0;
		static constexpr double width1 = 14.89060294791608;

		static constexpr double A2 = 14.65720057784397;
		static constexpr double x2 = 0;
		static constexpr double width2 = 7.000591626682274;
	};

	//���f���q, 1000 eV
	class N_1000eV {
	public:
		static constexpr double y0 = 0;

		static constexpr double A1 = 3.657996227826588;
		static constexpr double x1 = 0;
		static constexpr double width1 = 17.44280067582442;

		static constexpr double A2 = 11.18558859617259;
		static constexpr double x2 = 0;
		static constexpr double width2 = 8.060781092185545;
	};

	//�l�I�����q, 1000 eV
	class Ne_1000eV {
	public:
		static constexpr double y0 = 0;

		static constexpr double A1 = 1.934591395945878;
		static constexpr double x1 = 0;
		static constexpr double width1 = 23.07055198894904;

		static constexpr double A2 = 5.240446505025217;
		static constexpr double x2 = 0;
		static constexpr double width2 = 10.73669485959234;
	};

	//���f���q, 1000 eV
	class Cl_1000eV {
	public:
		static constexpr double y0 = 0;

		static constexpr double A1 = 9.129344975875995;
		static constexpr double x1 = 0;
		static constexpr double width1 = 16.01129868089606;

		static constexpr double A2 = 42.53411080123093;
		static constexpr double x2 = 0;
		static constexpr double width2 = 7.163525875790771;
	};

	//�A���S�����q, 1000 eV
	class Ar_1000eV {
	public:
		static constexpr double y0 = 0;

		static constexpr double A1 = 35.99857082703646;
		static constexpr double x1 = 0;
		static constexpr double width1 = 7.789543887983262;

		static constexpr double A2 = 7.226501412028382;
		static constexpr double x2 = 0;
		static constexpr double width2 = 17.58360677041105;
	};

	//�L�Z�m�����q, 1000 eV
	class Xe_1000eV {
	public:
		static constexpr double y0 = 0;

		static constexpr double A1 = 116.6179148528798;
		static constexpr double x1 = 0;
		static constexpr double width1 = 6.099721784422442;

		static constexpr double A2 = 18.96236804214671;
		static constexpr double x2 = 0;
		static constexpr double width2 = 14.57429917691583;
	};

};
//
////�w���E�����q
////1000 eV, �e���U��
//struct Parameters_TheoreticalAtomicDCS_Elastic32_He_1000eV {
//	static constexpr double y0 = 0;
//
//	static constexpr double A1 = 0.2061254115058485;
//	static constexpr double x1 = 0;
//	static constexpr double width1 = 22.21560539908612;
//
//	static constexpr double A2 = 0.4594778733989648;
//	static constexpr double x2 = 0;
//	static constexpr double width2 = 10.51057480365862;
//
//};
//
////�Y�f���q
////1000 eV, �e���U��
//struct Parameters_TheoreticalAtomicDCS_Elastic32_C_1000eV {
//	static constexpr double y0 = 0;
//
//	static constexpr double A1 = 4.940528216909887;
//	static constexpr double x1 = 0;
//	static constexpr double width1 = 14.89060294791608;
//
//	static constexpr double A2 = 14.65720057784397;
//	static constexpr double x2 = 0;
//	static constexpr double width2 = 7.000591626682274;
//
//};
//
////���f���q
////1000 eV, �e���U��
//struct Parameters_TheoreticalAtomicDCS_Elastic32_N_1000eV {
//	static constexpr double y0 = 0;
//
//	static constexpr double A1 = 3.657996227826588;
//	static constexpr double x1 = 0;
//	static constexpr double width1 = 17.44280067582442;
//
//	static constexpr double A2 = 11.18558859617259;
//	static constexpr double x2 = 0;
//	static constexpr double width2 = 8.060781092185545;
//
//};
//
////�l�I�����q
////1000 eV, �e���U��
//struct Parameters_TheoreticalAtomicDCS_Elastic32_Ne_1000eV {
//	static constexpr double y0 = 0;
//
//	static constexpr double A1 = 1.934591395945878;
//	static constexpr double x1 = 0;
//	static constexpr double width1 = 23.07055198894904;
//
//	static constexpr double A2 = 5.240446505025217;
//	static constexpr double x2 = 0;
//	static constexpr double width2 = 10.73669485959234;
//
//};
//
////���f���q
////1000 eV, �e���U��
//struct Parameters_TheoreticalAtomicDCS_Elastic32_Cl_1000eV {
//	static constexpr double y0 = 0;
//
//	static constexpr double A1 = 9.129344975875995;
//	static constexpr double x1 = 0;
//	static constexpr double width1 = 16.01129868089606;
//
//	static constexpr double A2 = 42.53411080123093;
//	static constexpr double x2 = 0;
//	static constexpr double width2 = 7.163525875790771;
//
//};
//
////�A���S�����q
////1000 eV, �e���U��
//struct Parameters_TheoreticalAtomicDCS_Elastic32_Ar_1000eV {
//	static constexpr double y0 = 0;
//
//	static constexpr double A1 = 35.99857082703646;
//	static constexpr double x1 = 0;
//	static constexpr double width1 = 7.789543887983262;
//
//	static constexpr double A2 = 7.226501412028382;
//	static constexpr double x2 = 0;
//	static constexpr double width2 = 17.58360677041105;
//
//};
//
////�L�Z�m�����q
////1000 eV, �e���U��
//struct Parameters_TheoreticalAtomicDCS_Elastic32_Xe_1000eV {
//	static constexpr double y0 = 0;
//
//	static constexpr double A1 = 116.6179148528798;
//	static constexpr double x1 = 0;
//	static constexpr double width1 = 6.099721784422442;
//
//	static constexpr double A2 = 18.96236804214671;
//	static constexpr double x2 = 0;
//	static constexpr double width2 = 14.57429917691583;
//
//};


//�X�L�}�[�ɂ�鑜�̍�������Phi_Offset�͂قڃ[��
//�Z���v���[�g���̉�]��IL�̘R�ꎥ��ɂ��


//�U���p�x�ւ̕ϊ��Ɏg�p����񎟑������̌W��
//�f�[�^�� e19111401
//�o�˃G�l���M�[ 1000 eV
struct Parameters_ConversionToDCS_e19111401 {
	static constexpr double K0 = 0;//�U���Ɋp Theta_deg ���猟�o�ʒu���S����̋��� R_mm �ւ̕ϊ��񎟑����� R_mm = F(theta_deg) �̒萔��
	static constexpr double K1 = 0.834573;//�U���Ɋp Theta_deg ���猟�o�ʒu���S����̋��� R_mm �ւ̕ϊ��񎟑����� R_mm = F(theta_deg) �̑��W��
	static constexpr double K2 = 0.0405143;//�U���Ɋp Theta_deg ���猟�o�ʒu���S����̋��� R_mm �ւ̕ϊ��񎟑����� R_mm = F(theta_deg) �̑��W��

	static constexpr double Phi_Offset = 0;//���o�풆�S�Ɋւ�����ʊp phi ���炱��������ƎU�����ʊp varphi �����߂���
	static constexpr LibFlag::AzimuthFormat Format_Phi_Offset = LibFlag::AzimuthFormat::Deg;
};





//�G�l���M�[�ւ̕ϊ��Ɏg�p����񎟑������̌W�����z
//�f�[�^�� e191121
//��͂� e200918.pxp
class CoefDist_TOFNoOffset2KES_191121 {
public:

	//���˃G�l���M�[ [eV]
	static constexpr double KE0 = 1000;

	//�W���v�Z���� bin ���
	static constexpr size_t DimSizeX = 161;
	static constexpr double DimDeltaX = 0.5;
	static constexpr double DimOffsetX = -40;

	static constexpr size_t DimSizeY = 161;
	static constexpr double DimDeltaY = 0.5;
	static constexpr double DimOffsetY = -40;


	//���o�ʒu (PosX, PosY) �̒����` bin ���Ƃɒx��������ɂ��g���K�x�����������s���� TOF_NoOffset ��Ԓl�C�^���G�l���M�[�̍� KES = KE - KE0 �������ɂ����񎟑����� TOF_NoOffset = f(KES)
	//����A���͂ɂ���ċ��߂��Ƃ��̊e�W���̈ʒu���z�D�ŋߐڎl�s�N�Z�����l�ɂ���Ԓl��Ԃ�
	// K0 ����[���W���CK0s ���t�B�b�e�B���O�덷�ȂǁD
	//��`��͈̔͊O�ŗ�O std::out_of_range �𑗏o����

	static double K0(const double PosX, const double PosY);
	static double K0s(const double PosX, const double PosY);

	static double K1(const double PosX, const double PosY);
	static double K1s(const double PosX, const double PosY);

	static double K2(const double PosX, const double PosY);
	static double K2s(const double PosX, const double PosY);

};




//���o�튴�x�␳�Ɏg�p����f�[�^
class SourceEvents_DetectorSensitivity_19122536 {
public:

	//r,phi,mcleanT_elastic,Ei=1000eV
	//; e191225
	//Parameter 1023, -4.22; PosX - value of center for r / phi coordinates
	//Parameter 1024, -1.18; PosY - value of center for r / phi coordinates

	static constexpr LibFlag::Sample Sample = LibFlag::Sample::He;
	static constexpr LibFlag::Condition Condition = LibFlag::Condition::mcleanT_elastic;
	static constexpr LibFlag::AzimuthFormat Format_Phi = LibFlag::AzimuthFormat::Deg;

	static constexpr size_t NumCoordinates = 2;
	static constexpr size_t NumEvents = 175726 / NumCoordinates;


	//r,phi��Ԃ�
	static std::tuple<double, double> Event(const size_t EventIndex);


};


