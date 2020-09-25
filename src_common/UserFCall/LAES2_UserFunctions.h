#pragma once


namespace LAES2 {

	//返値リスト
	enum UserFunctions_Ret {
		Success = 0,
		NothingDone = -1,
	};


	//ユーザー関数を内包するクラス，継承禁止
	class LAES2_UserFunctions final
	{
	public:
		LAES2_UserFunctions() = delete;
		//既存オブジェクトへのポインタ値で初期化
		LAES2_UserFunctions(CUserSpectrum* pCUserSpectrum, CFileIOHelper* pCFileIOHelper):pFCUserSP(pCUserSpectrum),pFH(pCFileIOHelper){}
		~LAES2_UserFunctions() = default;
		//コピー禁止
		LAES2_UserFunctions(const LAES2_UserFunctions&) = delete;
		LAES2_UserFunctions operator=(const LAES2_UserFunctions&) = delete;

		//コマンドが一致したら関数を実行する
		__int32 ExecuteFunction(CString csCommandString, CString csInput[], unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount) {
			
			if (CString(_T("msg")).Find(csCommandString) == 0)
				return MsgBox(csInput, ui32InputMaximumIndex, ui32WordCount) ? UserFunctions_Ret::Success : 1;

			if (CString(_T("err")).Find(csCommandString) == 0)
				return ErrorMsgBox(csInput, ui32InputMaximumIndex, ui32WordCount) ? UserFunctions_Ret::Success : 2;

			if (CString(_T("dump")).Find(csCommandString) == 0)
				return DumpCoboldCommandArgs(csInput, ui32InputMaximumIndex, ui32WordCount) ? UserFunctions_Ret::Success : 3;

			if (CString(_T("ssi")).Find(csCommandString) == 0)
				return SendSpectrumToIgor(csInput, ui32InputMaximumIndex, ui32WordCount) ? UserFunctions_Ret::Success : 4;

			if (CString(_T("cus")).Find(csCommandString) == 0)
				return PrintFCUserSP(csInput, ui32InputMaximumIndex, ui32WordCount) ? UserFunctions_Ret::Success : 5;




			return static_cast<__int32>(UserFunctions_Ret::NothingDone);
		}

		//ヘルプ表示に追加する
		CString& AppendHelpText(CString& dst) {
			CString str;
			
			str.Append(_T("\r\n\r\n---------------------- Made by Cobold_Shared1\\src_common\\UserFCall ----------------------\r\n\r\n"));
			str.Append(_T("msg : メッセージボックスを表示．動作確認用．\r\n"));
			str.Append(_T("err : エラーを起こしてメッセージ表示．動作確認用．\r\n"));
			str.Append(_T("dump[,Parameters,...] : 引数一覧をメッセージボックスに表示．動作確認用．\r\n"));
			str.Append(_T("cus : CUserSpectrumクラスの中身をメッセージボックスに表示．動作確認用．\r\n"));
			dst.Append(str);
			return dst;
		}


		//ユーザー関数
	private:
		//------------ 動作確認 ------------//
		//メッセージボックスを表示する
		bool MsgBox(CString csInput[], unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount);
		//エラーメッセージボックスを表示する
		bool ErrorMsgBox(CString csInput[], unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount);
		//Coboldコマンドラインで受け取った引数を順番に表示する
		bool DumpCoboldCommandArgs(CString csInput[], unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount);
		//CUserSpectrumクラスの中身を表示する
		bool PrintFCUserSP(CString csInput[], unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount);



		//スペクトルをIgorProへ転送する
		bool SendSpectrumToIgor(CString csInput[], unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount);



		//CoboldPCクラス
	private:
		CUserSpectrum* pFCUserSP;
		CFileIOHelper* pFH;

		//2020.5.14.	sub_GetCurrentDAqInformation()でメンバ変数にアクセスしたら型に関わらずCoboldが停止した　使わない方が無難かも		
		//2020.5.14.	ポインタ型メンバpFCUserSPなどが何らかの原因で無効になってメンバ変数のメモリ配置がおかしくなり，アクセス不能になったことが原因？pFCUserSPなどの実体をスマートポインタで確保したら直った
		//				if(pFCUserSP) AfxMessageBox(_T("ok"));などとして確認可能
	private:
		
		//lmfパス
		CString m_LMFpath;
		//オンラインならtrue，オフラインならfalse
		bool m_isOnline = false;

	};

} // namespace LAES2