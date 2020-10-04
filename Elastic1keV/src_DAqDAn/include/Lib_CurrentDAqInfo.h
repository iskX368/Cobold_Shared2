#pragma once

#include <string>
#include <stdexcept>
#include "Lib_Inputs.h"



//char
class CurrentDAqInfoA {
public:
	using type_char = char;
	using tstring = std::basic_string<type_char>;

	//�f�[�^��荞�݂̂Ƃ��̃\�[�XLMF�̃p�X��Ԃ�
	//�n�[�h�E�F�A��荞�݂̂Ƃ���"HARDWARE"�ƕԂ�
	static const tstring& SourceLMFilePath();

	//�f�[�^��荞�݂̂Ƃ��̃\�[�XLMF�̖��O��Ԃ�
	static tstring SourceLMFileName();

	//�n�[�h�E�F�A��荞�݂Ȃ�true, LMF��荞�݂Ȃ�false
	static bool IsOnline();

	//DAq�v���O��������ۑ����������e�������ɓn���ăe�L�X�g�t�@�C���֕ۑ�����
	//�ۑ���������񂪑�����������֒ǉ�
	static __int32 Save(const tstring& LMFilePath, bool isOnline);

};

//char
class CurrentDAqInfoW {
public:
	using type_char = wchar_t;
	using tstring = std::basic_string<type_char>;

	//LMF��荞�݂̂Ƃ��Ƀ\�[�X��LMF�̃p�X��Ԃ�
	//�n�[�h�E�F�A��荞�݂̂Ƃ���"HARDWARE"�ƕԂ�
	static const tstring& SourceLMFilePath();

	//�n�[�h�E�F�A��荞�݂Ȃ�true, LMF��荞�݂Ȃ�false
	static bool IsOnline();

	//DAq�v���O��������ۑ����������e�������ɓn���ăe�L�X�g�t�@�C���֕ۑ�����
	//�ۑ���������񂪑�����������֒ǉ�
	static __int32 Save(const tstring& LMFilePath, bool isOnline);

};


#ifdef UNICODE
#define CurrentDAqInfo CurrentDAqInfoW//������
#else
#define CurrentDAqInfo CurrentDAqInfoA
#endif // UNICODE
