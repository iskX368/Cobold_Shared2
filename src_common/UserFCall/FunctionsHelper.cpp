#include "StdAfx.h"
#include "LMInfo.h"
#include "CoboldUtilities.h"
#include "UserSP.h"
#include "Functions.h"

#include <math.h>

CFunctions::CFunctions(void)
	//:pFH(nullptr), pFCUserSP(nullptr), pLUFs(nullptr)//@初期化を変更
	:pFH(std::make_unique<CFileIOHelper>()),pFCUserSP(std::make_unique<CUserSpectrum>()),pLUFs(nullptr)
{
	//@コメントアウト
	//if (!pFH)
	//	//pFH = new CFileIOHelper();

	//if (!pFCUserSP)
	//	//pFCUserSP = new CUserSpectrum();

	//@自作クラスの初期化
	pLUFs = std::make_unique<LAES2::LAES2_UserFunctions>(pFCUserSP.get(), pFH.get());
	
}


CFunctions::~CFunctions(void)
{
	
	//@コメントアウト
	/*if(pFH)
	{
		delete pFH;
		pFH = nullptr;
	}
	if(pFCUserSP)
	{
		delete pFCUserSP;
		pFCUserSP = nullptr;
	}*/
}

void CFunctions::SetCommand(CString command)
{
	m_Command = command;
}

bool CFunctions::WriteHexString(FILE * fptr,char * in)
{
	for(unsigned __int32 i=0;i<strlen(in);i+=2) 
	{
		char c = in[i];
		char cc1=100;
		char cc2=100;

		if((c >=48) && (c <= 57))  
			cc1 = c-48;
		if((c >=65) && (c <= 70))  
			cc1 = c-65+10;
		if((c >=97) && (c <= 102)) 
			cc1 = c-97+10;

		c = in[i+1];
		if((c >=48) && (c <= 57))  
			cc2 = c-48;
		if((c >=65) && (c <= 70))  
			cc2 = c-65+10;
		if((c >=97) && (c <= 102)) 
			cc2 = c-97+10;

		if(cc1 != 100 && cc2 != 100) 
		{
			fputc(cc1*16+cc2,fptr);
			if(ferror(fptr)) 
				return false;
		}
	}
	return true;
}

bool CFunctions::Convert2Integer(CString pszInput, __int32 &output_int)
{
	output_int = 0;
	char *s = new char[pszInput.GetLength()+1];
	sprintf_s(s,pszInput.GetLength()+1,"%s",LPCSTR(pszInput));

	if(strlen(s) < 1)
		return false;

	while (s[0] == ' ' || s[0] == '\t')
	{
		for(unsigned __int32 i=0;i<strlen(s);i++) 
			s[i] = s[i+1];
	}

	if(strlen(s) < 1)
		return false;

	while (s[strlen(s)-1] == ' ' || s[strlen(s)-1] == '\t')
	{
		s[strlen(s)-1] = 0;
	}

	if(strlen(s) < 1)
		return false;

	__int32 len = (__int32)strlen(s);
	bool is_hex = false;
	bool is_number = true;

	if(len > 2)
	{
		if(s[0] == '0' && s[1] == 'x')
			is_hex = true;
		if(s[0] == '0' && s[1] == 'X')
		{
			s[1] = 'x';
			is_hex = true;
		}
	}

	if(is_hex)
	{
		for(__int32 i=2;i<len;i++)
		{
			if(s[i] >= '0' && s[i] <= '9')
			{
				output_int += unsigned __int32(pow(16.,(len-i-1))*(s[i]-'0'));
				continue;
			}
			else if(s[i] >= 'a' && s[i] <= 'f')
			{
				output_int += unsigned __int32(pow(16.,(len-i-1))*(s[i]-'a'+10));
				continue;
			}
			else if(s[i] >= 'A' && s[i] <= 'F')
			{
				output_int += unsigned __int32(pow(16.,(len-i-1))*(s[i]-'A'+10));
				continue;
			}
			else
			{
				is_number = false;
				break;
			}
		}
	}

	if(!is_hex)
	{
		for(__int32 i=0;i<len;i++)
		{
			if(s[i] < '0' || s[i] > '9')
			{
				if(i == 0 && (s[i] == '+' || s[i] == '-'))
					continue;
				is_number = false;
				break;
			}
		}
		output_int = atoi(s);
	}

	if(s)
	{
		delete []s;
		s = nullptr;
	}

	return is_number;
}

double CFunctions::gauss(double x, double amplitude, double x0, double fwhm)
{
	double a = x-x0;
	a *= a;
	double b = fwhm/2.35482;
	b *= b;
	return exp(-0.5*a/b)*amplitude;
}

double CFunctions::get_gauss_rms(__int32 index, __int32 left_bin, __int32 right_bin, double amp, double x0, double fwhm, double y_offset)
{
	double x_left = pFCUserSP->GetBinChannelX(index,left_bin);
	double bin_size = pFCUserSP->GetBinChannelX(index,1) - pFCUserSP->GetBinChannelX(index,0);
	double rms = 0.;
	for(__int32 i=left_bin; i<right_bin; i++) 
	{
		double x = x_left + (i-left_bin)*bin_size;
		double y_calc = y_offset + gauss(x, amp, x0, fwhm);
		double y_meas = pFCUserSP->GetBinValueAt(index,i);
		double err = y_meas - y_calc;
		rms += err*err;
	}
	return rms;
}
