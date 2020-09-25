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


//�Q�l
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

		//���W�̃|�C���g�����擾
		virtual type_igorpoint Size() const noexcept = 0;
		//���W�̃I�t�Z�b�g���擾
		virtual double Offset() const noexcept = 0;
		//���W�̑����ʂ��擾
		virtual double Delta() const noexcept = 0;
		//���W�̒P�ʂ��擾
		virtual std::string Unit() const = 0;

		//���W�̃X�P�[����ݒ�
		virtual void SetScale(double _Offset, double _Delta) noexcept = 0;
		//���W�̒P�ʂ�ݒ�
		virtual void SetUnit(std::string&& _Unit) noexcept = 0;

		//�|�C���g������W�ɕϊ��D�����W�O�Ȃ�NAN
		virtual double Pnt2Scale(type_igorpoint pnt) const noexcept = 0;
		//���W����|�C���g�ɕϊ��D�����W�O�Ȃ�NAN
		virtual type_igorpoint Scale2Pnt(double scaledvalue) const noexcept = 0;
		//�|�C���g�������W���ɂ���Ȃ�true�D�Ȃ��Ȃ�false
		virtual bool PntInRange(type_igorpoint pnt) const noexcept = 0;
		//���e��\��
		virtual  std::string& Print(std::string& dst) const = 0;

	};
	//���e��\���D�X�g���[����
	IGORPRO6ITXLOADER0__API std::basic_ostream<char, std::char_traits<char>>& operator<<(std::basic_ostream<char, std::char_traits<char>>& os, const Wave_Scale& sc);
	


	template<typename T, typename std::enable_if<(std::is_arithmetic<T>::value), std::nullptr_t>::type = nullptr>
	__interface IWave_Base
	{
		//�E�F�[�u�̎��������擾�D�ꎟ���Ȃ�1, �񎟌��Ȃ�2���X
		size_t WaveDims() const noexcept;
		//�E�F�[�u���̂��擾
		std::string& WaveName(std::string& dst) const;
		//�E�F�[�u�̓��e��\��
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
		//�|�C���g����l���擾
		virtual T At(type_igorpoint&& p) const = 0;
		//X�X�P�[���ւ̎Q�Ƃ��擾
		virtual Wave_Scale& DimX() = 0;
	
	};
	//�E�F�[�u�̓��e��\���D�X�g���[���ŁD
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
		//�|�C���g����l���擾
		virtual T At(type_igorpoint&& p, type_igorpoint&& q) const = 0;
		//X�X�P�[���ւ̎Q�Ƃ��擾
		virtual Wave_Scale& DimX() = 0;
		//Y�X�P�[���ւ̎Q�Ƃ��擾
		virtual Wave_Scale& DimY() = 0;
	};
	//�E�F�[�u�̓��e��\���D�X�g���[���ŁD
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
		//�|�C���g����l���擾
		virtual T At(type_igorpoint&& p, type_igorpoint&& q, type_igorpoint&& r) const = 0;
		//X�X�P�[���ւ̎Q�Ƃ��擾
		virtual Wave_Scale& DimX() = 0;
		//Y�X�P�[���ւ̎Q�Ƃ��擾
		virtual Wave_Scale& DimY() = 0;
		//Z�X�P�[���ւ̎Q�Ƃ��擾
		virtual Wave_Scale& DimZ() = 0;

	};
	//�E�F�[�u�̓��e��\���D�X�g���[���ŁD
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
		//�|�C���g����l���擾
		virtual T At(type_igorpoint&& p, type_igorpoint&& q, type_igorpoint&& r, type_igorpoint&& s) const = 0;
		//X�X�P�[���ւ̎Q�Ƃ��擾
		virtual Wave_Scale& DimX() = 0;
		//Y�X�P�[���ւ̎Q�Ƃ��擾
		virtual Wave_Scale& DimY() = 0;
		//Z�X�P�[���ւ̎Q�Ƃ��擾
		virtual Wave_Scale& DimZ() = 0;
		//T�X�P�[���ւ̎Q�Ƃ��擾
		virtual Wave_Scale& DimT() = 0;
	};
	//�E�F�[�u�̓��e��\���D�X�g���[���ŁD
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

		//�N���X��`�������ŕ�����Ă���̂�
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
