#include "StdAfx.h"
#include "LAES2_Extension.h"

#include "Lib_Inputs.h"


namespace LAES2 {


	//��͖���
	TCHAR cAquisitionName[64];
	//���o�͐�
	//constexpr TCHAR fname[] = "C:\\Program Files\\RoentDek Handels GmbH\\CoboldPC 2011 R5-2-x64 (Visual Studio .Net 2010 Compilation) V10.1.1412.2\\LAES2_TDC8PCI2_HEX\\Cobold_Shared1\\temp\\DAqinfo.dat";
	//�t���O���蓖��
	//constexpr TCHAR COBOLD_online = '1';
	//constexpr TCHAR COBOLD_offline = '0';


	//�w���v�ŕ\������擾��
	TCHAR* GetAquisitionName() {
		strcpy_s(cAquisitionName, CString("200313").GetString());
		return cAquisitionName;
	}

	__int32 SaveCurrentDAqInfo(CArchive* pAr, bool isOnline) {

		if (!pAr)
		{
			//�����ȃ|�C���^
			return 1;
		}

		//��񏑂����ރt�@�C��
		CStdioFile ff;
		if (!ff.Open(LibPrm::DAqInfo_FilePath, CFile::modeCreate | CFile::modeWrite | CFile::typeText)) {
			//�t�@�C���J���Ȃ�����
			return 2;
		}

		//���݂�lmf�̃p�X
		ff.WriteString(pAr->GetFile()->GetFilePath());

		//COBOLD��lmf�ǂݍ��݃��[�h�i�I�t���C���j���n�[�h�E�F�A��荞�݃��[�h�i�I�����C���j��
		CString OnOff = _T("\n");
		if (isOnline)
		{
			OnOff.AppendChar(_T(LibPrm::DAqInfo_COBOLD_online));
			ff.WriteString(OnOff);
		}
		else
		{
			OnOff.AppendChar(_T(LibPrm::DAqInfo_COBOLD_offline));
			ff.WriteString(OnOff);
		}

		ff.Close();
		return 0;
	}




} // namespace LAES2