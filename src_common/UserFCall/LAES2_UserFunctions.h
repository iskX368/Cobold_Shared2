#pragma once


namespace LAES2 {

	//�Ԓl���X�g
	enum UserFunctions_Ret {
		Success = 0,
		NothingDone = -1,
	};


	//���[�U�[�֐�������N���X�C�p���֎~
	class LAES2_UserFunctions final
	{
	public:
		LAES2_UserFunctions() = delete;
		//�����I�u�W�F�N�g�ւ̃|�C���^�l�ŏ�����
		LAES2_UserFunctions(CUserSpectrum* pCUserSpectrum, CFileIOHelper* pCFileIOHelper):pFCUserSP(pCUserSpectrum),pFH(pCFileIOHelper){}
		~LAES2_UserFunctions() = default;
		//�R�s�[�֎~
		LAES2_UserFunctions(const LAES2_UserFunctions&) = delete;
		LAES2_UserFunctions operator=(const LAES2_UserFunctions&) = delete;

		//�R�}���h����v������֐������s����
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

		//�w���v�\���ɒǉ�����
		CString& AppendHelpText(CString& dst) {
			CString str;
			
			str.Append(_T("\r\n\r\n---------------------- Made by Cobold_Shared1\\src_common\\UserFCall ----------------------\r\n\r\n"));
			str.Append(_T("msg : ���b�Z�[�W�{�b�N�X��\���D����m�F�p�D\r\n"));
			str.Append(_T("err : �G���[���N�����ă��b�Z�[�W�\���D����m�F�p�D\r\n"));
			str.Append(_T("dump[,Parameters,...] : �����ꗗ�����b�Z�[�W�{�b�N�X�ɕ\���D����m�F�p�D\r\n"));
			str.Append(_T("cus : CUserSpectrum�N���X�̒��g�����b�Z�[�W�{�b�N�X�ɕ\���D����m�F�p�D\r\n"));
			dst.Append(str);
			return dst;
		}


		//���[�U�[�֐�
	private:
		//------------ ����m�F ------------//
		//���b�Z�[�W�{�b�N�X��\������
		bool MsgBox(CString csInput[], unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount);
		//�G���[���b�Z�[�W�{�b�N�X��\������
		bool ErrorMsgBox(CString csInput[], unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount);
		//Cobold�R�}���h���C���Ŏ󂯎�������������Ԃɕ\������
		bool DumpCoboldCommandArgs(CString csInput[], unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount);
		//CUserSpectrum�N���X�̒��g��\������
		bool PrintFCUserSP(CString csInput[], unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount);



		//�X�y�N�g����IgorPro�֓]������
		bool SendSpectrumToIgor(CString csInput[], unsigned __int32 ui32InputMaximumIndex, unsigned __int32 ui32WordCount);



		//CoboldPC�N���X
	private:
		CUserSpectrum* pFCUserSP;
		CFileIOHelper* pFH;

		//2020.5.14.	sub_GetCurrentDAqInformation()�Ń����o�ϐ��ɃA�N�Z�X������^�Ɋւ�炸Cobold����~�����@�g��Ȃ����������		
		//2020.5.14.	�|�C���^�^�����opFCUserSP�Ȃǂ����炩�̌����Ŗ����ɂȂ��ă����o�ϐ��̃������z�u�����������Ȃ�C�A�N�Z�X�s�\�ɂȂ������Ƃ������HpFCUserSP�Ȃǂ̎��̂��X�}�[�g�|�C���^�Ŋm�ۂ����璼����
		//				if(pFCUserSP) AfxMessageBox(_T("ok"));�ȂǂƂ��Ċm�F�\
	private:
		
		//lmf�p�X
		CString m_LMFpath;
		//�I�����C���Ȃ�true�C�I�t���C���Ȃ�false
		bool m_isOnline = false;

	};

} // namespace LAES2