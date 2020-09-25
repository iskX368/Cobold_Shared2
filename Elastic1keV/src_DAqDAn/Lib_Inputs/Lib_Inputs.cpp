// Lib_Inputs.cpp : スタティック ライブラリ用の関数を定義します。
//

#include "pch.h"
#include "framework.h"

#include "Lib_Inputs.h"
#include "nameof.hpp"


__int32 LibFlag::Coordinate_ToString(std::basic_string<char>& Dst, const type_Flag FlagValue, const Coordinate Default)noexcept {
	//範囲内？
	if (FlagValue < Coordinate_Min || FlagValue > Coordinate_Max) {
		Dst = std::basic_string<char>(NAMEOF_ENUM(static_cast<Coordinate>(Default)));
		return 1;
	}
	else {
		Dst = std::basic_string<char>(NAMEOF_ENUM(static_cast<Coordinate>(FlagValue)));
		return 0;
	}
}


__int32 LibFlag::Condition_ToString(std::basic_string<char>& Dst, const type_Flag FlagValue, const Condition Default)noexcept {
	//範囲内？
	if (FlagValue < Condition_Min || FlagValue > Condition_Max) {
		Dst = std::basic_string<char>(NAMEOF_ENUM(static_cast<Condition>(Default)));
		return 1;
	}
	else {
		Dst = std::basic_string<char>(NAMEOF_ENUM(static_cast<Condition>(FlagValue)));
		return 0;
	}
}

