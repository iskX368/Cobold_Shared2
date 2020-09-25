// Lib_CCFileCondition.cpp : スタティック ライブラリ用の関数を定義します。
//

#include "pch.h"
#include "framework.h"

#include "Lib_CCFileCondition.h"

#include <regex>
#include <map>
#include <fstream>
#include <sstream>

#include <iostream>



//長いのでtypedef
using tstring = std::basic_string<TCHAR>;

using tregex = std::basic_regex<TCHAR>;

using tmatch = std::match_results<tstring::const_iterator>;

using tregex_iterator = std::regex_iterator<tstring::const_iterator>;




//データセット
class CppAddress_CCFileCoordinate::Data {
public:
	CppAddress_CCFileCoordinate::Data()
		:m_Map_double(), m_Map_int32(), m_Map_int64()
	{}

	std::map<tstring, double*> m_Map_double;
	std::map<tstring, __int32*> m_Map_int32;
	std::map<tstring, __int64*> m_Map_int64;
};
//グローバルなデータセットを定義
std::unique_ptr<CppAddress_CCFileCoordinate::Data> Global_up2CppAddress_CCFileCoordinate_Data = std::make_unique<CppAddress_CCFileCoordinate::Data>();


const CppAddress_CCFileCoordinate::Data* const CppAddress_CCFileCoordinate::Get()noexcept {
	return Global_up2CppAddress_CCFileCoordinate_Data.get();
}


void CppAddress_CCFileCoordinate::ClearAll()noexcept {
	Global_up2CppAddress_CCFileCoordinate_Data->m_Map_double.clear();
	Global_up2CppAddress_CCFileCoordinate_Data->m_Map_int32.clear();
	Global_up2CppAddress_CCFileCoordinate_Data->m_Map_int64.clear();
}

void CppAddress_CCFileCoordinate::Set_private(std::pair<std::basic_string<TCHAR>, __int32*>&& pr) {
	//すでにmapに存在
	if (Global_up2CppAddress_CCFileCoordinate_Data->m_Map_int32.count(pr.first) > 0) {
		char temp[512];
#ifdef UNICODE
		sprintf_s(temp, "CppAddress_CCFileCoordinate::Set_private, __int32 : Coordinate name already exists : \"%ls\"", pr.first.c_str());
#else
		sprintf_s(temp, "CppAddress_CCFileCoordinate::Set_private, __int32 : Coordinate name already exists : \"%s\"", pr.first.c_str());
#endif // UNICODE
		throw std::exception(temp);
	}

	Global_up2CppAddress_CCFileCoordinate_Data->m_Map_int32.insert(std::forward<std::pair<std::basic_string<TCHAR>, __int32*>>(pr));
}
void CppAddress_CCFileCoordinate::Set_private(std::pair<std::basic_string<TCHAR>, __int64*>&& pr) {
	//すでにmapに存在
	if (Global_up2CppAddress_CCFileCoordinate_Data->m_Map_int64.count(pr.first) > 0) {
		char temp[512];
#ifdef UNICODE
		sprintf_s(temp, "CppAddress_CCFileCoordinate::Set_private, __int64 : Coordinate name already exists : \"%ls\"", pr.first.c_str());
#else
		sprintf_s(temp, "CppAddress_CCFileCoordinate::Set_private, __int64 : Coordinate name already exists : \"%s\"", pr.first.c_str());
#endif // UNICODE
		throw std::exception(temp);
	}

	Global_up2CppAddress_CCFileCoordinate_Data->m_Map_int64.insert(std::forward<std::pair<std::basic_string<TCHAR>, __int64*>>(pr));
}
void CppAddress_CCFileCoordinate::Set_private(std::pair<std::basic_string<TCHAR>, double*>&& pr) {
	//すでにmapに存在
	if (Global_up2CppAddress_CCFileCoordinate_Data->m_Map_double.count(pr.first) > 0) {
		char temp[512];
#ifdef UNICODE
		sprintf_s(temp, "CppAddress_CCFileCoordinate::Set_private, double : Coordinate name already exists : \"%ls\"", pr.first.c_str());
#else
		sprintf_s(temp, "CppAddress_CCFileCoordinate::Set_private, double : Coordinate name already exists : \"%s\"", pr.first.c_str());
#endif // UNICODE
		throw std::exception(temp);
	}

	Global_up2CppAddress_CCFileCoordinate_Data->m_Map_double.insert(std::forward<std::pair<std::basic_string<TCHAR>, double*>>(pr));
}



//文字列から数値
#ifdef UNICODE
#define _ttof _wtof
#else
#define _ttof atof
#endif // UNICODE



//正規表現の部品
static const tstring tBegl = _T(R"(^)");//行頭
static const tstring tEndl = _T(R"((?:;.*$|$))");//行末　コメントとorする
static const tstring tSpace = _T(R"([\s\t]*)");//スペース
static const tstring tComma = _T(R"([\s\t]*,[\s\t]*)");//コンマ
static const tstring tNumber_ = _T(R"(([+-]?(?:\d+(?:\.\d*)?|\.\d+)(?:[eE][+-]?\d+)?))");//実数　サブマッチ用カッコつき
static const tstring tString_ = _T(R"((\w+))");//文字列　サブマッチ用カッコつき
static const tstring tStrings_ = _T(R"((\w+(?:[\s\t]*,[\s\t]*\w+)*[\s\t]*,?))");//文字列のコンマ並び　サブマッチ用カッコつき

static const tstring tLogic1_NOT = _T(R"(not)");//論理演算子は全部小文字でないとダメ
static const tstring tLogic1_ = _T(R"(()") + tLogic1_NOT + _T(R"())");		//一項論理演算子　サブマッチ用カッコつき

static const tstring tLogic2_AND = _T(R"(and)");//論理演算子は全部小文字でないとダメ
static const tstring tLogic2_OR = _T(R"(or)");
static const tstring tLogic2_NAND = _T(R"(nand)");
static const tstring tLogic2_XOR = _T(R"(xor)");
static const tstring tLogic2_ = _T(R"(()") + tLogic2_AND + _T(R"(|)") + tLogic2_OR + _T(R"(|)") + tLogic2_NAND + _T(R"(|)") + tLogic2_XOR + _T(R"())");		//二項論理演算子　サブマッチ用カッコつき

static const tstring tDeclParameter = _T(R"([Pp][Aa][Rr][Aa][Mm][Em][Tt][Ee][Rr])");//宣言
static const tstring tDeclCondition = _T(R"([Cc][Oo][Nn][Dd][Ii][Tt][Ii][Oo][Nn])");//宣言
static const tstring tDeclCoordinate = _T(R"([Cc][Oo][Oo][Rr][Dd][Ii][Nn][Aa][Tt][Ee])");//宣言


//正規表現
static const tregex r_Coordinate_Multi = tregex(tBegl + tDeclCoordinate + tSpace + tStrings_ + tSpace + tEndl);
static const tregex r_String = tregex(tString_);
static const tregex r_Condition_Window = tregex(tBegl + tDeclCondition + tSpace + tString_ + tComma + tNumber_ + tComma + tNumber_ + tComma + tString_ + tSpace + tEndl);
static const tregex r_Condition_Logical1 = tregex(tBegl + tDeclCondition + tSpace + tLogic1_ + tComma + tString_ + tComma + tString_ + tSpace + tEndl);
static const tregex r_Condition_Logical1_NOT = tregex(tLogic1_NOT);
static const tregex r_Condition_Logical2 = tregex(tBegl + tDeclCondition + tSpace + tString_ + tComma + tLogic2_ + tComma + tString_ + tComma + tString_ + tSpace + tEndl);
static const tregex r_Condition_Logical2_AND = tregex(tLogic2_AND);
static const tregex r_Condition_Logical2_OR = tregex(tLogic2_OR);
static const tregex r_Condition_Logical2_NAND = tregex(tLogic2_NAND);
static const tregex r_Condition_Logical2_XOR = tregex(tLogic2_XOR);



CCFileCondition::~CCFileCondition() {}



class CCFileCondition_impl :public CCFileCondition {
public:
	virtual ~CCFileCondition_impl() {}

	CCFileCondition_impl() = delete;


	//Conditionの内容
	struct Contents {
		enum class LogicType
		{
			Window,
			Logical_NOT,
			Logical_AND,
			Logical_OR,
			Logical_NAND,
			Logical_XOR,
		};

		enum class ValueType
		{
			Type_int32,
			Type_int64,
			Type_double,
		};

		Contents() = delete;

		Contents(const tstring& From_Condition1_, const tstring& To_Condition2_, const tstring& Logic_Coordinate_, const LogicType Type_)
			:From_Condition1(From_Condition1_),
			To_Condition2(To_Condition2_),
			Logic_Coordinate(Logic_Coordinate_),
			Type(Type_),
			From(Type_ == LogicType::Window ? _ttof(From_Condition1.c_str()) : 0),
			To(Type_ == LogicType::Window ? _ttof(To_Condition2.c_str()) : 0),
			p2_Condition1_Contents(nullptr),
			p2_Condition2_Contents(nullptr),
			p2_value(nullptr),
			valueType(static_cast<ValueType>(-1))
		{}

		tstring From_Condition1;
		tstring To_Condition2;
		tstring Logic_Coordinate;
		LogicType Type;
		double From;
		double To;
		Contents* p2_Condition1_Contents;
		Contents* p2_Condition2_Contents;
		void* p2_value;
		ValueType valueType;
	};


	CCFileCondition_impl(std::map<tstring, Contents>&& Condition_Contents)
		:m_mpContents(std::forward<std::map<tstring, Contents>>(Condition_Contents)), m_RecursionNumberLimit(0)
	{}


	//Conditionが成立しているならtrue，していないならfalse
	virtual bool IsTrue(const std::basic_string<TCHAR>& ConditionName)const {
		try {
			if (ConditionName == _T("always")) {
				return true;
			}

			//初回は検索する
			const auto& contents = m_mpContents.at(ConditionName);
			//再帰カウンタ変数を用意
			int RecLimit = 100;

			return IsTrue_Recursion(contents, RecLimit);
		}
		catch (std::out_of_range&) {
			throw;
		}
		catch (std::exception&) {
			throw;
		}
	}

private:
	bool IsTrue_Recursion(const Contents& contents, int& RecursionNumber)const {
		try {
			//再帰カウンタを減らす
			if (RecursionNumber < 0) {
				throw std::out_of_range("CCFileCondition::IsTure : Recursion onumber limit");
			}
			RecursionNumber--;

			//場合分け
			bool b1, b2;
			switch (contents.Type)
			{
			case Contents::LogicType::Window:
				//指定範囲内かどうか計算
				switch (contents.valueType)
				{
				case Contents::ValueType::Type_double:
				{
					const double v = *reinterpret_cast<double*>(contents.p2_value);
					return v >= contents.From && v <= contents.To;
				}
				case Contents::ValueType::Type_int32:
				{
					const double v = static_cast<double>(*reinterpret_cast<__int32*>(contents.p2_value));
					return v >= contents.From && v <= contents.To;
				}
				case Contents::ValueType::Type_int64:
				{
					const double v = static_cast<double>(*reinterpret_cast<__int64*>(contents.p2_value));
					return v >= contents.From && v <= contents.To;
				}
				default:
					//不明な型フラグ
					throw std::exception("CCFileCondition::IsTure : Unknown ValueType flag");
					break;
				}
				break;
			case Contents::LogicType::Logical_AND:
				//ポインタが有効なら，さらに真偽を計算
				if (contents.p2_Condition1_Contents == nullptr || contents.p2_Condition2_Contents == nullptr) {
					char temp[512];
					sprintf_s(temp, "CCFileCondition::IsTure : Sub-condition not found : AND");
					throw std::exception(temp);
				}
				b1 = IsTrue_Recursion(*contents.p2_Condition1_Contents, RecursionNumber);
				b2 = IsTrue_Recursion(*contents.p2_Condition2_Contents, RecursionNumber);
				return b1 && b2;
			case Contents::LogicType::Logical_OR:
				//ポインタが有効なら，さらに真偽を計算
				if (contents.p2_Condition1_Contents == nullptr || contents.p2_Condition2_Contents == nullptr) {
					char temp[512];
					sprintf_s(temp, "CCFileCondition::IsTure : Sub-condition not found : OR");
					throw std::exception(temp);
				}
				b1 = IsTrue_Recursion(*contents.p2_Condition1_Contents, RecursionNumber);
				b2 = IsTrue_Recursion(*contents.p2_Condition2_Contents, RecursionNumber);
				return b1 || b2;
			case Contents::LogicType::Logical_NAND:
				//ポインタが有効なら，さらに真偽を計算
				if (contents.p2_Condition1_Contents == nullptr || contents.p2_Condition2_Contents == nullptr) {
					char temp[512];
					sprintf_s(temp, "CCFileCondition::IsTure : Sub-condition not found : NAND");
					throw std::exception(temp);
				}
				b1 = IsTrue_Recursion(*contents.p2_Condition1_Contents, RecursionNumber);
				b2 = IsTrue_Recursion(*contents.p2_Condition2_Contents, RecursionNumber);
				return !b1 || !b2;
			case Contents::LogicType::Logical_XOR:
				//ポインタが有効なら，さらに真偽を計算
				if (contents.p2_Condition1_Contents == nullptr || contents.p2_Condition2_Contents == nullptr) {
					char temp[512];
					sprintf_s(temp, "CCFileCondition::IsTure : Sub-condition not found : XOR");
					throw std::exception(temp);
				}
				b1 = IsTrue_Recursion(*contents.p2_Condition1_Contents, RecursionNumber);
				b2 = IsTrue_Recursion(*contents.p2_Condition2_Contents, RecursionNumber);
				return (!b1 && b2) || (b1 && !b2);
			case Contents::LogicType::Logical_NOT:
				if (contents.p2_Condition1_Contents == nullptr) {
					char temp[512];
					sprintf_s(temp, "CCFileCondition::IsTure : Sub-condition not found : NOT");
					throw std::exception(temp);
				}
				b1 = IsTrue_Recursion(*contents.p2_Condition1_Contents, RecursionNumber);
				return !b1;

			default:
				//不明なタイプフラグ
				throw std::exception("CCFileCondition::IsTure : Unknown LogiType flag");
				break;
			}




		}
		catch (std::out_of_range&) {
			throw;
		}
		catch (std::exception&) {
			throw;
		}
	}



	//CCFのConditionとc++ソースコードのアドレスをペアリングする
	std::map<tstring, Contents> m_mpContents;

	int m_RecursionNumberLimit;
};


std::unique_ptr<CCFileCondition> make_CCFileCondition(std::initializer_list<std::basic_string<TCHAR>> List_CCFilePath, const CppAddress_CCFileCoordinate::Data* const Ptr2Data) {
	try {

		//map取り出し
		const auto& mp_double = Ptr2Data->m_Map_double;
		const auto& mp_int32 = Ptr2Data->m_Map_int32;
		const auto& mp_int64 = Ptr2Data->m_Map_int64;


		//型を略記
		using Contents = CCFileCondition_impl::Contents;
		using LogicType = CCFileCondition_impl::Contents::LogicType;
		using ValueType = CCFileCondition_impl::Contents::ValueType;

		constexpr size_t bufsize = 1024;
		TCHAR buf[bufsize];
		tmatch mch, mch1;
		tstring line, logic;
		LogicType logictype;


		//Conditionとその内容をまとめたstructの対応関係
		std::map<tstring, Contents> mp;


		//各ファイルについて
		for (const auto& CCFileName : List_CCFilePath) {

			//ファイルを開いて
			std::basic_ifstream<TCHAR> ifs(CCFileName);
			if (!ifs) {
				char temp[512];
#ifdef UNICODE
				sprintf_s(temp, "make_CCFileCondition : File open failure : \"%ls\"", CCFileName.c_str());
#else
				sprintf_s(temp, "make_CCFileCondition : File open failure : \"%s\"", CCFileName.c_str());
#endif // UNICODE
				throw std::invalid_argument(temp);
			}

			//一行ずつ読む
			size_t nline = 0;
			while (ifs.getline(buf, bufsize)) {

				line = buf;

				//condition定義と一致したらmapに追加

				if (std::regex_match(line, mch, r_Condition_Window)) {
					//Window
					//			1			2	  3	  4
					//condition coordinate, from, to, name;

					//すでにmapに存在
					if (mp.count(mch[4].str()) > 0) {
						char temp[512];
#ifdef UNICODE
						sprintf_s(temp, "make_CCFileCondition : Condition name already exists : \"%ls\"", mch[4].str().c_str());
#else
						sprintf_s(temp, "make_CCFileCondition : Condition name already exists : \"%s\"", mch[4].str().c_str());
#endif // UNICODE
						throw std::exception(temp);
					}

					mp.insert(std::make_pair(mch[4].str(), Contents(mch[2].str(), mch[3].str(), mch[1].str(), LogicType::Window)));
				}
				else if (std::regex_match(line, mch, r_Condition_Logical2)) {
					//Logic 2
					//			1			2	   3		   4
					//condition condition1, logic, condition2, name;

					logic = mch[2].str();

					//演算子を見分ける
					if (std::regex_match(logic, mch1, r_Condition_Logical2_AND)) {
						logictype = LogicType::Logical_AND;
					}
					else if (std::regex_match(logic, mch1, r_Condition_Logical2_OR)) {
						logictype = LogicType::Logical_OR;
					}
					else if (std::regex_match(logic, mch1, r_Condition_Logical2_NAND)) {
						logictype = LogicType::Logical_NAND;
					}
					else if (std::regex_match(logic, mch1, r_Condition_Logical2_XOR)) {
						logictype = LogicType::Logical_XOR;
					}
					else {
						//不明なlogicシンボル
						char temp[512];
#ifdef UNICODE
						sprintf_s(temp, "make_CCFileCondition : Unknown logical symbol \"%ls\" at line #%zu of the file \"%ls\"", logic.c_str(), nline, CCFileName.c_str());
#else
						sprintf_s(temp, "make_CCFileCondition : Unknown logical symbol \"%s\" at line #%zu of the file \"%s\"", logic.c_str(), nline, CCFileName.c_str());
#endif // UNICODE
						throw std::exception(temp);
					}

					//すでにmapに存在
					if (mp.count(mch[4].str()) > 0) {
						char temp[512];
#ifdef UNICODE
						sprintf_s(temp, "make_CCFileCondition : Condition name already exists : \"%ls\"", mch[4].str().c_str());
#else
						sprintf_s(temp, "make_CCFileCondition : Condition name already exists : \"%s\"", mch[4].str().c_str());
#endif // UNICODE		
						throw std::exception(temp);
					}

					mp.insert(std::make_pair(mch[4].str(), Contents(mch[1].str(), mch[3].str(), mch[2].str(), logictype)));

				}
				else if (std::regex_match(line, mch, r_Condition_Logical1)) {
					//Logic1
					//			1	   2		   3
					//condition logic, condition1, name;

					logic = mch[1].str();

					//演算子を見分ける
					if (std::regex_match(logic, mch1, r_Condition_Logical1_NOT)) {
						logictype = LogicType::Logical_NOT;
					}
					else {
						//不明なlogicシンボル
						char temp[512];
#ifdef UNICODE
						sprintf_s(temp, "make_CCFileCondition : Unknown logical symbol \"%ls\" at line #%zu of the file \"%ls\"", logic.c_str(), nline, CCFileName.c_str());
#else
						sprintf_s(temp, "make_CCFileCondition : Unknown logical symbol \"%s\" at line #%zu of the file \"%s\"", logic.c_str(), nline, CCFileName.c_str());
#endif // UNICODE
						throw std::exception(temp);
					}

					//すでにmapに存在
					if (mp.count(mch[2].str()) > 0) {
						char temp[512];
#ifdef UNICODE
						sprintf_s(temp, "make_CCFileCondition : Condition name already exists : \"%ls\"", mch[2].str().c_str());
#else
						sprintf_s(temp, "make_CCFileCondition : Condition name already exists : \"%s\"", mch[2].str().c_str());
#endif // UNICODE		
						throw std::exception(temp);
					}

					mp.insert(std::make_pair(mch[3].str(), Contents(mch[2].str(), _T(""), mch[1].str(), logictype)));

				}


				//行数カウンタ
				nline++;
			}


			//ファイル閉じる
			ifs.close();
		}

		int aaaaaaaaaaaaaaaaa = 0;

		//Conditionの関係を調べる
		//ポインタを記録する
		for (auto& pr : mp) {
			const auto& conditionName = std::get<0>(pr);
			auto& contents = std::get<1>(pr);


			if (contents.Type == LogicType::Logical_NOT) {
				//一項論理
				contents.p2_Condition1_Contents = &mp.at(contents.From_Condition1);
				contents.p2_Condition2_Contents = nullptr;
			}
			else if (contents.Type == LogicType::Logical_AND || contents.Type == LogicType::Logical_OR || contents.Type == LogicType::Logical_NAND || contents.Type == LogicType::Logical_XOR) {
				//二項論理
				contents.p2_Condition1_Contents = &mp.at(contents.From_Condition1);
				contents.p2_Condition2_Contents = &mp.at(contents.To_Condition2);
			}
			else if (contents.Type == LogicType::Window) {
				//ゲート
				contents.p2_Condition1_Contents = nullptr;
				contents.p2_Condition2_Contents = nullptr;

				bool Found = false;

				//Coordinateに対応したアドレスを記録する

				//double
				if (!Found) {
					if (mp_double.count(contents.Logic_Coordinate) > 0) {
						double* ptr = mp_double.at(contents.Logic_Coordinate);
						contents.p2_value = reinterpret_cast<void*>(ptr);
						contents.valueType = ValueType::Type_double;
						Found = true;
					}
				}

				//__int32
				if (!Found) {
					if (mp_int32.count(contents.Logic_Coordinate) > 0) {
						__int32* ptr = mp_int32.at(contents.Logic_Coordinate);
						contents.p2_value = reinterpret_cast<void*>(ptr);
						contents.valueType = ValueType::Type_int32;
						Found = true;
					}
				}

				//__int64
				if (!Found) {
					if (mp_int64.count(contents.Logic_Coordinate) > 0) {
						__int64* ptr = mp_int64.at(contents.Logic_Coordinate);
						contents.p2_value = reinterpret_cast<void*>(ptr);
						contents.valueType = ValueType::Type_int64;
						Found = true;
					}
				}

				if (!Found) {
					//CCFileで見つかったCondition記述に対応するCoordinateが引数に見つからなかった
					char temp[512];
#ifdef UNICODE
					sprintf_s(temp, "make_CCFileCondition : No correspoing address-coordinate pair found in the argument for the condition \"%ls\"", conditionName.c_str());
#else
					sprintf_s(temp, "make_CCFileCondition : No correspoing address-coordinate pair found in the argument for the condition \"%s\"", conditionName.c_str());
#endif // UNICODE
					throw std::invalid_argument(temp);
				}

			}
			else {
				//不明なロジック型
				char temp[512];
				sprintf_s(temp, "make_CCFileCondition : Unknown LogicType Flag : %d", static_cast<int>(contents.Type));
				throw std::exception(temp);
			}
		}


		//ここまで異常なかったらクラス初期化する
		return std::make_unique<CCFileCondition_impl>(std::move(mp));


	}
	catch (std::invalid_argument&) {
		throw;
	}
	catch (std::out_of_range&) {
		throw;
	}
	catch (std::exception&) {
		throw;
	}
	catch (...) {
		throw;
	}
}
