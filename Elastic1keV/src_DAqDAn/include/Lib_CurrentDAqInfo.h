#pragma once

#include <string>
#include <stdexcept>

//char
class CurrentDAqInfoA {
public:
	using type_char = char;
	using tstring = std::basic_string<type_char>;

	//LMF取り込みのときにソースのLMFのパスを返す
	//ハードウェア取り込みのときに"HARDWARE"と返す
	static const tstring& SourceLMFilePath();

	//ハードウェア取り込みならtrue, LMF取り込みならfalse
	static bool IsOnline();

	//DAqプログラムから保存したい内容を引数に渡してテキストファイルへ保存する
	//保存したい情報が増えたら引数へ追加
	static __int32 Save(const tstring& LMFilePath, bool isOnline);

};

//char
class CurrentDAqInfoW {
public:
	using type_char = wchar_t;
	using tstring = std::basic_string<type_char>;

	//LMF取り込みのときにソースのLMFのパスを返す
	//ハードウェア取り込みのときに"HARDWARE"と返す
	static const tstring& SourceLMFilePath();

	//ハードウェア取り込みならtrue, LMF取り込みならfalse
	static bool IsOnline();

	//DAqプログラムから保存したい内容を引数に渡してテキストファイルへ保存する
	//保存したい情報が増えたら引数へ追加
	static __int32 Save(const tstring& LMFilePath, bool isOnline);

};


#ifdef UNICODE
#define CurrentDAqInfo CurrentDAqInfoW
#else
#define CurrentDAqInfo CurrentDAqInfoA
#endif // UNICODE
