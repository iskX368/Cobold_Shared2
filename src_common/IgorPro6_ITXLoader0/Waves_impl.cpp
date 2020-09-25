#include "pch.h" // use stdafx.h in Visual Studio 2017 and earlier
#include "Waves_impl.h"
#include <type_traits>
#include <sstream>
#include <iostream>


namespace IgorPro6 {



	class Wave_Scale_impl
		:public Wave_Scale
	{
	public:
		Wave_Scale_impl() = delete;
		Wave_Scale_impl(type_igorpoint _Size, double _Offset, double _Delta, std::string& _Unit)
			:Size_(_Size), Offset_(_Offset), Delta_(_Delta), Unit_(_Unit) {}
		Wave_Scale_impl(type_igorpoint _Size)
			:Size_(_Size), Offset_(0), Delta_(1), Unit_("") {}
		~Wave_Scale_impl() = default;

	private:
		type_igorpoint Size_;
		double Offset_;
		double Delta_;
		std::string Unit_;
	public:
		//getter
		type_igorpoint Size() const noexcept override { return Size_; }
		double Offset() const noexcept override { return Offset_; }
		double Delta() const noexcept override { return Delta_; }
		std::string Unit() const override { return std::string(Unit_); }
	public:
		//setter
		void SetScale(double _Offset, double _Delta) noexcept override { Offset_ = _Offset; Delta_ = _Delta; }
		void SetUnit(std::string&& _Unit) noexcept { Unit_ = _Unit; }
	public:
		double Pnt2Scale(type_igorpoint pnt) const noexcept override { return (pnt >= 0 && pnt < Size_) ? (Offset_ + pnt * Delta_) : std::numeric_limits<double>::quiet_NaN(); }
		type_igorpoint Scale2Pnt(double scaledvalue) const noexcept override { type_igorpoint ans = static_cast<type_igorpoint>(floor((scaledvalue - Offset_) / Delta_ + 0.5)); return (ans >= 0 && ans < Size_) ? ans : std::numeric_limits<type_igorpoint>::quiet_NaN(); }
		bool PntInRange(type_igorpoint pnt)const noexcept override { return (pnt >= 0 && pnt < Size_); }

		virtual  std::string& Print(std::string& dst) const override {
			std::stringstream ss;
			ss << *this;
			dst = ss.str();
			return dst;
		}
	};
	std::basic_ostream<char, std::char_traits<char>>& operator<<(std::basic_ostream<char, std::char_traits<char>>& os, const Wave_Scale& sc) {
		return os << "Size = " << sc.Size() << ", Offset = " << sc.Offset() << ", Delta = " << sc.Delta() << ", Unit = " << sc.Unit();
	}


	class NonCopyable {
	protected:
		NonCopyable() = default;
		~NonCopyable() = default;
	private:
		NonCopyable(NonCopyable const&) = delete;
		NonCopyable& operator=(NonCopyable const&) = delete;
	};

	template<typename T, typename std::enable_if<(std::is_arithmetic<T>::value), std::nullptr_t>::type = nullptr>
	class Wave_Base
	{
		public:
			Wave_Base() = delete;

			Wave_Base(std::vector<T>& _ArrayFromITX, std::string& _WaveName)
				:m_Array(_ArrayFromITX), m_WaveName(_WaveName)
			{}
			Wave_Base(std::vector<T>&& _ArrayFromITX, std::string& _WaveName)
				:m_Array(_ArrayFromITX), m_WaveName(_WaveName)
			{}

			virtual ~Wave_Base() = default;


		protected://helpers

			//returns a0+b0 * (a1+b1 * (a2+b2 * (a3+b3 * 0)))
			template<typename PN, template<typename, typename> class U>
			PN haind1(U<PN, PN>&& p) const {
				return p.first;
			}
			template<typename PN, template<typename, typename> class U, template<typename, typename> class...Us>
			PN haind1(U<PN, PN>&& p, Us<PN, PN>&&... ps) const {
				return p.first + p.second * haind1(std::forward<Us<PN, PN>>(ps)...);
			}

			//checks index
			template<typename PN, template<typename, typename> class U>
			bool inrange1(U<PN, PN>&& p) const {
				return p.first >= 0 && p.first < p.second;
			}
			template<typename PN, template<typename, typename> class U, template<typename, typename> class...Us>
			bool inrange1(U<PN, PN>&& p, Us<PN, PN>&&... ps) const {
				return (p.first >= 0 && p.first < p.second) && inrange1(std::forward<Us<PN, PN>>(ps)...);
			}

			//gets value
			template<typename PN, template<typename, typename> class U, template<typename, typename> class...Us>
			T at1(U<PN, PN>&& p, Us<PN, PN>&&... ps) const {
				return inrange1(std::forward<U<PN, PN>>(p), std::forward<U<PN, PN>>(ps)...) ? m_Array[haind1(std::forward<U<PN, PN>>(p), std::forward<U<PN, PN>>(ps)...)] : std::numeric_limits<T>::quiet_NaN();
			}

			//print name, dims, and scales
			virtual std::basic_ostream<char, std::char_traits<char>>& s_name_dims_scales1(std::basic_ostream<char, std::char_traits<char>>& os) const = 0;

			//print array contents
			virtual std::basic_ostream<char, std::char_traits<char>>& s_arraycontents(std::basic_ostream<char, std::char_traits<char>>& os) const = 0;
			std::basic_ostream<char, std::char_traits<char>>& s_arraycontents_col(std::basic_ostream<char, std::char_traits<char>>& os, const size_t NumElms_In_Row) const {
				size_t cnt = 0;
				const size_t ne_r1 = NumElms_In_Row - 1;
				for (const auto& elm : m_Array) {
					if (cnt >= ne_r1) {
						os << elm << '\n';
						cnt = 0;
					}
					else {
						os << elm << '\t';
						cnt++;
					}
				}
				return os;
			}

			//print
			std::string& show1(std::string& dst) const {
				std::stringstream ss;
				s_name_dims_scales1(ss);
				s_arraycontents(ss);
				dst = ss.str();
				return dst;
			}

		protected:
			std::vector<T> m_Array;
			std::string m_WaveName;

	};
	


	template<typename T>
	class Wave_1D_impl :public Wave_Base<T>, public Wave_1D<T>, private NonCopyable {
	public:
		Wave_1D_impl() = delete;
		Wave_1D_impl(std::vector<T>&& _ArrayFromITX, std::string& _WaveName, type_igorpoint _DimSizeX)
			:Wave_Base<T>(_ArrayFromITX, _WaveName), m_DimX(new Wave_Scale_impl(_DimSizeX))
		{}
		Wave_1D_impl(std::vector<T>&& _ArrayFromITX, std::string& _WaveName, type_igorpoint _DimSizeX, double _DimOffsetX, double _DimDeltaX, std::string& _DimUnitX)
			:Wave_Base<T>(_ArrayFromITX, _WaveName), m_DimX(new Wave_Scale_impl(_DimSizeX, _DimOffsetX, _DimDeltaX, _DimUnitX))
		{}
		~Wave_1D_impl() = default;
		
	public://user funcs
		size_t WaveDims() const noexcept override { return 1; }
		std::string& WaveName(std::string& dst) const override { dst = this->m_WaveName; return dst; }
		std::string& Print(std::string& dst) const override { return this->show1(dst); }
		T At(type_igorpoint&& p) const override {
			return this->at1(
				std::make_pair<type_igorpoint, type_igorpoint>(std::forward<type_igorpoint>(p), m_DimX->Size())
			);
		}
		Wave_Scale& DimX() override { return *m_DimX; }

	protected:
		//printers
		virtual std::basic_ostream<char, std::char_traits<char>>& s_name_dims_scales1(std::basic_ostream<char, std::char_traits<char>>& os) const override {
			os << "WaveName = " << this->m_WaveName << "\n";
			os << "WaveDims = " << WaveDims() << "\n";
			os << "DimX : " << *m_DimX << "\n";
			return os;
		}
		virtual std::basic_ostream<char, std::char_traits<char>>& s_arraycontents(std::basic_ostream<char, std::char_traits<char>>& os) const override {
			return this->s_arraycontents_col(os, 1);
		}
		
	private:
		std::unique_ptr<Wave_Scale> m_DimX;
	};
	template<typename T> std::basic_ostream<char, std::char_traits<char>>& operator<<(std::basic_ostream<char, std::char_traits<char>>& os, const Wave_1D<T>& w) {
		std::string str;
		return os << w.Print(str);
	}


	
	template<typename T>
	class Wave_2D_impl :public Wave_Base<T>, public Wave_2D<T>, private NonCopyable {
	public:
		Wave_2D_impl() = delete;
		Wave_2D_impl(std::vector<T>&& _ArrayFromITX, std::string& _WaveName, type_igorpoint _DimSizeX, type_igorpoint _DimSizeY)
			:Wave_Base<T>(_ArrayFromITX, _WaveName), m_DimX(new Wave_Scale_impl(_DimSizeX)), m_DimY(new Wave_Scale_impl(_DimSizeY))
		{}
		Wave_2D_impl(std::vector<T>&& _ArrayFromITX, std::string& _WaveName, type_igorpoint _DimSizeX, double _DimOffsetX, double _DimDeltaX, std::string& _DimUnitX, type_igorpoint _DimSizeY, double _DimOffsetY, double _DimDeltaY, std::string& _DimUnitY)
			:Wave_Base<T>(_ArrayFromITX, _WaveName), m_DimX(new Wave_Scale_impl(_DimSizeX, _DimOffsetX, _DimDeltaX, _DimUnitX)), m_DimY(new Wave_Scale_impl(_DimSizeY, _DimOffsetY, _DimDeltaY, _DimUnitY))
		{}
		~Wave_2D_impl() = default;

		

	public://user funcs
		size_t WaveDims() const noexcept override { return 2; }
		std::string& WaveName(std::string& dst) const override { dst = this->m_WaveName; return dst; }
		std::string& Print(std::string& dst) const override { return this->show1(dst); }
		T At(type_igorpoint&& p, type_igorpoint&& q) const override {
			return this->at1(
				std::make_pair<type_igorpoint, type_igorpoint>(std::forward<type_igorpoint>(q), m_DimY->Size()),//q
				std::make_pair<type_igorpoint, type_igorpoint>(std::forward<type_igorpoint>(p), m_DimX->Size()) //p
			);
		}
		Wave_Scale& DimX() override { return *m_DimX; }
		Wave_Scale& DimY() override { return *m_DimY; }


		std::basic_ostream<char, std::char_traits<char>>& Print2Stream(std::basic_ostream<char, std::char_traits<char>>& os) {
			return os;
		}

	protected:
		//printers
		virtual std::basic_ostream<char, std::char_traits<char>>& s_name_dims_scales1(std::basic_ostream<char, std::char_traits<char>>& os) const override {
			os << "WaveName = " << this->m_WaveName << "\n";
			os << "WaveDims = " << WaveDims() << "\n";
			os << "DimX : " << *m_DimX << "\n";
			os << "DimY : " << *m_DimY << "\n";
			return os;
		}
		virtual std::basic_ostream<char, std::char_traits<char>>& s_arraycontents(std::basic_ostream<char, std::char_traits<char>>& os) const override {
			return this->s_arraycontents_col(os, m_DimY->Size());
		}
		
	private:
		std::unique_ptr<Wave_Scale> m_DimX;
		std::unique_ptr<Wave_Scale> m_DimY;

	};
	template<typename T> std::basic_ostream<char, std::char_traits<char>>& operator<<(std::basic_ostream<char, std::char_traits<char>>& os, const Wave_2D<T>& w) {
		std::string str;
		return os << w.Print(str);
	}




	template<typename T>
	class Wave_3D_impl :public Wave_Base<T>, public Wave_3D<T>, private NonCopyable {
	public:
		Wave_3D_impl() = delete;
		Wave_3D_impl(std::vector<T>&& _ArrayFromITX, std::string& _WaveName, type_igorpoint _DimSizeX, type_igorpoint _DimSizeY, type_igorpoint _DimSizeZ)
			:Wave_Base<T>(_ArrayFromITX, _WaveName), m_DimX(new Wave_Scale_impl(_DimSizeX)), m_DimY(new Wave_Scale_impl(_DimSizeY)), m_DimZ(new Wave_Scale_impl(_DimSizeZ))
		{}
		Wave_3D_impl(std::vector<T>&& _ArrayFromITX, std::string& _WaveName, type_igorpoint _DimSizeX, double _DimOffsetX, double _DimDeltaX, std::string& _DimUnitX, type_igorpoint _DimSizeY, double _DimOffsetY, double _DimDeltaY, std::string& _DimUnitY, type_igorpoint _DimSizeZ, double _DimOffsetZ, double _DimDeltaZ, std::string& _DimUnitZ)
			:Wave_Base<T>(_ArrayFromITX, _WaveName), m_DimX(new Wave_Scale_impl(_DimSizeX, _DimOffsetX, _DimDeltaX, _DimUnitX)), m_DimY(new Wave_Scale_impl(_DimSizeY, _DimOffsetY, _DimDeltaY, _DimUnitY)), m_DimZ(new Wave_Scale_impl(_DimSizeZ, _DimOffsetZ, _DimDeltaZ, _DimUnitZ))
		{}
		~Wave_3D_impl() = default;

	public://user funcs
		size_t WaveDims() const noexcept override { return 3; }
		std::string& WaveName(std::string& dst) const override { dst = this->m_WaveName; return dst; }
		std::string& Print(std::string& dst) const override { return this->show1(dst); }
		T At(type_igorpoint&& p, type_igorpoint&& q, type_igorpoint&& r) const override {
			return this->at1(
				std::make_pair<type_igorpoint, type_igorpoint>(std::forward<type_igorpoint>(q), m_DimY->Size()),//q
				std::make_pair<type_igorpoint, type_igorpoint>(std::forward<type_igorpoint>(p), m_DimX->Size()),//p
				std::make_pair<type_igorpoint, type_igorpoint>(std::forward<type_igorpoint>(r), m_DimZ->Size()) //r
			);
		}
		Wave_Scale& DimX() override { return *m_DimX; }
		Wave_Scale& DimY() override { return *m_DimY; }
		Wave_Scale& DimZ() override { return *m_DimZ; }

	protected:
		//printers
		virtual std::basic_ostream<char, std::char_traits<char>>& s_name_dims_scales1(std::basic_ostream<char, std::char_traits<char>>& os) const override {
			os << "WaveName = " << this->m_WaveName << "\n";
			os << "WaveDims = " << WaveDims() << "\n";
			os << "DimX : " << *m_DimX << "\n";
			os << "DimY : " << *m_DimY << "\n";
			os << "DimZ : " << *m_DimZ << "\n";
			return os;
		}
		virtual std::basic_ostream<char, std::char_traits<char>>& s_arraycontents(std::basic_ostream<char, std::char_traits<char>>& os) const override {
			return this->s_arraycontents_col(os, m_DimY->Size());
		}
		
	private:
		std::unique_ptr<Wave_Scale> m_DimX;
		std::unique_ptr<Wave_Scale> m_DimY;
		std::unique_ptr<Wave_Scale> m_DimZ;
	};
	template<typename T> std::basic_ostream<char, std::char_traits<char>>& operator<<(std::basic_ostream<char, std::char_traits<char>>& os, const Wave_3D<T>& w) {
		std::string str;
		return os << w.Print(str);
	}

	
	template<typename T>
	class Wave_4D_impl :public Wave_Base<T>, public Wave_4D<T>, private NonCopyable {
	public:
		Wave_4D_impl() = delete;
		Wave_4D_impl(std::vector<T>&& _ArrayFromITX, std::string& _WaveName, type_igorpoint _DimSizeX, type_igorpoint _DimSizeY, type_igorpoint _DimSizeZ, type_igorpoint _DimSizeT)
			:Wave_Base<T>(_ArrayFromITX, _WaveName),
			m_DimX(new Wave_Scale_impl(_DimSizeX)),
			m_DimY(new Wave_Scale_impl(_DimSizeY)),
			m_DimZ(new Wave_Scale_impl(_DimSizeZ)),
			m_DimT(new Wave_Scale_impl(_DimSizeT))
		{}

		Wave_4D_impl(std::vector<T>&& _ArrayFromITX, std::string& _WaveName,
			type_igorpoint _DimSizeX, double _DimOffsetX, double _DimDeltaX, std::string& _DimUnitX,
			type_igorpoint _DimSizeY, double _DimOffsetY, double _DimDeltaY, std::string& _DimUnitY,
			type_igorpoint _DimSizeZ, double _DimOffsetZ, double _DimDeltaZ, std::string& _DimUnitZ,
			type_igorpoint _DimSizeT, double _DimOffsetT, double _DimDeltaT, std::string& _DimUnitT)
			:Wave_Base<T>(_ArrayFromITX, _WaveName),
			m_DimX(new Wave_Scale_impl(_DimSizeX, _DimOffsetX, _DimDeltaX, _DimUnitX)),
			m_DimY(new Wave_Scale_impl(_DimSizeY, _DimOffsetY, _DimDeltaY, _DimUnitY)),
			m_DimZ(new Wave_Scale_impl(_DimSizeZ, _DimOffsetZ, _DimDeltaZ, _DimUnitZ)),
			m_DimT(new Wave_Scale_impl(_DimSizeT, _DimOffsetT, _DimDeltaT, _DimUnitT))
		{}

		~Wave_4D_impl() = default;

	public://user funcs
		size_t WaveDims() const noexcept override { return 4; }
		std::string& WaveName(std::string& dst) const override { dst = this->m_WaveName; return dst; }
		std::string& Print(std::string& dst) const override { return this->show1(dst); }
		T At(type_igorpoint&& p, type_igorpoint&& q, type_igorpoint&& r, type_igorpoint&& s) const override {
			return this->at1(
				std::make_pair<type_igorpoint, type_igorpoint>(std::forward<type_igorpoint>(q), m_DimY->Size()),//q
				std::make_pair<type_igorpoint, type_igorpoint>(std::forward<type_igorpoint>(p), m_DimX->Size()),//p
				std::make_pair<type_igorpoint, type_igorpoint>(std::forward<type_igorpoint>(r), m_DimZ->Size()),//r
				std::make_pair<type_igorpoint, type_igorpoint>(std::forward<type_igorpoint>(s), m_DimT->Size()) //s
			);
		}
		Wave_Scale& DimX() override { return *m_DimX; }
		Wave_Scale& DimY() override { return *m_DimY; }
		Wave_Scale& DimZ() override { return *m_DimZ; }
		Wave_Scale& DimT() override { return *m_DimT; }

	protected:
		//printers
		virtual std::basic_ostream<char, std::char_traits<char>>& s_name_dims_scales1(std::basic_ostream<char, std::char_traits<char>>& os) const override {
			os << "WaveName = " << this->m_WaveName << "\n";
			os << "WaveDims = " << WaveDims() << "\n";
			os << "DimX : " << *m_DimX << "\n";
			os << "DimY : " << *m_DimY << "\n";
			os << "DimZ : " << *m_DimZ << "\n";
			os << "DimT : " << *m_DimT << "\n";
			return os;
		}
		virtual std::basic_ostream<char, std::char_traits<char>>& s_arraycontents(std::basic_ostream<char, std::char_traits<char>>& os) const override {
			return this->s_arraycontents_col(os, m_DimY->Size());
		}
		
	private:
		std::unique_ptr<Wave_Scale> m_DimX;
		std::unique_ptr<Wave_Scale> m_DimY;
		std::unique_ptr<Wave_Scale> m_DimZ;
		std::unique_ptr<Wave_Scale> m_DimT;

	};
	template<typename T> std::basic_ostream<char, std::char_traits<char>>& operator<<(std::basic_ostream<char, std::char_traits<char>>& os, const Wave_4D<T>& w) {
		std::string str;
		return os << w.Print(str);
	}



	namespace Wave_Factory {
		template<typename T> std::unique_ptr<Wave_1D<T>> make(
			std::vector<T>&& _ArrayFromITX, std::string& _WaveName,
			type_igorpoint _DimSizeX, double _DimOffsetX, double _DimDeltaX, std::string& _DimUnitX
		) {
			return std::make_unique<Wave_1D_impl<T>>(
				std::forward<std::vector<T>>(_ArrayFromITX), _WaveName,
				_DimSizeX, _DimOffsetX, _DimDeltaX, _DimUnitX
				);
		}

		template<typename T> std::unique_ptr<Wave_2D<T>> make(
			std::vector<T>&& _ArrayFromITX, std::string& _WaveName,
			type_igorpoint _DimSizeX, double _DimOffsetX, double _DimDeltaX, std::string& _DimUnitX,
			type_igorpoint _DimSizeY, double _DimOffsetY, double _DimDeltaY, std::string& _DimUnitY
		) {
			return std::make_unique<Wave_2D_impl<T>>(
				std::forward<std::vector<T>>(_ArrayFromITX), _WaveName,
				_DimSizeX, _DimOffsetX, _DimDeltaX, _DimUnitX,
				_DimSizeY, _DimOffsetY, _DimDeltaY, _DimUnitY
				);
		}

		template<typename T> std::unique_ptr<Wave_3D<T>> make(
			std::vector<T>&& _ArrayFromITX, std::string& _WaveName,
			type_igorpoint _DimSizeX, double _DimOffsetX, double _DimDeltaX, std::string& _DimUnitX,
			type_igorpoint _DimSizeY, double _DimOffsetY, double _DimDeltaY, std::string& _DimUnitY,
			type_igorpoint _DimSizeZ, double _DimOffsetZ, double _DimDeltaZ, std::string& _DimUnitZ
		) {
			return std::make_unique<Wave_3D_impl<T>>(
				std::forward<std::vector<T>>(_ArrayFromITX), _WaveName,
				_DimSizeX, _DimOffsetX, _DimDeltaX, _DimUnitX,
				_DimSizeY, _DimOffsetY, _DimDeltaY, _DimUnitY,
				_DimSizeZ, _DimOffsetZ, _DimDeltaZ, _DimUnitZ
				);
		}

		template<typename T> std::unique_ptr<Wave_4D<T>> make(
			std::vector<T>&& _ArrayFromITX, std::string& _WaveName,
			type_igorpoint _DimSizeX, double _DimOffsetX, double _DimDeltaX, std::string& _DimUnitX,
			type_igorpoint _DimSizeY, double _DimOffsetY, double _DimDeltaY, std::string& _DimUnitY,
			type_igorpoint _DimSizeZ, double _DimOffsetZ, double _DimDeltaZ, std::string& _DimUnitZ,
			type_igorpoint _DimSizeT, double _DimOffsetT, double _DimDeltaT, std::string& _DimUnitT
		) {
			return std::make_unique<Wave_4D_impl<T>>(
				std::forward<std::vector<T>>(_ArrayFromITX), _WaveName,
				_DimSizeX, _DimOffsetX, _DimDeltaX, _DimUnitX,
				_DimSizeY, _DimOffsetY, _DimDeltaY, _DimUnitY,
				_DimSizeZ, _DimOffsetZ, _DimDeltaZ, _DimUnitZ,
				_DimSizeT, _DimOffsetT, _DimDeltaT, _DimUnitT
				);
		}
		
	};



	//–¾Ž¦“I“ÁŽê‰»
	//1D
	template Wave_1D_impl<double>;
	template std::unique_ptr<Wave_1D<double>> Wave_Factory::make(
		std::vector<double>&& _ArrayFromITX, std::string& _WaveName,
		type_igorpoint _DimSizeX, double _DimOffsetX, double _DimDeltaX, std::string& _DimUnitX
	);
	template Wave_1D_impl<__int32>;
	template std::unique_ptr<Wave_1D<__int32>> Wave_Factory::make(
		std::vector<__int32>&& _ArrayFromITX, std::string& _WaveName,
		type_igorpoint _DimSizeX, double _DimOffsetX, double _DimDeltaX, std::string& _DimUnitX
	);
	template Wave_1D_impl<unsigned __int32>;
	template std::unique_ptr<Wave_1D<unsigned __int32>> Wave_Factory::make(
		std::vector<unsigned __int32>&& _ArrayFromITX, std::string& _WaveName,
		type_igorpoint _DimSizeX, double _DimOffsetX, double _DimDeltaX, std::string& _DimUnitX
	);
	//template std::basic_ostream<char, std::char_traits<char>>& operator << (std::basic_ostream<char, std::char_traits<char>>& ss, const Wave_1D<double>& w);
	//template std::basic_ostream<char, std::char_traits<char>>& operator << (std::basic_ostream<char, std::char_traits<char>>& ss, const Wave_1D<__int32>& w);
	//template std::basic_ostream<char, std::char_traits<char>>& operator << (std::basic_ostream<char, std::char_traits<char>>& ss, const Wave_1D<unsigned __int32>& w);

	//2D
	template Wave_2D_impl<double>;
	template std::unique_ptr<Wave_2D<double>> Wave_Factory::make(
		std::vector<double>&& _ArrayFromITX, std::string& _WaveName,
		type_igorpoint _DimSizeX, double _DimOffsetX, double _DimDeltaX, std::string& _DimUnitX,
		type_igorpoint _DimSizeY, double _DimOffsetY, double _DimDeltaY, std::string& _DimUnitY
	);
	template Wave_2D_impl<__int32>;
	template std::unique_ptr<Wave_2D<__int32>> Wave_Factory::make(
		std::vector<__int32>&& _ArrayFromITX, std::string& _WaveName,
		type_igorpoint _DimSizeX, double _DimOffsetX, double _DimDeltaX, std::string& _DimUnitX,
		type_igorpoint _DimSizeY, double _DimOffsetY, double _DimDeltaY, std::string& _DimUnitY
	);
	template Wave_2D_impl<unsigned __int32>;
	template std::unique_ptr<Wave_2D<unsigned __int32>> Wave_Factory::make(
		std::vector<unsigned __int32>&& _ArrayFromITX, std::string& _WaveName,
		type_igorpoint _DimSizeX, double _DimOffsetX, double _DimDeltaX, std::string& _DimUnitX,
		type_igorpoint _DimSizeY, double _DimOffsetY, double _DimDeltaY, std::string& _DimUnitY
	);
	//template std::basic_ostream<char, std::char_traits<char>>& operator << (std::basic_ostream<char, std::char_traits<char>>& ss, const Wave_2D<double>& w);
	//template std::basic_ostream<char, std::char_traits<char>>& operator << (std::basic_ostream<char, std::char_traits<char>>& ss, const Wave_2D<__int32>& w);
	//template std::basic_ostream<char, std::char_traits<char>>& operator << (std::basic_ostream<char, std::char_traits<char>>& ss, const Wave_2D<unsigned __int32>& w);

	//3D
	template Wave_3D_impl<double>;
	template std::unique_ptr<Wave_3D<double>> Wave_Factory::make(
		std::vector<double>&& _ArrayFromITX, std::string& _WaveName,
		type_igorpoint _DimSizeX, double _DimOffsetX, double _DimDeltaX, std::string& _DimUnitX,
		type_igorpoint _DimSizeY, double _DimOffsetY, double _DimDeltaY, std::string& _DimUnitY,
		type_igorpoint _DimSizeZ, double _DimOffsetZ, double _DimDeltaZ, std::string& _DimUnitZ
	);
	template Wave_3D_impl<__int32>;
	template std::unique_ptr<Wave_3D<__int32>> Wave_Factory::make(
		std::vector<__int32>&& _ArrayFromITX, std::string& _WaveName,
		type_igorpoint _DimSizeX, double _DimOffsetX, double _DimDeltaX, std::string& _DimUnitX,
		type_igorpoint _DimSizeY, double _DimOffsetY, double _DimDeltaY, std::string& _DimUnitY,
		type_igorpoint _DimSizeZ, double _DimOffsetZ, double _DimDeltaZ, std::string& _DimUnitZ
	);
	template Wave_3D_impl<unsigned __int32>;
	template std::unique_ptr<Wave_3D<unsigned __int32>> Wave_Factory::make(
		std::vector<unsigned __int32>&& _ArrayFromITX, std::string& _WaveName,
		type_igorpoint _DimSizeX, double _DimOffsetX, double _DimDeltaX, std::string& _DimUnitX,
		type_igorpoint _DimSizeY, double _DimOffsetY, double _DimDeltaY, std::string& _DimUnitY,
		type_igorpoint _DimSizeZ, double _DimOffsetZ, double _DimDeltaZ, std::string& _DimUnitZ
	);
	//template std::basic_ostream<char, std::char_traits<char>>& operator << (std::basic_ostream<char, std::char_traits<char>>& ss, const Wave_3D<double>& w);
	//template std::basic_ostream<char, std::char_traits<char>>& operator << (std::basic_ostream<char, std::char_traits<char>>& ss, const Wave_3D<__int32>& w);
	//template std::basic_ostream<char, std::char_traits<char>>& operator << (std::basic_ostream<char, std::char_traits<char>>& ss, const Wave_3D<unsigned __int32>& w);

	//4D
	template Wave_4D_impl<double>;
	template std::unique_ptr<Wave_4D<double>> Wave_Factory::make(
		std::vector<double>&& _ArrayFromITX, std::string& _WaveName,
		type_igorpoint _DimSizeX, double _DimOffsetX, double _DimDeltaX, std::string& _DimUnitX,
		type_igorpoint _DimSizeY, double _DimOffsetY, double _DimDeltaY, std::string& _DimUnitY,
		type_igorpoint _DimSizeZ, double _DimOffsetZ, double _DimDeltaZ, std::string& _DimUnitZ,
		type_igorpoint _DimSizeT, double _DimOffsetT, double _DimDeltaT, std::string& _DimUnitT
	);
	template Wave_4D_impl<__int32>;
	template std::unique_ptr<Wave_4D<__int32>> Wave_Factory::make(
		std::vector<__int32>&& _ArrayFromITX, std::string& _WaveName,
		type_igorpoint _DimSizeX, double _DimOffsetX, double _DimDeltaX, std::string& _DimUnitX,
		type_igorpoint _DimSizeY, double _DimOffsetY, double _DimDeltaY, std::string& _DimUnitY,
		type_igorpoint _DimSizeZ, double _DimOffsetZ, double _DimDeltaZ, std::string& _DimUnitZ,
		type_igorpoint _DimSizeT, double _DimOffsetT, double _DimDeltaT, std::string& _DimUnitT
	);
	template Wave_4D_impl<unsigned __int32>;
	template std::unique_ptr<Wave_4D<unsigned __int32>> Wave_Factory::make(
		std::vector<unsigned __int32>&& _ArrayFromITX, std::string& _WaveName,
		type_igorpoint _DimSizeX, double _DimOffsetX, double _DimDeltaX, std::string& _DimUnitX,
		type_igorpoint _DimSizeY, double _DimOffsetY, double _DimDeltaY, std::string& _DimUnitY,
		type_igorpoint _DimSizeZ, double _DimOffsetZ, double _DimDeltaZ, std::string& _DimUnitZ,
		type_igorpoint _DimSizeT, double _DimOffsetT, double _DimDeltaT, std::string& _DimUnitT
	);
	//template std::basic_ostream<char, std::char_traits<char>>& operator << (std::basic_ostream<char, std::char_traits<char>>& ss, const Wave_4D<double>& w);
	//template std::basic_ostream<char, std::char_traits<char>>& operator << (std::basic_ostream<char, std::char_traits<char>>& ss, const Wave_4D<__int32>& w);
	//template std::basic_ostream<char, std::char_traits<char>>& operator << (std::basic_ostream<char, std::char_traits<char>>& ss, const Wave_4D<unsigned __int32>& w);









} // IgorPro6


