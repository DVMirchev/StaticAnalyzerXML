
// StaticAnalyzerXMLDlg.h : header file
//

#pragma once
#include "afxeditbrowsectrl.h"
#include "afxwin.h"
#include "afxcmn.h"

#include "..\MSCompilerXMLParser\MSCompilerXMLParser.h"


// CStaticAnalyzerXMLDlg dialog
class CStaticAnalyzerXMLDlg : public CDialogEx
{
// Construction
public:
	CStaticAnalyzerXMLDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_StaticAnalyzerXML_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();

	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSelchangeGroupByCombo();
	afx_msg void OnDblclkDefectsTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClickedRun();
	afx_msg void OnClickedHideSelectedWarning();
	afx_msg void OnClickedResetHiddenWarnings();
	afx_msg void OnClickedColapsePaths();
	afx_msg void OnClickedHideTopWarnings();

	DECLARE_MESSAGE_MAP()

public:

	void PopulateTheTree();
	void ExtractDefects();
	void ScanFoldersForXMLfiles();

private:
	CMFCEditBrowseCtrl m_ProjectRoot;
	CEdit m_AnalyzerXMLFileNamePrefix;
	CTreeCtrl m_WarningsTree;
	CComboBox m_GroupByCombo;

	SearchFileStartingWith m_SearchFile;
	std::vector<Defect> m_vectorDefects;
	size_t m_nCount = 0;

	CString m_strDefectsCount;
	CString m_strStatus;
	BOOL m_bOnlyProjectRoot;
	BOOL m_bCollapsePaths;
	UINT m_nTopWarningsToHide;
public:
	afx_msg void OnClickedOnlyProjectRoot();
	afx_msg void OnClickedRefresh();
};
