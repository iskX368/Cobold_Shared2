#pragma once

#include <memory>
#include <string>
#include <vector>


#include "Lib_Inputs.h"
#include "Lib_CppAddress_CCFileCoordinate.h"
#include "Lib_ScaleInformation.h"
#include "Lib_CurrentDAqInfo.h"


//----------------------------------------------------------------------------------------
//Cobold��Coordinate�̃C�x���g���Ƃ̐��l���t�@�C���֏������ނ��߂̕�������擾����N���X
//�t�@�C���͎g�p�҂��p�ӂ���
//----------------------------------------------------------------------------------------


class StringsForOutputFileA {
public:
	using type_char = char;
	using tstring = std::basic_string<type_char>;
	using CPPA_CCFC = CppAddress_CCFileCoordinateA;
	using CDI = CurrentDAqInfoA;


	virtual ~StringsForOutputFileA() = default;

	/// <summary>
	/// �t�@�C���`���ɋL�q��������擾����  �G���[���������炻�̍s�����b�Z�[�W�Œu��������
	/// </summary>
	/// <param name="Dst"></param>
	/// <exception cref="�Ȃ�"></exception>
	/// <returns><para>0 : ����</para><para>bit0 : �����擾�Ɏ��s</para><para>bit1 : �\�[�XLMF���擾�Ɏ��s</para></returns>
	virtual __int32 GetHeaderString(tstring& Dst)const noexcept = 0;

	/// <summary>
	/// Coordinate���̗̂���擾����
	/// </summary>
	/// <param name="Dst"></param>
	/// <exception cref="�Ȃ�"></exception>
	/// <returns><para>0</para></returns>
	virtual __int32 GetCoordinatesString(tstring& Dst)const noexcept = 0;

	/// <summary>
	/// Coordinate�ɑΉ����鐔�l���擾����  �G���[��������������ɉ������Ȃ�
	/// </summary>
	/// <param name="Dst"></param>
	/// <exception cref="�Ȃ�"></exception>
	/// <returns><para>0 : ����</para><para>1 : map::at�Ŏ��s</para><para>2 : �R���X�g���N�^�ł̌^�ݒ莸�s</para><para>3 : ���̑�</para></returns>
	virtual __int32 GetValuesString(tstring& Dst)noexcept = 0;

	/// <summary>
	/// <para>�N���X StringsForOutputFileA ���쐬����</para><para>���O��CppAddress_CCFileCoordinate��CurrentDAqInfo�̐ݒ�����Ă�������</para>
	/// </summary>
	/// <param name="List_FlagCoordinate"> : ���Ɏw�肳�ꂽ�L�^������Coordinate�ɑΉ�����Flag_Coordinate</param>
	/// <param name="Separator"> : �L�^����Ƃ��̗v�f�̃Z�p���[�^����</param>
	/// <exception cref="std::inalid_argument : �s���ȕϐ��̌^"></exception>
	/// <exception cref="std::out_of_range : �N���XCppAddress_CCFileCoordinate�ɑΉ�Coordinate�������邱�Ƃ��o���Ȃ�����"></exception>
	/// <exception cref="���̑� : std::make_unique�R���̗�O"></exception>
	/// <returns></returns>
	static std::unique_ptr<StringsForOutputFileA> make(std::initializer_list<LibFlag::Coordinate> List_FlagCoordinate, const type_char Separator);

	/// <summary>
	/// <para>�N���X StringsForOutputFileA ���쐬����</para><para>���O��CppAddress_CCFileCoordinate��CurrentDAqInfo�̐ݒ�����Ă�������</para>
	/// </summary>
	/// <param name="BitFlags_Coordinate"> : Flag_Coordinate�l�����̈ʒu�Ƃ���Coordinate�̃r�b�g�t���O</param>
	/// <param name="Separator"> : �L�^����Ƃ��̗v�f�̃Z�p���[�^����</param>
	/// <exception cref="std::inalid_argument : �s���ȕϐ��̌^"></exception>
	/// <exception cref="std::out_of_range : �N���XCppAddress_CCFileCoordinate�ɑΉ�Coordinate�������邱�Ƃ��o���Ȃ�����"></exception>
	/// <exception cref="���̑� : std::make_unique�R���̗�O"></exception>
	/// <returns></returns>
	static std::unique_ptr<StringsForOutputFileA> make(const LibFlag::type_Flag BitFlags_Coordinate, const type_char Separator);
};

//class StringsForOutputFileW :protected StringsForOutputFile_Flags {
//public:
//	using type_char = wchar_t;
//	using tstring = std::basic_string<type_char>;
//	using CPPA_CCFC = CppAddress_CCFileCoordinateW;
//	using CDI = CurrentDAqInfoW;
//
//
//	virtual ~StringsForOutputFileW() = default;
//
//	/// <summary>
//	/// �t�@�C���`���ɋL�q��������擾����  �G���[���������炻�̍s�����b�Z�[�W�Œu��������
//	/// </summary>
//	/// <param name="Dst"></param>
//	/// <exception cref="�Ȃ�"></exception>
//	/// <returns><para>0 : ����</para><para>bit0 : �����擾�Ɏ��s</para><para>bit1 : �\�[�XLMF���擾�Ɏ��s</para></returns>
//	virtual __int32 GetHeaderString(tstring& Dst)const noexcept = 0;
//
//	/// <summary>
//	/// Coordinate���̗̂���擾����
//	/// </summary>
//	/// <param name="Dst"></param>
//	/// <exception cref="�Ȃ�"></exception>
//	/// <returns><para>0</para></returns>
//	virtual __int32 GetCoordinatesString(tstring& Dst)const noexcept = 0;
//
//	/// <summary>
//	/// Coordinate�ɑΉ����鐔�l���擾����  �G���[��������������ɉ������Ȃ�
//	/// </summary>
//	/// <param name="Dst"></param>
//	/// <exception cref="�Ȃ�"></exception>
//	/// <returns><para>0 : ����</para><para>1 : map::at�Ŏ��s</para><para>2 : �R���X�g���N�^�ł̌^�ݒ莸�s</para><para>3 : ���̑�</para></returns>
//	virtual __int32 GetValuesString(tstring& Dst)noexcept = 0;
//
//	/// <summary>
//	/// <para>�N���X StringsForOutputFileW ���쐬����</para><para>���O��CppAddress_CCFileCoordinate��CurrentDAqInfo�̐ݒ�����Ă�������</para>
//	/// </summary>
//	/// <param name="List_FlagCoordinate"> : ���Ɏw�肳�ꂽ�L�^������Coordinate�ɑΉ�����Flag_Coordinate</param>
//	/// <param name="Separator"> : �L�^����Ƃ��̗v�f�̃Z�p���[�^����</param>
//	/// <exception cref="std::inalid_argument : �s���ȕϐ��̌^"></exception>
//	/// <exception cref="std::out_of_range : �N���XCppAddress_CCFileCoordinate�ɑΉ�Coordinate�������邱�Ƃ��o���Ȃ�����"></exception>
//	/// <exception cref="���̑� : std::make_unique�R���̗�O"></exception>
//	/// <returns></returns>
//	static std::unique_ptr<StringsForOutputFileW> make(std::initializer_list<StringsForOutputFile_Flags::Flag_Coordinate> List_FlagCoordinate, const type_char Separator);
//
//	/// <summary>
//	/// <para>�N���X StringsForOutputFileW ���쐬����</para><para>���O��CppAddress_CCFileCoordinate��CurrentDAqInfo�̐ݒ�����Ă�������</para>
//	/// </summary>
//	/// <param name="BitFlags_Coordinate"> : Flag_Coordinate�l�����̈ʒu�Ƃ���Coordinate�̃r�b�g�t���O</param>
//	/// <param name="Separator"> : �L�^����Ƃ��̗v�f�̃Z�p���[�^����</param>
//	/// <exception cref="std::inalid_argument : �s���ȕϐ��̌^"></exception>
//	/// <exception cref="std::out_of_range : �N���XCppAddress_CCFileCoordinate�ɑΉ�Coordinate�������邱�Ƃ��o���Ȃ�����"></exception>
//	/// <exception cref="���̑� : std::make_unique�R���̗�O"></exception>
//	/// <returns></returns>
//	static std::unique_ptr<StringsForOutputFileW> make(const type_Flag BitFlags_Coordinate, const type_char Separator);
//
//};
class StringsForOutputFileW;//������


#ifdef UNICODE
#define StringsForOutputFile StringsForOutputFileW
#else
#define StringsForOutputFile StringsForOutputFileA
#endif // UNICODE


#define make_StringsForOutputFile StringsForOutputFile::make




//Igor���ǂݍ��ރt�@�C���p
class StringsForIgorTextWaveA {
public:
	using type_char = char;
	using tstring = std::basic_string<type_char>;


	/// <summary>
	/// Igor �e�L�X�g�t�@�C���p�̕�������擾����@���l�p�E�F�[�u�Ǝ��p�E�F�[�u�̍��v���Ԃ�
	/// </summary>
	/// <param name="Dst"> : �������ݐ�</param>
	/// <param name="Name_ValueWave"> : ���l�E�F�[�u�̖���</param>
	/// <param name="Name_ScaleWave"> : x ���p�E�F�[�u�̖���</param>
	/// <param name="Vec"> : �ꎟ�����l�x�N�g��</param>
	/// <param name="ScaleInfo"> : x ���̏��</param>
	/// <exception cref="std::invalid_argument"></exception>
	/// <exception cref="std::out_of_range"></exception>
	/// <exception cref="std::exception"></exception>
	static void WaveText(tstring& Dst, const tstring& Name_ValueWave, const tstring& Name_ScaleWave, const std::vector<double>& Vec, const ScaleInformation& ScaleInfo);


	/// <summary>
	/// Igor �e�L�X�g�t�@�C���p�̕�������擾����@���l�p�E�F�[�u�Ǝ��p�E�F�[�u��̍��v�O��Ԃ�
	/// </summary>
	/// <param name="Dst"> : �������ݐ�</param>
	/// <param name="Name_ValueWave"> : �񎟌����l�E�F�[�u�̖���</param>
	/// <param name="Name_ScaleWave0"> : x ���p�E�F�[�u�̖���</param>
	/// <param name="Name_ScaleWave1"> : y ���p�E�F�[�u�̖���</param>
	/// <param name="Vec"> : �񎟌����l�x�N�g��</param>
	/// <param name="ScaleInfo0"> : x ���̏��</param>
	/// <param name="ScaleInfo1"> : y ���̏��</param>
	/// <exception cref="std::invalid_argument"></exception>
	/// <exception cref="std::out_of_range"></exception>
	/// <exception cref="std::exception"></exception>
	static void WaveText(tstring& Dst, const tstring& Name_ValueWave, const tstring& Name_ScaleWave0, const tstring& Name_ScaleWave1, const std::vector<std::vector<double>>& Vec, const ScaleInformation& ScaleInfo0, const ScaleInformation& ScaleInfo1);

};

class StringsForIgorTextWaveW {
public:
	using type_char = wchar_t;
	using tstring = std::basic_string<type_char>;


	//�������@std::exception
	static __int32 WaveText(tstring& Dst, const tstring& Name_ValueWave, const tstring& Name_ScaleWave, const std::vector<double>& Vec, const ScaleInformation& ScaleInfo);

	//�������@std::exception
	static __int32 WaveText(tstring& Dst, const tstring& Name_ValueWave, const tstring& Name_ScaleWave0, const tstring& Name_ScaleWave1, const std::vector<std::vector<double>>& Vec, const ScaleInformation& ScaleInfo0, const ScaleInformation& ScaleInfo1);


};



#ifdef UNICODE
#define StringsForIgorTextWave StringsForIgorTextWaveW
#else
#define StringsForIgorTextWave StringsForIgorTextWaveA
#endif // UNICODE
