#if !defined(AFX_LMFILEDIALOG_H__A9F64CD0_39F8_11D2_B17C_00A02465E0DD__INCLUDED_)
#define AFX_LMFILEDIALOG_H__A9F64CD0_39F8_11D2_B17C_00A02465E0DD__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// LMFileDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLMFileDialog dialog

class CLMFileDialog : public CFileDialog
{
	DECLARE_DYNAMIC(CLMFileDialog)

public:
	CLMFileDialog(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
	LPCTSTR lpszDefExt = nullptr,
	LPCTSTR lpszFileName = nullptr,
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
	LPCTSTR lpszFilter = nullptr,
	CWnd* pParentWnd = nullptr);
	CString m_LMComment;
	CString m_LMNumberOfEvents;
	CString m_LoadedCamacIni;

public:
protected:
	void ShowEmptyHeader();


protected:
	//{{AFX_MSG(CLMFileDialog)
	virtual BOOL OnInitDialog();
	virtual void OnFileNameChange();
	afx_msg void OnGetCamacIni();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LMFILEDIALOG_H__A9F64CD0_39F8_11D2_B17C_00A02465E0DD__INCLUDED_)
