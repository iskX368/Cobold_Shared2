#include "pch.h"
#include "framework.h"

#include "Lib_CppAddCCFCrd.h"

#include <memory>
#include <tuple>


using CC_CC = CppAddress_CCFileCoordinateW;

using type_char = CC_CC::type_char;
using tstring = std::basic_string<type_char>;

using i32map = std::map<tstring, __int32*>;
using i64map = std::map<tstring, __int64*>;
using dmap = std::map<tstring, double*>;

//グローバルなデータセットを定義
static std::unique_ptr<CC_CC::Data> Global_up2CppAddress_CCFileCoordinate_Data = std::make_unique<CC_CC::Data>(std::make_tuple<i32map, i64map, dmap>(
	i32map(), i64map(), dmap()
	));

inline i32map& Geti32map() {
	return std::get<0>(*Global_up2CppAddress_CCFileCoordinate_Data);
}
inline i64map& Geti64map() {
	return std::get<1>(*Global_up2CppAddress_CCFileCoordinate_Data);
}
inline dmap& Getdmap() {
	return std::get<2>(*Global_up2CppAddress_CCFileCoordinate_Data);
}

__int32 CC_CC::DataIndex(const tstring& Coordinate)noexcept {
	bool Found_int32 = false;
	bool Found_int64 = false;
	bool Found_double = false;

	if (Geti32map().count(Coordinate) > 0) {
		Found_int32 = true;
	}
	if (Geti64map().count(Coordinate) > 0) {
		Found_int64 = true;
	}
	if (Getdmap().count(Coordinate) > 0) {
		Found_double = true;
	}

	//重複か未発見はダメ

	if (Found_int32 && !Found_int64 && !Found_double) {
		return Data_Index_int32;
	}
	if (!Found_int32 && Found_int64 && !Found_double) {
		return Data_Index_int64;
	}
	if (!Found_int32 && !Found_int64 && Found_double) {
		return Data_Index_double;
	}

	return -1;
}

const CC_CC::Data& CC_CC::Get()noexcept {
	return *Global_up2CppAddress_CCFileCoordinate_Data;
}

void CC_CC::ClearAll()noexcept {
	Geti32map().clear();
	Geti64map().clear();
	Getdmap().clear();
}

void CC_CC::Set_private(std::pair<tstring, __int32*>&& pr) {
	//すでにmapに存在
	if (Geti32map().count(pr.first) > 0) {
		char temp[512];
		sprintf_s(temp, "CppAddress_CCFileCoordinate::Set_private, __int32 : Coordinate name already exists : \"%ls\"", pr.first.c_str());
		throw std::exception(temp);
	}

	Geti32map().insert(std::forward<std::pair<tstring, __int32*>>(pr));
}

void CC_CC::Set_private(std::pair<tstring, __int64*>&& pr) {
	//すでにmapに存在
	if (Geti64map().count(pr.first) > 0) {
		char temp[512];
		sprintf_s(temp, "CppAddress_CCFileCoordinate::Set_private, __int64 : Coordinate name already exists : \"%ls\"", pr.first.c_str());
		throw std::exception(temp);
	}

	Geti64map().insert(std::forward<std::pair<tstring, __int64*>>(pr));
}
void CC_CC::Set_private(std::pair<tstring, double*>&& pr) {
	//すでにmapに存在
	if (Getdmap().count(pr.first) > 0) {
		char temp[512];
		sprintf_s(temp, "CppAddress_CCFileCoordinate::Set_private, double : Coordinate name already exists : \"%ls\"", pr.first.c_str());
		throw std::exception(temp);
	}

	Getdmap().insert(std::forward<std::pair<tstring, double*>>(pr));
}