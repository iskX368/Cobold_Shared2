#include "StdAfx.h"
#include "UserSHM.h"
#include "UserFCall.h"
#include "LMInfo.h"
#include "UserSP.h"
#include "CoboldUtilities.h"
#include "Functions.h"


#include "LAES2_UserFunctions.h"




namespace LAES2 {

	//IgorPro�E�F�[�u���̒����ő�l
	constexpr unsigned __int32 Igor_WaveName_Max_Length = 31;
	//���o�͐�
	constexpr TCHAR fname_DAqInfo[] = _T("C:\\Program Files\\RoentDek Handels GmbH\\CoboldPC 2011 R5-2-x64 (Visual Studio .Net 2010 Compilation) V10.1.1412.2\\LAES2_TDC8PCI2_HEX\\Cobold_Shared1\\temp\\DAqinfo.dat");
	constexpr TCHAR fname_itx[] = "C:\\Program Files\\RoentDek Handels GmbH\\CoboldPC 2011 R5-2-x64 (Visual Studio .Net 2010 Compilation) V10.1.1412.2\\LAES2_TDC8PCI2_HEX\\Cobold_Shared1\\temp\\temp.itx";
	//�t���O���蓖��
	constexpr TCHAR COBOLD_online = '1';
	constexpr TCHAR COBOLD_offline = '0';

	//�ėp������
	CString msg, msg1;


	//DAq��lmf�w�b�_���e���L�q�����t�@�C������C���e��ǂݍ���
	__int32 sub_GetCurrentDAqInformation(CString& LMFpath, bool& isOnline);

	//Igor�̃E�F�[�u���̃t�H�[�}�b�g�ɍ����悤�ɒ��߂���
	__int32 sub_GetWaveName(CUserSpectrum* pFCUserSP, CString& csDst, const CString& csBaseName);

	//itx�̓��e���쐬����
	__int32 sub_GetWaveText(CUserSpectrum* pFCUserSP, CString& csDst, CString& csWaveName);





	bool LAES2_UserFunctions::MsgBox(CString csInput[], unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount) {
		AfxMessageBox(_T("hoge"), MB_OK);
		return true;
	}

	bool LAES2_UserFunctions::ErrorMsgBox(CString csInput[], unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount) {
		return false;
	}
	bool LAES2_UserFunctions::DumpCoboldCommandArgs(CString csInput[], unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount) {
		CString list = "", elem;

		elem.Format(_T("ui32InputMaximumIndex = %u, ui32WordCount = %u\r\n"), ui32InputMaximumIndex, ui32WordCount);
		list += elem;

		unsigned __int32 i = 0;
		do
		{
			elem.Format(_T("csInput[%u] = %s\r\n"), i, (LPCTSTR)csInput[i]);
			list += elem;
		} while (++i < ui32WordCount);

		AfxMessageBox(list, MB_OK);

		return true;
	}

	


	bool LAES2_UserFunctions::SendSpectrumToIgor(CString csInput[], unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount) {

		try {
			
			//DAq���L�^�����O���t�@�C������ǂݍ���
			//CString LMFpath; 
			//bool isOnline;
			auto flg = sub_GetCurrentDAqInformation(m_LMFpath, m_isOnline);
			if (flg != 0) {
				return false;//sub_GetCurrentDAqInformation�ŃG���[
			}
			
			//�I�����C���ł̎g�p�������Ȃ�
			if (m_isOnline)
			{
				AfxMessageBox(_T("Use SendSpectrumToIgor under the offline condition"));
				return false;
			}


			//�p�X�����񂩂�LMF���̂����o��
			auto pos_sep = m_LMFpath.ReverseFind('\\');
			CString LMFname_ext = m_LMFpath.Right(m_LMFpath.GetLength() - pos_sep - 1);
			auto pos_dot = LMFname_ext.ReverseFind('.');
			CString LMFname = LMFname_ext.Left(pos_dot);
			//AfxMessageBox(LMFname);
			

			//�E�F�[�u�������
			CString WaveName;
			sub_GetWaveName(pFCUserSP, WaveName, LMFname);


			//�m�F
			//msg.Format(_T("%s, %d"), WaveName.GetString(), m_isOnline);
			//AfxMessageBox(msg);


			//�E�F�[�u���
			CStdioFile df(fname_itx, CFile::modeCreate | CFile::modeWrite | CFile::typeText);
			df.WriteString(_T("IGOR\n"));
			CString itxt;
			sub_GetWaveText(pFCUserSP, itxt, WaveName);
			df.WriteString(itxt);
			df.Flush();
			df.Close();


			//igor�ɑ���
			Sleep(20);
			HINSTANCE h = ShellExecuteA(NULL, NULL, df.GetFilePath(), NULL, NULL, SW_SHOWNORMAL);
			uint64_t hh = reinterpret_cast<uintptr_t>(h);//�|�C���^�𐮐��փL���X�g
			if (hh <= 32) {
				//error
				switch (hh) {
				case 0:
					msg1 = _T("The operating system is out of memory or resources.");
					break;
				case ERROR_FILE_NOT_FOUND:
					msg1 = _T("The specified file was not found.");
					break;
				case ERROR_PATH_NOT_FOUND:
					msg1 = _T("The specified path was not found.");
					break;
				case ERROR_BAD_FORMAT:
					msg1 = _T("The .exe file is invalid (non-Win32 .exe or error in .exe image).");
					break;
				case SE_ERR_ACCESSDENIED:
					msg1 = _T("The operating system denied access to the specified file.");
					break;
				case SE_ERR_ASSOCINCOMPLETE:
					msg1 = _T("The file name association is incomplete or invalid.");
					break;
				case SE_ERR_DDEBUSY:
					msg1 = _T("The DDE transaction could not be completed because other DDE transactions were being processed.");
					break;
				case SE_ERR_DDEFAIL:
					msg1 = _T("The DDE transaction failed.");
					break;
				case SE_ERR_DDETIMEOUT:
					msg1 = _T("The DDE transaction could not be completed because the request timed out.");
					break;
				case SE_ERR_DLLNOTFOUND:
					msg1 = _T("The specified DLL was not found.");
					break;
					/*case SE_ERR_FNF:
						msg.Format(_T("ShellExecuteA Error : Code = %d\n%s"), hh,
							_T("The specified file was not found."));
						break;*/
				case SE_ERR_NOASSOC:
					msg1 = _T("There is no application associated with the given file name extension. This error will also be returned if you attempt to print a file that is not printable.");
					break;
				case SE_ERR_OOM:
					msg1 = _T("There was not enough memory to complete the operation.");
					break;
					/*case SE_ERR_PNF:
						msg.Format(_T("ShellExecuteA Error : Code = %d\n%s"), hh,
							_T("The specified path was not found."));
						break;*/
				case SE_ERR_SHARE:
					msg1 = _T("A sharing violation occurred.");
					break;
				default:
					msg1 = _T("Unknown");
					break;
				}

				msg.Format(_T("ShellExecuteA Error : Code = %d\n%s"), hh, msg1.GetString());
				AfxMessageBox(msg, MB_ICONERROR | MB_OK);
			}

			return true;
		}
		catch (CException& ce) {
			
			AfxMessageBox(_T("CException caught in SendSpectrumToIgor"));

			return false;
		}

	}

	bool LAES2_UserFunctions::PrintFCUserSP(CString csInput[], unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount)
	{

		__int32 index = (__int32)pFCUserSP->GetDisplayedSpectrumNumber();//�[���X�^�[�g
		msg.Format(_T("Displayed_Spectrum_Number : %d"), index);
		AfxMessageBox(msg);

		if (index < 0)
			return true;

		__int64 number_of_xbins = pFCUserSP->GetNumberOfXBins(index);
		__int64 number_of_ybins = pFCUserSP->GetNumberOfYBins(index);

		msg.Format(_T("number_of_xbins : %lld\nnumber_of_ybins : %lld"), number_of_xbins, number_of_ybins);
		AfxMessageBox(msg);

		__int64 pp = 100;
		double xx = pFCUserSP->GetBinChannelX(index, pp);

		__int64 qq = 100;
		double yy = pFCUserSP->GetBinChannelY(index, qq);

		msg.Format(_T("point(%lld,%lld) : (%.17g,%.17g)"), pp, qq, xx, yy);
		AfxMessageBox(msg);

		double val = pFCUserSP->GetBinValueAt(index, pp, qq);

		msg.Format(_T("value : %.17g"), val);
		AfxMessageBox(msg);


		return true;
	}


	
	__int32 sub_GetCurrentDAqInformation(CString& LMFpath, bool& isOnline) {

		CStdioFile ff;
		const __int32 NumLinesToBeRead = 2;//�ǂݍ��܂��\��̍s��

		//��ɂ���
		LMFpath.Empty();
		isOnline = false;

		//�t�@�C���J��
		if (!ff.Open(fname_DAqInfo, CFile::modeRead | CFile::typeText)) {
			return 1;//�t�@�C���J���̂����s
		}

		CString line;
		__int32 count = 0;
		while (ff.ReadString(line)) {
			if (count == 0)
			{
				//�p�X
				LMFpath = line;
			}
			else if (count == 1)
			{
				//�I�����C���H
				isOnline = line.GetAt(0) == '1' ? true : false;
			}
			else
			{
				break;
			}

			count++;
		}

		ff.Close();

		if (count != NumLinesToBeRead)
		{
			return 2;//�ǂݍ��ݍs�����ݒ�l�łȂ�
		}
		else
		{
			return 0;
		}

	}


	__int32 sub_GetWaveName(CUserSpectrum* pFCUserSP, CString& csDst, const CString& csBaseName)
	{
		//�\�[�X�X�y�N�g������
		CString SpName = pFCUserSP->GetDisplayedSpectrumName();
		

		//�x�[�X���̂ƃX�y�N�g�����̂�����
		CString bn_and_sp = csBaseName + SpName;
		
		//���ꕶ�����E�F�[�u���̂Ɋ܂ޏꍇ�ɔ�����''�ň͂�
		//����������������ꍇ�͏���ŃJ�b�g
		//�S�p��2�o�C�g�̊e�X����Ƃ��ăJ�E���g����̂Œ���
		if (bn_and_sp.GetLength() > Igor_WaveName_Max_Length - 2)
		{
			csDst = _T("'") + bn_and_sp.Left(Igor_WaveName_Max_Length - 2) + _T("'");//������n�o�C�g�����o��
		}
		else
		{
			csDst = _T("'") + bn_and_sp + _T("'");
		}

		return 0;
	}

	//itx�̓��e���쐬����
	__int32 sub_GetWaveText(CUserSpectrum* pFCUserSP, CString& csDst, CString& csWaveName)
	{
		//�X�y�N�g���ԍ�
		__int64 iSpNr = pFCUserSP->GetDisplayedSpectrumNumber();
		__int32 index32 = static_cast<__int32>(iSpNr);
		//�|�C���g��
		__int64 number_of_xbins = pFCUserSP->GetNumberOfXBins(index32);
		__int64 number_of_ybins = pFCUserSP->GetNumberOfYBins(index32);

		double val, x0, dx, y0, dy;

		CString str = _T("");
		CString tmp;
		if (number_of_xbins < 1) // x-binsize must be available
			return 3;
		if (number_of_ybins > 1) {
			//2D

			//��������

			tmp.Format(_T("WAVES/D/O/N=(%lld,%lld) %s\n"), number_of_xbins, number_of_ybins, csWaveName.GetString());
			str += tmp;
			str += _T("BEGIN\n");
			for (__int64 i = 0; i < number_of_xbins; i++) {

				//xx = pFCUserSP->GetBinChannelX(index, i);

				for (__int64 j = 0; j < number_of_ybins; j++) {

					//yy = pFCUserSP->GetBinChannelY(index, j);

					val = pFCUserSP->GetBinValueAt(index32, i, j);

					tmp.Format(_T("%.17g\t"), val);
					str += tmp;
				}
				str += _T("\n");
			}
			str += _T("\nEND\n");
			//�X�P�[�����O X
			x0 = pFCUserSP->GetBinChannelX(index32, 0);
			dx = pFCUserSP->GetBinChannelX(index32, 1) - x0;
			tmp.Format(_T("X SetScale/p x,%.17g,%.17g,\"\",%s\n"), x0, dx, csWaveName.GetString());
			str += tmp;

			//�X�P�[�����O Y
			y0 = pFCUserSP->GetBinChannelY(index32, 0);
			dy = pFCUserSP->GetBinChannelY(index32, 1) - y0;
			tmp.Format(_T("X SetScale/p y,%.17g,%.17g,\"\",%s\n"), y0, dy, csWaveName.GetString());
			str += tmp;

			
			csDst += str;


			return 0;
		}
		else {
			//1D


			//��������

			tmp.Format(_T("WAVES/D/O %s\n"), csWaveName.GetString());
			str += tmp;
			str += _T("BEGIN\n");
			for (__int64 i = 0; i < number_of_xbins; i++) {

				//xx = pFCUserSP->GetBinChannelX(index, i);

				val = pFCUserSP->GetBinValueAt(index32, i);
				tmp.Format(_T("%.17g\t"), val);
				str += tmp;
			}
			str += _T("\nEND\n");

			//�X�P�[�����O
			x0 = pFCUserSP->GetBinChannelX(index32, 0);
			dx = pFCUserSP->GetBinChannelX(index32, 1) - x0;
			tmp.Format(_T("X SetScale/p x,%.17g,%.17g,\"\",%s; SetScale y,0,0,\"\",%s\n"), x0, dx, csWaveName.GetString(), csWaveName.GetString());
			str += tmp;

			csDst += str;


			return 0;
		}


	}

} // namespace LAES2
