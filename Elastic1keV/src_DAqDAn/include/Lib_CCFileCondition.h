#pragma once

#include <string>
#include <memory>

#include "Lib_CppAddCCFCrd.h"
#include "Lib_Inputs.h"


//---------------------------------------------------------------------------------------
//CoboldのConditionをC++でも使えるようにするクラス
//---------------------------------------------------------------------------------------


class CCFileConditionA {
public:
    using type_char = char;
    using tstring = std::basic_string<type_char>;

    virtual ~CCFileConditionA() = default;

    /// <summary>
    /// CCFのConditionが成立しているか確認する
    /// </summary>
    /// <param name="ConditionName"> : CCFでのCondition名称</param>
    /// <exception cref="std::out_of_range">再帰処理回数が上限を超えた</exception>
    /// <exception cref="std::exception">不明な変数型フラグ</exception>
    /// <returns>Conditionが成立しているならtrue, そうでないならfalse</returns>
    virtual bool IsTrue(const tstring& ConditionName)const = 0;

    /// <summary>
    /// Cobold Command FileからConditionを読み取り，C++で使用するためのクラスを返す
    /// </summary>
    /// <param name="List_CCFileName"> : Conditionを読み取るファイルのパスリスト</param>
    /// <param name="Ptr2Data"> : CppAddress_CCFileCoordinate::SetPrで登録したデータ．Getで渡される</param>
    /// <exception cref="std::invalid_argument">ファイルを開けなかった</exception>
    /// <exception cref="std::out_of_range">Data*にConditionで必要なCoordinate名称が見つからなかった</exception>
    /// <exception cref="std::exception">不明フラグ</exception>
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
//    /// CCFのConditionが成立しているか確認する
//    /// </summary>
//    /// <param name="ConditionName"> : CCFでのCondition名称</param>
//    /// <exception cref="std::out_of_range">再帰処理回数が上限を超えた</exception>
//    /// <exception cref="std::exception">不明な変数型フラグ</exception>
//    /// <returns>Conditionが成立しているならtrue, そうでないならfalse</returns>
//    virtual bool IsTrue(const tstring& ConditionName)const = 0;
//
//    /// <summary>
//    /// <para>指定したConditionFlagをそのまま文字列へ変換する</para><para>エラーなら"always"をセットする</para>
//    /// </summary>
//    /// <param name="Dst"></param>
//    /// <param name="ConditionFlag"> : 文字列に変換したい列挙子に対応する数値</param>
//    /// <returns><para>0 : 成功</para><para>1 : 引数ConditionFlagValueが設定の範囲外だった</para></returns>
//    virtual __int32 ConditionFlag_ToString(tstring& Dst, const type_Flag ConditionFlagValue)const noexcept = 0;
//
//    /// <summary>
//    /// Cobold Command FileからConditionを読み取り，C++で使用するためのクラスを返す
//    /// </summary>
//    /// <param name="List_CCFileName"> : Conditionを読み取るファイルのパスリスト</param>
//    /// <param name="Ptr2Data"> : CppAddress_CCFileCoordinate::SetPrで登録したデータ．Getで渡される</param>
//    /// <exception cref="std::invalid_argument">ファイルを開けなかった</exception>
//    /// <exception cref="std::out_of_range">Data*にConditionで必要なCoordinate名称が見つからなかった</exception>
//    /// <exception cref="std::exception">不明フラグ</exception>
//    /// <exception cref="Exceptions from std::make_unique"></exception>
//    /// <returns></returns>
//    static std::unique_ptr<CCFileConditionW> make(std::initializer_list<tstring> List_CCFilePath, const CppAddress_CCFileCoordinateW::Data& Data);
//};
class CCFileConditionW;//未実装

#ifdef UNICODE
#define CCFileCondition CCFileConditionW
#else
#define CCFileCondition CCFileConditionA
#endif // UNICODE

#define make_CCFileCondition CCFileCondition::make


