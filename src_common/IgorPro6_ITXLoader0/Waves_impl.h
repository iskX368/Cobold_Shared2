#pragma once

#include "IgorPro6_ITXLoader0.h"


namespace IgorPro6 {
	
	namespace Wave_Factory {
		template<typename T> std::unique_ptr<Wave_1D<T>> make(
			std::vector<T>&& _ArrayFromITX, std::string& _WaveName,
			type_igorpoint _DimSizeX, double _DimOffsetX, double _DimDeltaX, std::string& _DimUnitX
		);

		template<typename T> std::unique_ptr<Wave_2D<T>> make(
			std::vector<T>&& _ArrayFromITX, std::string& _WaveName,
			type_igorpoint _DimSizeX, double _DimOffsetX, double _DimDeltaX, std::string& _DimUnitX,
			type_igorpoint _DimSizeY, double _DimOffsetY, double _DimDeltaY, std::string& _DimUnitY
		);

		template<typename T> std::unique_ptr<Wave_3D<T>> make(
			std::vector<T>&& _ArrayFromITX, std::string& _WaveName,
			type_igorpoint _DimSizeX, double _DimOffsetX, double _DimDeltaX, std::string& _DimUnitX,
			type_igorpoint _DimSizeY, double _DimOffsetY, double _DimDeltaY, std::string& _DimUnitY,
			type_igorpoint _DimSizeZ, double _DimOffsetZ, double _DimDeltaZ, std::string& _DimUnitZ
		);

		template<typename T> std::unique_ptr<Wave_4D<T>> make(
			std::vector<T>&& _ArrayFromITX, std::string& _WaveName,
			type_igorpoint _DimSizeX, double _DimOffsetX, double _DimDeltaX, std::string& _DimUnitX,
			type_igorpoint _DimSizeY, double _DimOffsetY, double _DimDeltaY, std::string& _DimUnitY,
			type_igorpoint _DimSizeZ, double _DimOffsetZ, double _DimDeltaZ, std::string& _DimUnitZ,
			type_igorpoint _DimSizeT, double _DimOffsetT, double _DimDeltaT, std::string& _DimUnitT
		);
		
	}


	template<typename T>
	std::basic_ostream<char, std::char_traits<char>>& operator << (std::basic_ostream<char, std::char_traits<char>>& ss, const Wave_1D<T>& w);
	
	template<typename T>
	std::basic_ostream<char, std::char_traits<char>>& operator << (std::basic_ostream<char, std::char_traits<char>>& ss, const Wave_2D<T>& w);

	template<typename T>
	std::basic_ostream<char, std::char_traits<char>>& operator << (std::basic_ostream<char, std::char_traits<char>>& ss, const Wave_3D<T>& w);

	template<typename T>
	std::basic_ostream<char, std::char_traits<char>>& operator << (std::basic_ostream<char, std::char_traits<char>>& ss, const Wave_4D<T>& w);

}