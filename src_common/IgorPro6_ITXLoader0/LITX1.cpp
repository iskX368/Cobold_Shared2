

#include "pch.h" // use stdafx.h in Visual Studio 2017 and earlier
//#include <iostream>
#include <sstream>
#include <tuple>
#include <regex>
#include <fstream>
#include "Waves_impl.h"
#include <limits>




namespace IgorPro6 {
	
	//static class
	namespace ITXLoader {
		
		//テンプレートを仮想関数にして，インターフェース作りメンバへのアクセス確保したかったが無理だった．とはいえ素のクラスをヘッダに記述してメンバ変数を露出させたくもなかった．
		//記述なしにメンバ変数のような所有物をもたせるため，グローバル変数を採用．ついでにこのクラスはただ一つ存在してほしいので，メンバ変数，メンバ関数全てstaticでコンストラクタへのアクセスを禁じたクラスとする．
		//であるなら名前空間でヘッダに公開関数のみ書けば良し．
		
		//staticでこのファイルでのみ有効とする
		static int m_V_Flag = 0;
		static std::ifstream m_ifs;
		

		//型選択
		template<size_t N, typename T> struct ModelWave_N_T;
		template<typename T> struct ModelWave_N_T<1, T> {
			using type = Wave_1D<T>;
			using uptr_type = std::unique_ptr<type>;
			using Vuptr_type = std::vector<uptr_type>;
		};
		template<typename T> struct ModelWave_N_T<2, T> {
			using type = Wave_2D<T>;
			using uptr_type = std::unique_ptr<type>;
			using Vuptr_type = std::vector<uptr_type>;
		};
		template<typename T> struct ModelWave_N_T<3, T> {
			using type = Wave_3D<T>;
			using uptr_type = std::unique_ptr<type>;
			using Vuptr_type = std::vector<uptr_type>;
		};
		template<typename T> struct ModelWave_N_T<4, T> {
			using type = Wave_4D<T>;
			using uptr_type = std::unique_ptr<type>;
			using Vuptr_type = std::vector<uptr_type>;
		};

		template<typename T> struct F_T;
		template<>struct F_T<double> {
			static inline double str2num(std::string& str) { return std::strtod(str.c_str(), NULL); }
		};
		template<>struct F_T<__int32> {
			static inline __int32 str2num(std::string& str) { return static_cast<__int32>(std::strtol(str.c_str(), NULL, 10)); }
		};
		template<>struct F_T<unsigned __int32> {
			static inline unsigned __int32 str2num(std::string& str) { return static_cast<unsigned __int32>(std::strtoul(str.c_str(), NULL, 10)); }
		};

		//Vuptr型を作成
		template<size_t N, typename T> inline auto make_Vuptr() { return std::vector<typename ModelWave_N_T<N, T>::uptr_type>(); }

		//tupleを構築
		static auto storage = std::make_tuple
			<
			typename ModelWave_N_T<1, double>::Vuptr_type, typename ModelWave_N_T<2, double>::Vuptr_type, typename ModelWave_N_T<3, double>::Vuptr_type, typename ModelWave_N_T<4, double>::Vuptr_type,
			typename ModelWave_N_T<1, __int32>::Vuptr_type, typename ModelWave_N_T<2, __int32>::Vuptr_type, typename ModelWave_N_T<3, __int32>::Vuptr_type, typename ModelWave_N_T<4, __int32>::Vuptr_type,
			typename ModelWave_N_T<1, unsigned __int32>::Vuptr_type, typename ModelWave_N_T<2, unsigned __int32>::Vuptr_type, typename ModelWave_N_T<3, unsigned __int32>::Vuptr_type, typename ModelWave_N_T<4, unsigned __int32>::Vuptr_type
			>
			(
				make_Vuptr<1, double>(), make_Vuptr<2, double>(), make_Vuptr<3, double>(), make_Vuptr<4, double>(),
				make_Vuptr<1, __int32>(), make_Vuptr<2, __int32>(), make_Vuptr<3, __int32>(), make_Vuptr<4, __int32>(),
				make_Vuptr<1, unsigned __int32>(), make_Vuptr<2, unsigned __int32>(), make_Vuptr<3, unsigned __int32>(), make_Vuptr<4, unsigned __int32>()
				);
		//tuple要素を取得
		template<size_t N, typename T> inline auto& get_Vuptr() { return std::get<typename ModelWave_N_T<N, T>::Vuptr_type>(storage); }

		void ClearWaves() {
			//賢い書き方あるかもしれないけど，とりあえず羅列
			get_Vuptr<1, double>().clear();
			get_Vuptr<2, double>().clear();
			get_Vuptr<3, double>().clear();
			get_Vuptr<4, double>().clear();
			get_Vuptr<1, __int32>().clear();
			get_Vuptr<2, __int32>().clear();
			get_Vuptr<3, __int32>().clear();
			get_Vuptr<4, __int32>().clear();
			get_Vuptr<1, unsigned __int32>().clear();
			get_Vuptr<2, unsigned __int32>().clear();
			get_Vuptr<3, unsigned __int32>().clear();
			get_Vuptr<4, unsigned __int32>().clear();
		}

		template<size_t N, typename T> void push_back_wave(typename ModelWave_N_T<N, T>::uptr_type&& uptr2wave) {
			auto& vuptr = get_Vuptr<N, T>();
			vuptr.push_back(std::forward<typename ModelWave_N_T<N, T>::uptr_type>(uptr2wave));
		}



		int V_Flag() noexcept { return m_V_Flag; }

		template<size_t N, typename T> size_t NumOfWaves() {
			return get_Vuptr<N, T>().size();
		}

		template<size_t N, typename T> std::string& PrintWaves(std::string& dst) {
			auto& vuptr = get_Vuptr<N, T>();
			std::stringstream ss;
			ss << "======== " << typeid(T).name() << ", " << N << " ========\n";
			for (size_t i = 0; i < vuptr.size(); i++) {
				ss << "#--- " << i << " ---\n" << *(vuptr[i]) << "\n";
			}
			ss << "\n";
			dst = ss.str();
			return dst;
		}
		
		std::string& PrintAllWaves(std::string& dst) {
			dst = "";
			std::string a;
			//とりあえずべた書き
			dst.append(PrintWaves<1, double>(a));
			dst.append(PrintWaves<2, double>(a));
			dst.append(PrintWaves<3, double>(a));
			dst.append(PrintWaves<4, double>(a));
			dst.append(PrintWaves<1, __int32>(a));
			dst.append(PrintWaves<2, __int32>(a));
			dst.append(PrintWaves<3, __int32>(a));
			dst.append(PrintWaves<4, __int32>(a));
			dst.append(PrintWaves<1, unsigned __int32>(a));
			dst.append(PrintWaves<2, unsigned __int32>(a));
			dst.append(PrintWaves<3, unsigned __int32>(a));
			dst.append(PrintWaves<4, unsigned __int32>(a));
			return dst;
		}


		int ReadWaveDefinition(std::string& dst_WaveName, std::vector<type_igorpoint>& dst_DimSizeList, std::string& dst_FlagString, std::string& src_WaveDefLine) {
			size_t pos_tab = src_WaveDefLine.find_first_of('\t');
			std::string AllFlags = src_WaveDefLine.substr(0, pos_tab);
			size_t pos_N = AllFlags.find_last_of('N');

			//ウェーブ名を取得
			dst_WaveName = src_WaveDefLine.substr(pos_tab + 1, std::string::npos);

			std::string FlagOthers;
			std::smatch smch;
			if (pos_N != std::string::npos) {
				//多次元

				//フラグNを分離
				std::string FlagN = AllFlags.substr(pos_N - 1, std::string::npos);
				FlagOthers = AllFlags.substr(0, pos_N - 1);
				//std::cout << FlagN << "," << FlagOthers << "\n";////

				//次元を取得
				std::regex parttern_dimsizes = std::regex(R"(\d+)");
				auto itr_dimsize_beg = std::sregex_iterator(FlagN.begin(), FlagN.end(), parttern_dimsizes);
				auto itr_dimsize_end = std::sregex_iterator();
				dst_DimSizeList.clear();
				for (auto itr = itr_dimsize_beg; itr != itr_dimsize_end; itr++) {
					smch = *itr;
					//std::cout << smch.str() << "\n";////
					dst_DimSizeList.push_back(static_cast<type_igorpoint>(std::strtol(smch.str().c_str(), NULL, 10)));
				}

			}
			else {
				//一次元
				FlagOthers = AllFlags;
				dst_DimSizeList.clear();
				dst_DimSizeList.push_back(0);//後で取得すること
			}

			//次元以外のフラグを取得
			dst_FlagString.clear();
			FlagOthers.replace(FlagOthers.begin(), FlagOthers.begin() + 5, "");//WAVESを削除
			if (FlagOthers == "") {
				//single(float)
				dst_FlagString = "/F";
			}
			else {
				dst_FlagString = FlagOthers;
			}

			return 0;
		}

		template<typename T, class U = typename std::enable_if<std::is_arithmetic<T>::value, std::nullptr_t>::type>
		int CreateAWave(const char eofLine, std::string& WaveName, std::vector<type_igorpoint>& DimSizeList) {
			if (!m_ifs) {
				return 1;
			}

			const T xnan = std::numeric_limits<T>::quiet_NaN();
			const T ninf = std::numeric_limits<T>::min();
			const T pinf = std::numeric_limits<T>::max();
			std::string line, elm;
			std::stringstream ss;

			type_igorpoint x_dimsize;
			double x_dimoffset;
			double x_dimdelta;
			std::string x_unit;

			type_igorpoint y_dimsize;
			double y_dimoffset;
			double y_dimdelta;
			std::string y_unit;

			type_igorpoint z_dimsize;
			double z_dimoffset;
			double z_dimdelta;
			std::string z_unit;

			type_igorpoint t_dimsize;
			double t_dimoffset;
			double t_dimdelta;
			std::string t_unit;

			std::vector<T> Array;

			int gate = 0;
			while (std::getline(m_ifs, line, eofLine)) {

				if (gate == 0) {
					if (line == "BEGIN") {
						gate = 1;
					}
				}
				else if (gate == 1) {
					if (line == "END") {
						gate = 2;

						//1Dならサイズをここで取得
						if (DimSizeList[0] == 0) {
							DimSizeList[0] = static_cast<type_igorpoint>(Array.size());
						}
						
					}
					else {
						//itxの数値を取得

						//容量確保
						if (DimSizeList.size() == 1) {
							Array.reserve(100);//適当
						}
						else if (DimSizeList.size() == 2) {
							Array.reserve(DimSizeList[0] * DimSizeList[1]);
						}
						else if (DimSizeList.size() == 3) {
							Array.reserve(DimSizeList[0] * DimSizeList[1] * DimSizeList[2]);
						}
						else if (DimSizeList.size() == 4) {
							Array.reserve(DimSizeList[0] * DimSizeList[1] * DimSizeList[2] * DimSizeList[3]);
						}

						ss << line;
						while (std::getline(ss, elm, '\t')) {
							if (elm == "") {
								continue;
							}
							else if (elm == "NAN") {
								Array.push_back(xnan);
							}
							else if (elm == "-inf") {
								Array.push_back(ninf);
							}
							else if (elm == "inf") {
								Array.push_back(pinf);
							}
							else {
								Array.push_back(F_T<T>::str2num(elm));
							}
						}
						ss.clear();

					}

				}
				else if (gate == 2) {
					gate++;
					//軸スケール情報を取得
					std::regex regex_SetScale = std::regex(R"(SetScale/P ([xyzt]) ([+-]?\d+(?:\.\d+)?),([+-]?\d+(?:\.\d+)?),\"(\w*)\", \w+;?)");

					auto words_begin = std::sregex_iterator(line.begin(), line.end(), regex_SetScale);
					auto words_end = std::sregex_iterator();
					for (auto itr = words_begin; itr != words_end; itr++) {
						std::smatch smch = *itr;

						if (smch[1].str() == "x") {
							x_dimsize = DimSizeList[0];
							x_dimoffset = std::atof(smch[2].str().c_str());
							x_dimdelta = std::atof(smch[3].str().c_str());
							x_unit = smch[4].str();
						}
						else if (smch[1].str() == "y") {
							y_dimsize = DimSizeList[1];
							y_dimoffset = std::atof(smch[2].str().c_str());
							y_dimdelta = std::atof(smch[3].str().c_str());
							y_unit = smch[4].str();
						}
						else if (smch[1].str() == "z") {
							z_dimsize = DimSizeList[2];
							z_dimoffset = std::atof(smch[2].str().c_str());
							z_dimdelta = std::atof(smch[3].str().c_str());
							z_unit = smch[4].str();
						}
						else if (smch[1].str() == "t") {
							t_dimsize = DimSizeList[3];
							t_dimoffset = std::atof(smch[2].str().c_str());
							t_dimdelta = std::atof(smch[3].str().c_str());
							t_unit = smch[4].str();
						}
					}
				}

				if (gate == 3) {
					gate++;
					//waveを作成
					if (DimSizeList.size() == 1) {
						//1D
						push_back_wave<1, T>(Wave_Factory::make<T>(std::move(Array), WaveName, x_dimsize, x_dimoffset, x_dimdelta, x_unit));
					}
					else if (DimSizeList.size() == 2) {
						//2D
						push_back_wave<2, T>(Wave_Factory::make<T>(std::move(Array), WaveName, x_dimsize, x_dimoffset, x_dimdelta, x_unit, y_dimsize, y_dimoffset, y_dimdelta, y_unit));
					}
					else if (DimSizeList.size() == 3) {
						//3D
						push_back_wave<3, T>(Wave_Factory::make<T>(std::move(Array), WaveName, x_dimsize, x_dimoffset, x_dimdelta, x_unit, y_dimsize, y_dimoffset, y_dimdelta, y_unit, z_dimsize, z_dimoffset, z_dimdelta, z_unit));
					}
					else if (DimSizeList.size() == 4) {
						//4D
						push_back_wave<4, T>(Wave_Factory::make<T>(std::move(Array), WaveName, x_dimsize, x_dimoffset, x_dimdelta, x_unit, y_dimsize, y_dimoffset, y_dimdelta, y_unit, z_dimsize, z_dimoffset, z_dimdelta, z_unit, t_dimsize, t_dimoffset, t_dimdelta, t_unit));
					}

				}
				
				if(gate==4){
					break;
				}

			} // while


			if (gate != 4) {
				return 2;
			}

			return 0;
		}

		template<typename T> std::vector<std::unique_ptr<Wave_1D<T>>> MoveWaves_1D() {
			auto tobemoved = make_Vuptr<1, T>();
			std::swap(get_Vuptr<1, T>(), tobemoved);
			return std::move(tobemoved);
		}
		template<typename T> std::vector<std::unique_ptr<Wave_2D<T>>> MoveWaves_2D() {
			auto tobemoved = make_Vuptr<2, T>();
			std::swap(get_Vuptr<2, T>(), tobemoved);
			return std::move(tobemoved);
		}
		template<typename T> std::vector<std::unique_ptr<Wave_3D<T>>> MoveWaves_3D() {
			auto tobemoved = make_Vuptr<3, T>();
			std::swap(get_Vuptr<3, T>(), tobemoved);
			return std::move(tobemoved);
		}
		template<typename T> std::vector<std::unique_ptr<Wave_4D<T>>> MoveWaves_4D() {
			auto tobemoved = make_Vuptr<4, T>();
			std::swap(get_Vuptr<4, T>(), tobemoved);
			return std::move(tobemoved);
		}





		int LoadWaves(std::string& filename, const char eofLine) {
			if (m_ifs) {
				m_ifs.close();
			}
			
			m_ifs.open(filename);

			if (m_ifs.fail()) {
				m_V_Flag = static_cast<int>(V_Flag::FileOpenFailure);
				return 1;
			}

			std::string line;

			std::getline(m_ifs, line, eofLine);
			if (line != "IGOR") {
				//std::cout << line << "\n";
				m_V_Flag = static_cast<int>(V_Flag::NotITX);
				m_ifs.close();
				return 1;
			}

			std::string WaveName;
			std::vector<type_igorpoint> DimSizeList;
			std::string FlagString;
			int DataType = 0;
			int Load_Failed = 0;
			std::regex parttern_defineWave = std::regex(R"(^WAVES(/[A-M|O-Z])*(/N=\((?:(\d+),)*(\d+)\))?\t(\w+)$)");

			while (std::getline(m_ifs, line, eofLine)) {
				if (std::regex_match(line, parttern_defineWave)) {

					ReadWaveDefinition(WaveName, DimSizeList, FlagString, line);

					//
					/*std::cout << WaveName << "\n" << FlagString << "\n";
					for (auto itr = DimSizeList.begin(); itr != DimSizeList.end(); itr++) {
						std::cout << *itr << "\t";
					}
					std::cout << "\n-------------\n";*/
					//


					__int32 ret = -1;
					if (FlagString == "/D" || FlagString == "/F") {
						ret = CreateAWave<double>(eofLine, WaveName, DimSizeList);
					}
					else if (FlagString == "/I" || FlagString == "/W" || FlagString == "/B") {
						ret = CreateAWave<__int32>(eofLine, WaveName, DimSizeList);
					}
					else if (FlagString == "/I/U" || FlagString == "/B/U" || FlagString == "/W/U") {
						ret = CreateAWave<unsigned __int32>(eofLine, WaveName, DimSizeList);
					}


					if (ret < 0) {
						//std::cout << "Unknown Flags : " << FlagString << "\n";
						Load_Failed = 1;
					}
					else if (ret > 0) {
						//std::cout << "WaveName : " << WaveName << ", Error in CreateAWave : " << ret << "\n";
						Load_Failed = 2;
					}
					else {
						//std::cout << WaveName << " " << FlagString << " Loaded\n";
					}



				}
			}

			m_ifs.close();

			if (Load_Failed == 1) {
				m_V_Flag = static_cast<int>(V_Flag::UnknownWaveFlag);
				return 1;
			}
			else if (Load_Failed  > 1) {
				m_V_Flag = static_cast<int>(V_Flag::NonLoadedWaveExists);
				return 1;
			}
			else {
				m_V_Flag = static_cast<int>(V_Flag::Success);
				return 0;
			}

		}


		//明示的インスタンス化
		/*template size_t NumOfWaves<1, double>();
		template size_t NumOfWaves<2, double>();
		template size_t NumOfWaves<3, double>();
		template size_t NumOfWaves<4, double>();
		template std::string& PrintWaves<1, double>(std::string& dst);
		template std::string& PrintWaves<2, double>(std::string& dst);
		template std::string& PrintWaves<3, double>(std::string& dst);
		template std::string& PrintWaves<4, double>(std::string& dst);
		template std::vector<std::unique_ptr<Wave_1D<double>>> MoveWaves_1D();
		template std::vector<std::unique_ptr<Wave_2D<double>>> MoveWaves_2D();
		template std::vector<std::unique_ptr<Wave_3D<double>>> MoveWaves_3D();
		template std::vector<std::unique_ptr<Wave_4D<double>>> MoveWaves_4D();

		template size_t NumOfWaves<1, __int32>();
		template size_t NumOfWaves<2, __int32>();
		template size_t NumOfWaves<3, __int32>();
		template size_t NumOfWaves<4, __int32>();
		template std::string& PrintWaves<1, __int32>(std::string& dst);
		template std::string& PrintWaves<2, __int32>(std::string& dst);
		template std::string& PrintWaves<3, __int32>(std::string& dst);
		template std::string& PrintWaves<4, __int32>(std::string& dst);
		template std::vector<std::unique_ptr<Wave_1D<__int32>>> MoveWaves_1D();
		template std::vector<std::unique_ptr<Wave_2D<__int32>>> MoveWaves_2D();
		template std::vector<std::unique_ptr<Wave_3D<__int32>>> MoveWaves_3D();
		template std::vector<std::unique_ptr<Wave_4D<__int32>>> MoveWaves_4D();

		template size_t NumOfWaves<1, unsigned __int32>();
		template size_t NumOfWaves<2, unsigned __int32>();
		template size_t NumOfWaves<3, unsigned __int32>();
		template size_t NumOfWaves<4, unsigned __int32>();
		template std::string& PrintWaves<1, unsigned __int32>(std::string& dst);
		template std::string& PrintWaves<2, unsigned __int32>(std::string& dst);
		template std::string& PrintWaves<3, unsigned __int32>(std::string& dst);
		template std::string& PrintWaves<4, unsigned __int32>(std::string& dst);
		template std::vector<std::unique_ptr<Wave_1D<unsigned __int32>>> MoveWaves_1D();
		template std::vector<std::unique_ptr<Wave_2D<unsigned __int32>>> MoveWaves_2D();
		template std::vector<std::unique_ptr<Wave_3D<unsigned __int32>>> MoveWaves_3D();
		template std::vector<std::unique_ptr<Wave_4D<unsigned __int32>>> MoveWaves_4D();*/


	}





}