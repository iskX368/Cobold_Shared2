// Lib_CurrentDAqInfo.cpp : スタティック ライブラリ用の関数を定義します。
//

#include "pch.h"
#include "framework.h"

#include "Lib_CurrentDAqInfo.h"

#include <fstream>


using type_char = CurrentDAqInfoW::type_char;
using tstring = std::basic_string<type_char>;
using tifstream = std::basic_ifstream<type_char>;
using tofstream = std::basic_ofstream<type_char>;


//wchar_t
static constexpr type_char TempFilePathW[] = L"C:\\Program Files\\RoentDek Handels GmbH\\CoboldPC 2011 R5-2-x64 (Visual Studio .Net 2010 Compilation) V10.1.1412.2\\LAES2_TDC8PCI2_HEX\\Cobold_Shared1\\temp\\DAqInfo.dat";
static constexpr type_char Flag_OnlineW = L'1';
static constexpr type_char Flag_Not_OnlineW = L'0';
static const tstring g_SourceLMFilePath_InCase_HardwaveW = L"HARDWARE";

//Globals
static bool g_isLoaded = false;
static bool g_IsOnline = false;
static tstring g_SourceLMFilePathW;


//グローバル変数をロードする
void LoadInfoW() {
	try {
		
		tifstream ifs(TempFilePathW);
		if (!ifs) {
			throw std::exception("TempFile not found");
		}

		tstring line;

		//line #0
		if (std::getline(ifs, line)) {
			g_SourceLMFilePathW = line;
		}
		else {
			throw std::exception("Invalid TempFile format");
		}
		//

		//line #1
		if (std::getline(ifs, line)) {
			if (line.at(0) == Flag_OnlineW) {
				g_IsOnline = true;
			}
			else if (line.at(0) == Flag_Not_OnlineW) {
				g_IsOnline = false;
			}
			else {
				throw std::exception("Invalid TempFile format");
			}
		}
		else {
			throw std::exception("Invalid TempFile format");
		}
		//

		ifs.close();

		return;
	}
	catch (std::exception&) {
		throw;
	}
}


const tstring& CurrentDAqInfoW::SourceLMFilePath() {
	try {
		if (!g_isLoaded) {
			LoadInfoW();
			g_isLoaded = true;
		}

		if (g_IsOnline) {
			return g_SourceLMFilePath_InCase_HardwaveW;
		}
		else {
			return g_SourceLMFilePathW;
		}
	}
	catch (std::exception&) {
		throw;
	}
}


bool CurrentDAqInfoW::IsOnline() {
	try {
		if (!g_isLoaded) {
			LoadInfoW();
			g_isLoaded = true;
		}

		return g_IsOnline;
	}
	catch (std::exception&) {
		throw;
	}
}

//DAqプログラムから保存したい内容を引数に渡してテキストファイルへ保存する
__int32 CurrentDAqInfoW::Save(const tstring& LMFilePath, bool isOnline) {

	
	tofstream ofs(TempFilePathW);
	if (!ofs) {
		return 1;
	}

	//line #0
	ofs << LMFilePath << std::endl;

	//line #1
	if (isOnline) {
		ofs << Flag_OnlineW << std::endl;
	}
	else {
		ofs << Flag_Not_OnlineW << std::endl;
	}

	ofs.close();

	return 0;
}