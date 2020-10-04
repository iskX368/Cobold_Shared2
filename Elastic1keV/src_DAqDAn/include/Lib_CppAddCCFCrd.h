#pragma once

#include <string>
#include <stdexcept>
#include <map>


//----------------------------------------------------------------------------------------------------------------
//CppAddress_CCFileCoordinate::SetPrでC++ソースファイルのグローバル変数アドレスとCCFのCoordinateの対応を登録する
//登録を終えたらmake_CCFileCondition関数にファイルパスと共にCppAddress_CCFileCoordinate::Getで渡す
//----------------------------------------------------------------------------------------------------------------


struct CppAddress_CCFileCoordinateA
{
    using type_char = char;
    using tstring = std::basic_string<type_char>;


    //対応データ
    using Data = std::tuple < std::map<tstring, __int32*>, std::map<tstring, __int64*>, std::map<tstring, double*>>;
    static constexpr __int32 Data_Index_int32 = 0;
    static constexpr __int32 Data_Index_int64 = 1;
    static constexpr __int32 Data_Index_double = 2;

    /// <summary>
    /// 対応データインデックスを取得する．型の判別に使用
    /// </summary>
    /// <param name="Coordinate"></param>
    /// <exception cref="なし"></exception>
    /// <returns>Data_Index_xxx  見つからなかったり重複があった場合は-1</returns>
    static __int32 DataIndex(const tstring& Coordinate)noexcept;

    /// <summary>
    /// 対応データを取り出す
    /// </summary>
    /// <exception cref="なし"></exception>
    /// <returns></returns>
    static const Data& Get()noexcept;


    /// <summary>
    /// 対応データを全て消去する
    /// </summary>
    /// <exception cref="なし"></exception>
    /// <returns></returns>
    static void ClearAll()noexcept;



    /// <summary>
    /// __int32型配列要素アドレスとCoordinate名称の対応を登録する
    /// </summary>
    /// <param name="CCFileCoordinate"> : CCFでCoordinate宣言された文字列</param>
    /// <param name="Ar"> : __int32型グローバル変数配列</param>
    /// <param name="Shift"> : 配列の位置</param>
    /// <exception cref="std::out_of_range"></exception>
    /// <exception cref="std::exception">登録Coordinate名称の重複</exception>
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
    /// __int32型変数アドレスとCoordinate名称の対応を登録する
    /// </summary>
    /// <param name="CCFileCoordinate"> : CCFでCoordinate宣言された文字列</param>
    /// <param name="Va"> : __int32型グローバル変数</param>
    /// <exception cref="std::exception">登録Coordinate名称の重複</exception>
    static void SetPr(tstring&& CCFileCoordinate, __int32& Va) {
        try {
            Set_private(std::make_pair<tstring, __int32*>(std::forward<tstring>(CCFileCoordinate), &Va));
        }
        catch (std::exception&) {
            throw;
        }
    }

    /// <summary>
    /// __int64型配列要素アドレスとCoordinate名称の対応を登録する
    /// </summary>
    /// <param name="CCFileCoordinate"> : CCFでCoordinate宣言された文字列</param>
    /// <param name="Ar"> : __int64型グローバル変数配列</param>
    /// <param name="Shift"> : 配列の位置</param>
    /// <exception cref="std::out_of_range"></exception>
    /// <exception cref="std::exception">登録Coordinate名称の重複</exception>
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
    /// __int64型変数アドレスとCoordinate名称の対応を登録する
    /// </summary>
    /// <param name="CCFileCoordinate"> : CCFでCoordinate宣言された文字列</param>
    /// <param name="Va"> : __int64型グローバル変数</param>
    /// <exception cref="std::exception">登録Coordinate名称の重複</exception>
    static void SetPr(tstring&& CCFileCoordinate, __int64& Va) {
        try {
            Set_private(std::make_pair<tstring, __int64*>(std::forward<tstring>(CCFileCoordinate), &Va));
        }
        catch (std::exception&) {
            throw;
        }
    }

    /// <summary>
    /// double型配列要素アドレスとCoordinate名称の対応を登録する
    /// </summary>
    /// <param name="CCFileCoordinate"> : CCFでCoordinate宣言された文字列</param>
    /// <param name="Ar"> : double型グローバル変数配列</param>
    /// <param name="Shift"> : 配列の位置</param>
    /// <exception cref="std::out_of_range"></exception>
    /// <exception cref="std::exception">登録Coordinate名称の重複</exception>
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
    /// double型変数アドレスとCoordinate名称の対応を登録する
    /// </summary>
    /// <param name="CCFileCoordinate"> : CCFでCoordinate宣言された文字列</param>
    /// <param name="Va"> : double型グローバル変数</param>
    /// <exception cref="std::exception">登録Coordinate名称の重複</exception>
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


    //対応データ
    //class Data;
    using Data = std::tuple < std::map<tstring, __int32*>, std::map<tstring, __int64*>, std::map<tstring, double*>>;
    static constexpr __int32 Data_Index_int32 = 0;
    static constexpr __int32 Data_Index_int64 = 1;
    static constexpr __int32 Data_Index_double = 2;

    /// <summary>
    /// 対応データインデックスを取得する．型の判別に使用
    /// </summary>
    /// <param name="Coordinate"></param>
    /// <exception cref="なし"></exception>
    /// <returns>Data_Index_xxx  見つからなかったり重複があった場合は-1</returns>
    static __int32 DataIndex(const tstring& Coordinate)noexcept;

    /// <summary>
    /// 対応データを取り出す
    /// </summary>
    /// <exception cref="なし"></exception>
    /// <returns></returns>
    static const Data& Get()noexcept;


    /// <summary>
    /// 対応データを全て消去する
    /// </summary>
    /// <exception cref="なし"></exception>
    /// <returns></returns>
    static void ClearAll()noexcept;



    /// <summary>
    /// __int32型配列要素アドレスとCoordinate名称の対応を登録する
    /// </summary>
    /// <param name="CCFileCoordinate"> : CCFでCoordinate宣言された文字列</param>
    /// <param name="Ar"> : __int32型グローバル変数配列</param>
    /// <param name="Shift"> : 配列の位置</param>
    /// <exception cref="std::out_of_range"></exception>
    /// <exception cref="std::exception">登録Coordinate名称の重複</exception>
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
    /// __int32型変数アドレスとCoordinate名称の対応を登録する
    /// </summary>
    /// <param name="CCFileCoordinate"> : CCFでCoordinate宣言された文字列</param>
    /// <param name="Va"> : __int32型グローバル変数</param>
    /// <exception cref="std::exception">登録Coordinate名称の重複</exception>
    static void SetPr(tstring&& CCFileCoordinate, __int32& Va) {
        try {
            Set_private(std::make_pair<tstring, __int32*>(std::forward<tstring>(CCFileCoordinate), &Va));
        }
        catch (std::exception&) {
            throw;
        }
    }

    /// <summary>
    /// __int64型配列要素アドレスとCoordinate名称の対応を登録する
    /// </summary>
    /// <param name="CCFileCoordinate"> : CCFでCoordinate宣言された文字列</param>
    /// <param name="Ar"> : __int64型グローバル変数配列</param>
    /// <param name="Shift"> : 配列の位置</param>
    /// <exception cref="std::out_of_range"></exception>
    /// <exception cref="std::exception">登録Coordinate名称の重複</exception>
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
    /// __int64型変数アドレスとCoordinate名称の対応を登録する
    /// </summary>
    /// <param name="CCFileCoordinate"> : CCFでCoordinate宣言された文字列</param>
    /// <param name="Va"> : __int64型グローバル変数</param>
    /// <exception cref="std::exception">登録Coordinate名称の重複</exception>
    static void SetPr(tstring&& CCFileCoordinate, __int64& Va) {
        try {
            Set_private(std::make_pair<tstring, __int64*>(std::forward<tstring>(CCFileCoordinate), &Va));
        }
        catch (std::exception&) {
            throw;
        }
    }

    /// <summary>
    /// double型配列要素アドレスとCoordinate名称の対応を登録する
    /// </summary>
    /// <param name="CCFileCoordinate"> : CCFでCoordinate宣言された文字列</param>
    /// <param name="Ar"> : double型グローバル変数配列</param>
    /// <param name="Shift"> : 配列の位置</param>
    /// <exception cref="std::out_of_range"></exception>
    /// <exception cref="std::exception">登録Coordinate名称の重複</exception>
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
    /// double型変数アドレスとCoordinate名称の対応を登録する
    /// </summary>
    /// <param name="CCFileCoordinate"> : CCFでCoordinate宣言された文字列</param>
    /// <param name="Va"> : double型グローバル変数</param>
    /// <exception cref="std::exception">登録Coordinate名称の重複</exception>
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
