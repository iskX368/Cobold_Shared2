#pragma once

#include <string>
#include <stdexcept>
#include <map>


//----------------------------------------------------------------------------------------------------------------
//CppAddress_CCFileCoordinate::SetPr��C++�\�[�X�t�@�C���̃O���[�o���ϐ��A�h���X��CCF��Coordinate�̑Ή���o�^����
//�o�^���I������make_CCFileCondition�֐��Ƀt�@�C���p�X�Ƌ���CppAddress_CCFileCoordinate::Get�œn��
//----------------------------------------------------------------------------------------------------------------


struct CppAddress_CCFileCoordinateA
{
    using type_char = char;
    using tstring = std::basic_string<type_char>;


    //�Ή��f�[�^
    using Data = std::tuple < std::map<tstring, __int32*>, std::map<tstring, __int64*>, std::map<tstring, double*>>;
    static constexpr __int32 Data_Index_int32 = 0;
    static constexpr __int32 Data_Index_int64 = 1;
    static constexpr __int32 Data_Index_double = 2;

    /// <summary>
    /// �Ή��f�[�^�C���f�b�N�X���擾����D�^�̔��ʂɎg�p
    /// </summary>
    /// <param name="Coordinate"></param>
    /// <exception cref="�Ȃ�"></exception>
    /// <returns>Data_Index_xxx  ������Ȃ�������d�����������ꍇ��-1</returns>
    static __int32 DataIndex(const tstring& Coordinate)noexcept;

    /// <summary>
    /// �Ή��f�[�^�����o��
    /// </summary>
    /// <exception cref="�Ȃ�"></exception>
    /// <returns></returns>
    static const Data& Get()noexcept;


    /// <summary>
    /// �Ή��f�[�^��S�ď�������
    /// </summary>
    /// <exception cref="�Ȃ�"></exception>
    /// <returns></returns>
    static void ClearAll()noexcept;



    /// <summary>
    /// __int32�^�z��v�f�A�h���X��Coordinate���̂̑Ή���o�^����
    /// </summary>
    /// <param name="CCFileCoordinate"> : CCF��Coordinate�錾���ꂽ������</param>
    /// <param name="Ar"> : __int32�^�O���[�o���ϐ��z��</param>
    /// <param name="Shift"> : �z��̈ʒu</param>
    /// <exception cref="std::out_of_range"></exception>
    /// <exception cref="std::exception">�o�^Coordinate���̂̏d��</exception>
    template<size_t SIZE>
    static void SetPr(tstring&& CCFileCoordinate, __int32(&Ar)[SIZE], const size_t Shift) {
        try {
            if (Shift >= SIZE) {
                throw std::out_of_range("CppAddress_CCFileCoordinate::SetPr, __int32 : Out of range");
            }
            Set_private(std::make_pair<tstring, __int32*>(std::forward<tstring>(CCFileCoordinate), Ar + Shift));
        }
        catch (std::out_of_range&) {
            throw;
        }
        catch (std::exception&) {
            throw;
        }
    }

    /// <summary>
    /// __int32�^�ϐ��A�h���X��Coordinate���̂̑Ή���o�^����
    /// </summary>
    /// <param name="CCFileCoordinate"> : CCF��Coordinate�錾���ꂽ������</param>
    /// <param name="Va"> : __int32�^�O���[�o���ϐ�</param>
    /// <exception cref="std::exception">�o�^Coordinate���̂̏d��</exception>
    static void SetPr(tstring&& CCFileCoordinate, __int32& Va) {
        try {
            Set_private(std::make_pair<tstring, __int32*>(std::forward<tstring>(CCFileCoordinate), &Va));
        }
        catch (std::exception&) {
            throw;
        }
    }

    /// <summary>
    /// __int64�^�z��v�f�A�h���X��Coordinate���̂̑Ή���o�^����
    /// </summary>
    /// <param name="CCFileCoordinate"> : CCF��Coordinate�錾���ꂽ������</param>
    /// <param name="Ar"> : __int64�^�O���[�o���ϐ��z��</param>
    /// <param name="Shift"> : �z��̈ʒu</param>
    /// <exception cref="std::out_of_range"></exception>
    /// <exception cref="std::exception">�o�^Coordinate���̂̏d��</exception>
    template<size_t SIZE>
    static void SetPr(tstring&& CCFileCoordinate, __int64(&Ar)[SIZE], const size_t Shift) {
        try {
            if (Shift >= SIZE) {
                throw std::out_of_range("CppAddress_CCFileCoordinate::SetPr, __int64 : Out of range");
            }
            Set_private(std::make_pair<tstring, __int64*>(std::forward<tstring>(CCFileCoordinate), Ar + Shift));
        }
        catch (std::out_of_range&) {
            throw;
        }
        catch (std::exception&) {
            throw;
        }
    }

    /// <summary>
    /// __int64�^�ϐ��A�h���X��Coordinate���̂̑Ή���o�^����
    /// </summary>
    /// <param name="CCFileCoordinate"> : CCF��Coordinate�錾���ꂽ������</param>
    /// <param name="Va"> : __int64�^�O���[�o���ϐ�</param>
    /// <exception cref="std::exception">�o�^Coordinate���̂̏d��</exception>
    static void SetPr(tstring&& CCFileCoordinate, __int64& Va) {
        try {
            Set_private(std::make_pair<tstring, __int64*>(std::forward<tstring>(CCFileCoordinate), &Va));
        }
        catch (std::exception&) {
            throw;
        }
    }

    /// <summary>
    /// double�^�z��v�f�A�h���X��Coordinate���̂̑Ή���o�^����
    /// </summary>
    /// <param name="CCFileCoordinate"> : CCF��Coordinate�錾���ꂽ������</param>
    /// <param name="Ar"> : double�^�O���[�o���ϐ��z��</param>
    /// <param name="Shift"> : �z��̈ʒu</param>
    /// <exception cref="std::out_of_range"></exception>
    /// <exception cref="std::exception">�o�^Coordinate���̂̏d��</exception>
    template<size_t SIZE>
    static void SetPr(tstring&& CCFileCoordinate, double(&Ar)[SIZE], const size_t Shift) {
        try {
            if (Shift >= SIZE) {
                throw std::out_of_range("CppAddress_CCFileCoordinate::SetPr, double : Out of range");
            }
            Set_private(std::make_pair<tstring, double*>(std::forward<tstring>(CCFileCoordinate), Ar + Shift));
        }
        catch (std::out_of_range&) {
            throw;
        }
        catch (std::exception&) {
            throw;
        }
    }

    /// <summary>
    /// double�^�ϐ��A�h���X��Coordinate���̂̑Ή���o�^����
    /// </summary>
    /// <param name="CCFileCoordinate"> : CCF��Coordinate�錾���ꂽ������</param>
    /// <param name="Va"> : double�^�O���[�o���ϐ�</param>
    /// <exception cref="std::exception">�o�^Coordinate���̂̏d��</exception>
    static void SetPr(tstring&& CCFileCoordinate, double& Va) {
        try {
            Set_private(std::make_pair<tstring, double*>(std::forward<tstring>(CCFileCoordinate), &Va));
        }
        catch (std::exception&) {
            throw;
        }
    }

private:
    static void Set_private(std::pair<tstring, __int32*>&& pr);
    static void Set_private(std::pair<tstring, __int64*>&& pr);
    static void Set_private(std::pair<tstring, double*>&& pr);
};


struct CppAddress_CCFileCoordinateW
{
    using type_char = wchar_t;
    using tstring = std::basic_string<type_char>;


    //�Ή��f�[�^
    //class Data;
    using Data = std::tuple < std::map<tstring, __int32*>, std::map<tstring, __int64*>, std::map<tstring, double*>>;
    static constexpr __int32 Data_Index_int32 = 0;
    static constexpr __int32 Data_Index_int64 = 1;
    static constexpr __int32 Data_Index_double = 2;

    /// <summary>
    /// �Ή��f�[�^�C���f�b�N�X���擾����D�^�̔��ʂɎg�p
    /// </summary>
    /// <param name="Coordinate"></param>
    /// <exception cref="�Ȃ�"></exception>
    /// <returns>Data_Index_xxx  ������Ȃ�������d�����������ꍇ��-1</returns>
    static __int32 DataIndex(const tstring& Coordinate)noexcept;

    /// <summary>
    /// �Ή��f�[�^�����o��
    /// </summary>
    /// <exception cref="�Ȃ�"></exception>
    /// <returns></returns>
    static const Data& Get()noexcept;


    /// <summary>
    /// �Ή��f�[�^��S�ď�������
    /// </summary>
    /// <exception cref="�Ȃ�"></exception>
    /// <returns></returns>
    static void ClearAll()noexcept;



    /// <summary>
    /// __int32�^�z��v�f�A�h���X��Coordinate���̂̑Ή���o�^����
    /// </summary>
    /// <param name="CCFileCoordinate"> : CCF��Coordinate�錾���ꂽ������</param>
    /// <param name="Ar"> : __int32�^�O���[�o���ϐ��z��</param>
    /// <param name="Shift"> : �z��̈ʒu</param>
    /// <exception cref="std::out_of_range"></exception>
    /// <exception cref="std::exception">�o�^Coordinate���̂̏d��</exception>
    template<size_t SIZE>
    static void SetPr(tstring&& CCFileCoordinate, __int32(&Ar)[SIZE], const size_t Shift) {
        try {
            if (Shift >= SIZE) {
                throw std::out_of_range("CppAddress_CCFileCoordinate::SetPr, __int32 : Out of range");
            }
            Set_private(std::make_pair<tstring, __int32*>(std::forward<tstring>(CCFileCoordinate), Ar + Shift));
        }
        catch (std::out_of_range&) {
            throw;
        }
        catch (std::exception&) {
            throw;
        }
    }

    /// <summary>
    /// __int32�^�ϐ��A�h���X��Coordinate���̂̑Ή���o�^����
    /// </summary>
    /// <param name="CCFileCoordinate"> : CCF��Coordinate�錾���ꂽ������</param>
    /// <param name="Va"> : __int32�^�O���[�o���ϐ�</param>
    /// <exception cref="std::exception">�o�^Coordinate���̂̏d��</exception>
    static void SetPr(tstring&& CCFileCoordinate, __int32& Va) {
        try {
            Set_private(std::make_pair<tstring, __int32*>(std::forward<tstring>(CCFileCoordinate), &Va));
        }
        catch (std::exception&) {
            throw;
        }
    }

    /// <summary>
    /// __int64�^�z��v�f�A�h���X��Coordinate���̂̑Ή���o�^����
    /// </summary>
    /// <param name="CCFileCoordinate"> : CCF��Coordinate�錾���ꂽ������</param>
    /// <param name="Ar"> : __int64�^�O���[�o���ϐ��z��</param>
    /// <param name="Shift"> : �z��̈ʒu</param>
    /// <exception cref="std::out_of_range"></exception>
    /// <exception cref="std::exception">�o�^Coordinate���̂̏d��</exception>
    template<size_t SIZE>
    static void SetPr(tstring&& CCFileCoordinate, __int64(&Ar)[SIZE], const size_t Shift) {
        try {
            if (Shift >= SIZE) {
                throw std::out_of_range("CppAddress_CCFileCoordinate::SetPr, __int64 : Out of range");
            }
            Set_private(std::make_pair<tstring, __int64*>(std::forward<tstring>(CCFileCoordinate), Ar + Shift));
        }
        catch (std::out_of_range&) {
            throw;
        }
        catch (std::exception&) {
            throw;
        }
    }

    /// <summary>
    /// __int64�^�ϐ��A�h���X��Coordinate���̂̑Ή���o�^����
    /// </summary>
    /// <param name="CCFileCoordinate"> : CCF��Coordinate�錾���ꂽ������</param>
    /// <param name="Va"> : __int64�^�O���[�o���ϐ�</param>
    /// <exception cref="std::exception">�o�^Coordinate���̂̏d��</exception>
    static void SetPr(tstring&& CCFileCoordinate, __int64& Va) {
        try {
            Set_private(std::make_pair<tstring, __int64*>(std::forward<tstring>(CCFileCoordinate), &Va));
        }
        catch (std::exception&) {
            throw;
        }
    }

    /// <summary>
    /// double�^�z��v�f�A�h���X��Coordinate���̂̑Ή���o�^����
    /// </summary>
    /// <param name="CCFileCoordinate"> : CCF��Coordinate�錾���ꂽ������</param>
    /// <param name="Ar"> : double�^�O���[�o���ϐ��z��</param>
    /// <param name="Shift"> : �z��̈ʒu</param>
    /// <exception cref="std::out_of_range"></exception>
    /// <exception cref="std::exception">�o�^Coordinate���̂̏d��</exception>
    template<size_t SIZE>
    static void SetPr(tstring&& CCFileCoordinate, double(&Ar)[SIZE], const size_t Shift) {
        try {
            if (Shift >= SIZE) {
                throw std::out_of_range("CppAddress_CCFileCoordinate::SetPr, double : Out of range");
            }
            Set_private(std::make_pair<tstring, double*>(std::forward<tstring>(CCFileCoordinate), Ar + Shift));
        }
        catch (std::out_of_range&) {
            throw;
        }
        catch (std::exception&) {
            throw;
        }
    }

    /// <summary>
    /// double�^�ϐ��A�h���X��Coordinate���̂̑Ή���o�^����
    /// </summary>
    /// <param name="CCFileCoordinate"> : CCF��Coordinate�錾���ꂽ������</param>
    /// <param name="Va"> : double�^�O���[�o���ϐ�</param>
    /// <exception cref="std::exception">�o�^Coordinate���̂̏d��</exception>
    static void SetPr(tstring&& CCFileCoordinate, double& Va) {
        try {
            Set_private(std::make_pair<tstring, double*>(std::forward<tstring>(CCFileCoordinate), &Va));
        }
        catch (std::exception&) {
            throw;
        }
    }

private:
    static void Set_private(std::pair<tstring, __int32*>&& pr);
    static void Set_private(std::pair<tstring, __int64*>&& pr);
    static void Set_private(std::pair<tstring, double*>&& pr);
};





#ifdef UNICODE
#define CppAddress_CCFileCoordinate CppAddress_CCFileCoordinateW
#else
#define CppAddress_CCFileCoordinate CppAddress_CCFileCoordinateA
#endif // UNICODE
