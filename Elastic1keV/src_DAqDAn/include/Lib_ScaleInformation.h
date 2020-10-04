#pragma once

#include <memory>
#include <stdexcept>
#include <functional>
#include <vector>
#include <tuple>


//--------------------------------------------------------------------------------------------------------------------------------------------------------
//不均一なグラフ目盛りを扱うためのクラス
//PointInformation クラスによって，ゼロ始まり整数インデックスと区間端の開閉を指定する
//ScaleInformation クラスに PointInformation と整数インデックスと目盛り数値との対応関数 f を渡して，目盛り数値 f(0), f(1), ... とその対応区間を作成する
//共通の PointInformation を渡すことで，合成関数など複数パラメータでbinの区間端の扱いを統一したい場合に対応する
//--------------------------------------------------------------------------------------------------------------------------------------------------------







//区間の開閉 { = ( or [, } = ) or ] として
//I_n = { k, k+1 } |-> F_n = { f(k), f(k+1) }
//の対応において
//
//1. 単位幅をもつ : I_n(1) - I_n(0) == 1
//2. 端で繋がっている : I_(n+1)(0) == I_n(1)
//
//そのような区間の集合 I_n (n=0, ... , N-1) を作る
//各区間 I_n はそれぞれ区間代表値 n をもつ
struct PointInformation
{
	virtual ~PointInformation();

	enum class Difference {
		Forward,	//	前進差分 : I_n = [ n , n-1 ) ( n = 0, ... ,N-1 )
		Backward,	//	後退差分 : I_n = ( n-1 , n ] ( n = 0, ... ,N-1 )
		Central0,	//	中心差分その０ : I_n = [ n-1/2 , n+1/2 ) ( n = 0, ... ,N-1 )
		Central1,	//	中心差分その１ : I_n = ( n-1/2 , n+1/2 ] ( n = 0, ... ,N-1 )

		Central2,	//	中心差分その２ : I_n = [ n-1/2 , n+1/2 ] ( n = 0, ... ,N-1 )

	};

	/// <summary>
	/// ポイントの個数
	/// </summary>
	///<exception cref="なし"></exception>
	/// <returns></returns>
	virtual size_t NumPoints()const noexcept = 0;

	/// <summary>
	/// 区間始まりが含まれいているのならtrue
	/// </summary>
	///<exception cref="なし"></exception>
	/// <returns></returns>
	virtual bool IsClosed0()const noexcept = 0;

	/// <summary>
	/// 区間終わりが含まれいているのならtrue
	/// </summary>
	///<exception cref="なし"></exception>
	/// <returns></returns>
	virtual bool IsClosed1()const noexcept = 0;

	/// <summary>
	/// 登録した差分の種類
	/// </summary>
	///<exception cref="なし"></exception>
	/// <returns></returns>
	virtual Difference Enum_Difference()const noexcept = 0;

	/// <summary>
	/// 区間代表値から一致する区間 I を取得する
	/// </summary>
	/// <param name="Point"></param>
	/// <exception cref = "std::out_of_range"></exception>
	/// <returns></returns>
	virtual std::pair<double, double> Point2PointInterval(const size_t Point)const = 0;


	/// <summary>
	/// 全区間の始まりを返す
	/// </summary>
	/// <exception cref = "std::out_of_range"></exception>
	/// <returns></returns>
	virtual double PointRange0()const = 0;

	/// <summary>
	/// 全区間の終わりを返す
	/// </summary>
	/// <exception cref = "std::out_of_range"></exception>
	/// <returns></returns>
	virtual double PointRange1()const = 0;


	/// <summary>
	/// 区間代表値 0, 1, ..., (NumberOfPoints-1) から差分 dif に従って I 区間の集合クラス PointInformation への shared_ptr を作成する
	/// </summary>
	/// <param name="NumberOfPoints"> : 区間（代表ポイント）の個数</param>
	/// <param name="dif"> : 差分</param>
	/// <exception cref = "std::invalid_argument"></exception>
	/// <exception cref = "std::exception"></exception>
	/// <returns></returns>
	static std::shared_ptr<PointInformation> make(const size_t NumberOfPoints, PointInformation::Difference dif);
};
bool operator==(const PointInformation& t1, const PointInformation& t2) noexcept;
bool operator!=(const PointInformation& t1, const PointInformation& t2) noexcept;

//エントリーポイント
#define make_PointInformation PointInformation::make





//区間の開閉 { = ( or [, } = ) or ] として
//I_n = { p, p+1 } |-> F_n = { f(p), f(p+1) }
//の対応において，返り値の区間の集合 F_n (n=0, ... , N-1) を作る
//f(p) < f(p+1) であるとは限らない
//全ての n について向きが一致した不等号である必要がある
//各区間 F_n はそれぞれ区間代表値 f(n) をもつ
struct ScaleInformation {
	virtual ~ScaleInformation();


	/// <summary>
	/// I 区間集合クラスへのポインタを取得する
	/// </summary>
	///<exception cref = "std::invalid_argument"></exception>
	/// <returns></returns>
	virtual const std::shared_ptr<PointInformation>& PointInfo()const = 0;


	/// <summary>
	/// 共通の I 区間集合クラスをもつならtrue, そうでないならfalse
	/// </summary>
	/// <param name="scaleInformation"></param>
	///<exception cref = "なし"></exception>
	/// <returns></returns>
	virtual bool HasCommonPointInfo(const ScaleInformation& scaleInformation)const noexcept = 0;


	/// <summary>
	/// 共通の I 区間をもつ目盛りクラスの対応代表値を求める
	/// </summary>
	/// <param name="another_ScaleInformation_Sharing_PntInfo"></param>
	/// <param name="Xvalue_Of_ThisScale"></param>
	///<exception cref = "std::invalid_argument"></exception>
	///<exception cref = "std::out_of_range"></exception>
	/// <returns></returns>
	virtual double Convert2AnotherRepresentative(const ScaleInformation& another_ScaleInformation_Sharing_PntInfo, const double Xvalue_Of_ThisScale)const = 0;


	/// <summary>
	/// /F 区間の始端を含むならtrue
	/// </summary>
	/// <exception cref = "なし"></exception>
	/// <returns></returns>
	virtual bool IsClosed0()const noexcept = 0;


	/// <summary>
	/// F 区間の終端を含むならtrue
	/// </summary>
	/// <exception cref = "なし"></exception>
	/// <returns></returns>
	virtual bool IsClosed1()const noexcept = 0;


	/// <summary>
	/// <para>狭義単調増加ならtrue，狭義単調減少ならfalse</para>
	/// <para>狭義でないケースは事前にはじいている</para>
	/// </summary>
	/// <exception cref = "なし"></exception>
	/// <returns></returns>
	virtual bool IsMonotonicallyIncreasing()const noexcept = 0;


	/// <summary>
	/// F 区間の個数を取得する
	/// </summary>
	///<exception cref="なし"></exception>
	virtual size_t NumIntervals()const noexcept = 0;


	/// <summary>
	/// F 区間を取得する
	/// </summary>
	///<exception cref="std::out_of_range">区間インデックスが範囲外</exception>
	/// <param name="Interval_Index">区間インデックス : [0, NumIntervals ) </param>
	/// <returns>区間始まりを first, 区間終わりを second とする pair</returns>
	virtual const std::pair<double, double>& Interval(const size_t Interval_Index)const = 0;


	/// <summary>
	/// F 区間の区間終わりと区間始まりの差を取得する
	/// </summary>
	///<exception cref="std::out_of_range">区間インデックスが範囲外</exception>
	/// <param name="Interval_Index">区間インデックス : [0, NumIntervals ) </param>
	/// <returns>（区間終わり）-（区間始まり）</returns>
	virtual double IntervalWidth(const size_t Interval_Index)const = 0;


	/// <summary>
	/// 全区間がカバーする範囲の始まりと終わりを取得する
	/// </summary>
	///<exception cref="std::out_of_range">コンストラクタエラーでRangeが生成されなかった</exception>
	/// <returns>範囲始まりを first, 範囲終わりを second とする pair</returns>
	virtual const std::pair<double, double>& Range()const = 0;


	/// <summary>
	/// 実数 Xvalue が全 F 区間がカバーする範囲に属しているなら true, そうでないなら false
	/// </summary>
	///<exception cref="なし"></exception>
	virtual bool IsInRange(const double Xvalue)const = 0;


	/// <summary>
	/// 実数 Xvalue が区間インデックス Interval_Index で指定される F 区間に属するなら true, そうでないなら false
	/// </summary>
	/// <param name="Interval_Index">区間インデックス : [0, NumIntervals ) </param>
	///<exception cref="なし"></exception>
	virtual bool IsInInterval(const double Xvalue, const size_t Interval_Index)const = 0;


	/// <summary>
	/// <para>実数 Xvalue が属する F 区間のインデックスを取得する</para>
	/// <para>例外回避のため IsInRange で事前に確認すること</para>
	/// </summary>
	///<exception cref="std::out_of_range">XValue が属する区間が見つからなかった</exception>
	virtual size_t X2IntervalIndex(const double Xvalue)const = 0;


	/// <summary>
	/// <para>実数 Xvalue 属する F 区間が代表する目盛り数値を取得する</para>
	/// <para>例外回避のため IsInRange で事前に確認すること</para>
	/// </summary>
	///<exception cref="std::out_of_range">XValue が属する区間が見つからなかった</exception>
	/// <returns></returns>
	virtual double X2XRepresentative(const double Xvalue)const = 0;


	/// <summary>
	/// <para>区間インデックス Interval_Index から F 区間が代表する目盛り数値を取得する</para>
	/// <para>例外回避のため NumIntervals で事前に確認すること</para>
	/// </summary>
	/// <param name="Interval_Index">  区間インデックス : [0, NumIntervals ) </param>
	///<exception cref="std::out_of_range">区間インデックスが範囲外</exception>
	/// <returns></returns>
	virtual double IntervalIndex2XRepresentative(const size_t Interval_Index)const = 0;



	/// <summary>
	/// ScaleInformationを作成する
	/// </summary>
	/// <param name="sppointInformation"> : I 区間の集合へのshared_ptr．共有カウンタを１つ増やす</param>
	/// <param name="f"><para> : I 区間から F 区間へ対応させる関数．PointInformation の全範囲にわたって狭義単調増加または狭義単調減少でなければならない</para><para>   中心差分を使用する場合は線形な関数とすること</para></param>
	/// <exception cref = "std::invalid_argument"></exception>
	/// <exception cref = "std::out_of_range"></exception>
	/// <exception cref = "std::exception"></exception>
	/// <returns></returns>
	static std::unique_ptr<ScaleInformation> make(const std::shared_ptr<PointInformation>& sppointInformation, const std::function<double(const double)>& f);


	/// <summary>
	/// ScaleInformationを作成する
	/// </summary>
	/// <param name="sppointInformation"> : I 区間の集合へのshared_ptr．共有カウンタを１つ増やす</param>
	/// <param name="Pnt2LinearScale_Ratio"> : I 区間から F 区間へ対応させる線形関数の傾き</param>
	/// <param name="Pnt2LinearScale_PntOffset"> : I 区間から F 区間へ対応させる線形関数のオフセット</param>
	/// <exception cref = "std::invalid_argument"></exception>
	/// <exception cref = "std::out_of_range"></exception>
	/// <exception cref = "std::exception"></exception>
	/// <returns></returns>
	static std::unique_ptr<ScaleInformation> make(const std::shared_ptr<PointInformation>& sppointInformation, const double Pnt2LinearScale_Ratio = 1.0, const double Pnt2LinearScale_PntOffset = 0.0);

};
bool operator==(const ScaleInformation& t1, const ScaleInformation& t2);
bool operator!=(const ScaleInformation& t1, const ScaleInformation& t2);


#define make_ScaleInformation ScaleInformation::make





//
//class ScaledHistogram1D {
//public:
//
//	virtual ~ScaledHistogram1D();
//
//	/// <summary>
//	/// ヒストグラムに数値を加算する
//	/// </summary>
//	/// <param name="Xvalue"> : 加算する横軸の数値  最も近い目盛り値が選ばれる</param>
//	/// <param name="Weight"> : 加算する量  これを重み割り算関数で割った値が加えられる</param>
//	/// <exception cref="std::out_of_range"></exception>
//	/// <exception cref="std::exception"></exception>
//	/// <returns></returns>
//	virtual void AddValue(const double Xvalue, const double Weight = 1.0) = 0;
//
//
//	/// <summary>
//	/// ヒストグラムに数値をそのままセットする
//	/// </summary>
//	/// <param name="Xvalue"> : 加算する横軸の数値  最も近い目盛り値が選ばれる</param>
//	/// <param name="Intensity"> : セットする値</param>
//	/// <exception cref="std::out_of_range"></exception>
//	/// <exception cref="std::exception"></exception>
//	/// <returns></returns>
//	virtual void SetValue(const double Xvalue, const double Intensity) = 0;
//
//
//	/// <summary>
//	/// 一次元ヒストグラムの数値をすべてゼロにする
//	/// </summary>
//	/// <returns></returns>
//	virtual void ClearHistogram() noexcept = 0;
//
//	/// <summary>
//	///	一次元ヒストグラムを取得する
//	/// </summary>
//	/// <returns></returns>
//	virtual const std::vector<double>& Get() const noexcept = 0;
//
//	
//	virtual double GetAt(const double Xvalue) const = 0;
//
//	virtual double GetAt(const size_t Pnt) const = 0;
//
//
//
//	/// <summary>
//	/// クラス ScaleInformation を取得する
//	/// </summary>
//	/// <returns></returns>
//	virtual const ScaleInformation& ScaleInfo()const noexcept= 0;
//
//	/// <summary>
//	/// クラス ScaledHistogram1D を作成する
//	/// </summary>
//	/// <param name="sppointInformation">クラス PointInformation のshared_ptr</param>
//	/// <param name="f_Pnt2HorizontalScale"> : ゼロ以上の整数（ポイント数）から横軸目盛り値への変換関数</param>
//	/// <param name="f_WeightDivisionFactor"> : ヒストグラム加算量 (Weight) はこの関数値で割られた後に加えられる</param>
//	/// <exception cref="std::invalid_argument"></exception>
//	/// <exception cref="std::out_of_range"></exception>
//	/// <exception cref = "Exceptions from std::make_unique"></exception>
//	/// <returns></returns>
//	static std::unique_ptr<ScaledHistogram1D> make(const std::shared_ptr<PointInformation>& sppointInformation, std::function<double(const double)> f_Pnt2HorizontalScale, std::function<double(const double)> f_WeightDivisionFactor);
//	
//	/// <summary>
//	/// <para>クラス ScaledHistogram1D を作成する</para><para>横軸目盛りがゼロ以上の整数（ポイント数）に対して線形とする</para>
//	/// </summary>
//	/// <param name="sppointInformation">クラス PointInformation のshared_ptr</param>
//	/// <param name="Pnt2LinearScale_Ratio"> : ポイント数から横軸目盛り値への線形変換の傾き</param>
//	/// <param name="Pnt2LinearScale_PntOffset"> : ポイント数から横軸目盛り値への線形変換のポイント数オフセット</param>
//	/// <returns></returns>
//	static std::unique_ptr<ScaledHistogram1D> make(const std::shared_ptr<PointInformation>& sppointInformation, const double Pnt2LinearScale_Ratio = 1.0, const double Pnt2LinearScale_PntOffset = 0.0);
//
//
//};
//
//#define make_ScaledHistogram1D ScaledHistogram1D::make







class ScaledHistogram1D {
public:

	virtual ~ScaledHistogram1D();

	/// <summary>
	/// ヒストグラムに数値を加算する
	/// </summary>
	/// <param name="Xvalue"> : 加算する横軸の数値  最も近い目盛り値が選ばれる</param>
	/// <param name="Weight"> : 加算する量  これを重み割り算関数で割った値が加えられる</param>
	/// <exception cref="std::out_of_range"></exception>
	/// <exception cref="std::exception"></exception>
	/// <returns></returns>
	//virtual void AddValue(const double Xvalue, const double Weight = 1.0) = 0;



	virtual void AddValue(const double Xvalue, const double Weight, const double AbsoluteErrorOfWeight) = 0;


	/// <summary>
	/// 一次元ヒストグラムの数値をすべてゼロにする
	/// </summary>
	/// <returns></returns>
	virtual void ClearHistogram() noexcept = 0;

	//Getする前に実行すること
	//virtual void RefineHistogram() = 0;


	/// <summary>
	///	一次元ヒストグラムを取得する
	/// </summary>
	/// <exception cref="std::out_of_range"></exception>
	/// <returns></returns>
	virtual std::vector<double> Get()const = 0;


	/// <summary>
	/// 一次元ヒストグラムの絶対誤差を取得する
	/// </summary>
	/// <exception cref="std::out_of_range"></exception>
	/// <returns></returns>
	virtual std::vector<double> GetE() const = 0;


	/// <summary>
	/// 一次元ヒストグラムの相対誤差を取得する
	/// </summary>
	/// <exception cref="std::out_of_range"></exception>
	/// <returns></returns>
	virtual std::vector<double> GetE1() const = 0;


	/// <summary>
	/// 一次元ヒストグラムの横軸における bin 幅を取得する　確認用
	/// </summary>
	/// <returns></returns>
	virtual const std::vector<double>& GetWidth()const noexcept = 0;

	/// <summary>
	/// 一次元ヒストグラムの横軸スケールによる修正前の値を取得する　確認用
	/// </summary>
	/// <returns></returns>
	virtual const std::vector<double>& GetRaw()const noexcept = 0;

	/// <summary>
	/// 一次元ヒストグラムの数値を取得する　一番近い目盛りの数値を返す
	/// </summary>
	/// <param name="Xvalue"></param>
	/// <exception cref="std::out_of_range"></exception>
	/// <returns></returns>
	virtual double GetAt(const double Xvalue)const = 0;

	/// <summary>
	/// 一次元ヒストグラムの数値を取得する
	/// </summary>
	/// <param name="Pnt"></param>
	/// <exception cref="std::out_of_range"></exception>
	/// <returns></returns>
	virtual double GetAt(const size_t Pnt)const = 0;

	/// <summary>
	/// 一次元ヒストグラムの絶対誤差を取得する　一番近い目盛りの数値を返す
	/// </summary>
	/// <param name="Xvalue"></param>
	/// <exception cref="std::out_of_range"></exception>
	/// <returns></returns>
	virtual double GetEAt(const double Xvalue)const = 0;

	/// <summary>
	/// 一次元ヒストグラムの絶対誤差を取得する
	/// </summary>
	/// <param name="Pnt"></param>
	/// <exception cref="std::out_of_range"></exception>
	/// <returns></returns>
	virtual double GetEAt(const size_t Pnt)const = 0;

	/// <summary>
	/// 一次元ヒストグラムの絶対誤差を取得する　一番近い目盛りの数値を返す
	/// </summary>
	/// <param name="Xvalue"></param>
	/// <exception cref="std::out_of_range"></exception>
	/// <returns></returns>
	virtual double GetE1At(const double Xvalue)const = 0;

	/// <summary>
	/// 一次元ヒストグラムの相対誤差を取得する
	/// </summary>
	/// <param name="Pnt"></param>
	/// <exception cref="std::out_of_range"></exception>
	/// <returns></returns>
	virtual double GetE1At(const size_t Pnt)const = 0;

	/// <summary>
	/// クラス ScaleInformation を取得する
	/// </summary>
	/// <returns></returns>
	virtual const ScaleInformation& ScaleInfo()const noexcept = 0;

	/// <summary>
	/// 差分で計算
	/// </summary>
	/// <param name="sppointInformation"></param>
	/// <param name="f_Pnt2HorizontalScale"></param>
	/// <returns></returns>
	static std::unique_ptr<ScaledHistogram1D> make(const std::shared_ptr<PointInformation>& sppointInformation, std::function<double(const double)> f_Pnt2HorizontalScale);

	static std::unique_ptr<ScaledHistogram1D> make(const std::shared_ptr<PointInformation>& sppointInformation, const double Pnt2LinearScale_Ratio = 1.0, const double Pnt2LinearScale_PntOffset = 0.0);
};

#define make_ScaledHistogram1D ScaledHistogram1D::make








class ScaledHistogram2D {
public:

	virtual ~ScaledHistogram2D();

	/// <summary>
	/// ヒストグラムに数値を加算する  成功したらゼロを返す
	/// </summary>
	/// <param name="Xvalue"> : 加算する横軸の数値その１  最も近い目盛り値が選ばれる</param>
	/// <param name="Yvalue"> : 加算する横軸の数値その２  最も近い目盛り値が選ばれる</param>
	/// <param name="Weight"> : 加算する量  これを重み割り算関数で割った値が加えられる</param>
	/// <exception cref="std::out_of_range"></exception>
	/// <exception cref="std::exception"></exception>
	/// <returns></returns>
	//virtual void AddValue(const double Xvalue, const double Yvalue, const double Weight = 1.0) = 0;


	virtual void AddValue(const double Xvalue, const double Yvalue, const double Weight, const double AbsoluteErrorOfWeight) = 0;


	/// <summary>
	/// 二次元ヒストグラムの数値をすべてゼロにする
	/// </summary>
	/// <exception cref="なし"></exception>
	/// <returns></returns>
	virtual void ClearHistogram() noexcept = 0;

	/// <summary>
	/// <para>二次元ヒストグラムを取得する
	/// </summary>
	/// <exception cref="std::out_of_range"></exception>
	/// <returns></returns>
	virtual std::vector<std::vector<double>> Get()const = 0;


	/// <summary>
	/// <para>二次元ヒストグラムの絶対誤差を取得する
	/// </summary>
	/// <exception cref="std::out_of_range"></exception>
	/// <returns></returns>
	virtual std::vector<std::vector<double>> GetE()const = 0;

	/// <summary>
	/// <para>二次元ヒストグラムの相対誤差を取得する
	/// </summary>
	/// <exception cref="std::out_of_range"></exception>
	/// <returns></returns>
	virtual std::vector<std::vector<double>> GetE1()const = 0;

	/// <summary>
	/// 二次元ヒストグラムの横軸における bin 幅を取得する　確認用
	/// </summary>
	/// <returns></returns>
	virtual const std::vector<std::vector<double>>& GetWidth()const noexcept = 0;

	/// <summary>
	/// 二次元ヒストグラムの横軸スケールによる修正前の値を取得する　確認用
	/// </summary>
	/// <returns></returns>
	virtual const std::vector<std::vector<double>>& GetRaw()const noexcept = 0;

	/// <summary>
	/// 二次元ヒストグラムの数値を取得する　一番近い目盛りの数値を返す
	/// </summary>
	/// <param name="Xvalue"></param>
	/// <param name="Yvalue"></param>
	/// <exception cref="std::out_of_range"></exception>
	/// <returns></returns>
	virtual double GetAt(const double Xvalue, const double Yvalue)const = 0;

	/// <summary>
	/// 二次元ヒストグラムの数値を取得する
	/// </summary>
	/// <param name="Pnt0"></param>
	/// <param name="Pnt1"></param>
	/// <exception cref="std::out_of_range"></exception>
	/// <returns></returns>
	virtual double GetAt(const size_t Pnt0, const size_t Pnt1)const = 0;

	/// <summary>
	/// 二次元ヒストグラムの絶対誤差を取得する　一番近い目盛りの数値を返す
	/// </summary>
	/// <param name="Xvalue"></param>
	/// <param name="Yvalue"></param>
	/// <exception cref="std::out_of_range"></exception>
	/// <returns></returns>
	virtual double GetEAt(const double Xvalue, const double Yvalue)const = 0;

	/// <summary>
	/// 二次元ヒストグラムの絶対誤差を取得する
	/// </summary>
	/// <param name="Pnt0"></param>
	/// <param name="Pnt1"></param>
	/// <exception cref="std::out_of_range"></exception>
	/// <returns></returns>
	virtual double GetEAt(const size_t Pnt0, const size_t Pnt1)const = 0;

	/// <summary>
	/// 二次元ヒストグラムの相対誤差を取得する　一番近い目盛りの数値を返す
	/// </summary>
	/// <param name="Xvalue"></param>
	/// <param name="Yvalue"></param>
	/// <exception cref="std::out_of_range"></exception>
	/// <returns></returns>
	virtual double GetE1At(const double Xvalue, const double Yvalue)const = 0;

	/// <summary>
	/// 二次元ヒストグラムの相対誤差を取得する
	/// </summary>
	/// <param name="Pnt0"></param>
	/// <param name="Pnt1"></param>
	/// <exception cref="std::out_of_range"></exception>
	/// <returns></returns>
	virtual double GetE1At(const size_t Pnt0, const size_t Pnt1)const = 0;


	/// <summary>
	/// 横軸その０を担当する ScaleInformation を取得する
	/// </summary>
	/// <exception cref="std::exception">返値の実体が存在しない</exception>
	/// <returns></returns>
	virtual const ScaleInformation& ScaleInfo0()const noexcept = 0;

	/// <summary>
	/// 横軸その１を担当する ScaleInformation を取得する
	/// </summary>
	/// <exception cref="std::exception">返値の実体が存在しない</exception>
	/// <returns></returns>
	virtual const ScaleInformation& ScaleInfo1()const noexcept = 0;



	/// <summary>
	/// クラス ScaledHistogram2D を作成する
	/// </summary>
	/// <param name="sppointInformation0"> : 横軸その０を表すクラス PointInformation のshared_ptr</param>
	/// <param name="sppointInformation1"> : 横軸その１を表すクラス PointInformation のshared_ptr</param>
	/// <param name="f_Pnt2HorizontalScale0"> : ゼロ以上の整数（ポイント数）から横軸その０目盛り値への変換関数</param>
	/// <param name="f_Pnt2HorizontalScale1"> : ゼロ以上の整数（ポイント数）から横軸その１目盛り値への変換関数</param>
	/// /// <exception cref="std::invalid_argument"></exception>
	/// <exception cref="std::out_of_range"></exception>
	/// <exception cref = "Exceptions from std::make_unique"></exception>
	/// <returns></returns>
	static std::unique_ptr<ScaledHistogram2D> make(const std::shared_ptr<PointInformation>& sppointInformation0, const std::shared_ptr<PointInformation>& sppointInformation1, std::function<double(const double)> f_Pnt2HorizontalScale0, std::function<double(const double)> f_Pnt2HorizontalScale1);


};


#define make_ScaledHistogram2D ScaledHistogram2D::make