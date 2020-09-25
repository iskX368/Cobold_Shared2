#pragma once


class CUserSpectrum;
class CFileIOHelper;

//@ここでインクルード
#include "LAES2_UserFunctions.h"
#include <memory>


// class of functions used by DoFunction
class CFunctions
{
public:
	CFunctions(void);
	~CFunctions(void);
	void SetCommand(CString command);

protected:

	CString &ToOneLine(CString csInput[],__int32 first_index, __int32 ui32InputLength,unsigned __int32 ui32WordCount);

public:
	bool Help();
	bool ExportAsciiMulti(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount);
	bool Locate(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount);
	bool LinReg(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount);
	bool Angle(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount);
	bool MultiDefine1(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount);
	bool MultiDefine2(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount);
	bool LoadImg(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount);
	bool ExportTIF24BitRGB(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount);
	bool ExportTIF16Bit(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount);
	bool FitG(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount);
	bool Gauss(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount);
	bool GetCoordinateIndex(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount);
	bool Beep(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount);
	bool Distance(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount);
	bool ParameterBitMask(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount);
	bool NormalizeIntegralY(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount);
	bool Echo(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount);
	bool Wait(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount);
	bool GetHighestBin(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount);
	bool Proj1D(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount);
	bool ReBin(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount);
	bool Normalize(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount);
	bool Normalize_Y(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount);
	bool SetDAnResolutionParameters(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount);
	bool SetDAqCoordinates(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount);
	bool SetHitMatrixCoordinatesAndSpectrum(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount);
	bool Rescale(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount);
	bool PhdAnalysis(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount);
	bool DynIntegral(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount);
	bool GetMeanWithoutZeros(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount);
	
protected:
	//@生ポインタからユニークポインタへ変更　生ポインタだとLAES2::LAES2_UserFunctionsの中で繰り返し使用すると実体が消滅してCoboldPCが落ちた
	//CUserSpectrum *pFCUserSP;
	//CFileIOHelper *pFH;
	std::unique_ptr<CUserSpectrum> pFCUserSP;
	std::unique_ptr<CFileIOHelper> pFH;


	CString m_Command;

	// functions helper declaration
	bool WriteHexString(FILE * fptr,char * in);
	bool Convert2Integer(CString pszInput, __int32 &output_int);
	//bool FileExists_char(char * strFilename);
	//bool FileExists(CString strFilename);
	double gauss(double x, double amplitude, double x0, double fwhm);
	double get_gauss_rms(__int32 index, __int32 left_bin, __int32 right_bin, double amp, double x0, double fwhm, double y_offset);


	//@ここに自作関数をメンバにもつクラスを追加
public:
	std::unique_ptr<LAES2::LAES2_UserFunctions> pLUFs;
};

