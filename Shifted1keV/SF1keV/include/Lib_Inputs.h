#pragma once
#include <string>
#include <stdexcept>


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
	//追加したらMaxを書き換えて，CppAdress_CCFileCoordinate::SetPrでグローバル変数アドレスと対応Coordinate文字列を登録する
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
		KineticEnergy,

	};
	//追加したらこれを更新
	static constexpr type_Flag Coordinate_Min = 0;
	static constexpr type_Flag Coordinate_Max = 9;
	//enumの要素を文字列へ変換する
	static __int32 Coordinate_ToString(std::basic_string<char>& Dst, const type_Flag FlagValue, const Coordinate Default = static_cast<Coordinate>(0U))noexcept;



	//外部ファイルへ書き込みたいイベントをフィルタリングするConditionを数値で一つ選択
	//CCFのCondition文字列と一致させる
	//追加したらMaxを書き換える
	enum class Condition :type_Flag
	{
		always,
		clean,
		mclean,
		mcleanT,
		mcleanT_elastic,

		WithIR,
		WithoutIR,
		
		mcleanT_WithIR,
		mcleanT_WithoutIR,

		KESp2,
		KESp1,
		KES0,
		KESn1,
		KESn2,

		KESp2R,
		KESp1R,
		KES0R,
		KESn1R,
		KESn2R,

		mcleanT_KESp2R,
		mcleanT_KESp1R,
		mcleanT_KES0R,
		mcleanT_KESn1R,
		mcleanT_KESn2R,

		mcleanT_WithIR_KESp2R,
		mcleanT_WithIR_KESp1R,
		mcleanT_WithIR_KES0R,
		mcleanT_WithIR_KESn1R,
		mcleanT_WithIR_KESn2R,

		mcleanT_WithoutIR_KESp2R,
		mcleanT_WithoutIR_KESp1R,
		mcleanT_WithoutIR_KES0R,
		mcleanT_WithoutIR_KESn1R,
		mcleanT_WithoutIR_KESn2R,
	};
	//追加したらこれを変更
	static constexpr type_Flag Condition_Min = 0;
	static constexpr type_Flag Condition_Max = 33;
	//enumの要素を文字列へ変換する
	static __int32 Condition_ToString(std::basic_string<char>& Dst, const type_Flag FlagValue, const Condition Default = Condition::always)noexcept;



	//解析対象の試料を数値で一つ選択
	//追加したらElastic32_FittedByDblGaussにフィット結果のパラメータを追加する
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
	enum class AzimuthFormat :type_Flag
	{
		Rad,//RAD [-pi..pi]
		Rad0,//RAD [0..2pi]
		Deg,//DEG [-180..180]
		Deg0,//DEG [0..360]
	};
	//範囲を取得する
	static std::pair<double, double> AzimuthRange(const AzimuthFormat Format);
	//範囲内へ収まるようにシフトする
	static double AzimuthIntoRnge(const double Azimuth, const AzimuthFormat Format);
	//フォーマットを変更する
	static double ConvertAzimuthFormat(const double Azimuth, const LibFlag::AzimuthFormat Format_Before, const LibFlag::AzimuthFormat Format_After);
	//r,phiを取得する
	static void OrthoToRPhi(double dX, double dY, __int32 iAFormat, double dCX, double dCY, double& dR, double& dPhi);

};


//ファイルパスなどのパラメータ
class LibPrm {
public:
	//DAq情報を一時的に保存するファイル  CurrentDAqInfoにて使用
	static constexpr char DAqInfo_FilePath[] = "C:\\Program Files\\RoentDek Handels GmbH\\CoboldPC 2011 R5-2-x64 (Visual Studio .Net 2010 Compilation) V10.1.1412.2\\LAES2_TDC8PCI2_HEX\\Cobold_Shared2\\temp\\DAqinfo.dat";
	//上記にファイルへ書き込まれるフラグ
	static constexpr char DAqInfo_COBOLD_online = '1';
	static constexpr char DAqInfo_COBOLD_offline = '0';
	//ハードウェア取り込みの場合上記一時ファイルへ書き込む文字列
	static constexpr char SourceLMFilePath_InCase_Hardwave[] = "HARDWARE";




	//イベントごとのCoordinate値が数表で出力される
	static constexpr char WriteForEachEvents_FilePath[] = "C:\\Program Files\\RoentDek Handels GmbH\\CoboldPC 2011 R5-2-x64 (Visual Studio .Net 2010 Compilation) V10.1.1412.2\\LAES2_TDC8PCI2_HEX\\Cobold_Shared2\\temp\\Events.txt";
	//イベントごとのCoordinate値が数表で出力される
	static constexpr char WriteForEachEvents_FilePath1[] = "C:\\Program Files\\RoentDek Handels GmbH\\CoboldPC 2011 R5-2-x64 (Visual Studio .Net 2010 Compilation) V10.1.1412.2\\LAES2_TDC8PCI2_HEX\\Cobold_Shared2\\temp\\Events1.txt";
	//イベントごとのCoordinate値が数表で出力される
	static constexpr char WriteForEachEvents_FilePath2[] = "C:\\Program Files\\RoentDek Handels GmbH\\CoboldPC 2011 R5-2-x64 (Visual Studio .Net 2010 Compilation) V10.1.1412.2\\LAES2_TDC8PCI2_HEX\\Cobold_Shared2\\temp\\Events2.txt";
	//イベントごとのCoordinate値が数表で出力される
	static constexpr char WriteForEachEvents_FilePath3[] = "C:\\Program Files\\RoentDek Handels GmbH\\CoboldPC 2011 R5-2-x64 (Visual Studio .Net 2010 Compilation) V10.1.1412.2\\LAES2_TDC8PCI2_HEX\\Cobold_Shared2\\temp\\Events3.txt";
	//イベントごとのCoordinate値が数表で出力される
	static constexpr char WriteForEachEvents_FilePath4[] = "C:\\Program Files\\RoentDek Handels GmbH\\CoboldPC 2011 R5-2-x64 (Visual Studio .Net 2010 Compilation) V10.1.1412.2\\LAES2_TDC8PCI2_HEX\\Cobold_Shared2\\temp\\Events4.txt";



	//DCSヒストグラムがIgor Text 形式で出力される
	static constexpr char WriteDCSHisto_FilePath[] = "C:\\Program Files\\RoentDek Handels GmbH\\CoboldPC 2011 R5-2-x64 (Visual Studio .Net 2010 Compilation) V10.1.1412.2\\LAES2_TDC8PCI2_HEX\\Cobold_Shared2\\temp\\DCSHisto.itx";
	//DCSヒストグラムがIgor Text 形式で出力される
	static constexpr char WriteDCSHisto_FilePath1[] = "C:\\Program Files\\RoentDek Handels GmbH\\CoboldPC 2011 R5-2-x64 (Visual Studio .Net 2010 Compilation) V10.1.1412.2\\LAES2_TDC8PCI2_HEX\\Cobold_Shared2\\temp\\DCSHisto1.itx";
	//DCSヒストグラムがIgor Text 形式で出力される
	static constexpr char WriteDCSHisto_FilePath2[] = "C:\\Program Files\\RoentDek Handels GmbH\\CoboldPC 2011 R5-2-x64 (Visual Studio .Net 2010 Compilation) V10.1.1412.2\\LAES2_TDC8PCI2_HEX\\Cobold_Shared2\\temp\\DCSHisto2.itx";
	//DCSヒストグラムがIgor Text 形式で出力される
	static constexpr char WriteDCSHisto_FilePath3[] = "C:\\Program Files\\RoentDek Handels GmbH\\CoboldPC 2011 R5-2-x64 (Visual Studio .Net 2010 Compilation) V10.1.1412.2\\LAES2_TDC8PCI2_HEX\\Cobold_Shared2\\temp\\DCSHisto3.itx";
	//DCSヒストグラムがIgor Text 形式で出力される
	static constexpr char WriteDCSHisto_FilePath4[] = "C:\\Program Files\\RoentDek Handels GmbH\\CoboldPC 2011 R5-2-x64 (Visual Studio .Net 2010 Compilation) V10.1.1412.2\\LAES2_TDC8PCI2_HEX\\Cobold_Shared2\\temp\\DCSHisto4.itx";


	//この ccf ファイルからConditionを読み込む
	static constexpr char ReadConditionsFromCCF_FilePath[] = "C:\\Program Files\\RoentDek Handels GmbH\\CoboldPC 2011 R5-2-x64 (Visual Studio .Net 2010 Compilation) V10.1.1412.2\\LAES2_TDC8PCI2_HEX\\Cobold_Shared2\\Shifted1keV\\200918\\DAn_Condition.ccf";


};




//Elastic32による弾性散乱のDCSをDblGauss関数
//y=f1(x)+f2(x), f(x)=y0 + A*exp(-((x-x0)/width)^2)
//によってフィッテイングした結果の係数
class Elastic32_FittedByDblGauss {
public:

	//ヘリウム原子, 1000 eV
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

	//炭素原子, 1000 eV
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

	//窒素原子, 1000 eV
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

	//ネオン原子, 1000 eV
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

	//塩素原子, 1000 eV
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

	//アルゴン原子, 1000 eV
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

	//キセノン原子, 1000 eV
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
////ヘリウム原子
////1000 eV, 弾性散乱
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
////炭素原子
////1000 eV, 弾性散乱
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
////窒素原子
////1000 eV, 弾性散乱
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
////ネオン原子
////1000 eV, 弾性散乱
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
////塩素原子
////1000 eV, 弾性散乱
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
////アルゴン原子
////1000 eV, 弾性散乱
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
////キセノン原子
////1000 eV, 弾性散乱
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



//スキマーによる像の削れ方からPhi_Offsetはほぼゼロ
//校正プレート像の回転はILの漏れ磁場による


//散乱角度への変換に使用する二次多項式の係数
//データは e19111401
//出射エネルギー 1000 eV
struct Parameters_ConversionToDCS_e19111401 {
	static constexpr double K0 = 0;//散乱極角 Theta_deg から検出位置中心からの距離 R_mm への変換二次多項式 R_mm = F(theta_deg) の定数項
	static constexpr double K1 = 0.834573;//散乱極角 Theta_deg から検出位置中心からの距離 R_mm への変換二次多項式 R_mm = F(theta_deg) の第一係数
	static constexpr double K2 = 0.0405143;//散乱極角 Theta_deg から検出位置中心からの距離 R_mm への変換二次多項式 R_mm = F(theta_deg) の第二係数

	static constexpr double Phi_Offset = 0;//検出器中心に関する方位角 phi からこれを引くと散乱方位角 varphi が求められる
	static constexpr LibFlag::AzimuthFormat Format_Phi_Offset = LibFlag::AzimuthFormat::Deg;
};

//e19111402
//出射エネルギー 1000 + 1.55 eV
struct Parameters_ConversionToDCS_e19111402 {
	static constexpr double K0 = 0;//散乱極角 Theta_deg から検出位置中心からの距離 R_mm への変換二次多項式 R_mm = F(theta_deg) の定数項
	static constexpr double K1 = 0.67572;//散乱極角 Theta_deg から検出位置中心からの距離 R_mm への変換二次多項式 R_mm = F(theta_deg) の第一係数
	static constexpr double K2 = 0.04682;//散乱極角 Theta_deg から検出位置中心からの距離 R_mm への変換二次多項式 R_mm = F(theta_deg) の第二係数

	static constexpr double Phi_Offset = 0;//検出器中心に関する方位角 phi からこれを引くと散乱方位角 varphi が求められる
	static constexpr LibFlag::AzimuthFormat Format_Phi_Offset = LibFlag::AzimuthFormat::Deg;
};


//e19111402
//出射エネルギー 1000 + 3.10 eV
struct Parameters_ConversionToDCS_e19111403 {
	static constexpr double K0 = 0;//散乱極角 Theta_deg から検出位置中心からの距離 R_mm への変換二次多項式 R_mm = F(theta_deg) の定数項
	static constexpr double K1 = 0.5073231279348395;//散乱極角 Theta_deg から検出位置中心からの距離 R_mm への変換二次多項式 R_mm = F(theta_deg) の第一係数
	static constexpr double K2 = 0.05529602236903003;//散乱極角 Theta_deg から検出位置中心からの距離 R_mm への変換二次多項式 R_mm = F(theta_deg) の第二係数

	static constexpr double Phi_Offset = 0;//検出器中心に関する方位角 phi からこれを引くと散乱方位角 varphi が求められる
	static constexpr LibFlag::AzimuthFormat Format_Phi_Offset = LibFlag::AzimuthFormat::Deg;
};





//エネルギーへの変換に使用する二次多項式の係数分布
//データは e191121
//解析は e200918.pxp
class CoefDist_TOFNoOffset2KES_191121 {
public:

	//入射エネルギー [eV]
	static constexpr double KE0 = 1000;

	//係数計算時の bin 情報
	static constexpr size_t DimSizeX = 161;
	static constexpr double DimDeltaX = 0.5;
	static constexpr double DimOffsetX = -40;

	static constexpr size_t DimSizeY = 161;
	static constexpr double DimDeltaY = 0.5;
	static constexpr double DimOffsetY = -40;

	
	//検出位置 (PosX, PosY) の長方形 bin ごとに遅延発生器によるトリガ遅れを除いた飛行時間 TOF_NoOffset を返値，運動エネルギーの差 KES = KE - KE0 を引数にした二次多項式 TOF_NoOffset = f(KES)
	//を回帰分析によって求めたときの各係数の位置分布．最近接四ピクセル数値による補間値を返す
	// K0 が第ゼロ係数，K0s がフィッティング誤差など．
	//定義域の範囲外で例外 std::out_of_range を送出する

	static double K0(const double PosX, const double PosY);
	static double K0s(const double PosX, const double PosY);

	static double K1(const double PosX, const double PosY);
	static double K1s(const double PosX, const double PosY);
	
	static double K2(const double PosX, const double PosY);
	static double K2s(const double PosX, const double PosY);

};




//検出器感度補正に使用するデータ
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


	//r,phiを返す
	static std::tuple<double, double> Event(const size_t EventIndex);


};



