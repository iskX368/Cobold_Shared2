// Lib_Inputs.cpp : スタティック ライブラリ用の関数を定義します。
//

#include "pch.h"
#include "framework.h"

#include "Lib_Inputs.h"
#include "nameof.hpp"

#include "CoefDist_TOFNoOffset2KES_K0.h"
#include "CoefDist_TOFNoOffset2KES_K1.h"
#include "CoefDist_TOFNoOffset2KES_K2.h"

#include "SourceEvents_DetectorSensitivity_.h"



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


std::pair<double, double> LibFlag::AzimuthRange(const AzimuthFormat Format) {
	switch (Format)
	{
	case LibFlag::AzimuthFormat::Deg:
		return std::make_pair<double, double>(-180, 180);
	case LibFlag::AzimuthFormat::Deg0:
		return std::make_pair<double, double>(0, 360);
	case LibFlag::AzimuthFormat::Rad:
		return std::make_pair<double, double>(-M_PI, M_PI);
	case LibFlag::AzimuthFormat::Rad0:
		return std::make_pair<double, double>(0, M_PI + M_PI);
	default:
		throw std::invalid_argument("LibFlag::AzimuthRange : Format ?");
	}
}

double LibFlag::AzimuthIntoRnge(const double Azimuth, const AzimuthFormat Format) {
	try {

		double az0 = Azimuth;

		const auto range = AzimuthRange(Format);
		const double del = range.second - range.first;

		if (az0 < range.first) {
			do {
				az0 += del;
			} while (az0 < range.first);
			return az0;
		}
		else if (az0 > range.second) {
			do {
				az0 -= del;
			} while (az0 > range.second);
			return az0;
		}
		else {
			return az0;
		}

	}
	catch (std::invalid_argument&) {
		throw;
	}
}


double LibFlag::ConvertAzimuthFormat(const double Azimuth, const LibFlag::AzimuthFormat Format_Before, const LibFlag::AzimuthFormat Format_After) {
	try {

		if (Format_Before == Format_After) {
			return Azimuth;
		}

		//範囲
		const auto range = LibFlag::AzimuthRange(Format_Before);
		if (Azimuth < range.first || Azimuth > range.second) {
			throw std::out_of_range("ConversionToDCS1<Params>::ConvertAzimuthFormat : Azimuth out of range");
		}


		//単位
		auto UnitRad = [](const LibFlag::AzimuthFormat Format_) {
			switch (Format_)
			{
			case LibFlag::AzimuthFormat::Deg:
				return false;
			case LibFlag::AzimuthFormat::Deg0:
				return false;
			case LibFlag::AzimuthFormat::Rad:
				return true;
			case LibFlag::AzimuthFormat::Rad0:
				return true;
			default:
				throw std::invalid_argument("UnitRad : Format ?");
			}
		};

		//単位変換
		auto Mult = [=](const LibFlag::AzimuthFormat Format_Before_, const LibFlag::AzimuthFormat Format_After_) {
			try {
				const bool ur0 = UnitRad(Format_Before_);
				const bool ur1 = UnitRad(Format_After_);
				if (ur0 == true && ur1 == false) {
					return gv_rad2deg;
				}
				else if (ur0 == false && ur1 == true) {
					return gv_deg2rad;
				}
				else {
					return 1.0;
				}
			}
			catch (std::invalid_argument&) {
				throw;
			}
		};

		//オフセット
		auto AngleOffset_Before = LibFlag::AzimuthRange(Format_Before).first;
		auto AngleOffset_After = LibFlag::AzimuthRange(Format_After).first;

		return (Azimuth - AngleOffset_Before) * Mult(Format_Before, Format_After) + AngleOffset_After;

	}
	catch (std::invalid_argument&) {
		throw;
	}
	catch (std::out_of_range&) {
		throw;
	}
}


//修正版
void LibFlag::OrthoToRPhi(double dX, double dY, __int32 iAFormat, double dCX, double dCY, double& dR, double& dPhi)
{
	// move x,y to center of rotation point
	double x = dX - dCX;
	double y = dY - dCY;

	// calculate radius
	dR = sqrt(x * x + y * y);

	// if radius is to small then set phi to 0
	if (dR > 1e-15)
	{
		// if y is to small then quadrant correction is not possible
		if (fabs(y) > 1e-15)
			dPhi = acos(x / (dR)) * y / fabs(y);//acos : [0,pi] * sgn(y)
		else
			dPhi = acos(x / (dR));
	}
	else
		dPhi = 0.;

	double ph = LibFlag::ConvertAzimuthFormat(dPhi, LibFlag::AzimuthFormat::Rad, static_cast<LibFlag::AzimuthFormat>(iAFormat));
	dPhi = ph;
	//// convert angle to specified dataformat
	//if (iAFormat == AF_RAD0)
	//{
	//	if (dPhi < -PI)
	//		dPhi += (2.0 * PI);
	//}
	//if (iAFormat == AF_DEG || iAFormat == AF_DEG0)
	//{
	//	dPhi = dPhi / PI * 180.;
	//	if (iAFormat == AF_DEG0)
	//	{
	//		if (dPhi < -180.0)
	//			dPhi += 360.0;
	//	}
	//}
}





//二次元補間
template<size_t Size0, size_t Size1>
static double interp2D(const double(&Array2D)[Size0][Size1], const double offsetx, const double dx, const double offsety, const double dy, const double x, const double y) {

	/*
	*z = a + b(x-x0) + c(y-y0) + d(x-x0)(y-y0)
	*で補間
	*
	*  2+-----------+3
	*	|			|
	* dy| +(x,y)	|
	*	|			|
	*  0+-----------+1
	*		 dx
	*
	* 4点を通る条件から係数を求めると任意の点(x,y)に対する補間値は
	*
	*	z = z0 + z10*dX + z20*dY + (z30-z10-z20)*dX*dY
	*	  = z0(1-dX-dY) + z1(dX-dX*dY) + z2(dY-dX*dY) + z3(dX*dY)
	*
	* ただし
	*	dX=(x-x0)/dx
	*	dY=(y-y0)/dy
	* など
	*/


	double _dx = 1 / dx;
	double _dy = 1 / dy;
	double _dxdy = 1 / (dx * dy);

	//(x,y)を囲む最近接四ポイントのうち(offsetx,offsety)側の点(x0,y0)を見つける

	double dp = (x - offsetx) * _dx;
	double dq = (y - offsety) * _dy;
	if (dp < 0 || dp >= Size0) {
		throw std::out_of_range("interp2D : x out of range");
	}
	if (dq < 0 || dq >= Size1) {
		throw std::out_of_range("interp2D : y out of range");
	}
	double fdp = floor(dp);
	double fdq = floor(dq);

	double x0 = offsetx + fdp * dx;
	double y0 = offsety + fdq * dy;
	size_t p0 = static_cast<size_t>(fdp);
	size_t q0 = static_cast<size_t>(fdq);

	double xx = (x - x0) * _dx;
	double yy = (y - y0) * _dy;
	double xxyy = xx * yy;

	double V0 = (1 - xx - yy + xxyy) * Array2D[p0][q0];
	double V1 = (xx - xxyy) * Array2D[p0 + 1][q0];
	double V2 = (yy - xxyy) * Array2D[p0][q0 + 1];
	double V3 = xxyy * Array2D[p0 + 1][q0 + 1];

	return V0 + V1 + V2 + V3;
}



double CoefDist_TOFNoOffset2KES_191121::K0(const double PosX, const double PosY) {
	try {
		return interp2D(W_coef_K0_191121, DimOffsetX, DimDeltaX, DimOffsetY, DimDeltaY, PosX, PosY);
	}
	catch (std::out_of_range&) {
		throw;
	}
}
double CoefDist_TOFNoOffset2KES_191121::K0s(const double PosX, const double PosY) {
	try {
		return interp2D(W_sigma_K0_191121, DimOffsetX, DimDeltaX, DimOffsetY, DimDeltaY, PosX, PosY);
	}
	catch (std::out_of_range&) {
		throw;
	}
}

double CoefDist_TOFNoOffset2KES_191121::K1(const double PosX, const double PosY) {
	try {
		return interp2D(W_coef_K1_191121, DimOffsetX, DimDeltaX, DimOffsetY, DimDeltaY, PosX, PosY);
	}
	catch (std::out_of_range&) {
		throw;
	}
}
double CoefDist_TOFNoOffset2KES_191121::K1s(const double PosX, const double PosY) {
	try {
		return interp2D(W_sigma_K1_191121, DimOffsetX, DimDeltaX, DimOffsetY, DimDeltaY, PosX, PosY);
	}
	catch (std::out_of_range&) {
		throw;
	}
}

double CoefDist_TOFNoOffset2KES_191121::K2(const double PosX, const double PosY) {
	try {
		return interp2D(W_coef_K2_191121, DimOffsetX, DimDeltaX, DimOffsetY, DimDeltaY, PosX, PosY);
	}
	catch (std::out_of_range&) {
		throw;
	}
}
double CoefDist_TOFNoOffset2KES_191121::K2s(const double PosX, const double PosY) {
	try {
		return interp2D(W_sigma_K2_191121, DimOffsetX, DimDeltaX, DimOffsetY, DimDeltaY, PosX, PosY);
	}
	catch (std::out_of_range&) {
		throw;
	}
}



std::tuple<double, double> SourceEvents_DetectorSensitivity_19122536::Event(const size_t EventIndex) {
	try {
		if (EventIndex >= NumEvents) {
			throw std::out_of_range("SourceEvents_DetectorSensitivity::Event : Out of range");
		}
		return std::tuple<double, double>(r_phi_19122536[EventIndex * 2], r_phi_19122536[EventIndex * 2 + 1]);
	}
	catch (std::out_of_range&) {
		throw;
	}
}











