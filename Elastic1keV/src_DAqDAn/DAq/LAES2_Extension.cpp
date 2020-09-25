#include "StdAfx.h"
#include "LAES2_Extension.h"

#include "Lib_Inputs.h"


namespace LAES2 {


	//解析名称
	TCHAR cAquisitionName[64];
	//情報出力先
	//constexpr TCHAR fname[] = "C:\\Program Files\\RoentDek Handels GmbH\\CoboldPC 2011 R5-2-x64 (Visual Studio .Net 2010 Compilation) V10.1.1412.2\\LAES2_TDC8PCI2_HEX\\Cobold_Shared1\\temp\\DAqinfo.dat";
	//フラグ割り当て
	//constexpr TCHAR COBOLD_online = '1';
	//constexpr TCHAR COBOLD_offline = '0';


	//ヘルプで表示する取得名
	TCHAR* GetAquisitionName() {
		strcpy_s(cAquisitionName, CString("200313").GetString());
		return cAquisitionName;
	}

	__int32 SaveCurrentDAqInfo(CArchive* pAr, bool isOnline) {

		if (!pAr)
		{
			//無効なポインタ
			return 1;
		}

		//情報書き込むファイル
		CStdioFile ff;
		if (!ff.Open(LibPrm::DAqInfo_FilePath, CFile::modeCreate | CFile::modeWrite | CFile::typeText)) {
			//ファイル開けなかった
			return 2;
		}

		//現在のlmfのパス
		ff.WriteString(pAr->GetFile()->GetFilePath());

		//COBOLDがlmf読み込みモード（オフライン）かハードウェア取り込みモード（オンライン）か
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