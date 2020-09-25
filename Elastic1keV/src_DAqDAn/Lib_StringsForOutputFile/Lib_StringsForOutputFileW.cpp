//// Lib_StringsForOutputFile.cpp : スタティック ライブラリ用の関数を定義します。
////
//
#include "pch.h"
#include "framework.h"
//
//#include "Lib_StringsForOutputFile.h"
//
//#include <sstream>
//#include <iomanip>
//#include <time.h>
//#include <vector>
//#include <tuple>
//
//
////IgorProウェーブ名称長さ最大値
//constexpr unsigned __int32 Igor_WaveName_Max_Length = 31;
//
//
//
//
//
//class StringsForOutputFileW_impl :public StringsForOutputFileW {
//public:
//	using tstringstream = std::basic_stringstream<type_char>;
//
//
//
//	StringsForOutputFileW_impl() = delete;
//
//	StringsForOutputFileW_impl(const type_char sep, std::vector<std::tuple<tstring, __int32, size_t>>&& dataIndex, std::vector<__int32*>&& Ad_int32, std::vector<__int64*>&& Ad_int64, std::vector<double*>&& Ad_double)
//		:
//		m_sep(sep),
//		m_DataIndex(std::forward<std::vector<std::tuple<tstring, __int32, size_t>>>(dataIndex)),
//		m_Ad_int32(std::forward<std::vector<__int32*>>(Ad_int32)),
//		m_Ad_int64(std::forward<std::vector<__int64*>>(Ad_int64)),
//		m_Ad_double(std::forward<std::vector<double*>>(Ad_double)),
//		m_ValueStringFlag(0)
//	{}
//
//
//	virtual ~StringsForOutputFileW_impl() = default;
//
//	virtual __int32 GetHeaderString(tstring& Dst)const noexcept {
//
//		tstringstream tss;
//		tstring tstr;
//		__int32 ret = 0;
//		//日時
//		if (GetTimeString(tstr) == 0) {
//			tss << tstr << std::endl;
//		}
//		else {
//			tss << L"Check \"GetTimeString\"" << std::endl;
//			ret += 1;
//		}
//
//		//ソースLMFパス
//		try {
//			tstr = CDI::SourceLMFilePath();
//		}
//		catch (std::exception&) {
//			tstr = L"Check \"CurrentDAqInfo::SourceLMFilePath\"";
//			ret += 2;
//		}
//		tss << tstr << std::endl;
//
//		Dst.clear();
//		Dst.append(tss.str().c_str());
//
//		return ret;
//	}
//
//	virtual __int32 GetCoordinatesString(tstring& Dst)const noexcept {
//
//		tstringstream tss;
//
//		for (const auto& dataIndex : m_DataIndex) {
//			tss << std::get<0>(dataIndex) << m_sep;
//		}
//
//		tss << std::endl;
//
//		Dst.clear();
//		Dst.append(tss.str().c_str());
//
//		return 0;
//	}
//
//	virtual __int32 GetValuesString(tstring& Dst)noexcept {
//
//		//もし既にエラーが出ている場合は何もしない
//		if (m_ValueStringFlag != 0) {
//			return m_ValueStringFlag;
//		}
//
//		try {
//			tstringstream tss;
//			type_char temp[512];
//
//			for (const auto& dataIndex : m_DataIndex) {
//
//				//型を判別
//				switch (std::get<1>(dataIndex))
//				{
//				case CPPA_CCFC::Data_Index_int32:
//					//__int32
//					swprintf_s(temp, 512, L"%I32d%c", *m_Ad_int32.at(std::get<2>(dataIndex)), m_sep);
//					tss << temp;
//					break;
//				case CPPA_CCFC::Data_Index_int64:
//					//__int64
//					swprintf_s(temp, 512, L"%I64d%c", *m_Ad_int64.at(std::get<2>(dataIndex)), m_sep);
//					tss << temp;
//					break;
//				case CPPA_CCFC::Data_Index_double:
//					//double
//					swprintf_s(temp, 512, L"%lf%c", *m_Ad_double.at(std::get<2>(dataIndex)), m_sep);
//					tss << temp;
//					break;
//				default:
//					//見つからなかった
//					char emsg[256];
//					sprintf_s(emsg, "StringsForOutputFileA::GetValuesString : Invalid DataIndex tuple<1> configuration");
//					throw std::exception(emsg);
//					break;
//				}
//
//			}
//
//			tss << std::endl;
//
//			Dst.clear();
//			Dst.append(tss.str().c_str());
//
//			m_ValueStringFlag = 0;
//
//		}
//		catch (std::out_of_range&) {
//			m_ValueStringFlag = 1;
//		}
//		catch (std::exception&) {
//			m_ValueStringFlag = 2;
//		}
//		catch (...) {
//			m_ValueStringFlag = 3;
//		}
//
//		return m_ValueStringFlag;
//	}
//
//private:
//
//	//日時を取得
//	static __int32 GetTimeString(tstring& Dst) {
//
//		//struct tm newtime;
//		//__time64_t long_time;
//
//		//type_char am_pm[] = L"AM";
//		//type_char timebuf[26];
//
//		//errno_t err;
//
//		//// Get time as 64-bit integer.
//		//_time64(&long_time);
//		//// Convert to local time.
//		//err = _localtime64_s(&newtime, &long_time);
//		//if (err)
//		//{
//		//	return 1;
//		//}
//		//if (newtime.tm_hour > 12)        // Set up extension.
//		//	wcscpy_s(am_pm, sizeof(am_pm) / sizeof(type_char), L"PM");
//		//if (newtime.tm_hour > 12)        // Convert from 24-hour
//		//	newtime.tm_hour -= 12;        // to 12-hour clock.
//		//if (newtime.tm_hour == 0)        // Set hour to 12 if midnight.
//		//	newtime.tm_hour = 12;
//
//		//// Convert to an ASCII representation.
//		//err = _wasctime_s(timebuf, 26, &newtime);
//		//if (err)
//		//{
//		//	return 1;
//		//}
//
//		//Dst.clear();
//		//Dst += timebuf;
//		//Dst += L" ";
//		//Dst += am_pm;
//
//		struct tm newtime;
//		__time64_t long_time;
//
//		_time64(&long_time);
//
//		// Convert to local time.
//		errno_t err = _localtime64_s(&newtime, &long_time);
//
//		tstringstream ss;
//
//		ss << std::put_time(&newtime, L"%Y/%m/%d  %H:%M:%S");//"%d-%m-%Y %H-%M-%S"
//
//		Dst.clear();
//		Dst = ss.str();
//
//		return 0;
//	}
//
//
//	//セパレータ
//	type_char m_sep;
//
//	//Coordinateと型とインデックス
//	std::vector<std::tuple<tstring, __int32, size_t>> m_DataIndex;
//
//	//アドレス
//	std::vector<__int32*> m_Ad_int32;
//	std::vector<__int64*> m_Ad_int64;
//	std::vector<double*> m_Ad_double;
//
//	//読み取り成功フラグ
//	__int32 m_ValueStringFlag;
//
//};
//
//
//std::unique_ptr<StringsForOutputFileW> StringsForOutputFileW::make(std::initializer_list<StringsForOutputFile_Flags::Flag_Coordinate> List_FlagCoordinate, const type_char Separator) {
//	try {
//
//		tstring nam;
//		__int32 di;
//
//		//目的
//		std::vector<std::tuple<tstring, __int32, size_t>> vdi(0);
//		std::vector<__int32*> vad_int32(0);
//		std::vector<__int64*> vad_int64(0);
//		std::vector<double*> vad_double(0);
//
//		//カウンタ
//		size_t i_int32 = 0;
//		size_t i_int64 = 0;
//		size_t i_double = 0;
//
//		//enumを文字列へ変換
//		std::vector<tstring> flagCoordinates;
//		flagCoordinates.reserve(List_FlagCoordinate.size());
//
//		for (const auto& flag : List_FlagCoordinate) {
//
//			//こうやって掃引するとNAMEOF_ENUMは成功する
//			for (type_Flag f = Flag_Coordinate_Min; f <= Flag_Coordinate_Max; f++) {
//				StringsForOutputFile_Flags::Flag_Coordinate ff = static_cast<StringsForOutputFile_Flags::Flag_Coordinate>(f);
//				if (flag == ff) {
//					//wchar_tへ変換
//					std::string nam(NAMEOF_ENUM(ff));
//					wchar_t wbuf[100];
//					size_t ReturnValue;
//					mbstowcs_s(&ReturnValue, wbuf, nam.c_str(), nam.length());
//					flagCoordinates.push_back(tstring(wbuf));
//				}
//			}
//
//		}
//
//		//要素を掃引
//		for (const auto& nam : flagCoordinates) {
//
//			//型を検査
//			di = CPPA_CCFC::DataIndex(nam);
//
//			//見つかったらアドレスと位置を記録
//			switch (di)
//			{
//			case CPPA_CCFC::Data_Index_int32:
//				vdi.push_back(std::tuple<tstring, __int32, size_t>(nam, di, i_int32));
//				vad_int32.push_back(std::get<CPPA_CCFC::Data_Index_int32>(CPPA_CCFC::Get()).at(nam));
//				i_int32++;
//				break;
//			case CPPA_CCFC::Data_Index_int64:
//				vdi.push_back(std::tuple<tstring, __int32, size_t>(nam, di, i_int64));
//				vad_int64.push_back(std::get<CPPA_CCFC::Data_Index_int64>(CPPA_CCFC::Get()).at(nam));
//				i_int64++;
//				break;
//			case CPPA_CCFC::Data_Index_double:
//				vdi.push_back(std::tuple<tstring, __int32, size_t>(nam, di, i_double));
//				vad_double.push_back(std::get<CPPA_CCFC::Data_Index_double>(CPPA_CCFC::Get()).at(nam));
//				i_double++;
//				break;
//			default:
//				//不明な型
//				char temp[512];
//				sprintf_s(temp, "StringsForOutputFileA::make : Failure in finding Bit_Coordinate name \"%ls\"", nam.c_str());
//				throw std::invalid_argument(temp);
//				break;
//			}
//		}
//
//		//ここまで異常がないなら作成
//		return std::make_unique<StringsForOutputFileW_impl>(Separator, std::move(vdi), std::move(vad_int32), std::move(vad_int64), std::move(vad_double));
//
//	}
//	catch (std::invalid_argument&) {
//		throw;
//	}
//	catch (std::out_of_range&) {
//		throw;
//	}
//	catch (...) {
//		throw;
//	}
//}
//
//
//std::unique_ptr<StringsForOutputFileW> StringsForOutputFileW::make(const type_Flag BitFlags_Coordinate, const type_char Separator) {
//	try {
//
//		tstring nam;
//		__int32 di;
//
//		//目的
//		std::vector<std::tuple<tstring, __int32, size_t>> vdi(0);
//		std::vector<__int32*> vad_int32(0);
//		std::vector<__int64*> vad_int64(0);
//		std::vector<double*> vad_double(0);
//
//		//カウンタ
//		size_t i_int32 = 0;
//		size_t i_int64 = 0;
//		size_t i_double = 0;
//
//		//ビットフラグをenum文字列へ変換
//		std::vector<tstring> flagCoordinates;
//
//		type_Flag x;
//		//こうやって掃引するとNAMEOF_ENUMは成功する
//		for (type_Flag f = Flag_Coordinate_Min; f <= Flag_Coordinate_Max; f++) {
//			StringsForOutputFile_Flags::Flag_Coordinate ff = static_cast<StringsForOutputFile_Flags::Flag_Coordinate>(f);
//
//			x = 1 << f;
//
//			if ((BitFlags_Coordinate & x) != 0) {
//				//wchar_tへ変換
//				std::string nam(NAMEOF_ENUM(ff));
//				wchar_t wbuf[100];
//				size_t ReturnValue;
//				mbstowcs_s(&ReturnValue, wbuf, nam.c_str(), nam.length());
//				flagCoordinates.push_back(tstring(wbuf));
//			}
//
//		}
//
//		//要素を掃引
//		for (const auto& nam : flagCoordinates) {
//
//			//型を検査
//			di = CPPA_CCFC::DataIndex(nam);
//
//			//見つかったらアドレスと位置を記録
//			switch (di)
//			{
//			case CPPA_CCFC::Data_Index_int32:
//				vdi.push_back(std::tuple<tstring, __int32, size_t>(nam, di, i_int32));
//				vad_int32.push_back(std::get<CPPA_CCFC::Data_Index_int32>(CPPA_CCFC::Get()).at(nam));
//				i_int32++;
//				break;
//			case CPPA_CCFC::Data_Index_int64:
//				vdi.push_back(std::tuple<tstring, __int32, size_t>(nam, di, i_int64));
//				vad_int64.push_back(std::get<CPPA_CCFC::Data_Index_int64>(CPPA_CCFC::Get()).at(nam));
//				i_int64++;
//				break;
//			case CPPA_CCFC::Data_Index_double:
//				vdi.push_back(std::tuple<tstring, __int32, size_t>(nam, di, i_double));
//				vad_double.push_back(std::get<CPPA_CCFC::Data_Index_double>(CPPA_CCFC::Get()).at(nam));
//				i_double++;
//				break;
//			default:
//				//不明な型
//				char temp[512];
//				sprintf_s(temp, "StringsForOutputFileA::make : Failure in finding Bit_Coordinate name \"%ls\"", nam.c_str());
//				throw std::invalid_argument(temp);
//				break;
//			}
//		}
//
//		//ここまで異常がないなら作成
//		return std::make_unique<StringsForOutputFileW_impl>(Separator, std::move(vdi), std::move(vad_int32), std::move(vad_int64), std::move(vad_double));
//
//	}
//	catch (std::invalid_argument&) {
//		throw;
//	}
//	catch (std::out_of_range&) {
//		throw;
//	}
//	catch (...) {
//		throw;
//	}
//}
//
//
//
//
//
//
//__int32 StringsForIgorTextWaveW::WaveText(tstring& Dst, const tstring& Name_ValueWave, const tstring& Name_ScaleWave, const std::vector<double>& Vec, const ScaleInformation& ScaleInfo) {
//	throw std::exception("StringsForIgorTextWaveW::WaveText : Not yet implemented");
//	return 0;
//}
//
//
//__int32 StringsForIgorTextWaveW::WaveText(tstring& Dst, const tstring& Name_ValueWave, const tstring& Name_ScaleWave0, const tstring& Name_ScaleWave1, const std::vector<std::vector<double>>& Vec, const ScaleInformation& ScaleInfo0, const ScaleInformation& ScaleInfo1) {
//	throw std::exception("StringsForIgorTextWaveW::WaveText : Not yet implemented");
//	return 0;
//}