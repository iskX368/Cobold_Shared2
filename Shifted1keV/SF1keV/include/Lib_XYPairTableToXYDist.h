#pragma once


#include <memory>
#include <vector>
#include <stdexcept>


/// <summary>
/// <para>等間隔目盛りの二次元カウント数分布</para>
/// <para>[i][j]の強度は長方形領域 [ x(i)-binx/2 , x(i)+binx/2 ) * [ y(j)-biny/2 , y(j)+biny/2 ) に含まれるカウント数を表す</para>
/// </summary>
class XYPairTableToXYDist
{
public:
	virtual ~XYPairTableToXYDist();

	//二次元分布の要素の型
	using type_Elm_XYDist = size_t;

	//二次元分布の型
	using type_XYDist = std::vector<std::vector<type_Elm_XYDist>>;

	/// <summary>
	/// 二次元分布のカウントをクリアする
	/// </summary>
	/// <returns></returns>
	virtual void ClearCounts() noexcept = 0;

	/// <summary>
	/// 二次元分布を取得する
	/// </summary>
	/// <returns></returns>
	virtual const type_XYDist& Get()const = 0;

	/// <summary>
	/// 二次元分布の特定ポイントでの値を返す
	/// </summary>
	/// <param name="i"> : X方向のポイント数</param>
	/// <param name="j"> : Y方向のポイント数</param>
	/// <exception cref="std::out_of_range"></exception>
	/// <returns>カウント数</returns>
	virtual type_Elm_XYDist At(const size_t i, const size_t j)const = 0;

	/// <summary>
	/// <para>各イベントのXY座標を記録した一次元配列 { X0, Y0, X1, Y1, ..., Xn, Yn } から読み込んで二次元分布へ追加する</para>
	/// <para>例外により中断した場合，データは変更されない</para>
	/// </summary>
	/// <param name="XYPairTable"> : XY座標を記録した一次元配列</param>
	/// <exception cref="std::out_of_range"></exception>
	/// <exception cref="std::invalid_argument">コンストラクタでの異常または引数の要素数の問題</exception>
	/// <returns></returns>
	virtual void Appned_XY1DTable_Events_To_XYDist(const std::vector<double>& XYPairTable) = 0;

	/// <summary>
	/// <para>各イベントのXY座標を記録した一次元配列 { X0, Y0, X1, Y1, ..., Xn, Yn } から読み込んで二次元分布へ追加する</para>
	/// <para>例外により中断した場合，データは変更されない</para>
	/// </summary>
	/// <param name="XYPairTable"> : XY座標を記録した一次元配列</param>
	/// <param name="NumElements"> : 一次元配列の要素数．イベント総数 (n+1) の二倍に等しい．</param>
	/// <exception cref="std::out_of_range"></exception>
	/// <exception cref="std::invalid_argument">コンストラクタでの異常または引数の要素数の問題</exception>
	/// <returns></returns>
	virtual void Appned_XY1DTable_Events_To_XYDist(const double* XYPairTable, const size_t NumElements) = 0;



	struct UniformScale {
		virtual ~UniformScale();
		virtual explicit operator bool()const noexcept = 0;

		/// <summary>
		/// <para>区間の個数 N</para>
		/// <para>区間インデックス（Igor Pro のポイントと同義）は 0 から (N-1) までの整数</para>
		/// </summary>
		/// <returns></returns>
		virtual size_t NumBins()const noexcept = 0;

		/// <summary>
		/// <para>区間の始端 x(0)-binx/2</para>
		/// <para>区間代表値 x(0) とは異なる</para>
		/// </summary>
		/// <returns></returns>
		virtual double Range_Beg()const noexcept = 0;

		/// <summary>
		/// <para>区間の終端 x(N-1)+binx/2</para>
		/// <para>区間代表値 x(N-1) とは異なる</para>
		/// </summary>
		/// <returns></returns>
		virtual double Range_Last()const noexcept = 0;

		/// <summary>
		/// 区間幅 binx
		/// </summary>
		/// <returns></returns>
		virtual double Bin()const noexcept = 0;

		/// <summary>
		/// 区間インデックスから区間代表値を取得する
		/// </summary>
		/// <param name="Pnt"> : 区間インデックス（Igor Pro のポイントと同義）．0 から (NumBins()-1) までの整数</param>
		/// <exception cref="std::out_of_range">区間インデックスが範囲外</exception>
		/// <returns>区間を代表する値</returns>
		virtual double Pnt2Scale(const size_t Pnt)const = 0;
	};

	/// <summary>
	/// 目盛り情報の X 方向
	/// </summary>
	/// <exception cref="std::invalid_argument">コンストラクタでの異常によりオブジェクト取得に失敗</exception>
	/// <returns> X 方向に関する UniformScale への参照</returns>
	virtual const XYPairTableToXYDist::UniformScale& X()const = 0;

	/// <summary>
	/// 目盛り情報の Y 方向
	/// </summary>
	/// <exception cref="std::invalid_argument">コンストラクタでの異常によりオブジェクト取得に失敗</exception>
	/// <returns> Y 方向に関する UniformScale への参照</returns>
	virtual const XYPairTableToXYDist::UniformScale& Y()const = 0;

	/// <summary>
	/// コンストラクタが成功していたら true, 失敗していたら false を返す
	/// </summary>
	/// <returns></returns>
	virtual bool Exists()const noexcept = 0;


	/// <summary>
	/// <para>クラス XYPairTableToXYDist を作成してそのポインタを返す</para>
	/// <para>引数に矛盾が生じた場合例外を投げる</para>
	/// </summary>
	/// <exception cref = "std::invalid_argument">NumBinが正でない，Binが正でない</exception>
	/// <exception cref = "std::out_of_range"></exception>
	/// <exception cref = "Exceptions of std::make_unique or std::make_share"></exception>
	/// <returns></returns>
	std::unique_ptr<XYPairTableToXYDist> make(const size_t NumBins_X, const double Offset_X, const double Bin_X, const size_t NumBins_Y, const double Offset_Y, const double Bin_Y);
};


#define make_XYPairTableToXYDist XYPairTableToXYDist::make