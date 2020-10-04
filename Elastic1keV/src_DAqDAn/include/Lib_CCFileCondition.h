#pragma once

#include <string>
#include <memory>

#include "Lib_CppAddCCFCrd.h"
#include "Lib_Inputs.h"


//---------------------------------------------------------------------------------------
//Cobold��Condition��C++�ł��g����悤�ɂ���N���X
//---------------------------------------------------------------------------------------


class CCFileConditionA {
public:
    using type_char = char;
    using tstring = std::basic_string<type_char>;

    virtual ~CCFileConditionA() = default;

    /// <summary>
    /// CCF��Condition���������Ă��邩�m�F����
    /// </summary>
    /// <param name="ConditionName"> : CCF�ł�Condition����</param>
    /// <exception cref="std::out_of_range">�ċA�����񐔂�����𒴂���</exception>
    /// <exception cref="std::exception">�s���ȕϐ��^�t���O</exception>
    /// <returns>Condition���������Ă���Ȃ�true, �����łȂ��Ȃ�false</returns>
    virtual bool IsTrue(const tstring& ConditionName)const = 0;

    /// <summary>
    /// Cobold Command File����Condition��ǂݎ��CC++�Ŏg�p���邽�߂̃N���X��Ԃ�
    /// </summary>
    /// <param name="List_CCFileName"> : Condition��ǂݎ��t�@�C���̃p�X���X�g</param>
    /// <param name="Ptr2Data"> : CppAddress_CCFileCoordinate::SetPr�œo�^�����f�[�^�DGet�œn�����</param>
    /// <exception cref="std::invalid_argument">�t�@�C�����J���Ȃ�����</exception>
    /// <exception cref="std::out_of_range">Data*��Condition�ŕK�v��Coordinate���̂�������Ȃ�����</exception>
    /// <exception cref="std::exception">�s���t���O</exception>
    /// <exception cref="Exceptions from std::make_unique"></exception>
    /// <returns></returns>
    static std::unique_ptr<CCFileConditionA> make(std::initializer_list<tstring> List_CCFilePath, const CppAddress_CCFileCoordinateA::Data& Data);
};

//class CCFileConditionW: protected CCFileCondition_Flags {
//public:
//    using type_char = wchar_t;
//    using tstring = std::basic_string<type_char>;
//
//    virtual ~CCFileConditionW() = default;
//
//    /// <summary>
//    /// CCF��Condition���������Ă��邩�m�F����
//    /// </summary>
//    /// <param name="ConditionName"> : CCF�ł�Condition����</param>
//    /// <exception cref="std::out_of_range">�ċA�����񐔂�����𒴂���</exception>
//    /// <exception cref="std::exception">�s���ȕϐ��^�t���O</exception>
//    /// <returns>Condition���������Ă���Ȃ�true, �����łȂ��Ȃ�false</returns>
//    virtual bool IsTrue(const tstring& ConditionName)const = 0;
//
//    /// <summary>
//    /// <para>�w�肵��ConditionFlag�����̂܂ܕ�����֕ϊ�����</para><para>�G���[�Ȃ�"always"���Z�b�g����</para>
//    /// </summary>
//    /// <param name="Dst"></param>
//    /// <param name="ConditionFlag"> : ������ɕϊ��������񋓎q�ɑΉ����鐔�l</param>
//    /// <returns><para>0 : ����</para><para>1 : ����ConditionFlagValue���ݒ�͈̔͊O������</para></returns>
//    virtual __int32 ConditionFlag_ToString(tstring& Dst, const type_Flag ConditionFlagValue)const noexcept = 0;
//
//    /// <summary>
//    /// Cobold Command File����Condition��ǂݎ��CC++�Ŏg�p���邽�߂̃N���X��Ԃ�
//    /// </summary>
//    /// <param name="List_CCFileName"> : Condition��ǂݎ��t�@�C���̃p�X���X�g</param>
//    /// <param name="Ptr2Data"> : CppAddress_CCFileCoordinate::SetPr�œo�^�����f�[�^�DGet�œn�����</param>
//    /// <exception cref="std::invalid_argument">�t�@�C�����J���Ȃ�����</exception>
//    /// <exception cref="std::out_of_range">Data*��Condition�ŕK�v��Coordinate���̂�������Ȃ�����</exception>
//    /// <exception cref="std::exception">�s���t���O</exception>
//    /// <exception cref="Exceptions from std::make_unique"></exception>
//    /// <returns></returns>
//    static std::unique_ptr<CCFileConditionW> make(std::initializer_list<tstring> List_CCFilePath, const CppAddress_CCFileCoordinateW::Data& Data);
//};
class CCFileConditionW;//������

#ifdef UNICODE
#define CCFileCondition CCFileConditionW
#else
#define CCFileCondition CCFileConditionA
#endif // UNICODE

#define make_CCFileCondition CCFileCondition::make


