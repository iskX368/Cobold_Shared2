#pragma once
#include <string>


#define _USE_MATH_DEFINES
#include <math.h>



//よく使うグローバル変数

static constexpr double gv_deg2rad = M_PI / 180;
static constexpr double gv_rad2deg = 180 / M_PI;



//ライブラリで使うフラグのまとめ
class LibFlag {
public:
	//非負
	using type_Flag = unsigned;


	//外部ファイルへ書き込みたいイベントのCoordinateをビットフラグで複数選択
	//CCFのCoordinate文字列と一致させる
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
	//追加したらこれを更新
	static constexpr type_Flag Coordinate_Min = 0;
	static constexpr type_Flag Coordinate_Max = 8;
	//enumの要素を文字列へ変換する
	static __int32 Coordinate_ToString(std::basic_string<char>& Dst, const type_Flag FlagValue, const Coordinate Default = static_cast<Coordinate>(0U))noexcept;



	//外部ファイルへ書き込みたいイベントをフィルタリングするConditionを数値で一つ選択
	//CCFのCondition文字列と一致させる
	enum class Condition :type_Flag
	{
		always,
		clean,
		mclean,
		mcleanT,
		mcleanT_elastic,
	};
	//追加したらこれを変更
	static constexpr type_Flag Condition_Min = 0;
	static constexpr type_Flag Condition_Max = 4;
	//enumの要素を文字列へ変換する
	static __int32 Condition_ToString(std::basic_string<char>& Dst, const type_Flag FlagValue, const Condition Default = Condition::always)noexcept;



	//解析対象の試料を数値で一つ選択
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



	//方位角のフォーマット
	//Parameter1003と設定を揃えること
	enum class AzimuthFormat :type_Flag
	{
		Rad,//RAD [-pi..pi]
		Rad0,//RAD [0..2pi]
		Deg,//DEG [-180..180]
		Deg0,//DEG [0..360]
	};
	


};


//ファイルパスなどのパラメータ
class LibPrm {
public:
	//CurrentDAqInfoにて使用
	static constexpr char DAqInfo_FilePath[] = "C:\\Program Files\\RoentDek Handels GmbH\\CoboldPC 2011 R5-2-x64 (Visual Studio .Net 2010 Compilation) V10.1.1412.2\\LAES2_TDC8PCI2_HEX\\Cobold_Shared1\\temp\\DAqinfo.dat";
	static constexpr char DAqInfo_COBOLD_online = '1';
	static constexpr char DAqInfo_COBOLD_offline = '0';


	//このファイルへイベントごとのCoordinate値を書き込む
	static constexpr char WriteForEachEvents_FilePath[] = "C:\\Program Files\\RoentDek Handels GmbH\\CoboldPC 2011 R5-2-x64 (Visual Studio .Net 2010 Compilation) V10.1.1412.2\\LAES2_TDC8PCI2_HEX\\Cobold_Shared1\\temp\\Events.txt";


	//このファイルからConditionを読み込む
	static constexpr char ReadConditionsFromCCF_FilePath[] = "C:\\Program Files\\RoentDek Handels GmbH\\CoboldPC 2011 R5-2-x64 (Visual Studio .Net 2010 Compilation) V10.1.1412.2\\LAES2_TDC8PCI2_HEX\\Cobold_Shared1\\Elastic1keV\\200313\\DAn_Condition.ccf";





};

