﻿// Lib_CurrentDAqInfo.cpp : スタティック ライブラリ用の関数を定義します。
//

#include "pch.h"
#include "framework.h"

#include "Lib_CurrentDAqInfo.h"

#include <fstream>


using type_char = CurrentDAqInfoA::type_char;
using tstring = std::basic_string<type_char>;
using tifstream = std::basic_ifstream<type_char>;
using tofstream = std::basic_ofstream<type_char>;


static const tstring g_SourceLMFilePath_InCase_HardwaveA = LibPrm::SourceLMFilePath_InCase_Hardwave;

//Globals
static bool g_isLoaded = false;
static bool g_IsOnline = false;
static tstring g_SourceLMFilePathA;



//グローバル変数をロードする
void LoadInfoA() {
	try {
		
		tifstream ifs(LibPrm::DAqInfo_FilePath);
		if (!ifs) {
			throw std::exception("TempFile not found");
		}

		tstring line;

		//line #0
		if (std::getline(ifs, line)) {
			g_SourceLMFilePathA = line;
		}
		else {
			throw std::exception("Invalid TempFile format");
		}
		//

		//line #1
		if (std::getline(ifs, line)) {
			if (line.at(0) == LibPrm::DAqInfo_COBOLD_online) {
				g_IsOnline = true;
			}
			else if (line.at(0) == LibPrm::DAqInfo_COBOLD_offline) {
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


const tstring& CurrentDAqInfoA::SourceLMFilePath() {
	try {
		if (!g_isLoaded) {
			LoadInfoA();
			g_isLoaded = true;
		}

		if (g_IsOnline) {
			return g_SourceLMFilePath_InCase_HardwaveA;
		}
		else {
			return g_SourceLMFilePathA;
		}
	}
	catch (std::exception&) {
		g_isLoaded = false;
		throw;
	}
}


//データ取り込みのときのソースLMFの名前を返す
tstring CurrentDAqInfoA::SourceLMFileName() {
	try {
		auto path = SourceLMFilePath();

		//拡張子
		const auto pos1 = path.find_last_of('.');
		if (pos1 == tstring::npos) {
			throw std::exception("CurrentDAqInfoA::SourceLMFileName : extension ?");
		}

		//セパレータ
		const auto pos0 = path.find_last_of('\\');
		if (pos0 == tstring::npos) {
			throw std::exception("CurrentDAqInfoA::SourceLMFileName : Path separator ?");
		}

		//コピーする
		return tstring(path.begin() + pos0 + 1, path.begin() + pos1);

	}
	catch (std::exception&) {
		throw;
	}
}



bool CurrentDAqInfoA::IsOnline() {
	try {
		if (!g_isLoaded) {
			LoadInfoA();
			g_isLoaded = true;
		}
		
		return g_IsOnline;
	}
	catch (std::exception&) {
		g_isLoaded = false;
		throw;
	}
}

//DAqプログラムから保存したい内容を引数に渡してテキストファイルへ保存する
__int32 CurrentDAqInfoA::Save(const tstring& LMFilePath, bool isOnline) {
	
	tofstream ofs(LibPrm::DAqInfo_FilePath);
	if (!ofs) {
		return 1;
	}

	//line #0
	ofs << LMFilePath << std::endl;

	//line #1
	if (isOnline) {
		ofs << LibPrm::DAqInfo_COBOLD_online << std::endl;
	}
	else {
		ofs << LibPrm::DAqInfo_COBOLD_offline << std::endl;
	}

	ofs.close();

	return 0;
}