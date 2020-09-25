#pragma once
#include <string>
#include <memory>
#include <vector>
#include <ostream>
#include <limits>


#ifdef IGORPRO6ITXLOADER0_EXPORTS
#define IGORPRO6ITXLOADER0__API __declspec(dllexport)
#define EXTN_TEMPLATE template
#else
#define IGORPRO6ITXLOADER0__API __declspec(dllimport)
#define EXTN_TEMPLATE extern template
#endif


//参考
//http://snak.tdiary.net/20030529.html
//https://stackoverflow.com/questions/8130602/using-extern-template-c11
//https://stackoverflow.com/questions/666628/importing-explicitly-instantiated-template-class-from-dll
//https://qiita.com/Kogia_sima/items/b7a1e23a78f81d913089



namespace IgorPro6 {


	typedef size_t type_igorpoint;


	class IGORPRO6ITXLOADER0__API Wave_Scale {
	public:
		Wave_Scale() = default;
		virtual ~Wave_Scale() = default;
		Wave_Scale(Wave_Scale const&) = delete;
		Wave_Scale& operator=(Wave_Scale const&) = delete;
		Wave_Scale(Wave_Scale&&) = delete;
		Wave_Scale& operator=(Wave_Scale&&) = delete;

		//座標のポイント数を取得
		virtual type_igorpoint Size() const noexcept = 0;
		//座標のオフセットを取得
		virtual double Offset() const noexcept = 0;
		//座標の増加量を取得
		virtual double Delta() const noexcept = 0;
		//座標の単位を取得
		virtual std::string Unit() const = 0;

		//座標のスケールを設定
		virtual void SetScale(double _Offset, double _Delta) noexcept = 0;
		//座標の単位を設定
		virtual void SetUnit(std::string&& _Unit) noexcept = 0;

		//ポイントから座標に変換．レンジ外ならNAN
		virtual double Pnt2Scale(type_igorpoint pnt) const noexcept = 0;
		//座標からポイントに変換．レンジ外ならNAN
		virtual type_igorpoint Scale2Pnt(double scaledvalue) const noexcept = 0;
		//ポイントがレンジ内にあるならtrue．ないならfalse
		virtual bool PntInRange(type_igorpoint pnt) const noexcept = 0;
		//内容を表示
		virtual  std::string& Print(std::string& dst) const = 0;

	};
	//内容を表示．ストリーム版
	IGORPRO6ITXLOADER0__API std::basic_ostream<char, std::char_traits<char>>& operator<<(std::basic_ostream<char, std::char_traits<char>>& os, const Wave_Scale& sc);
	


	template<typename T, typename std::enable_if<(std::is_arithmetic<T>::value), std::nullptr_t>::type = nullptr>
	__interface IWave_Base
	{
		//ウェーブの次元数を取得．一次元なら1, 二次元なら2等々
		size_t WaveDims() const noexcept;
		//ウェーブ名称を取得
		std::string& WaveName(std::string& dst) const;
		//ウェーブの内容を表示
		std::string& Print(std::string& dst) const;
	};

	template<typename T>
	class Wave_1D :public IWave_Base<T>{
	public://constructor destructor
		Wave_1D() = default;
		virtual ~Wave_1D() = default;
		Wave_1D(Wave_1D const&) = delete;
		Wave_1D& operator=(Wave_1D const&) = delete;
		Wave_1D(Wave_1D&&) = delete;
		Wave_1D& operator=(Wave_1D&&) = delete;

	public://methods
		//ポイントから値を取得
		virtual T At(type_igorpoint&& p) const = 0;
		//Xスケールへの参照を取得
		virtual Wave_Scale& DimX() = 0;
	
	};
	//ウェーブの内容を表示．ストリーム版．
	template<typename T> std::basic_ostream<char, std::char_traits<char>>& operator<<(std::basic_ostream<char, std::char_traits<char>>& os, const Wave_1D<T>& w);


	template<typename T>
	class Wave_2D :public IWave_Base<T> {
	public://const dest
		Wave_2D() = default;
		virtual ~Wave_2D() = default;
		Wave_2D(Wave_2D const&) = delete;
		Wave_2D& operator=(Wave_2D const&) = delete;
		Wave_2D(Wave_2D&&) = delete;
		Wave_2D& operator=(Wave_2D&&) = delete;

	public://methods
		//ポイントから値を取得
		virtual T At(type_igorpoint&& p, type_igorpoint&& q) const = 0;
		//Xスケールへの参照を取得
		virtual Wave_Scale& DimX() = 0;
		//Yスケールへの参照を取得
		virtual Wave_Scale& DimY() = 0;
	};
	//ウェーブの内容を表示．ストリーム版．
	template<typename T> std::basic_ostream<char, std::char_traits<char>>& operator<<(std::basic_ostream<char, std::char_traits<char>>& os, const Wave_2D<T>& w);


	template<typename T>
	class Wave_3D :public IWave_Base<T> {
	public://const dest
		Wave_3D() = default;
		virtual ~Wave_3D() = default;
		Wave_3D(Wave_3D const&) = delete;
		Wave_3D& operator=(Wave_3D const&) = delete;
		Wave_3D(Wave_3D&&) = delete;
		Wave_3D& operator=(Wave_3D&&) = delete;

	public://methods
		//ポイントから値を取得
		virtual T At(type_igorpoint&& p, type_igorpoint&& q, type_igorpoint&& r) const = 0;
		//Xスケールへの参照を取得
		virtual Wave_Scale& DimX() = 0;
		//Yスケールへの参照を取得
		virtual Wave_Scale& DimY() = 0;
		//Zスケールへの参照を取得
		virtual Wave_Scale& DimZ() = 0;

	};
	//ウェーブの内容を表示．ストリーム版．
	template<typename T> std::basic_ostream<char, std::char_traits<char>>& operator<<(std::basic_ostream<char, std::char_traits<char>>& os, const Wave_3D<T>& w);


	template<typename T>
	class Wave_4D :public IWave_Base<T> {
	public://const dest
		Wave_4D() = default;
		virtual ~Wave_4D() = default;
		Wave_4D(Wave_4D const&) = delete;
		Wave_4D& operator=(Wave_4D const&) = delete;
		Wave_4D(Wave_4D&&) = delete;
		Wave_4D& operator=(Wave_4D&&) = delete;


	public://methods
		//ポイントから値を取得
		virtual T At(type_igorpoint&& p, type_igorpoint&& q, type_igorpoint&& r, type_igorpoint&& s) const = 0;
		//Xスケールへの参照を取得
		virtual Wave_Scale& DimX() = 0;
		//Yスケールへの参照を取得
		virtual Wave_Scale& DimY() = 0;
		//Zスケールへの参照を取得
		virtual Wave_Scale& DimZ() = 0;
		//Tスケールへの参照を取得
		virtual Wave_Scale& DimT() = 0;
	};
	//ウェーブの内容を表示．ストリーム版．
	template<typename T> std::basic_ostream<char, std::char_traits<char>>& operator<<(std::basic_ostream<char, std::char_traits<char>>& os, const Wave_4D<T>& w);




	namespace ITXLoader {
		
		enum V_Flag {
			Success = 0,
			FileOpenFailure = 1,
			NotITX = 2,
			NonLoadedWaveExists=3,
			UnknownWaveFlag=4,

		};


		IGORPRO6ITXLOADER0__API int LoadWaves(std::string& filename, const char eofLine);
		
		IGORPRO6ITXLOADER0__API int V_Flag() noexcept;

		template<size_t N, typename T> size_t NumOfWaves();
		
		template<size_t N, typename T> std::string& PrintWaves(std::string& dst);
		
		IGORPRO6ITXLOADER0__API std::string& PrintAllWaves(std::string& dst);

		IGORPRO6ITXLOADER0__API void ClearWaves();

		//クラス定義が次元で分かれているので
		template<typename T> std::vector<std::unique_ptr<Wave_1D<T>>> MoveWaves_1D();
		template<typename T> std::vector<std::unique_ptr<Wave_2D<T>>> MoveWaves_2D();
		template<typename T> std::vector<std::unique_ptr<Wave_3D<T>>> MoveWaves_3D();
		template<typename T> std::vector<std::unique_ptr<Wave_4D<T>>> MoveWaves_4D();



		EXTN_TEMPLATE IGORPRO6ITXLOADER0__API size_t NumOfWaves<1, double>();
		EXTN_TEMPLATE IGORPRO6ITXLOADER0__API size_t NumOfWaves<2, double>();
		EXTN_TEMPLATE IGORPRO6ITXLOADER0__API size_t NumOfWaves<3, double>();
		EXTN_TEMPLATE IGORPRO6ITXLOADER0__API size_t NumOfWaves<4, double>();
		EXTN_TEMPLATE IGORPRO6ITXLOADER0__API std::string& PrintWaves<1, double>(std::string& dst);
		EXTN_TEMPLATE IGORPRO6ITXLOADER0__API std::string& PrintWaves<2, double>(std::string& dst);
		EXTN_TEMPLATE IGORPRO6ITXLOADER0__API std::string& PrintWaves<3, double>(std::string& dst);
		EXTN_TEMPLATE IGORPRO6ITXLOADER0__API std::string& PrintWaves<4, double>(std::string& dst);
		EXTN_TEMPLATE IGORPRO6ITXLOADER0__API std::vector<std::unique_ptr<Wave_1D<double>>> MoveWaves_1D();
		EXTN_TEMPLATE IGORPRO6ITXLOADER0__API std::vector<std::unique_ptr<Wave_2D<double>>> MoveWaves_2D();
		EXTN_TEMPLATE IGORPRO6ITXLOADER0__API std::vector<std::unique_ptr<Wave_3D<double>>> MoveWaves_3D();
		EXTN_TEMPLATE IGORPRO6ITXLOADER0__API std::vector<std::unique_ptr<Wave_4D<double>>> MoveWaves_4D();

		EXTN_TEMPLATE IGORPRO6ITXLOADER0__API size_t NumOfWaves<1, __int32>();
		EXTN_TEMPLATE IGORPRO6ITXLOADER0__API size_t NumOfWaves<2, __int32>();
		EXTN_TEMPLATE IGORPRO6ITXLOADER0__API size_t NumOfWaves<3, __int32>();
		EXTN_TEMPLATE IGORPRO6ITXLOADER0__API size_t NumOfWaves<4, __int32>();
		EXTN_TEMPLATE IGORPRO6ITXLOADER0__API std::string& PrintWaves<1, __int32>(std::string& dst);
		EXTN_TEMPLATE IGORPRO6ITXLOADER0__API std::string& PrintWaves<2, __int32>(std::string& dst);
		EXTN_TEMPLATE IGORPRO6ITXLOADER0__API std::string& PrintWaves<3, __int32>(std::string& dst);
		EXTN_TEMPLATE IGORPRO6ITXLOADER0__API std::string& PrintWaves<4, __int32>(std::string& dst);
		EXTN_TEMPLATE IGORPRO6ITXLOADER0__API std::vector<std::unique_ptr<Wave_1D<__int32>>> MoveWaves_1D();
		EXTN_TEMPLATE IGORPRO6ITXLOADER0__API std::vector<std::unique_ptr<Wave_2D<__int32>>> MoveWaves_2D();
		EXTN_TEMPLATE IGORPRO6ITXLOADER0__API std::vector<std::unique_ptr<Wave_3D<__int32>>> MoveWaves_3D();
		EXTN_TEMPLATE IGORPRO6ITXLOADER0__API std::vector<std::unique_ptr<Wave_4D<__int32>>> MoveWaves_4D();

		EXTN_TEMPLATE IGORPRO6ITXLOADER0__API size_t NumOfWaves<1, unsigned __int32>();
		EXTN_TEMPLATE IGORPRO6ITXLOADER0__API size_t NumOfWaves<2, unsigned __int32>();
		EXTN_TEMPLATE IGORPRO6ITXLOADER0__API size_t NumOfWaves<3, unsigned __int32>();
		EXTN_TEMPLATE IGORPRO6ITXLOADER0__API size_t NumOfWaves<4, unsigned __int32>();
		EXTN_TEMPLATE IGORPRO6ITXLOADER0__API std::string& PrintWaves<1, unsigned __int32>(std::string& dst);
		EXTN_TEMPLATE IGORPRO6ITXLOADER0__API std::string& PrintWaves<2, unsigned __int32>(std::string& dst);
		EXTN_TEMPLATE IGORPRO6ITXLOADER0__API std::string& PrintWaves<3, unsigned __int32>(std::string& dst);
		EXTN_TEMPLATE IGORPRO6ITXLOADER0__API std::string& PrintWaves<4, unsigned __int32>(std::string& dst);
		EXTN_TEMPLATE IGORPRO6ITXLOADER0__API std::vector<std::unique_ptr<Wave_1D<unsigned __int32>>> MoveWaves_1D();
		EXTN_TEMPLATE IGORPRO6ITXLOADER0__API std::vector<std::unique_ptr<Wave_2D<unsigned __int32>>> MoveWaves_2D();
		EXTN_TEMPLATE IGORPRO6ITXLOADER0__API std::vector<std::unique_ptr<Wave_3D<unsigned __int32>>> MoveWaves_3D();
		EXTN_TEMPLATE IGORPRO6ITXLOADER0__API std::vector<std::unique_ptr<Wave_4D<unsigned __int32>>> MoveWaves_4D();

	}


	EXTN_TEMPLATE IGORPRO6ITXLOADER0__API class IGORPRO6ITXLOADER0__API Wave_1D<double>;
	EXTN_TEMPLATE IGORPRO6ITXLOADER0__API class IGORPRO6ITXLOADER0__API Wave_2D<double>;
	EXTN_TEMPLATE IGORPRO6ITXLOADER0__API class IGORPRO6ITXLOADER0__API Wave_3D<double>;
	EXTN_TEMPLATE IGORPRO6ITXLOADER0__API class IGORPRO6ITXLOADER0__API Wave_4D<double>;
	EXTN_TEMPLATE IGORPRO6ITXLOADER0__API class IGORPRO6ITXLOADER0__API Wave_1D<__int32>;
	EXTN_TEMPLATE IGORPRO6ITXLOADER0__API class IGORPRO6ITXLOADER0__API Wave_2D<__int32>;
	EXTN_TEMPLATE IGORPRO6ITXLOADER0__API class IGORPRO6ITXLOADER0__API Wave_3D<__int32>;
	EXTN_TEMPLATE IGORPRO6ITXLOADER0__API class IGORPRO6ITXLOADER0__API Wave_4D<__int32>;
	EXTN_TEMPLATE IGORPRO6ITXLOADER0__API class IGORPRO6ITXLOADER0__API Wave_1D<unsigned __int32>;
	EXTN_TEMPLATE IGORPRO6ITXLOADER0__API class IGORPRO6ITXLOADER0__API Wave_2D<unsigned __int32>;
	EXTN_TEMPLATE IGORPRO6ITXLOADER0__API class IGORPRO6ITXLOADER0__API Wave_3D<unsigned __int32>;
	EXTN_TEMPLATE IGORPRO6ITXLOADER0__API class IGORPRO6ITXLOADER0__API Wave_4D<unsigned __int32>;

	EXTN_TEMPLATE IGORPRO6ITXLOADER0__API std::basic_ostream<char, std::char_traits<char>>& operator << (std::basic_ostream<char, std::char_traits<char>>& ss, const Wave_1D<double>& w);
	EXTN_TEMPLATE IGORPRO6ITXLOADER0__API std::basic_ostream<char, std::char_traits<char>>& operator << (std::basic_ostream<char, std::char_traits<char>>& ss, const Wave_1D<__int32>& w);
	EXTN_TEMPLATE IGORPRO6ITXLOADER0__API std::basic_ostream<char, std::char_traits<char>>& operator << (std::basic_ostream<char, std::char_traits<char>>& ss, const Wave_1D<unsigned __int32>& w);
	EXTN_TEMPLATE IGORPRO6ITXLOADER0__API std::basic_ostream<char, std::char_traits<char>>& operator << (std::basic_ostream<char, std::char_traits<char>>& ss, const Wave_2D<double>& w);
	EXTN_TEMPLATE IGORPRO6ITXLOADER0__API std::basic_ostream<char, std::char_traits<char>>& operator << (std::basic_ostream<char, std::char_traits<char>>& ss, const Wave_2D<__int32>& w);
	EXTN_TEMPLATE IGORPRO6ITXLOADER0__API std::basic_ostream<char, std::char_traits<char>>& operator << (std::basic_ostream<char, std::char_traits<char>>& ss, const Wave_2D<unsigned __int32>& w);
	EXTN_TEMPLATE IGORPRO6ITXLOADER0__API std::basic_ostream<char, std::char_traits<char>>& operator << (std::basic_ostream<char, std::char_traits<char>>& ss, const Wave_3D<double>& w);
	EXTN_TEMPLATE IGORPRO6ITXLOADER0__API std::basic_ostream<char, std::char_traits<char>>& operator << (std::basic_ostream<char, std::char_traits<char>>& ss, const Wave_3D<__int32>& w);
	EXTN_TEMPLATE IGORPRO6ITXLOADER0__API std::basic_ostream<char, std::char_traits<char>>& operator << (std::basic_ostream<char, std::char_traits<char>>& ss, const Wave_3D<unsigned __int32>& w);
	EXTN_TEMPLATE IGORPRO6ITXLOADER0__API std::basic_ostream<char, std::char_traits<char>>& operator << (std::basic_ostream<char, std::char_traits<char>>& ss, const Wave_4D<double>& w);
	EXTN_TEMPLATE IGORPRO6ITXLOADER0__API std::basic_ostream<char, std::char_traits<char>>& operator << (std::basic_ostream<char, std::char_traits<char>>& ss, const Wave_4D<__int32>& w);
	EXTN_TEMPLATE IGORPRO6ITXLOADER0__API std::basic_ostream<char, std::char_traits<char>>& operator << (std::basic_ostream<char, std::char_traits<char>>& ss, const Wave_4D<unsigned __int32>& w);
	
}
