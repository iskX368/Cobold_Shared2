// Lib_StringsForOutputFile.cpp : スタティック ライブラリ用の関数を定義します。
//

#include "pch.h"
#include "framework.h"

#include "Lib_StringsForOutputFile.h"

#include <sstream>
#include <iomanip>
#include <time.h>
#include <vector>
#include <tuple>

#include <ctime>
#include <type_traits>


//IgorProウェーブ名称長さ最大値
constexpr unsigned __int32 Igor_WaveName_Max_Length = 31;


void GetWaveName(StringsForIgorTextWaveA::tstring& Dst, const StringsForIgorTextWaveA::tstring& SourceName) {
	try {

		//特殊文字をウェーブ名称に含む場合に備えて''で囲む
		//文字数が長すぎる場合は上限でカット

		if (SourceName.length() > Igor_WaveName_Max_Length - 2) {
			Dst = "'" + StringsForIgorTextWaveA::tstring(SourceName.begin(), SourceName.begin() + Igor_WaveName_Max_Length - 2) + "'";
		}
		else {
			Dst = "'" + SourceName + "'";
		}

	}
	catch (std::exception&) {
		throw;
	}
}



class StringsForOutputFileA_impl :public StringsForOutputFileA {
public:
	using tstringstream = std::basic_stringstream<type_char>;
	


	StringsForOutputFileA_impl() = delete;

	StringsForOutputFileA_impl(const type_char sep, std::vector<std::tuple<tstring, __int32, size_t>>&& dataIndex, std::vector<__int32*>&& Ad_int32, std::vector<__int64*>&& Ad_int64, std::vector<double*>&& Ad_double)
		:
		m_sep(sep),
		m_DataIndex(std::forward<std::vector<std::tuple<tstring, __int32, size_t>>>(dataIndex)),
		m_Ad_int32(std::forward<std::vector<__int32*>>(Ad_int32)),
		m_Ad_int64(std::forward<std::vector<__int64*>>(Ad_int64)),
		m_Ad_double(std::forward<std::vector<double*>>(Ad_double)),
		m_ValueStringFlag(0)
	{}


	virtual ~StringsForOutputFileA_impl() = default;

	virtual __int32 GetHeaderString(tstring& Dst)const noexcept {
		
		tstringstream tss;
		tstring tstr;
		__int32 ret = 0;
		//日時
		if (GetTimeString(tstr) == 0) {
			tss << tstr << std::endl;
		}
		else {
			tss << "Check \"GetTimeString\"" << std::endl;
			ret += 1;
		}

		//ソースLMFパス
		try {
			tstr = CDI::SourceLMFilePath();
		}
		catch (std::exception&) {
			tstr = "Check \"CurrentDAqInfo::SourceLMFilePath\"";
			ret += 2;
		}
		tss << tstr;
		
		
		//tss<< std::endl

		Dst.clear();
		Dst.append(tss.str().c_str());

		return ret;
	}

	virtual __int32 GetCoordinatesString(tstring& Dst)const noexcept {
		
		tstringstream tss;

		for (const auto& dataIndex : m_DataIndex) {
			tss << std::get<0>(dataIndex) << m_sep;
		}

		//tss << std::endl;

		Dst.clear();
		Dst.append(tss.str().c_str());

		return 0;
	}

	virtual __int32 GetValuesString(tstring& Dst)noexcept {
		
		//もし既にエラーが出ている場合は何もしない
		if (m_ValueStringFlag != 0) {
			return m_ValueStringFlag;
		}

		try {
			tstringstream tss;
			type_char temp[512];

			for (const auto& dataIndex : m_DataIndex) {

				//型を判別
				switch (std::get<1>(dataIndex))
				{
				case CPPA_CCFC::Data_Index_int32:
					//__int32
					sprintf_s(temp, "%I32d%c", *m_Ad_int32.at(std::get<2>(dataIndex)), m_sep);
					tss << temp;
					break;
				case CPPA_CCFC::Data_Index_int64:
					//__int64
					sprintf_s(temp, "%I64d%c", *m_Ad_int64.at(std::get<2>(dataIndex)), m_sep);
					tss << temp;
					break;
				case CPPA_CCFC::Data_Index_double:
					//double
					sprintf_s(temp, "%lf%c", *m_Ad_double.at(std::get<2>(dataIndex)), m_sep);
					tss << temp;
					break;
				default:
					//見つからなかった
					sprintf_s(temp, "StringsForOutputFileA::GetValuesString : Invalid DataIndex tuple<1> configuration");
					throw std::exception(temp);
					break;
				}

			}

			//tss << std::endl;

			Dst.clear();
			Dst.append(tss.str().c_str());

			m_ValueStringFlag = 0;

		}
		catch (std::out_of_range&) {
			m_ValueStringFlag = 1;
		}
		catch (std::exception&) {
			m_ValueStringFlag = 2;
		}
		catch (...) {
			m_ValueStringFlag = 3;
		}

		return m_ValueStringFlag;
	}

private:


	//日時を取得
	static __int32 GetTimeString(tstring& Dst) {
		
		//struct tm newtime;
		//__time64_t long_time;
		//
		//type_char am_pm[] = "AM";
		//type_char timebuf[26];
		//
		//errno_t err;

		//// Get time as 64-bit integer.
		//_time64(&long_time);
		//// Convert to local time.
		//err = _localtime64_s(&newtime, &long_time);
		//if (err)
		//{
		//	return 1;
		//}
		//if (newtime.tm_hour > 12)        // Set up extension.
		//	strcpy_s(am_pm, sizeof(am_pm) / sizeof(type_char), "PM");
		//if (newtime.tm_hour > 12)        // Convert from 24-hour
		//	newtime.tm_hour -= 12;        // to 12-hour clock.
		//if (newtime.tm_hour == 0)        // Set hour to 12 if midnight.
		//	newtime.tm_hour = 12;

		//// Convert to an ASCII representation.
		//err = asctime_s(timebuf, 26, &newtime);
		//if (err)
		//{
		//	return 1;
		//}

		//Dst.clear();
		//Dst += timebuf;
		//Dst += " ";
		//Dst += am_pm;

		struct tm newtime;
		__time64_t long_time;

		_time64(&long_time);

		// Convert to local time.
		errno_t err = _localtime64_s(&newtime, &long_time);

		tstringstream ss;

		ss << std::put_time(&newtime, "%Y/%m/%d  %H:%M:%S");//"%d-%m-%Y %H-%M-%S"

		Dst.clear();
		Dst = ss.str();

		return 0;
	}

	//セパレータ
	type_char m_sep;

	//Coordinateと型とインデックス
	std::vector<std::tuple<tstring, __int32, size_t>> m_DataIndex;

	//アドレス
	std::vector<__int32*> m_Ad_int32;
	std::vector<__int64*> m_Ad_int64;
	std::vector<double*> m_Ad_double;

	//読み取り成功フラグ
	__int32 m_ValueStringFlag;

};


std::unique_ptr<StringsForOutputFileA> StringsForOutputFileA::make(std::initializer_list<LibFlag::Coordinate> List_FlagCoordinate, const type_char Separator) {
	try {

		tstring nam;
		__int32 di;

		//目的
		std::vector<std::tuple<tstring, __int32, size_t>> vdi(0);
		std::vector<__int32*> vad_int32(0);
		std::vector<__int64*> vad_int64(0);
		std::vector<double*> vad_double(0);

		//カウンタ
		size_t i_int32 = 0;
		size_t i_int64 = 0;
		size_t i_double = 0;

		//enumを文字列へ変換
		std::vector<tstring> flagCoordinates;
		flagCoordinates.reserve(List_FlagCoordinate.size());
		
		for (const auto& flag : List_FlagCoordinate) {
			
			//こうやって掃引するとNAMEOF_ENUMは成功する
			/*for (type_Flag f = Flag_Coordinate_Min; f <= Flag_Coordinate_Max; f++) {
				StringsForOutputFile_Flags::Flag_Coordinate ff = static_cast<StringsForOutputFile_Flags::Flag_Coordinate>(f);
				if (flag == ff) {
					flagCoordinates.push_back(tstring(NAMEOF_ENUM(ff)));
				}
			}*/

			for (LibFlag::type_Flag f = LibFlag::Coordinate_Min; f <= LibFlag::Coordinate_Max; f++) {
				const auto ff = static_cast<LibFlag::Coordinate>(f);
				if (flag == ff) {
					flagCoordinates.push_back(tstring(NAMEOF_ENUM(ff)));
				}
			}

		}
		
		//要素を掃引
		for (const auto& nam : flagCoordinates) {
			
			//型を検査
			di = CPPA_CCFC::DataIndex(nam);

			//見つかったらアドレスと位置を記録
			switch (di)
			{
			case CPPA_CCFC::Data_Index_int32:
				vdi.push_back(std::tuple<tstring, __int32, size_t>(nam, di, i_int32));
				vad_int32.push_back(std::get<CPPA_CCFC::Data_Index_int32>(CPPA_CCFC::Get()).at(nam));
				i_int32++;
				break;
			case CPPA_CCFC::Data_Index_int64:
				vdi.push_back(std::tuple<tstring, __int32, size_t>(nam, di, i_int64));
				vad_int64.push_back(std::get<CPPA_CCFC::Data_Index_int64>(CPPA_CCFC::Get()).at(nam));
				i_int64++;
				break;
			case CPPA_CCFC::Data_Index_double:
				vdi.push_back(std::tuple<tstring, __int32, size_t>(nam, di, i_double));
				vad_double.push_back(std::get<CPPA_CCFC::Data_Index_double>(CPPA_CCFC::Get()).at(nam));
				i_double++;
				break;
			default:
				//不明な型
				char temp[512];
				sprintf_s(temp, "StringsForOutputFileA::make : Failure in finding Bit_Coordinate name \"%s\"", nam.c_str());
				throw std::invalid_argument(temp);
				break;
			}
		}

		//ここまで異常がないなら作成
		return std::make_unique<StringsForOutputFileA_impl>(Separator, std::move(vdi), std::move(vad_int32), std::move(vad_int64), std::move(vad_double));

	}
	catch (std::invalid_argument&) {
		throw;
	}
	catch (std::out_of_range&) {
		throw;
	}
	catch (...) {
		throw;
	}
}


std::unique_ptr<StringsForOutputFileA> StringsForOutputFileA::make(const LibFlag::type_Flag BitFlags_Coordinate, const type_char Separator) {
	try {

		tstring nam;
		__int32 di;

		//目的
		std::vector<std::tuple<tstring, __int32, size_t>> vdi(0);
		std::vector<__int32*> vad_int32(0);
		std::vector<__int64*> vad_int64(0);
		std::vector<double*> vad_double(0);

		//カウンタ
		size_t i_int32 = 0;
		size_t i_int64 = 0;
		size_t i_double = 0;

		//ビットフラグをenum文字列へ変換
		std::vector<tstring> flagCoordinates;

		LibFlag::type_Flag x;
		//こうやって掃引するとNAMEOF_ENUMは成功する
		for (LibFlag::type_Flag f = LibFlag::Coordinate_Min; f <= LibFlag::Coordinate_Max; f++) {
			
			x = 1 << f;

			if ((BitFlags_Coordinate & x) != 0) {
				const auto ff = static_cast<LibFlag::Coordinate>(f);
				flagCoordinates.push_back(tstring(NAMEOF_ENUM(ff)));
			}
			
		}

		//要素を掃引
		for (const auto& nam : flagCoordinates) {

			//型を検査
			di = CPPA_CCFC::DataIndex(nam);

			//見つかったらアドレスと位置を記録
			switch (di)
			{
			case CPPA_CCFC::Data_Index_int32:
				vdi.push_back(std::tuple<tstring, __int32, size_t>(nam, di, i_int32));
				vad_int32.push_back(std::get<CPPA_CCFC::Data_Index_int32>(CPPA_CCFC::Get()).at(nam));
				i_int32++;
				break;
			case CPPA_CCFC::Data_Index_int64:
				vdi.push_back(std::tuple<tstring, __int32, size_t>(nam, di, i_int64));
				vad_int64.push_back(std::get<CPPA_CCFC::Data_Index_int64>(CPPA_CCFC::Get()).at(nam));
				i_int64++;
				break;
			case CPPA_CCFC::Data_Index_double:
				vdi.push_back(std::tuple<tstring, __int32, size_t>(nam, di, i_double));
				vad_double.push_back(std::get<CPPA_CCFC::Data_Index_double>(CPPA_CCFC::Get()).at(nam));
				i_double++;
				break;
			default:
				//不明な型
				char temp[512];
				sprintf_s(temp, "StringsForOutputFileA::make : Failure in finding Bit_Coordinate name \"%s\"", nam.c_str());
				throw std::invalid_argument(temp);
				break;
			}
		}

		//ここまで異常がないなら作成
		return std::make_unique<StringsForOutputFileA_impl>(Separator, std::move(vdi), std::move(vad_int32), std::move(vad_int64), std::move(vad_double));

	}
	catch (std::invalid_argument&) {
		throw;
	}
	catch (std::out_of_range&) {
		throw;
	}
	catch (...) {
		throw;
	}
}






//void StringsForIgorTextWaveA::WaveText(tstring& Dst, const tstring& Name_ValueWave, const tstring& Name_ScaleWave, const std::vector<double>& Vec, const ScaleInformation& ScaleInfo) {
//	try {
//		if (Vec.size() != ScaleInfo.NumIntervals()) {
//			throw std::invalid_argument("StringsForIgorTextWaveA::WaveText : Different sizes of Vec and ScaleInfo.NumIntervals");
//		}
//
//		tstring ans = "";
//
//		tstring wn = "";
//		GetWaveName(wn, Name_ValueWave);
//
//		tstring wns = "";
//		GetWaveName(wns, Name_ScaleWave);
//
//		type_char buf[128];
//
//		sprintf_s(buf, "WAVES/D/O\t%s\t%s\nBEGIN\n", wns.c_str(), wn.c_str());
//		ans += buf;
//
//		for (size_t i = 0; i < Vec.size(); i++) {
//			sprintf_s(buf, "%.17g\t%.17g\n", ScaleInfo.IntervalIndex2XRepresentative(i), Vec.at(i));
//			ans += buf;
//		}
//		ans += "END\n";
//		
//		sprintf_s(buf, "X SetScale/P x 0,1,\"\", %s; SetScale y 0,0,\"\", %s\n", wns.c_str(), wns.c_str());
//		ans += buf;
//
//		sprintf_s(buf, "X SetScale/P x 0,1,\"\", %s; SetScale y 0,0,\"\", %s\n", wn.c_str(), wn.c_str());
//		ans += buf;
//
//		ans += "\n";
//
//		Dst.clear();
//		Dst = ans;
//
//	}
//	catch (std::invalid_argument&) {
//		throw;
//	}
//	catch (std::out_of_range&) {
//		throw;
//	}
//	catch (std::exception&) {
//		throw;
//	}
//}


void StringsForIgorTextWaveA::WaveText1(tstring& Dst, std::initializer_list<std::tuple<std::vector<double>, tstring, tstring>> list_Value1D_ValueWaveName_Comment)noexcept {
	type_char buf[128];

	auto WT = [&buf](const std::vector<double>& Value1D, const tstring& ValueWaveName, const tstring& Comment)->tstring
	{
		tstring ans = "";

		tstring wn = "";
		GetWaveName(wn, ValueWaveName);

		sprintf_s(buf, "WAVES/D/O\t%s\nBEGIN\n", wn.c_str());
		ans += buf;

		for (const auto& v : Value1D) {
			sprintf_s(buf, "%.17g\n", v);
			ans += buf;
		}

		ans += "END\n";

		sprintf_s(buf, "X SetScale/P x 0,1,\"\", %s; SetScale y 0,0,\"\", %s\n", wn.c_str(), wn.c_str());
		ans += buf;

		//X Note w, "wave"
		sprintf_s(buf, "X Note %s, \"%s\"\n", wn.c_str(), Comment.c_str());
		ans += buf;

		return std::move(ans);
	};


	tstring ans = "";

	for (const auto& tpl : list_Value1D_ValueWaveName_Comment) {
		ans += WT(std::get<0>(tpl), std::get<1>(tpl), std::get<2>(tpl));
		ans += "\n";
	}


	Dst.clear();
	Dst = std::move(ans);
}



void StringsForIgorTextWaveA::WaveText2(tstring& Dst, std::initializer_list<std::tuple<std::vector<std::vector<double>>, tstring, tstring>> list_Value2D_ValueWaveName_Comment)noexcept {

	type_char buf[256];

	auto WT = [&buf](const std::vector<std::vector<double>>& Value2D, const tstring& ValueWaveName, const tstring& Comment)->tstring
	{
		const size_t im = Value2D.size();
		const size_t jm = Value2D.at(0).size();
		
		for (const auto& vv : Value2D) {
			if (jm != vv.size()) {
				throw std::invalid_argument("A jagged array");
			}
		}

		tstring ans = "";

		tstring wn = "";
		GetWaveName(wn, ValueWaveName);

		sprintf_s(buf, "WAVES/D/O/N=(%lld,%lld)\t%s\nBEGIN\n", im, jm, wn.c_str());
		ans += buf;

		for (const auto& vv : Value2D) {
			for (const auto& val : vv) {
				sprintf_s(buf, "%.17g\t", val);
				ans += buf;
			}
			ans += "\n";
		}
		
		ans += "END\n";

		sprintf_s(buf, "X SetScale/P x 0,1,\"\", %s; SetScale y 0,0,\"\", %s; SetScale d 0,0,\"\", %s\n", wn.c_str(), wn.c_str(), wn.c_str());
		ans += buf;

		//X Note w, "wave"
		sprintf_s(buf, "X Note %s, \"%s\"\n", wn.c_str(), Comment.c_str());
		ans += buf;

		return std::move(ans);
	};


	tstring ans = "";

	for (const auto& tpl : list_Value2D_ValueWaveName_Comment) {
		ans += WT(std::get<0>(tpl), std::get<1>(tpl), std::get<2>(tpl));
		ans += "\n";
	}


	Dst.clear();
	Dst = std::move(ans);

}




//void StringsForIgorTextWaveA::WaveText(tstring& Dst, const tstring& Name_ValueWave, const tstring& Name_ScaleWave0, const tstring& Name_ScaleWave1, const std::vector<std::vector<double>>& Vec, const ScaleInformation& ScaleInfo0, const ScaleInformation& ScaleInfo1) {
//	try {
//
//		const size_t im = ScaleInfo0.NumIntervals();
//		const size_t jm = ScaleInfo1.NumIntervals();
//
//		if (Vec.size() != im) {
//			throw std::invalid_argument("StringsForIgorTextWaveA::WaveText : Different sizes of Vec.at and ScaleInfo0.NumIntervals");
//		}
//
//		for (size_t i = 0; i < im; i++) {
//			if (Vec.at(i).size() != jm) {
//				throw std::invalid_argument("StringsForIgorTextWaveA::WaveText : Different sizes of Vec.at.at and ScaleInfo1.NumIntervals");
//			}
//		}
//
//
//		tstring ans = "";
//
//		tstring wn = "";
//		GetWaveName(wn, Name_ValueWave);
//
//		tstring wns0 = "";
//		GetWaveName(wns0, Name_ScaleWave0);
//
//		tstring wns1 = "";
//		GetWaveName(wns1, Name_ScaleWave1);
//
//		type_char buf[256];
//
//
//		sprintf_s(buf, "WAVES/D/O/N=(%lld,%lld)\t%s\nBEGIN\n", im, jm, wn.c_str());
//		ans += buf;
//
//		for (size_t i = 0; i < im; i++) {
//			for (size_t j = 0; j < jm; j++) {
//				sprintf_s(buf, "%.17g\t", Vec.at(i).at(j));
//				ans += buf;
//			}
//			ans += "\n";
//		}
//
//		ans += "END\n";
//
//		sprintf_s(buf, "X SetScale/P x 0,1,\"\", %s; SetScale y 0,0,\"\", %s; SetScale d 0,0,\"\", %s\n", wn.c_str(), wn.c_str(), wn.c_str());
//		ans += buf;
//
//		ans += "\n";
//
//
//
//
//
//		//イメージの境界を定義するので二次元データよりポイント数が一つ大きい
//		sprintf_s(buf, "WAVES/D/O\t%s\nBEGIN\n", wns0.c_str());
//		ans += buf;
//		
//		for (size_t i = 0; i < im; i++) {
//			sprintf_s(buf, "%.17g\n", ScaleInfo0.Interval(i).first);
//			ans += buf;
//		}
//		sprintf_s(buf, "%.17g\n", ScaleInfo0.Interval(im - 1).second);
//		ans += buf;
//
//		ans += "END\n";
//
//		sprintf_s(buf, "X SetScale/P x 0,1,\"\", %s; SetScale y 0,0,\"\", %s\n", wns0.c_str(), wns0.c_str());
//		ans += buf;
//
//		ans += "\n";
//
//
//
//
//		//イメージの境界を定義するので二次元データよりポイント数が一つ大きい
//		sprintf_s(buf, "WAVES/D/O\t%s\nBEGIN\n", wns1.c_str());
//		ans += buf;
//
//		for (size_t j = 0; j < jm; j++) {
//			sprintf_s(buf, "%.17g\n", ScaleInfo1.Interval(j).first);
//			ans += buf;
//		}
//		sprintf_s(buf, "%.17g\n", ScaleInfo1.Interval(jm - 1).second);
//		ans += buf;
//
//		ans += "END\n";
//
//		sprintf_s(buf, "X SetScale/P x 0,1,\"\", %s; SetScale y 0,0,\"\", %s\n", wns1.c_str(), wns1.c_str());
//		ans += buf;
//
//		ans += "\n";
//
//
//		Dst.clear();
//		Dst = ans;
//
//
//	}
//	catch (std::invalid_argument&) {
//		throw;
//	}
//	catch (std::out_of_range&) {
//		throw;
//	}
//	catch (std::exception&) {
//		throw;
//	}
//}









