#pragma once

#include <memory>
#include <string>
#include <vector>


#include "Lib_Inputs.h"
#include "Lib_CppAddress_CCFileCoordinate.h"
#include "Lib_ScaleInformation.h"
#include "Lib_CurrentDAqInfo.h"


//----------------------------------------------------------------------------------------
//CoboldのCoordinateのイベントごとの数値をファイルへ書き込むための文字列を取得するクラス
//ファイルは使用者が用意する
//----------------------------------------------------------------------------------------


class StringsForOutputFileA {
public:
	using type_char = char;
	using tstring = std::basic_string<type_char>;
	using CPPA_CCFC = CppAddress_CCFileCoordinateA;
	using CDI = CurrentDAqInfoA;


	virtual ~StringsForOutputFileA() = default;

	/// <summary>
	/// ファイル冒頭に記述する情報を取得する  エラーがあったらその行をメッセージで置き換える
	/// </summary>
	/// <param name="Dst"></param>
	/// <exception cref="なし"></exception>
	/// <returns><para>0 : 成功</para><para>bit0 : 日時取得に失敗</para><para>bit1 : ソースLMF名取得に失敗</para></returns>
	virtual __int32 GetHeaderString(tstring& Dst)const noexcept = 0;

	/// <summary>
	/// Coordinate名称の列を取得する
	/// </summary>
	/// <param name="Dst"></param>
	/// <exception cref="なし"></exception>
	/// <returns><para>0</para></returns>
	virtual __int32 GetCoordinatesString(tstring& Dst)const noexcept = 0;

	/// <summary>
	/// Coordinateに対応する数値を取得する  エラーがあったら引数に何もしない
	/// </summary>
	/// <param name="Dst"></param>
	/// <exception cref="なし"></exception>
	/// <returns><para>0 : 成功</para><para>1 : map::atで失敗</para><para>2 : コンストラクタでの型設定失敗</para><para>3 : その他</para></returns>
	virtual __int32 GetValuesString(tstring& Dst)noexcept = 0;

	/// <summary>
	/// <para>クラス StringsForOutputFileA を作成する</para><para>事前にCppAddress_CCFileCoordinateとCurrentDAqInfoの設定をしておくこと</para>
	/// </summary>
	/// <param name="List_FlagCoordinate"> : 順に指定された記録したいCoordinateに対応するFlag_Coordinate</param>
	/// <param name="Separator"> : 記録するときの要素のセパレータ文字</param>
	/// <exception cref="std::inalid_argument : 不明な変数の型"></exception>
	/// <exception cref="std::out_of_range : クラスCppAddress_CCFileCoordinateに対応Coordinateを見つけることが出来なかった"></exception>
	/// <exception cref="その他 : std::make_unique由来の例外"></exception>
	/// <returns></returns>
	static std::unique_ptr<StringsForOutputFileA> make(std::initializer_list<LibFlag::Coordinate> List_FlagCoordinate, const type_char Separator);

	/// <summary>
	/// <para>クラス StringsForOutputFileA を作成する</para><para>事前にCppAddress_CCFileCoordinateとCurrentDAqInfoの設定をしておくこと</para>
	/// </summary>
	/// <param name="BitFlags_Coordinate"> : Flag_Coordinate値を桁の位置としたCoordinateのビットフラグ</param>
	/// <param name="Separator"> : 記録するときの要素のセパレータ文字</param>
	/// <exception cref="std::inalid_argument : 不明な変数の型"></exception>
	/// <exception cref="std::out_of_range : クラスCppAddress_CCFileCoordinateに対応Coordinateを見つけることが出来なかった"></exception>
	/// <exception cref="その他 : std::make_unique由来の例外"></exception>
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
//	/// ファイル冒頭に記述する情報を取得する  エラーがあったらその行をメッセージで置き換える
//	/// </summary>
//	/// <param name="Dst"></param>
//	/// <exception cref="なし"></exception>
//	/// <returns><para>0 : 成功</para><para>bit0 : 日時取得に失敗</para><para>bit1 : ソースLMF名取得に失敗</para></returns>
//	virtual __int32 GetHeaderString(tstring& Dst)const noexcept = 0;
//
//	/// <summary>
//	/// Coordinate名称の列を取得する
//	/// </summary>
//	/// <param name="Dst"></param>
//	/// <exception cref="なし"></exception>
//	/// <returns><para>0</para></returns>
//	virtual __int32 GetCoordinatesString(tstring& Dst)const noexcept = 0;
//
//	/// <summary>
//	/// Coordinateに対応する数値を取得する  エラーがあったら引数に何もしない
//	/// </summary>
//	/// <param name="Dst"></param>
//	/// <exception cref="なし"></exception>
//	/// <returns><para>0 : 成功</para><para>1 : map::atで失敗</para><para>2 : コンストラクタでの型設定失敗</para><para>3 : その他</para></returns>
//	virtual __int32 GetValuesString(tstring& Dst)noexcept = 0;
//
//	/// <summary>
//	/// <para>クラス StringsForOutputFileW を作成する</para><para>事前にCppAddress_CCFileCoordinateとCurrentDAqInfoの設定をしておくこと</para>
//	/// </summary>
//	/// <param name="List_FlagCoordinate"> : 順に指定された記録したいCoordinateに対応するFlag_Coordinate</param>
//	/// <param name="Separator"> : 記録するときの要素のセパレータ文字</param>
//	/// <exception cref="std::inalid_argument : 不明な変数の型"></exception>
//	/// <exception cref="std::out_of_range : クラスCppAddress_CCFileCoordinateに対応Coordinateを見つけることが出来なかった"></exception>
//	/// <exception cref="その他 : std::make_unique由来の例外"></exception>
//	/// <returns></returns>
//	static std::unique_ptr<StringsForOutputFileW> make(std::initializer_list<StringsForOutputFile_Flags::Flag_Coordinate> List_FlagCoordinate, const type_char Separator);
//
//	/// <summary>
//	/// <para>クラス StringsForOutputFileW を作成する</para><para>事前にCppAddress_CCFileCoordinateとCurrentDAqInfoの設定をしておくこと</para>
//	/// </summary>
//	/// <param name="BitFlags_Coordinate"> : Flag_Coordinate値を桁の位置としたCoordinateのビットフラグ</param>
//	/// <param name="Separator"> : 記録するときの要素のセパレータ文字</param>
//	/// <exception cref="std::inalid_argument : 不明な変数の型"></exception>
//	/// <exception cref="std::out_of_range : クラスCppAddress_CCFileCoordinateに対応Coordinateを見つけることが出来なかった"></exception>
//	/// <exception cref="その他 : std::make_unique由来の例外"></exception>
//	/// <returns></returns>
//	static std::unique_ptr<StringsForOutputFileW> make(const type_Flag BitFlags_Coordinate, const type_char Separator);
//
//};
class StringsForOutputFileW;//未実装


#ifdef UNICODE
#define StringsForOutputFile StringsForOutputFileW
#else
#define StringsForOutputFile StringsForOutputFileA
#endif // UNICODE


#define make_StringsForOutputFile StringsForOutputFile::make




//Igorが読み込むファイル用
class StringsForIgorTextWaveA {
public:
	using type_char = char;
	using tstring = std::basic_string<type_char>;


	/// <summary>
	/// Igor テキストファイル用の文字列を取得する　数値用ウェーブと軸用ウェーブの合計二つを返す
	/// </summary>
	/// <param name="Dst"> : 書き込み先</param>
	/// <param name="Name_ValueWave"> : 数値ウェーブの名称</param>
	/// <param name="Name_ScaleWave"> : x 軸用ウェーブの名称</param>
	/// <param name="Vec"> : 一次元数値ベクトル</param>
	/// <param name="ScaleInfo"> : x 軸の情報</param>
	/// <exception cref="std::invalid_argument"></exception>
	/// <exception cref="std::out_of_range"></exception>
	/// <exception cref="std::exception"></exception>
	static void WaveText(tstring& Dst, const tstring& Name_ValueWave, const tstring& Name_ScaleWave, const std::vector<double>& Vec, const ScaleInformation& ScaleInfo);


	/// <summary>
	/// Igor テキストファイル用の文字列を取得する　数値用ウェーブと軸用ウェーブ二つの合計三つを返す
	/// </summary>
	/// <param name="Dst"> : 書き込み先</param>
	/// <param name="Name_ValueWave"> : 二次元数値ウェーブの名称</param>
	/// <param name="Name_ScaleWave0"> : x 軸用ウェーブの名称</param>
	/// <param name="Name_ScaleWave1"> : y 軸用ウェーブの名称</param>
	/// <param name="Vec"> : 二次元数値ベクトル</param>
	/// <param name="ScaleInfo0"> : x 軸の情報</param>
	/// <param name="ScaleInfo1"> : y 軸の情報</param>
	/// <exception cref="std::invalid_argument"></exception>
	/// <exception cref="std::out_of_range"></exception>
	/// <exception cref="std::exception"></exception>
	static void WaveText(tstring& Dst, const tstring& Name_ValueWave, const tstring& Name_ScaleWave0, const tstring& Name_ScaleWave1, const std::vector<std::vector<double>>& Vec, const ScaleInformation& ScaleInfo0, const ScaleInformation& ScaleInfo1);

};

class StringsForIgorTextWaveW {
public:
	using type_char = wchar_t;
	using tstring = std::basic_string<type_char>;


	//未実装　std::exception
	static __int32 WaveText(tstring& Dst, const tstring& Name_ValueWave, const tstring& Name_ScaleWave, const std::vector<double>& Vec, const ScaleInformation& ScaleInfo);

	//未実装　std::exception
	static __int32 WaveText(tstring& Dst, const tstring& Name_ValueWave, const tstring& Name_ScaleWave0, const tstring& Name_ScaleWave1, const std::vector<std::vector<double>>& Vec, const ScaleInformation& ScaleInfo0, const ScaleInformation& ScaleInfo1);


};



#ifdef UNICODE
#define StringsForIgorTextWave StringsForIgorTextWaveW
#else
#define StringsForIgorTextWave StringsForIgorTextWaveA
#endif // UNICODE
