#include "StdAfx.h"
#include "UserSHM.h"
#include "UserFCall.h"
#include "LMInfo.h"
#include "UserSP.h"
#include "CoboldUtilities.h"
#include "Functions.h"
#include "UserFCall.h"

#include <math.h>

//////////////////////////////////////////////////////////////////////////
// Help
//////////////////////////////////////////////////////////////////////////
bool CFunctions::Help()
{
	static CString m_TextBox;
	m_TextBox.Empty();
	m_TextBox += "The following commands are available in UserFCall dll:\r\n";
	m_TextBox += "\r\n";
	m_TextBox += "    parameters in [] can be omitted\r\n";
	m_TextBox += "\r\n";
	m_TextBox += "angle\r\n";
	m_TextBox += "beep\r\n";
	m_TextBox += "distance\r\n";
	m_TextBox += "dynintegral\r\n";
	m_TextBox += "echo[,message]\r\n";
	m_TextBox += "export_ascii_multi, file name, hist name, number of hists\r\n";
	m_TextBox += "export_tif_16bit,file_name[,index_of_histogram]\r\n";
	m_TextBox += "export_tif_24bit_rgb,file_name[,index_of_histogram]\r\n";
	m_TextBox += "fitg[,index_of_histogram]  does a gauss fit\r\n";
	m_TextBox += "gauss[,index_of_histogram]\r\n";
	m_TextBox += "    (draws a gauss curve through 4 given points.\r\n";
	m_TextBox += "     The first point defines the y-base level.)\r\n";
	m_TextBox += "get_coordinate_index[,name of coordinate]\r\n";
	m_TextBox += "get_mean_without_zeros\r\n";
	m_TextBox += "gethighestbin[,index_of_histogram]\r\n";
	m_TextBox += "linreg\r\n";
	m_TextBox += "load_img,file_name[,name_of_histogram]\r\n";
	m_TextBox += "locate\r\n";
	m_TextBox += "multidefine1,number_of_hists,usual definitions\r\n";
	m_TextBox += "multidefine2,number_of_hists,usual definitions\r\n";
	m_TextBox += "normalize[,value][,index_of_histogram]\r\n";
	m_TextBox += "normalize_integral_y[,index_of_histogram]\r\n";
	m_TextBox += "     normalize the integral of each column to 1.\r\n";
	m_TextBox += "normalize_y[,index_of_histogram]\r\n";
	m_TextBox += "     normalize the maximum of each column to 1.\r\n";
	m_TextBox += "parameter_bitmask[,parameter index][,bit pattern]\r\n";
	m_TextBox += "phd_analysis\r\n";
	m_TextBox += "proj1d,binsize[,index_of_histogram] projects a 1D-histogram\r\n";
	m_TextBox += "rebin,factorX,factorY[,index_of_histogram]\r\n";
	m_TextBox += "rescale\r\n";
	m_TextBox += "setdanresolutionparameters (presets parameter 1013 - 1019)\r\n";
	m_TextBox += "setdaqcoordinates (please see the CCFs for examples)\r\n";
	m_TextBox += "sethitmatrixcoordinatesandspectrum (please see the CCFs for examples)\r\n";
	m_TextBox += "wait[,seconds]\r\n";


	//@ユーザー関数のヘルプを追加
	pLUFs->AppendHelpText(m_TextBox);


	static CString Title = CString("UserFCall Help Information");
	pFH->ShowInformationForm(Title,m_TextBox,false);

	return true;
}

//////////////////////////////////////////////////////////////////////////
// export_ascii_multi
//////////////////////////////////////////////////////////////////////////
bool CFunctions::ExportAsciiMulti(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount)
{
	if(ui32WordCount < 6) 
	{
		AfxMessageBox("the correct syntax is: export_ascii_multi, file name, hist name, number of hists");
		return true;
	}
	
	__int32 index = -1;
	if(!Convert2Integer(csInput[4], index))
		index = pFCUserSP->FindSpectrum((char*)LPCSTR(csInput[4]));
	
	if(index < 0)
		return true;;
			
	__int32 num = atoi(LPCSTR(csInput[5]));
			
	if(num < 1) 
	{
		AfxMessageBox("the number of histograms must be greater than zero");
		return true;;
	}
	
	if(num > 100) 
	{
		AfxMessageBox("warning: more than 100 files will be written");
	}
#define filename_size	500
	char filename[filename_size];
	
	for(__int32 i=0;i<num;i++) 
	{
		sprintf_s(filename,filename_size,"%s_%i.txt",LPCSTR(csInput[3]),i);
		errno_t err;
		FILE * fi = nullptr;
		err = fopen_s(&fi,filename,"wt");
		if(fi) 
		{
			if(ferror(fi)) 
			{
				fclose(fi); 
				fi = nullptr;
			}
		}
		if(!fi) 
		{
			CString msg; 
			msg.Format("error while writing this file:\n%s",filename); 
			AfxMessageBox(msg); 
			break;
		}
	
		__int32 numx = __int32(pFCUserSP->GetNumberOfXBins(index+i));
		__int32 numy = __int32(pFCUserSP->GetNumberOfYBins(index+i));
			
		for(__int32 y = 0;y < numy;y++) 
		{
			for(__int32 x = 0;x < numx;x++) 
			{
				double content = pFCUserSP->GetBinValueAt(index+i,x,y);
				double x_pos = pFCUserSP->GetBinChannelX(index+i,x);
				double y_pos = pFCUserSP->GetBinChannelY(index+i,y);
				if(fi)
					fprintf(fi,"%lg,%lg,%lg\n",x_pos,y_pos,content);
				if(ferror(fi)) 
				{
					fclose(fi); 
					fi = nullptr;
				}
			}
		}
	
		if(fi) 
		{
			fclose(fi); 
			fi = nullptr;
		}
#undef filename_size
	}
	return true;;
}

//////////////////////////////////////////////////////////////////////////
// locate
//////////////////////////////////////////////////////////////////////////
bool CFunctions::Locate(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount)
{
	__int32 index = __int32(pFCUserSP->GetDisplayedSpectrumNumber());
	if(index < 0) 
		return true;

	CString path = GetApplicationPath();
	path += CString("\\locate.txt");

	CString contents = "";

	contents += "The following list was written into the file\n";
	contents += path;
	contents += "\n\n";

	FILE * fi = nullptr;
	errno_t err;
	err = fopen_s(&fi,LPCSTR(path),"wt");

	double x,y;
	double x_old = -1.e100;
	double y_old = -1.e100;
	double c;

	__int64 number_of_ybins = pFCUserSP->GetNumberOfYBins(index);

	if(number_of_ybins > 1) 
		contents +="x   \t\ty   \t\tbin content\n";
	if(number_of_ybins < 2) 
		contents +="x   \t\tbin content\n";

#define temp_string_size	300
	char temp_string[temp_string_size];

	while (true) 
	{
		pFCUserSP->GetXYCursor(x,y,true);
		if(x_old == x && y_old == y) 
			break;

		if(number_of_ybins > 1) 
		{
			c = pFCUserSP->GetValueAt(index,x,y);
			sprintf_s(temp_string,temp_string_size,"%lg    \t%lg    \t%lg    \n",x,y,c);
		} 
		else 
		{
			c = pFCUserSP->GetValueAt(index,x);
			sprintf_s(temp_string,temp_string_size,"%lg    \t%lg    \n",x,c);
		}
		fprintf(fi, temp_string);
		contents += temp_string;
		x_old = x;
		y_old = y;
	}
#undef temp_string_size
	fclose(fi);

	AfxMessageBox(contents);

	return true;
}

//////////////////////////////////////////////////////////////////////////
// linreg
//////////////////////////////////////////////////////////////////////////
bool CFunctions::LinReg(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount)
{
	__int32 index = __int32(pFCUserSP->GetDisplayedSpectrumNumber());
	if (index < 0) 
		return true;

	__int32 nof_points = 0;
	double * x_array = new double[1000];
	double * y_array = new double[1000];

	while (true) 
	{
		pFCUserSP->GetXYCursor(x_array[nof_points],y_array[nof_points],true);
		if (nof_points > 0)
		{ 
			if ((x_array[nof_points] == x_array[nof_points-1]) && (y_array[nof_points] == y_array[nof_points-1]))
				break;
		}

		nof_points++;		
	}

	if (nof_points < 2)
	{
		delete[] x_array; x_array = 0;
		delete[] y_array; y_array = 0;
		return true;
	}

	double x_mean = 0;
	double y_mean = 0;
	for (__int32 i=0;i<nof_points;i++)
	{
		x_mean += x_array[i];
		y_mean += y_array[i];
	}
	x_mean /= nof_points;
	y_mean /= nof_points;

	double SSxy = 0;
	double SSxx = 0;
	for (__int32 i=0;i<nof_points;i++)
	{
		SSxy +=(x_array[i] - x_mean) * (y_array[i] - y_mean);
		SSxx +=(x_array[i] - x_mean)*(x_array[i] - x_mean);
	}

	double b = SSxy/SSxx;
	double a = y_mean - b*x_mean;		

	delete[] x_array; x_array = 0;
	delete[] y_array; y_array = 0;

	CString msg;
	msg.Format("y=%lg + %lg*x   (%lg deg)",a,b,atan2(b,1)*180./3.14159265);
	AfxMessageBox(msg);

	return true;
}

//////////////////////////////////////////////////////////////////////////
// angle
//////////////////////////////////////////////////////////////////////////
bool CFunctions::Angle(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount)
{
	__int32 index = __int32(pFCUserSP->GetDisplayedSpectrumNumber());
	if(index < 0) 
		return true;

	CString contents = "";

	double x1,y1;
	double x2,y2;

#define temp_string_size	300
	char temp_string[temp_string_size];

	pFCUserSP->GetXYCursor(x1,y1,true);
	pFCUserSP->GetXYCursor(x2,y2,true);

	double d = sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));

	double angle = 180.*atan2(x2-x1,y2-y1)/3.14159265;

	angle = -angle+90;

	sprintf_s(temp_string,temp_string_size,"(%lg, %lg) to (%lg, %lg) = %lg angle = %lg deg = %lg rad\n",x1,y1,x2,y2,d,angle,angle*3.14159265/180.);

	contents += temp_string;
#undef temp_string_size
	AfxMessageBox(contents);

	return true;
}

//////////////////////////////////////////////////////////////////////////
// multidefine1
//////////////////////////////////////////////////////////////////////////
bool CFunctions::MultiDefine1(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount)
{
	__int32 num_hists = atoi(LPCSTR(csInput[3]));
	if(ui32WordCount < 12 || ui32WordCount > 12+2) 
	{
		AfxMessageBox("the correct syntax is:\nmultidefine1,number_of_histograms, and then the normal define1-definitions");
		return true;
	}

	CString basecommand = "try define1 ";
	for(__int32 k=4;k<=12-2;k++) 
	{
		basecommand += csInput[k]; 
		basecommand += ",";
	}
	basecommand += csInput[12-1];

	CString logx = "";
	CString logy = "";
	if(ui32WordCount > 12)
		logx = csInput[12];
	if(ui32WordCount > 12+1)
		logy = csInput[12+1];

	for(__int32 i=0;i< num_hists;i++) 
	{
#define temp_string_size	10
		char number[temp_string_size];
		sprintf_s(number,temp_string_size,"%i",i);
#undef temp_string_size

		m_Command = basecommand + number;
		if(ui32WordCount > 12)
			m_Command += ","+logx;
		if(ui32WordCount > 12+1)
			m_Command += ","+logy;

		m_Command += ";";
		ExecuteCommand(m_Command);
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
// multidefine2
//////////////////////////////////////////////////////////////////////////
bool CFunctions::MultiDefine2(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount)
{
	__int32 num_hists = atoi(LPCSTR(csInput[3]));
	if(ui32WordCount < 12+5 || ui32WordCount > 12+5+3) 
	{
		AfxMessageBox("the correct syntax is:\nmultidefine2,number_of_histograms, and then the normal define2-definitions");
		return true;
	}

	CString basecommand = "try define2 ";
	for(__int32 k=4;k<=12+3;k++) 
	{
		basecommand += csInput[k];
		basecommand += ",";
	}
	basecommand += csInput[12+4];

	CString logx = "";
	CString logy = "";
	CString logz = "";
	if(ui32WordCount > 12+5)
		logx = csInput[12+5];
	if(ui32WordCount > 12+5+1)
		logy = csInput[12+5+1];
	if(ui32WordCount > 12+5+2)
		logz = csInput[12+5+2];

	for(__int32 i=0;i< num_hists;i++) 
	{
#define temp_string_size	10
		char number[temp_string_size];
		sprintf_s(number,temp_string_size,"%i",i);
#undef temp_string_size
		m_Command = basecommand + number;
		if(ui32WordCount > 12+5  )
			m_Command += ","+logx;
		if(ui32WordCount > 12+5+1)
			m_Command += ","+logy;
		if(ui32WordCount > 12+5+2)
			m_Command += ","+logz;

		m_Command += ";";
		ExecuteCommand(m_Command);
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
// load_img
//////////////////////////////////////////////////////////////////////////
bool CFunctions::LoadImg(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount)
{
	CImage img;
	HRESULT hResult;
	CString hist_name = "";
	CString path_name = "";
	__int32 index;

	if(ui32WordCount >= 4) 
	{
		path_name = csInput[3];
	} 
	else 
	{
		AfxMessageBox("No filename was given.\n");
		return true;
	}

	hResult = img.Load(path_name);

	if(hResult != 0) 
	{
		AfxMessageBox("Could not load the image-file. File not found.");
		return true;
	}

	__int32 nof_xbins = img.GetWidth();
	__int32 nof_ybins = img.GetHeight();

	if(ui32WordCount > 4) 
	{
		hist_name = csInput[4];
	} 
	else 
	{
		hist_name = path_name;
		index = hist_name.ReverseFind('/');
		if(index >= 0) 
			hist_name = hist_name.Right(hist_name.GetLength()-index-1);
		index = hist_name.ReverseFind('\\');
		if(index >= 0) 
			hist_name = hist_name.Right(hist_name.GetLength()-index-1);
	}

	index = pFCUserSP->FindSpectrum((char*)LPCSTR(hist_name));
	if(index == -1) 
	{
		m_Command.Format("define2 0,%i,1.,none,x,0,%i,1.,none,y,none,always,%s", nof_xbins, nof_ybins, LPCSTR(hist_name));
		ExecuteCommand(m_Command);
		index = pFCUserSP->FindSpectrum((char*)LPCSTR(hist_name));
	}

	if(index == -1) 
		return true;

	__int32 hist_nof_xbins = __int32(pFCUserSP->GetNumberOfXBins(index));
	__int32 hist_nof_ybins = __int32(pFCUserSP->GetNumberOfYBins(index));

	if(hist_nof_xbins < nof_xbins) 
	{
		CString msg;
		msg.Format("the target histogram is too small in x-direction (%i instead of %i bins)",hist_nof_xbins,nof_xbins);
		AfxMessageBox(msg);
		return true;
	}
	if(hist_nof_ybins < nof_ybins) 
	{
		CString msg;
		msg.Format("the target histogram is too small in y-direction (%i instead of %i bins)",hist_nof_ybins,nof_ybins);
		AfxMessageBox(msg);
		return true;
	}

	for(__int32 ix=0;ix<hist_nof_xbins;ix++) 
	{
		for(__int32 iy=0;iy<hist_nof_ybins;iy++) 
		{
			if(ix >= nof_xbins) 
			{
				pFCUserSP->SetBinValueAt(index,ix,hist_nof_ybins-iy-1,0); 
				continue;
			}
			if(iy >= nof_ybins) 
			{
				pFCUserSP->SetBinValueAt(index,ix,hist_nof_ybins-iy-1,0); 
				continue;
			}
			unsigned rgb = img.GetPixel(ix, iy);
			unsigned content = rgb & 0xff;
			if(content < ((rgb & 0xff00  )>>8 )) 
				content = ((rgb & 0xff00  )>>8 );
			if(content < ((rgb & 0xff0000)>>16)) 
				content = ((rgb & 0xff0000)>>16);
			pFCUserSP->SetBinValueAt(index,ix,hist_nof_ybins-iy-1,255-double(content));
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
// export_tif_24bit_rgb
//////////////////////////////////////////////////////////////////////////
bool CFunctions::ExportTIF24BitRGB(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount)
{
	__int32 index = -1;
	if(ui32WordCount == 4) 
		index = __int32(pFCUserSP->GetDisplayedSpectrumNumber());
	if(ui32WordCount == 5)
		index = atoi(LPCTSTR(csInput[4]));
	if(index == -1) 
		return false;
	__int64 number_of_xbins = pFCUserSP->GetNumberOfXBins(index);
	__int64 number_of_ybins = pFCUserSP->GetNumberOfYBins(index);
	if(number_of_xbins < 1) 
		return false;
	if(number_of_ybins < 1) 
		return false;

	double min =  1.e200;
	double max = -1.e200;
	for(__int32 j=__int32(number_of_ybins)-1;j>=0;j--)
	{
		for(__int32 i=0;i<__int32(number_of_xbins);i++)
		{
			double content = pFCUserSP->GetBinValueAt(index,__int64(i),__int64(j));
			if(content < min) 
				min = content;
			if(content > max) 
				max = content;
		}
	}

	if(min < 0.) 
	{
		AfxMessageBox("At least one pixel has a value smaller than zero.\n");
	}

	if(min < 0.) 
	{
		AfxMessageBox("At least one pixel has a value greater than 255.\nYou can use the function \"normalize\" in this DLL to solve this problem.\n");
	}

	CString filename = pFH->GetPath(csInput[3]) + CString("\\") + pFH->GetFileNameWithoutExtension(csInput[3]);
	filename.Replace("\\\\", "\\");					// remove possible double backslash
	filename.TrimLeft("\\\\");
	CString fileextension = ".tif";

	while (pFH->FileExists(filename+fileextension)) 
	{
		__int32 i = filename.GetLength()-1;
		while (i>=0) 
		{
			char c = filename.GetAt(i);
			if(c < '0'	|| c > '9') 
				break;
			i--;
		}
		i++;
		if(i >= filename.GetLength()) 
		{
			filename += "1";
			continue;
		}
#define num_size	50
		char num[num_size];
		sprintf_s(num,num_size,"%s",LPCSTR(filename.Right(filename.GetLength()-i)));
		__int32 number = atoi(num);
		number++;
		sprintf_s(num,num_size,"%i",number);
		filename.Delete(i,filename.GetLength()-i);
		filename+=num;
#undef num_size
	}

	filename += fileextension;
	FILE *fptr = nullptr;
	errno_t err;

	err = fopen_s(&fptr,LPCTSTR(filename),"wb");

	if(!fptr) 
		return false;
	if(ferror(fptr)) 
		return false;

	//4002=8+34+x*y*2
	/* Write the header */
	WriteHexString(fptr,"4d4d002a");    /* little endian & TIFF identifier */
	unsigned __int32 offset = unsigned __int32(number_of_xbins * number_of_ybins * 3 + 8);
	putc((offset & 0xff000000) / 16777216,fptr);
	putc((offset & 0x00ff0000) / 65536,fptr);
	putc((offset & 0x0000ff00) / 256,fptr);
	putc((offset & 0x000000ff),fptr);

	//WriteHexString(fptr,"696d616765206465");
	//WriteHexString(fptr,"736372697074696f6e0000dd6d00a086");
	//WriteHexString(fptr,"010000dd6d00a0860100");


	/* Write the binary data */
	for(__int32 j=__int32(number_of_ybins)-1;j>=0;j--)
	{
		for(__int32 i=0;i<__int32(number_of_xbins);i++)
		{
			double content = pFCUserSP->GetBinValueAt(index,__int64(i),__int64(j));
			if(content > 255) 
				content = 255.1;
			if(content < 0. ) 
				content = 0.;
			unsigned __int8 cont8bit = unsigned __int8(content);
			cont8bit = 255 - cont8bit;
			fputc(cont8bit,fptr); // red
			fputc(cont8bit,fptr); // green
			fputc(cont8bit,fptr); // blue
		}
	}

	/* Write the footer */
	WriteHexString(fptr,"000e");  /* The number of directory entries (14) */

	/* Width tag, short int */
	WriteHexString(fptr,"0100000300000001");
	fputc((number_of_xbins & 0xff00) / 256,fptr);    /* Image width */
	fputc((number_of_xbins & 0x00ff),fptr);
	WriteHexString(fptr,"0000");

	/* Height tag, short int */
	WriteHexString(fptr,"0101000300000001");
	fputc((number_of_ybins & 0xff00) / 256,fptr);    /* Image height */
	fputc((number_of_ybins & 0x00ff),fptr);
	WriteHexString(fptr,"0000");

	/* Bits per sample tag, short int */
	WriteHexString(fptr,"0102000300000003");
	offset = unsigned __int32(number_of_xbins * number_of_ybins * 3 + 182);
	putc((offset & 0xff000000) / 16777216,fptr);
	putc((offset & 0x00ff0000) / 65536,fptr);
	putc((offset & 0x0000ff00) / 256,fptr);
	putc((offset & 0x000000ff),fptr);

	/* Compression flag, short int */
	WriteHexString(fptr,"010300030000000100010000");

	/* Photometric interpolation tag, short int */
	WriteHexString(fptr,"010600030000000100020000");

	/* Strip offset tag, long int */
	WriteHexString(fptr,"011100040000000100000008");

	/* Orientation flag, short int */
	WriteHexString(fptr,"011200030000000100010000");

	/* Sample per pixel tag, short int */
	WriteHexString(fptr,"011500030000000100030000");

	/* Rows per strip tag, short int */
	WriteHexString(fptr,"0116000300000001");
	fputc((number_of_ybins & 0xff00) / 256,fptr);
	fputc((number_of_ybins & 0x00ff),fptr);
	WriteHexString(fptr,"0000");

	/* Strip byte count flag, long int */
	WriteHexString(fptr,"0117000400000001");
	offset = unsigned __int32(number_of_xbins * number_of_ybins * 3);
	putc((offset & 0xff000000) / 16777216,fptr);
	putc((offset & 0x00ff0000) / 65536,fptr);
	putc((offset & 0x0000ff00) / 256,fptr);
	putc((offset & 0x000000ff),fptr);

	/* Minimum sample value flag, short int */
	WriteHexString(fptr,"0118000300000003");
	offset = unsigned __int32(number_of_xbins * number_of_ybins * 3 + 188);
	putc((offset & 0xff000000) / 16777216,fptr);
	putc((offset & 0x00ff0000) / 65536,fptr);
	putc((offset & 0x0000ff00) / 256,fptr);
	putc((offset & 0x000000ff),fptr);

	/* Maximum sample value tag, short int */
	WriteHexString(fptr,"0119000300000003");
	offset = unsigned __int32(number_of_xbins * number_of_ybins * 3 + 194);
	putc((offset & 0xff000000) / 16777216,fptr);
	putc((offset & 0x00ff0000) / 65536,fptr);
	putc((offset & 0x0000ff00) / 256,fptr);
	putc((offset & 0x000000ff),fptr);

	/* Planar configuration tag, short int */
	WriteHexString(fptr,"011c00030000000100010000");

	/* Sample format tag, short int */
	WriteHexString(fptr,"0153000300000003");
	offset = unsigned __int32(number_of_xbins * number_of_ybins * 3 + 200);
	putc((offset & 0xff000000) / 16777216,fptr);
	putc((offset & 0x00ff0000) / 65536,fptr);
	putc((offset & 0x0000ff00) / 256,fptr);
	putc((offset & 0x000000ff),fptr);

	/* End of the directory entry */
	WriteHexString(fptr,"00000000");

	/* Bits for each color channel */
	WriteHexString(fptr,"000800080008");

	/* Minimum value for each component */
	WriteHexString(fptr,"000000000000");

	/* Maximum value per channel */
	WriteHexString(fptr,"00ff00ff00ff");

	/* Samples per pixel for each channel */
	WriteHexString(fptr,"000100010001");

	fclose(fptr);

	return true;
}

//////////////////////////////////////////////////////////////////////////
// export_tif_16bit
// (16 bit special gray scale format)
//////////////////////////////////////////////////////////////////////////
bool CFunctions::ExportTIF16Bit(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount)
{
	__int32 index = -1;
	if(ui32WordCount == 4) 
		index = __int32(pFCUserSP->GetDisplayedSpectrumNumber());
	if(ui32WordCount == 5)
		index = atoi(LPCTSTR(csInput[4]));
	if(index == -1) 
		return false;
	__int64 number_of_xbins = pFCUserSP->GetNumberOfXBins(index);
	__int64 number_of_ybins = pFCUserSP->GetNumberOfYBins(index);
	if(number_of_xbins < 1) 
		return false;
	if(number_of_ybins < 1) 
		return false;

	CString filename = pFH->GetPath(csInput[3]) + CString("\\") + pFH->GetFileNameWithoutExtension(csInput[3]);
	filename.Replace("\\\\", "\\");					// remove possible double backslash
	filename.TrimLeft("\\\\");
	CString fileextension = ".tif";

	while (pFH->FileExists(filename+fileextension)) 
	{
		__int32 i = filename.GetLength()-1;
		while (i>=0) 
		{
			char c = filename.GetAt(i);
			if(c < '0'	|| c > '9') 
				break;
			i--;
		}
		i++;
		if(i >= filename.GetLength()) 
		{
			filename += "1";
			continue;
		}
#define num_size	50
		char num[num_size];
		sprintf_s(num,num_size,"%s",LPCSTR(filename.Right(filename.GetLength()-i)));
		__int32 number = atoi(num);
		number++;
		sprintf_s(num,num_size,"%i",number);
		filename.Delete(i,filename.GetLength()-i);
		filename+=num;
#undef num_size
	}

	filename += fileextension;
	FILE * fptr = nullptr;
	errno_t err;

	err = fopen_s(&fptr,LPCTSTR(filename),"wb");

	if(!fptr) 
		return false;
	if(ferror(fptr)) 
		return false;

	//4002=8+34+x*y*2
	/* Write the header */
	WriteHexString(fptr,"49492a00");    /* Big endian & TIFF identifier */
	unsigned __int32 offset = unsigned __int32(number_of_xbins * number_of_ybins * 2 + 8+34);
	putc((offset & 0x000000ff),fptr);
	putc((offset & 0x0000ff00) >>  8,fptr);
	putc((offset & 0x00ff0000) >>  16,fptr);
	putc((offset & 0xff000000) >>  24,fptr);

	WriteHexString(fptr,"696d616765206465");
	WriteHexString(fptr,"736372697074696f6e0000dd6d00a086");
	WriteHexString(fptr,"010000dd6d00a0860100");

	/* Write the binary data */
	for(__int32 j=__int32(number_of_ybins)-1;j>=0;j--) 
	{
		for(__int32 i=0;i<__int32(number_of_xbins);i++) 
		{
			double content = pFCUserSP->GetBinValueAt(index,__int64(i),__int64(j));
			unsigned __int16 cont16bit = unsigned __int16(content);
			fputc((cont16bit & 0x00ff),fptr);
			fputc((cont16bit & 0xff00) >> 8,fptr);
		}
	}

	/* Write the footer */
	WriteHexString(fptr,"1f00fe00040001000000000000000001");
	WriteHexString(fptr,"040001000000");

	offset = unsigned __int32(number_of_xbins);
	putc((offset & 0x000000ff),fptr);
	putc((offset & 0x0000ff00) >>  8,fptr);
	putc((offset & 0x00ff0000) >>  16,fptr);
	putc((offset & 0xff000000) >>  24,fptr);

	WriteHexString(fptr,"010104000100");
	WriteHexString(fptr,"0000");

	offset = unsigned __int32(number_of_ybins);
	putc((offset & 0x000000ff),fptr);
	putc((offset & 0x0000ff00) >>  8,fptr);
	putc((offset & 0x00ff0000) >>  16,fptr);
	putc((offset & 0xff000000) >>  24,fptr); 

	WriteHexString(fptr,"02010300010000001000");
	WriteHexString(fptr,"00000301030001000000010000000601");
	//WriteHexString(fptr,"030001000000010000000e0102001200"); // 0 is black, 0xffff is white
	WriteHexString(fptr,"030000000000010000000e0102001200"); // 0 is white, 0xffff is black
	WriteHexString(fptr,"00000800000011010400010000002a00");
	WriteHexString(fptr,"00001201030001000000010000001501");
	WriteHexString(fptr,"03000100000001000000160104000100");
	WriteHexString(fptr,"0000");
	offset = unsigned __int32(number_of_ybins);
	putc((offset & 0x000000ff),fptr);
	putc((offset & 0x0000ff00) >>  8,fptr);
	putc((offset & 0x00ff0000) >>  16,fptr);
	putc((offset & 0xff000000) >>  24,fptr);
	WriteHexString(fptr,"1701040001000000");
	offset = unsigned __int32(number_of_xbins * number_of_ybins * 2);
	putc((offset & 0x000000ff),fptr);
	putc((offset & 0x0000ff00) >>  8,fptr);
	putc((offset & 0x00ff0000) >>  16,fptr);
	putc((offset & 0xff000000) >>  24,fptr);
	WriteHexString(fptr,"1a010500010000001a0000001b01");
	WriteHexString(fptr,"05000100000022000000280103000100");
	WriteHexString(fptr,"000002000000e084020000000000e703");
	WriteHexString(fptr,"0000e184020000000000e7030000e284");
	WriteHexString(fptr,"020000000000e7030000e38403000100");
	WriteHexString(fptr,"000000000000e4840300010000000800");
	WriteHexString(fptr,"0000e58403000100000010000000e684");
	WriteHexString(fptr,"01000000000000000000e78401000100");
	WriteHexString(fptr,"000000000000e8840100010000000000");
	WriteHexString(fptr,"0000e98401000000000000000000ea84");
	WriteHexString(fptr,"01000000000000000000eb8401000200");
	WriteHexString(fptr,"000000000000ec840100010000000000");
	WriteHexString(fptr,"0000ed8402000000000000000000ee84");
	WriteHexString(fptr,"04000100000000000000ef8401000000");
	WriteHexString(fptr,"0000000000000000000000");

	fclose(fptr);

	return true;
}

//////////////////////////////////////////////////////////////////////////
// fitg
//////////////////////////////////////////////////////////////////////////
bool CFunctions::FitG(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount)
{
	__int32 index = -1;
	if(ui32WordCount == 3)
		index = __int32(pFCUserSP->GetDisplayedSpectrumNumber());
	if(ui32WordCount == 4)
	{
		if(!Convert2Integer(csInput[3], index)) 
			index = pFCUserSP->FindSpectrum((char*)LPCSTR(csInput[3]));
	}
	if(index == -1) 
		return false;
	__int64 number_of_xbins = pFCUserSP->GetNumberOfXBins(index);
	__int64 number_of_ybins = pFCUserSP->GetNumberOfYBins(index);
	if(number_of_xbins < 1) // x-binsize must be available
		return false;
	if(number_of_ybins > 0) // runs only for 1 dim spectra
		return false;

	double x1,y1;
	double x2,y2;

	pFCUserSP->GetXYCursor(x1,y1,true);
	pFCUserSP->GetXYCursor(x2,y2,true);

	if(x2 < x1) 
	{
		double temp;
		temp = x1;	
		x1 = x2;	
		x2 = temp;
		temp = y1;	
		y1 = y2;	
		y2 = temp;
	}

	double x0, fwhm, amp, y_offset;

	__int32 left_bin  = __int32(pFCUserSP->GetBinIndexX(index,x1));
	__int32 right_bin = __int32(pFCUserSP->GetBinIndexX(index,x2));
	double x_left = pFCUserSP->GetBinChannelX(index,left_bin);

	y_offset = 1.e200;
	for(__int32 i=left_bin; i<right_bin; i++) 
	{
		double y = pFCUserSP->GetBinValueAt(index,i);
		if(y < y_offset) 
			y_offset = y;
	}
	if(y_offset == 0.) 
		y_offset = 1.;

	amp = -1.e200;
	for(__int32 i=left_bin; i<right_bin; i++) 
	{
		double y = pFCUserSP->GetBinValueAt(index,i) - y_offset;
		if(amp < y) 
		{
			amp = y; 
			x0 = pFCUserSP->GetBinChannelX(index,i) - x_left;
		}
	}

	for(__int32 i=left_bin; i<right_bin; i++) 
	{
		double y = pFCUserSP->GetBinValueAt(index,i) - y_offset;
		if(amp < y)
			amp = y;
	}

	double temp_x1 = -1.e200;
	double temp_x2 = -1.e200;
	double min_diff = 1.e200;
	__int32 flanke = -1;
	for(__int32 i=left_bin; i<right_bin; i++) 
	{
		double y = pFCUserSP->GetBinValueAt(index,i) - y_offset;
		double diff = fabs(y-amp*0.5);
		if(min_diff > diff) 
		{
			min_diff = diff;
			if(flanke == -1) 
				temp_x1 = pFCUserSP->GetBinChannelX(index,i);
			if(flanke == +1) 
				temp_x2 = pFCUserSP->GetBinChannelX(index,i);
		}
		if(flanke == -1 && pFCUserSP->GetBinChannelX(index,i) > x0+x_left) 
		{
			flanke = +1;
			min_diff = 1.e200;
		}
	}
	if(temp_x1 > -1.e199 && temp_x2 > -1.e199) 
	{
		fwhm = temp_x2 - temp_x1;
	} 
	else 
	{
		fwhm = (x2-x1)*0.1;
	}

	double old_rms = get_gauss_rms(index, left_bin, right_bin, amp, x0+x_left, fwhm, y_offset);
	double fac_fwhm = 1.01;
	double fac_amp = 1.01;
	double fac_x0 = 1.01;
	double fac_y_offs = 1.1;
	__int32 last_dir_fwhm = 0;
	__int32 last_dir_amp = 0;
	__int32 last_dir_x0 = 0;
	__int32 last_dir_y_offs = 0;

	__int32 counter = 0;
	double limit = 1.0000001;
L200:
	counter++;
	double diff_rms_fwhm	= get_gauss_rms(index, left_bin, right_bin, amp, x0+x_left, fwhm*fac_fwhm, y_offset) - old_rms;
	double diff_rms_amp		= get_gauss_rms(index, left_bin, right_bin, amp*fac_amp, x0+x_left, fwhm, y_offset) - old_rms;
	double diff_rms_x0		= get_gauss_rms(index, left_bin, right_bin, amp, x0*fac_x0+x_left, fwhm, y_offset) - old_rms;
	double diff_rms_y_offs	= get_gauss_rms(index, left_bin, right_bin, amp, x0+x_left, fwhm, y_offset*fac_y_offs) - old_rms;

	double abs_diff_rms_fwhm	=	fabs(diff_rms_fwhm);
	double abs_diff_rms_amp		=	fabs(diff_rms_amp);
	double abs_diff_rms_x0		=	fabs(diff_rms_x0);
	double abs_diff_rms_y_offs	=	fabs(diff_rms_y_offs);

	if(abs_diff_rms_fwhm > abs_diff_rms_amp && abs_diff_rms_fwhm > abs_diff_rms_x0 && abs_diff_rms_fwhm > abs_diff_rms_y_offs) 
	{
		if(diff_rms_fwhm < 0) 
		{
			fwhm *= fac_fwhm;
			if(last_dir_fwhm == +1) 
				fac_fwhm = (fac_fwhm-1)*0.25 + 1.;
			last_dir_fwhm = -1;
		} 
		else 
		{
			fwhm /= fac_fwhm;
			if(last_dir_fwhm == -1) 
				fac_fwhm = (fac_fwhm-1)*0.25 + 1.;
			last_dir_fwhm = +1;
		}
	} 
	else if(abs_diff_rms_amp > abs_diff_rms_fwhm && abs_diff_rms_amp > abs_diff_rms_x0 && abs_diff_rms_amp > abs_diff_rms_y_offs) 
	{
		if(diff_rms_amp < 0) 
		{
			amp *= fac_amp;
			if(last_dir_amp == +1) 
				fac_amp = (fac_amp-1)*0.25 + 1.;
			last_dir_amp = -1;
		} 
		else 
		{
			amp /= fac_amp;
			if(last_dir_amp == -1) 
				fac_amp = (fac_amp-1)*0.25 + 1.;
			last_dir_amp = +1;
		}
	} 
	else if(abs_diff_rms_x0 > abs_diff_rms_amp && abs_diff_rms_x0 > abs_diff_rms_fwhm && abs_diff_rms_x0 > abs_diff_rms_y_offs) 
	{
		if(diff_rms_x0 < 0) 
		{
			x0 *= fac_x0;
			if(last_dir_x0 == +1) 
				fac_x0 = (fac_x0-1)*0.25 + 1.;
			last_dir_x0 = -1;
		} 
		else 
		{
			x0 /= fac_x0;
			if(last_dir_x0 == -1) 
				fac_x0 = (fac_x0-1)*0.25 + 1.;
			last_dir_x0 = +1;
		}
	} 
	else 
	{
		if(diff_rms_y_offs < 0) 
		{
			y_offset *= fac_y_offs;
			if(last_dir_y_offs == +1) 
				fac_y_offs = (fac_y_offs-1)*0.25 + 1.;
			last_dir_y_offs = -1;
		} 
		else 
		{
			y_offset /= fac_y_offs;
			if(last_dir_y_offs == -1) 
				fac_y_offs = (fac_y_offs-1)*0.25 + 1.;
			last_dir_y_offs = +1;
		}
	}
	old_rms = get_gauss_rms(index, left_bin, right_bin, amp, x0+x_left, fwhm, y_offset);
	if(fac_amp > limit || fac_fwhm > limit || fac_x0 > limit)
	{
		if(counter < 1000) 
			goto L200;
	}

	if(fac_y_offs > limit && fabs(y_offset*fac_y_offs)*500 > fabs(amp)) 
	{
		if(counter < 1000) 
			goto L200;
	}

	pFCUserSP->SetGaussFitValues(index,x0+x_left,amp,fwhm/2.35482,y_offset,-1.e200,1.e200,-1.e200,1.e200);
	return true;
}

//////////////////////////////////////////////////////////////////////////
// gauss
//////////////////////////////////////////////////////////////////////////
bool CFunctions::Gauss(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount)
{
	__int32 index = -1;
	if(ui32WordCount == 3)
		index = __int32(pFCUserSP->GetDisplayedSpectrumNumber());
	if(ui32WordCount == 4)
	{
		if(!Convert2Integer(csInput[3], index)) 
			index = pFCUserSP->FindSpectrum((char*)LPCSTR(csInput[3]));
	}
	if(index == -1) 
		return false;
	__int64 number_of_xbins = pFCUserSP->GetNumberOfXBins(index);
	__int64 number_of_ybins = pFCUserSP->GetNumberOfYBins(index);
	if(number_of_xbins < 1) // x-binsize must be available
		return false;
	if(number_of_ybins > 0) // runs only for 1 dim spectra
		return false;

	double y_offset;
	double x1,y1;
	double x[3],y[3];

L100:

	pFCUserSP->GetXYCursor(x1,y_offset,true);

	if(y_offset < 0.)
	{
		y_offset = 0.;
	}

	while(true)
	{
		pFCUserSP->GetXYCursor(x1,y1,true);
		y1 -= y_offset;
		if(y1 > 0.) 
			break;
		AfxMessageBox("y must be > 0. Please click again.");
		goto L100;
	}
	x[0] = x1; y[0] = log(y1);

	while(true)
	{
		pFCUserSP->GetXYCursor(x1,y1,true);
		y1 -= y_offset;
		if(y1 > 0.) 
			break;
		AfxMessageBox("y must be > 0. Please click again.");
		goto L100;
	}
	x[1] = x1; y[1] = log(y1);

	while(true)
	{
		pFCUserSP->GetXYCursor(x1,y1,true);
		y1 -= y_offset;
		if(y1 > 0.) 
			break;
		AfxMessageBox("y must be > 0. Please click again.");
		goto L100;
	}
	x[2] = x1; y[2] = log(y1);

	for(__int32 i=0;i<2;i++) 
	{
		if(x[i]>x[i+1]) 
		{
			double x_temp = x[i];
			double y_temp = y[i];
			x[i] = x[i+1];
			y[i] = y[i+1];
			x[i+1] = x_temp;
			y[i+1] = y_temp;
			i = -1;
		}
	}

	double x0_2 = x[0]*x[0];
	double x1_2 = x[1]*x[1];
	double x2_2 = x[2]*x[2];
	double y0_2 = y[0]*y[0];
	double y1_2 = y[1]*y[1];
	double y2_2 = y[2]*y[2];
	double V = (y[1]*x0_2*x[2]-y[2]*x0_2*x[1]+y[2]*x[0]*x1_2-y[1]*x[0]*x2_2 - y[0]*x1_2*x[2]+y[0]*x[1]*x2_2)/(x0_2*x[2]-x0_2*x[1]+x[0]*x1_2-x[0]*x2_2-x1_2*x[2]+x[1]*x2_2);
	double B = -(x[0]*y[1]-x[1]*y[0]-x[0]*y[2]+x[2]*y[0]+x[1]*y[2]-x[2]*y[1]) / (x0_2*x[2]-x0_2*x[1]+x[0]*x1_2-x[0]*x2_2-x1_2*x[2]+x[1]*x2_2);
	double x0 = (x0_2*y[1]-x1_2*y[0]-x0_2*y[2]+x2_2*y[0]+x1_2*y[2]-x2_2*y[1]) / (2*x[0]*y[1]-2*x[1]*y[0]-2*x[0]*y[2]+2*x[2]*y[0]+2*x[1]*y[2]-2*x[2]*y[1]);
	double y0 = exp(V+B*x0*x0);
	double fwhm = sqrt(1./B)*1.66511;
	pFCUserSP->SetGaussFitValues(index,x0,y0,fwhm/2.35482,y_offset,-1.e200,1.e200,-1.e200,1.e200);
	return true;
}

//////////////////////////////////////////////////////////////////////////
// get_coordinate_index
//////////////////////////////////////////////////////////////////////////
bool CFunctions::GetCoordinateIndex(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount)
{
	CString msg;
	__int32 index = pFCUserSP->GetCoordinateIndexByName((char*)LPCTSTR(csInput[3]));
	if(index >-1) 
		msg.Format("Coordinate \"%s\" has index %i",csInput[3],index);
	if(index == -1) 
		msg.Format("Coordinate \"%s\" does not exist",csInput[3]);
	AfxMessageBox(msg,MB_ICONEXCLAMATION|MB_OK);
	return true;
}

//////////////////////////////////////////////////////////////////////////
// beep
//////////////////////////////////////////////////////////////////////////
bool CFunctions::Beep(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount)
{
	if(ui32WordCount < 3) 
		return false;
	if(ui32WordCount > 4) 
		return false;

	unsigned __int32 input = 0xffffffff;
	MessageBeep(input);
	return true;
}

//////////////////////////////////////////////////////////////////////////
// distance
//////////////////////////////////////////////////////////////////////////
bool CFunctions::Distance(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount)
{
	double x1,y1,x2,y2;
	//pFCUserSP->GetXYBoxCursor(x1,y1,x2,y2, true);
	pFCUserSP->GetXYCursor(x1,y1,true);
	pFCUserSP->GetXYCursor(x2,y2,true);
	double dx = x2-x1;
	double dy = y2-y1;
	double d = sqrt(dx*dx+dy*dy);
	CString msg;
	msg.Format("The distance between the points\nx1=%lg, y1=%lg\nand\nx2=%lg, y2=%lg\nis\n%lg    (dx=%lg,  dy=%lg)",x1,y1,x2,y2,d,dx,dy);
	AfxMessageBox(msg,MB_ICONEXCLAMATION|MB_OK);
	return true;
}

//////////////////////////////////////////////////////////////////////////
// parameter_bitmask
//////////////////////////////////////////////////////////////////////////
bool CFunctions::ParameterBitMask(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount)
{
	CDoubleArray &Parameter = GetDAqDAnParameters();
	__int32 index = atoi(LPCTSTR(csInput[3]));
	CString csMask = csInput[4];
	csMask.Remove(' ');
	csMask.Remove('\t');
	double dn = 1;
	double value = 0.;
	while(csMask.GetLength() > 0) 
	{
		CString r = csMask.Right(1);
		csMask.Truncate(csMask.GetLength()-1);
		if(r == CString("1")) 
		{
			value += dn;
			dn *= 2.;
		}
		if(r == CString("0")) 
			dn *= 2.;
	}
	Parameter.SetAtGrow(index,value);
	return true;
}

//////////////////////////////////////////////////////////////////////////
// normalize_integral_y
//////////////////////////////////////////////////////////////////////////
bool CFunctions::NormalizeIntegralY(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount)
{
	__int32 index = -1;
	bool use_current = false;
	if(ui32WordCount == 3)
	{
		index = __int32(pFCUserSP->GetDisplayedSpectrumNumber());
		use_current = true;
	}
	if(ui32WordCount == 4)
	{
		if(!Convert2Integer(csInput[3], index)) 
			index = pFCUserSP->FindSpectrum((char*)LPCSTR(csInput[3]));
	}
	if(index == -1) 
		return false;
	__int64 number_of_xbins = pFCUserSP->GetNumberOfXBins(index);
	__int64 number_of_ybins = pFCUserSP->GetNumberOfYBins(index);
	if(number_of_xbins < 1) 
		return false;
	if(number_of_ybins < 1)			// must be a 2 dim spectrum so xbins and ybins must be valid
		return false;

	__int32 max_binx = 0;
	__int32 max_biny = 0;

	CString msg;

	for(__int32 i = 0;i < number_of_xbins;++i) 
	{
		double integral = 0;
		for(__int32 j = 0;j < number_of_ybins;++j)
			integral += pFCUserSP->GetBinValueAt(index,__int64(i),__int64(j));
		for(__int32 j = 0;j < number_of_ybins;++j)
		{
			double content = pFCUserSP->GetBinValueAt(index,__int64(i),__int64(j));
			pFCUserSP->SetBinValueAt(index,__int64(i),__int64(j),content/integral);
		}
	}

	if (use_current) 
		ExecuteCommand("update");

	return true;
} 



//////////////////////////////////////////////////////////////////////////
// normalize_y
//////////////////////////////////////////////////////////////////////////
bool CFunctions::Normalize_Y(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount)
{
	__int32 index = -1;
	bool use_current = false;
	if(ui32WordCount == 3) 
	{
		index = __int32(pFCUserSP->GetDisplayedSpectrumNumber());
		use_current = true;
	}
	if(ui32WordCount == 4)
	{
		if (!Convert2Integer(csInput[3], index)) 
			index = pFCUserSP->FindSpectrum((char*)LPCSTR(csInput[3]));
	}
	if(index == -1) 
		return false;
	__int64 number_of_xbins = pFCUserSP->GetNumberOfXBins(index);
	__int64 number_of_ybins = pFCUserSP->GetNumberOfYBins(index);
	if(number_of_xbins < 1) 
		return false;
	if(number_of_ybins < 1)			// must be a 2 dim spectrum so xbins and ybins must be valid
		return false;

	__int32 max_binx = 0;
	__int32 max_biny = 0;

	CString msg;

	for(__int32 i = 0;i < number_of_xbins;++i) 
	{
		double max = -1.e200;
		for(__int32 j = 0;j < number_of_ybins;++j) 
		{
			double v = pFCUserSP->GetBinValueAt(index,__int64(i),__int64(j));
			if (v > max) 
				max = v;
		}
		for(__int32 j = 0;j < number_of_ybins;++j)
		{
			double content = pFCUserSP->GetBinValueAt(index,__int64(i),__int64(j));
			pFCUserSP->SetBinValueAt(index,__int64(i),__int64(j),content/max);
		}
	}

	if (use_current) 
		ExecuteCommand("update");

	return true;
} 

//////////////////////////////////////////////////////////////////////////
// echo
//////////////////////////////////////////////////////////////////////////
bool CFunctions::Echo(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount)
{
	CString msg;
	msg.Format(LPCSTR(csInput[3]));
	AfxMessageBox(msg,MB_ICONEXCLAMATION|MB_OK);
	return true;
}

//////////////////////////////////////////////////////////////////////////
// wait
//////////////////////////////////////////////////////////////////////////
bool CFunctions::Wait(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount)
{
	if(ui32WordCount != 4) 
	{
		CString msg;
		msg.Format("wrong number of arguments for \"wait\"");
		AfxMessageBox(msg,MB_ICONEXCLAMATION|MB_OK);
		return false;
	}
	CString Cobold_command;
	double delay_seconds = atof(LPCSTR(csInput[3]));
	Sleep(1000*__int32(delay_seconds));
	return true;
}

//////////////////////////////////////////////////////////////////////////
// gethighestbin
//////////////////////////////////////////////////////////////////////////
bool CFunctions::GetHighestBin(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount)
{
	__int32 index = -1;
	if(ui32WordCount == 3)
		index = __int32(pFCUserSP->GetDisplayedSpectrumNumber());
	if(ui32WordCount == 4)
	{
		if(!Convert2Integer(csInput[3], index)) 
			index = pFCUserSP->FindSpectrum((char*)LPCSTR(csInput[3]));
	}
	if(index == -1) 
		return false;

	__int32 max_binx = 0;
	__int32 max_biny = 0;
	double max_bin_content;
	CString msg;	

	__int64 number_of_xbins = pFCUserSP->GetNumberOfXBins(index);
	__int64 number_of_ybins = pFCUserSP->GetNumberOfYBins(index);
	if(number_of_xbins < 1)
		return false;

	if(number_of_ybins < 1) 
	{
		max_bin_content = pFCUserSP->GetBinValueAt(index,__int64(0));
		for(__int32 i = 0;i < number_of_xbins;++i) 
		{
			double content = pFCUserSP->GetBinValueAt(index,__int64(i));
			if(max_bin_content < content) 
			{
				max_bin_content = content;
				max_binx = i;
			}
		}
		msg.Format("Histrogram %i\nPeakmaximum in:\nbin %i\ncontent %lg\n",index,max_binx,max_bin_content);
	} 
	else 
	{
		max_bin_content = pFCUserSP->GetBinValueAt(index,__int64(0),__int64(0));
		for(__int32 i = 0;i < number_of_xbins;++i)
		{
			for(__int32 j = 0;j < number_of_ybins;++j)
			{
				double content = pFCUserSP->GetBinValueAt(index,__int64(i),__int64(j));
				if(max_bin_content < content)
				{
					max_bin_content = content;
					max_binx = i;
					max_biny = j;
				}
			}
		}
		msg.Format("Histrogram %i\nPeakmaximum in:\nbinx %i, biny %i\ncontent = %lg\n",index,max_binx,max_biny,max_bin_content);
	}

	AfxMessageBox(msg,MB_ICONEXCLAMATION|MB_OK);
	return true;
} 

//////////////////////////////////////////////////////////////////////////
// proj1d
//////////////////////////////////////////////////////////////////////////
bool CFunctions::Proj1D(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount)
{
	__int32 index = -1;
	if(ui32WordCount == 4)
		index = __int32(pFCUserSP->GetDisplayedSpectrumNumber());
	if(ui32WordCount == 5)
	{
		if(!Convert2Integer(csInput[4], index)) 
			index = pFCUserSP->FindSpectrum((char*)LPCSTR(csInput[4]));
	}
	if(index == -1) 
		return false;
	double binsize = atof((char*)LPCTSTR(csInput[3]));

	double x1,y1;
	pFCUserSP->GetXYCursor(x1,y1,true);

	double x2,y2;
	pFCUserSP->GetXYCursor(x2,y2,true);

	if(x2 < x1) 
	{
		double a = x2;
		x2 = x1;
		x1 = a;
	}

	if(y2 < y1) 
	{
		double a = y2;
		y2 = y1;
		y1 = a;
	}

	CString hist_name = pFCUserSP->GetSpectrumName(index) + CString("_pro");
	CString index_string = "";

	__int32 h_index = 1;
	while(true)
	{
		index_string.Format("%i",h_index);
		CString aa = hist_name+index_string;
		if(pFCUserSP->FindSpectrum((char*)LPCTSTR(aa)) == -1)
		{
			hist_name = aa;
			break;
		}
		h_index++;
	}

	CString cmd;
	cmd.Format("define1 %lg,%lg,%lg,none,,none,always,%s",y1,y2,binsize,hist_name);
	ExecuteCommand(cmd);
	__int32 index2 = __int32(pFCUserSP->FindSpectrum((char*)LPCTSTR(hist_name)));
	if(index2 == -1) 
		return false;

	__int32 xbin1 = __int32(pFCUserSP->GetBinIndexX(index,x1));
	__int32 xbin2 = __int32(pFCUserSP->GetBinIndexX(index,x2));

	__int32 nbins = __int32(pFCUserSP->GetNumberOfXBins(index));
	double * buf = new double[nbins];
	if(!buf)
		return false;
	memset(buf,0,nbins*sizeof(double));

	pFCUserSP->GetHistogramDataArray(index,buf,__int64(nbins));

	for(__int32 xbin = xbin1;xbin < xbin2; ++xbin)
	{
		double content = buf[xbin];
		pFCUserSP->AddValueAt(index2,content,1.);
	}

	if(buf)
	{
		delete buf; 
		buf = nullptr;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
// rebin
//////////////////////////////////////////////////////////////////////////
bool CFunctions::ReBin(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount) 
{
	__int32 index = -1;
	if(ui32WordCount == 4 || ui32WordCount == 5)
		index = __int32(pFCUserSP->GetDisplayedSpectrumNumber());
	if(ui32WordCount == 6)
	{
		if(!Convert2Integer(csInput[5], index)) 
			index = pFCUserSP->FindSpectrum((char*)LPCSTR(csInput[5]));
	}
	if(index == -1) 
		return false;

	__int32 facX=1;
	__int32 facY=1;

	if(ui32WordCount >= 4) 
		facX = atoi((char*)LPCTSTR(csInput[3]));
	if(ui32WordCount >= 5) 
		facY = atoi((char*)LPCTSTR(csInput[4]));

	__int32 nbinsx = __int32(pFCUserSP->GetNumberOfXBins(index));
	__int32 nbinsy = __int32(pFCUserSP->GetNumberOfYBins(index));
	if(nbinsy < 1)
		nbinsy = 1;

	double * buf = new double[nbinsx*nbinsy];
	if(!buf)
		return false;
	memset(buf,0,nbinsx*nbinsy*sizeof(double));

	pFCUserSP->GetHistogramDataArray(index,buf,__int64(nbinsx*nbinsy));

	for(__int32 ybin = 0;ybin < nbinsy/facY; ++ybin)
	{
		for(__int32 xbin = 0;xbin < nbinsx/facX; ++xbin)
		{

			double content = 0.;
			__int32 bins = 0;
			for(__int32 ybin_ = 0;ybin_ < facY; ++ybin_)
			{
				for(__int32 xbin_ = 0;xbin_ < facX; ++xbin_) 
				{
					__int32 xb = xbin*facX+xbin_;
					__int32 yb = ybin*facY+ybin_;
					if(xb < nbinsx && yb < nbinsy)
					{
						content += buf[yb*nbinsx+xb];
						bins++;
					}
				}
			}

			content /= bins;

			for(__int32 ybin_ = 0;ybin_ < facY; ++ybin_) 
			{
				for(__int32 xbin_ = 0;xbin_ < facX; ++xbin_) 
				{
					__int32 xb = xbin*facX+xbin_;
					__int32 yb = ybin*facY+ybin_;
					if(xb < nbinsx && yb < nbinsy) 
						buf[yb*nbinsx+xb] = content;
				}
			}

		}
	}

	pFCUserSP->SetHistogramDataArray(index,buf,__int64(nbinsx*nbinsy));

	if(buf) 
	{
		delete buf; 
		buf = nullptr;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
// normalize
//////////////////////////////////////////////////////////////////////////
bool CFunctions::Normalize(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount) 
{
	__int32 index = -1;
	if(ui32WordCount < 4) 
	{
		AfxMessageBox("not enough arguments for this command.");
		return false;
	}
	bool use_current = false;
	if(ui32WordCount == 4)
	{
		index = __int32(pFCUserSP->GetDisplayedSpectrumNumber());
		use_current = true;
	}
	if(ui32WordCount == 5)
	{
		if(!Convert2Integer(csInput[4], index)) 
			index = pFCUserSP->FindSpectrum((char*)LPCSTR(csInput[4]));
	}
	if(index == -1) 
		return false;
	double new_max_content = atof((char*)LPCTSTR(csInput[3]));

	__int64 number_of_xbins = pFCUserSP->GetNumberOfXBins(index);
	__int64 number_of_ybins = pFCUserSP->GetNumberOfYBins(index);
	if(number_of_xbins < 1) 
		return false;

	__int32 max_binx = 0;
	__int32 max_biny = 0;
	double max_bin_content;

	if(number_of_ybins < 1) 
	{
		max_bin_content = pFCUserSP->GetBinValueAt(index,__int64(0));
		for(__int32 i = 0;i < number_of_xbins;++i) 
		{
			double content = pFCUserSP->GetBinValueAt(index,__int64(i));
			if(max_bin_content < content) 
			{
				max_bin_content = content;
				max_binx = i;
			}
		}
		if(!max_bin_content) 
			return false;
		for(__int32 i = 0;i < number_of_xbins;++i)
		{
			double content = pFCUserSP->GetBinValueAt(index,__int64(i));
			content *= new_max_content/max_bin_content;
			pFCUserSP->SetBinValueAt(index,__int64(i),content);
		}
	} 
	else 
	{
		max_bin_content = pFCUserSP->GetBinValueAt(index,__int64(0),__int64(0));
		for(__int32 i = 0;i < number_of_xbins;++i) 
		{
			for(__int32 j = 0;j < number_of_ybins;++j) 
			{
				double content = pFCUserSP->GetBinValueAt(index,__int64(i),__int64(j));
				if(max_bin_content < content) 
				{
					max_bin_content = content;
					max_binx = i;
					max_biny = j;
				}
			}
		}
		if(!max_bin_content) 
			return false;
		for(__int32 i = 0;i < number_of_xbins;++i) 
		{
			for(__int32 j = 0;j < number_of_ybins;++j) 
			{
				double content = pFCUserSP->GetBinValueAt(index,__int64(i),__int64(j));
				content *= new_max_content/max_bin_content;
				pFCUserSP->SetBinValueAt(index,__int64(i),__int64(j),content);
			}
		}
	}

	if (use_current) 
		ExecuteCommand("update");

	return true;
} 

//////////////////////////////////////////////////////////////////////////
// setdanresolutionparameters
//////////////////////////////////////////////////////////////////////////
bool CFunctions::SetDAnResolutionParameters(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount)
{
	if(theUserFCallApp.SharedClassInstance->bDAnIsRunning) 
		return false;
	CDoubleArray &Parameter = GetDAqDAnParameters();

	// extend Parameter size to 1019
	Parameter.SetAtGrow(1020,0);

	double p20 = Parameter[20];
	if(p20 < 0.0009)
		p20 = 0.25;
	while(p20 < 0.0124)
		p20 *= 2;

	Parameter[1013] = Parameter[1010] * 0.5 * p20;  // high resolution binning 
	Parameter[1014] = Parameter[1011] * 0.5 * p20;  // high resolution binning 
	Parameter[1015] = Parameter[1010] * 2.0 * p20;  // normal resolution binning 
	Parameter[1016] = Parameter[1011] * 2.0 * p20;  // normal resolution binning 
	Parameter[1017] = Parameter[1012] * 2.0 * p20;  // normal resolution binning 
	Parameter[1018] = Parameter[1010] * 8.0 * p20;  // coarse resolution binning 
	Parameter[1019] = Parameter[1011] * 8.0 * p20;  // coarse resolution binning
	return true;
}

//////////////////////////////////////////////////////////////////////////
// setdaqcoordinates
//////////////////////////////////////////////////////////////////////////
bool CFunctions::SetDAqCoordinates(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount)
{
	if(theUserFCallApp.SharedClassInstance->bDAnIsRunning ) 
		return false;
	if(ui32WordCount < 5) 
		return false;
	CDoubleArray &Parameters = GetDAqDAnParameters();
	CString Channeln = csInput[3];
	CString Channelv = csInput[4];
	while(Channeln.Left(1) == " ") 
		Channeln = Channeln.Right(Channeln.GetLength() - 1);
	while(Channeln.Right(1) == " ") 
		Channeln = Channeln.Left(Channeln.GetLength() - 1);
	while(Channelv.Left(1) == " ") 
		Channelv = Channelv.Right(Channelv.GetLength() - 1);
	while(Channelv.Right(1) == " ") 
		Channelv = Channelv.Left(Channelv.GetLength() - 1);

	__int32 number_of_channels = RoundToNearestInt32(Parameters[32]);
	__int32 number_of_hits = RoundToNearestInt32(Parameters[33]);

	__int32 number_of_channels2 = 0;
	if(RoundToNearestInt32(Parameters[8]) == 5) 
	{
		number_of_channels2 = RoundToNearestInt32(Parameters[34]);
		__int32 number_of_hits2 = RoundToNearestInt32(Parameters[35]);
		if(number_of_hits2 != number_of_hits) 
		{
			AfxMessageBox("Error: Parameters 33 and 35 are not equal.");
			return false;
		}
	}

	__int32 Channeln_left = 0;
	__int32 Channeln_middle = 0;
	__int32 Channeln_right = 0;
	CString temp_string = Channeln;
	Channeln_left       = temp_string.Find("?");
	temp_string         = temp_string.Right(temp_string.GetLength() - Channeln_left);
	__int32 i = temp_string.GetLength();
	for(Channeln_middle = 0; Channeln_middle < i; ++Channeln_middle) 
		if(temp_string.GetAt(Channeln_middle) != '?') 
			break;
	Channeln_right = temp_string.GetLength() - Channeln_middle;

	__int32 Channelv_left	 = 0;
	__int32 Channelv_middle1 = 0;
	__int32 Channelv_middle2 = 0;
	__int32 Channelv_middle3 = 0;
	__int32 Channelv_right = 0;
	temp_string          = Channelv;
	Channelv_left        = temp_string.Find("?");
	temp_string          = temp_string.Right(temp_string.GetLength() - Channelv_left);
	i                    = temp_string.GetLength();
	for(Channelv_middle1 = 0; Channelv_middle1 < i; ++Channelv_middle1) 
		if(temp_string.GetAt(Channelv_middle1) != '?') 
			break;
	temp_string = temp_string.Right(temp_string.GetLength() - Channelv_middle1);
	i = temp_string.GetLength();
	for(Channelv_middle2 = 0; Channelv_middle2 < i; ++Channelv_middle2) 
		if(temp_string.GetAt(Channelv_middle2) == '?') 
			break;
	temp_string = temp_string.Right(temp_string.GetLength() - Channelv_middle2);
	i = temp_string.GetLength();
	for(Channelv_middle3 = 0; Channelv_middle3 < i; ++Channelv_middle3) 
		if(temp_string.GetAt(Channelv_middle3) != '?') 
			break;
	Channelv_right = temp_string.GetLength() - Channelv_middle3;

	for(__int32 ch = 0; ch < number_of_channels+number_of_channels2;++ch) 
	{
		CString Cobold_command;
#define chartempn_size	100
		char chartempn[chartempn_size];
		_itoa_s(ch+1,chartempn,chartempn_size,10);
		CString number = chartempn;
		while(number.GetLength() < Channeln_middle)
			number = "0"+number;
		Cobold_command.Format("Coordinate %s%s%s",Channeln.Left(Channeln_left),number,Channeln.Right(Channeln_right));

		for(__int32 i = 0; i < number_of_hits;++i) 
		{
			_itoa_s(ch+1,chartempn,chartempn_size,10);
			number = chartempn;
			while(number.GetLength() < Channelv_middle1) 
				number = "0" + number;
			char chartempv[100];
			_itoa_s(i+1,chartempv,chartempn_size,10);
			CString value = chartempv;
			while(value.GetLength() < Channelv_middle3) 
				value = "0"+value;
			CString ggg;
			ggg.Format(",%s%s%s%s%s",Channelv.Left(Channelv_left),number,Channelv.Mid(Channelv_left+Channelv_middle1,Channelv_middle2),value,Channelv.Right(Channelv_right));
			Cobold_command += ggg;
		}
		Cobold_command += "; DAq RAW coordinates";
		ExecuteCommand(Cobold_command);
#undef chartempn_size
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
// sethitmatrixcoordinatesandspectrum
//////////////////////////////////////////////////////////////////////////
bool CFunctions::SetHitMatrixCoordinatesAndSpectrum(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount)
{
	if(theUserFCallApp.SharedClassInstance->bDAnIsRunning) 
		return false;
	CDoubleArray &Parameter = GetDAqDAnParameters();

	if(ui32WordCount < 7)
		return false;

	CString ConstMask = csInput[3];
	CString CHnMask = csInput[4];
	CString nMatrixX = csInput[5];
	CString nMatrixY = csInput[6];
	__int32 maxChannels = RoundToNearestInt32(Parameter[32]);
	__int32 maxHits = RoundToNearestInt32(Parameter[33]);
	CString MaxChannels;
	CString MaxHits;

	CString Command;

	Command = CString("Coordinate ");
	for(__int32 i=0; i<maxChannels; i++)
	{
		CString Const = ConstMask;
		CString Number;
		Number.Format("%02d",i+1);
		Const.Replace("??",Number);
		Command += Const;
		if(i < maxChannels-1)
			Command += CString(",");
	}
	Command += CString("; internal coordinates for the hit matrix plot");
	ExecuteCommand(Command);					// coordinate ConstXX... 

	Command.Replace("Coordinate ","try CoordinateSet ??,");
	Command.Replace("??", nMatrixX);
	if(Command.Find(";") >= 0)
		Command = Command.Left(Command.Find(";"));
	Command += CString("; internal CoordinateSet for the hit matrix plot");
	ExecuteCommand(Command);					// try CoordinateSet n_matrix_x...

	Command = CString("try CoordinateSet ") + nMatrixY + CString(",");
	for(__int32 i=0; i<maxChannels; i++)
	{
		CString CHn = CHnMask;
		CString Number;
		Number.Format("%02d",i+1);
		CHn.Replace("??",Number);
		Command += CHn;
		if(i < maxChannels-1)
			Command += CString(",");
	}
	Command += CString("; internal CoordinateSet for the hit matrix plot");
	ExecuteCommand(Command);					// try CoordinateSet n_matrix_y...

	MaxChannels.Format("%d",maxChannels >= 4 ? maxChannels+1 : 5);
	MaxHits.Format("%d",maxHits >= 4 ? maxHits+1 : 5);
	Command = CString("define2 0,") + MaxChannels + CString(",1,") + nMatrixX + CString(",TDC channel number,") + 
		CString("-0,") + MaxHits + CString(",1,") + nMatrixY + CString(",number of hits,none,always,hit statistics");
	Command += CString("; auto generated hit matrix");
	ExecuteCommand(Command);					// define2 matrix spectrum
	return true;
}

//////////////////////////////////////////////////////////////////////////
// rescale
//////////////////////////////////////////////////////////////////////////
bool CFunctions::Rescale(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount) 
{
	__int32 index = __int32(pFCUserSP->GetDisplayedSpectrumNumber());
	if (index < 0)
		return true;;

	__int32 nof_bins_x = __int32(pFCUserSP->GetNumberOfXBins(index));
	__int32 nof_bins_y = __int32(pFCUserSP->GetNumberOfYBins(index));

	if (nof_bins_y < 1) 
		nof_bins_y = 1;

	if (nof_bins_y < 2) 
	{
		if (ui32WordCount < 3+4) 
			return true;
	} 
	else
	{
		if (ui32WordCount < 3+4+4) 
			return true;
	}

	CString title = pFCUserSP->GetSpectrumName(index);

	double * buf = new double[nof_bins_x * nof_bins_y];
	if (!buf)
		return true;

	double old_x_min = pFCUserSP->GetBinChannelX(index, 0);
	double old_x_max = pFCUserSP->GetBinChannelX(index, nof_bins_x-1);
	double old_x_binsize = pFCUserSP->GetBinChannelX(index, 1) - pFCUserSP->GetBinChannelX(index, 0);

	double xl_old = atof(LPCSTR(csInput[3+0]));
	double xl_new = atof(LPCSTR(csInput[3+1]));
	double xr_old = atof(LPCSTR(csInput[3+2]));
	double xr_new = atof(LPCSTR(csInput[3+3]));

	double xzoom = (xr_new - xl_new) / (xr_old - xl_old);
	double xoffs = xl_new - xzoom*xl_old;
	double new_x_min = xoffs+xzoom*old_x_min;
	double new_x_max = xoffs+xzoom*old_x_max;
	double new_x_binsize = (new_x_max - new_x_min) / nof_bins_x;

	double old_y_min = 0.;
	double old_y_max = 0.;
	double old_y_binsize = 0.;
	double yzoom = 1;
	double yoffs = 0.;
	double new_y_min = 0;
	double new_y_max = 0;
	double yl_old, yl_new, yr_old, yr_new;
	double new_y_binsize = 1;
	if (nof_bins_y > 1) 
	{
		old_y_min = pFCUserSP->GetBinChannelY(index, 0);
		old_y_max = pFCUserSP->GetBinChannelY(index, nof_bins_y-1);
		old_y_binsize = pFCUserSP->GetBinChannelY(index, 1) - pFCUserSP->GetBinChannelY(index, 0);

		yl_old = atof(LPCSTR(csInput[3+4]));
		yl_new = atof(LPCSTR(csInput[3+5]));
		yr_old = atof(LPCSTR(csInput[3+6]));
		yr_new = atof(LPCSTR(csInput[3+7]));

		yzoom = (yr_new - yl_new) / (yr_old - yl_old);
		yoffs = yl_new - yzoom*yl_old;
		new_y_min = yoffs+yzoom*old_y_min;
		new_y_max = yoffs+yzoom*old_y_max;
		new_y_binsize = (new_y_max - new_y_min) / nof_bins_y;
	}

	pFCUserSP->GetHistogramDataArray(index, buf, nof_bins_x*nof_bins_y);
				
	CString comm;
	comm.Format("del sp,%i", index);
	ExecuteCommand(comm);

	if (nof_bins_y < 2) 
		comm.Format("define1 %lg,%lg,%lg,none,x,none,always,%s",new_x_min,new_x_max,new_x_binsize,LPCSTR(title));
	else
		comm.Format("define2 %lg,%lg,%lg,none,x,%lg,%lg,%lg,none,y,none,always,%s",new_x_min,new_x_max,new_x_binsize, new_y_min,new_y_max,new_y_binsize,LPCSTR(title));

	ExecuteCommand(comm);

	index = pFCUserSP->FindSpectrum((char*)LPCSTR(title));
	if (index > -1) 
		pFCUserSP->SetHistogramDataArray(index, buf, nof_bins_x*nof_bins_y);

	comm.Format("view %i");
	ExecuteCommand(comm);

	if (buf) 
	{
		delete[] buf; 
		buf = 0;
	}

	return true;;
}

//////////////////////////////////////////////////////////////////////////
// phd_analysis
//////////////////////////////////////////////////////////////////////////
bool CFunctions::PhdAnalysis(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount) 
{
	__int32 index = __int32(pFCUserSP->GetDisplayedSpectrumNumber());
	if (index < 0)
		return true;;

	if (pFCUserSP->GetNumberOfYBins(index) > 0) 
	{
			AfxMessageBox("This command works only with 1D-histograms.");
			return true;
	}

	__int32 max_bin = __int32(pFCUserSP->GetNumberOfXBins(index));
	double integral = 0.;
	for (__int32 i=0;i<max_bin;i++)
		integral += pFCUserSP->GetBinValueAt(index, i);

	CString msg = "";
	double cont = 0.;
	__int32 percentage = 10;
	for (__int32 i=0;i<max_bin;i++) 
	{
		cont += pFCUserSP->GetBinValueAt(index, i);
		if (cont / integral >= double(percentage)*0.01) 
		{
			CString nline;
			nline.Format("%i @ x=%lg\n",percentage, pFCUserSP->GetBinChannelX(index, i));
			msg += nline;
			percentage += 10;
			if (percentage == 100) 
				break;
		}
	}

	AfxMessageBox(msg);

	return true;;
}

//////////////////////////////////////////////////////////////////////////
// dynintegral
//////////////////////////////////////////////////////////////////////////
bool CFunctions::DynIntegral(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount) 
{
	__int32 index = __int32(pFCUserSP->GetDisplayedSpectrumNumber());
	if (index < 0) 
		return true;

	double x_start=0, y_start=0;
	pFCUserSP->GetXYCursor(x_start, y_start, true);
		
	__int32 x_start_bin = __int32(pFCUserSP->GetBinIndexX(index, x_start));
	__int32 y_start_bin = __int32(pFCUserSP->GetBinIndexX(index, y_start));

	__int32 old_x_bin = -1000;
	__int32 old_y_bin = -1000;

	while (true)
	{
		double x_new=0, y_new=0;
		pFCUserSP->GetXYCursor(x_new, y_new, true);
		__int32 new_x_bin = __int32(pFCUserSP->GetBinIndexX(index, x_new));
		__int32 new_y_bin = 0;
		double integral = 0.;

		if (pFCUserSP->GetNumberOfYBins(index) > 1) 
		{
			new_y_bin = __int32(pFCUserSP->GetBinIndexY(index, y_new));
			if (old_x_bin == new_x_bin && old_y_bin == new_y_bin) 
				break;

			__int32 xsbin = x_start_bin;
			__int32 xebin = new_x_bin;
			if (new_x_bin < x_start_bin) 
			{
				xebin = x_start_bin; 
				xsbin = new_x_bin;
			}
			__int32 ysbin = y_start_bin;
			__int32 yebin = new_y_bin;
			if (new_y_bin < y_start_bin) 
			{
				yebin = y_start_bin; 
				ysbin = new_y_bin;
			}

			for (__int32 xbin = xsbin; xbin <= xebin; xbin++) 
			{
				for (__int32 ybin = ysbin; ybin <= yebin; ybin++)
					integral += pFCUserSP->GetBinValueAt(index, xbin, ybin);
			}

			CString msg;
			msg.Format("int %lg/%lg to %lg/%lg = %lg\n",x_start,y_start,x_new,y_new, integral);
			//if (!AddText2LogWindow(msg)) 
				AfxMessageBox(msg);
		} 
		else
		{
			if (old_x_bin == new_x_bin && old_y_bin == new_y_bin) 
				break;
			__int32 xsbin = x_start_bin;
			__int32 xebin = new_x_bin;
			if (new_x_bin < x_start_bin)
			{
				xebin = x_start_bin; 
				xsbin = new_x_bin;
			}

			for (__int32 xbin = xsbin; xbin <= xebin; xbin++) 
				integral += pFCUserSP->GetBinValueAt(index, xbin);

			double a,b;
			if (x_start < x_new) 
			{
				a = x_start;
				b = x_new;
			}
			else
			{
				a = x_new; 
				b = x_start;
			}
			CString msg;
			msg.Format("int %lg to %lg = %lg\n", a, b, integral);
			AfxMessageBox(msg);
		}

		old_x_bin = new_x_bin;
		old_y_bin = new_y_bin;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
// get_mean_without_zeros
//////////////////////////////////////////////////////////////////////////
bool CFunctions::GetMeanWithoutZeros(CString csInput[],unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount) 
{
	__int32 index = __int32(pFCUserSP->GetDisplayedSpectrumNumber());
	if (index < 0)
		return true;

	__int64 number_of_ybins = pFCUserSP->GetNumberOfYBins(index);

	double x1=0,y1=0;
	pFCUserSP->GetXYCursor(x1,y1,true);
	double x2=0,y2=0;
	pFCUserSP->GetXYCursor(x2,y2,true);

	__int32 x1_bin = __int32(pFCUserSP->GetBinIndexX(index,x1));
	__int32 x2_bin = __int32(pFCUserSP->GetBinIndexX(index,x2));
	__int32 y1_bin = 0;
	__int32 y2_bin = 0;
	if (number_of_ybins > 0) 
	{
		y1_bin = __int32(pFCUserSP->GetBinIndexY(index,y1));
		y2_bin = __int32(pFCUserSP->GetBinIndexY(index,y2));
	}
	if (x1_bin > x2_bin) 
	{
		__int32 temp = x2_bin;
		x2_bin = x1_bin;
		x1_bin = temp;
	}
	if (y1_bin > y2_bin) 
	{
		__int32 temp = y2_bin;
		y2_bin = y1_bin;
		y1_bin = temp;
	}

	__int32 counter = 0;
	double mean = 0.;

	for (__int32 xbin = x1_bin; xbin <= x2_bin;xbin++)
	{
		if (number_of_ybins < 1) 
		{
			double content = pFCUserSP->GetBinValueAt(index, xbin);
			if (content != 0.) 
			{
				mean += content;
				counter++;
			}
		} 
		else
		{
			for (__int32 ybin = y1_bin; ybin <= y2_bin;ybin++) 
			{
				double content = pFCUserSP->GetBinValueAt(index, xbin, ybin);
				if (content != 0.) 
				{
					mean += content;
					counter++;
				}
			}
		}
	}
	if (counter > 0) 
		mean /= counter; 
	else
		mean = 0.;
		
	CString msg;
	msg.Format("mean without zeros = %lg\n", mean);
	AfxMessageBox(msg);
	return true;
}