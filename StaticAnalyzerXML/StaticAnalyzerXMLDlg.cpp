// StaticAnalyzerXMLDlg.cpp : implementation file
//

#include "stdafx.h"

#include <vector>
#include <string>

#include "StaticAnalyzerXML.h"
#include "StaticAnalyzerXMLDlg.h"
#include "afxdialogex.h"
#include <map>
#include <iterator>
#include <sstream>
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CStaticAnalyzerXMLDlg dialog

CStaticAnalyzerXMLDlg::CStaticAnalyzerXMLDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_StaticAnalyzerXML_DIALOG, pParent)
	  , m_strDefectsCount(_T(""))
	  , m_strStatus(_T(""))
	  , m_bOnlyProjectRoot(FALSE)
	  , m_bCollapsePaths(TRUE)
	  , m_nTopWarningsToHide(1)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CStaticAnalyzerXMLDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GET_PROJECTROOT, m_ProjectRoot);
	DDX_Control(pDX, IDC_ANALYZER_XML_FILE_NAME, m_AnalyzerXMLFileNamePrefix);
	DDX_Control(pDX, IDC_DEFECTS_TREE, m_WarningsTree);
	DDX_Text(pDX, IDC_DEFECTS_COUNT, m_strDefectsCount);
	DDX_Text(pDX, IDC_STATUS, m_strStatus);
	DDX_Check(pDX, IDC_ONLY_PROJECT_ROOT, m_bOnlyProjectRoot);
	DDX_Control(pDX, IDC_GROUP_BY_COMBO, m_GroupByCombo);
	DDX_Check(pDX, IDC_COLAPSE_PATHS, m_bCollapsePaths);
	DDX_Text(pDX, IDC_TOP_WARNINGS_TO_HIDE, m_nTopWarningsToHide);
	DDV_MinMaxUInt(pDX, m_nTopWarningsToHide, 1, 10000);
}

BEGIN_MESSAGE_MAP(CStaticAnalyzerXMLDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_RUN, &CStaticAnalyzerXMLDlg::OnClickedRun)
	ON_CBN_SELCHANGE(IDC_GROUP_BY_COMBO, &CStaticAnalyzerXMLDlg::OnSelchangeGroupByCombo)
	ON_NOTIFY(NM_DBLCLK, IDC_DEFECTS_TREE, &CStaticAnalyzerXMLDlg::OnDblclkDefectsTree)
	ON_BN_CLICKED(IDC_HIDE_SELECTED_WARNING, &CStaticAnalyzerXMLDlg::OnClickedHideSelectedWarning)
	ON_BN_CLICKED(IDC_RESET_HIDDEN_WARNINGS, &CStaticAnalyzerXMLDlg::OnClickedResetHiddenWarnings)
	ON_BN_CLICKED(IDC_COLAPSE_PATHS, &CStaticAnalyzerXMLDlg::OnClickedColapsePaths)
	ON_BN_CLICKED(IDC_HIDE_TOP_WARNINGS, &CStaticAnalyzerXMLDlg::OnClickedHideTopWarnings)
	ON_BN_CLICKED(IDC_ONLY_PROJECT_ROOT, &CStaticAnalyzerXMLDlg::OnClickedOnlyProjectRoot)
	ON_BN_CLICKED(IDC_REFRESH, &CStaticAnalyzerXMLDlg::OnClickedRefresh)
	END_MESSAGE_MAP()


// CStaticAnalyzerXMLDlg message handlers

BOOL CStaticAnalyzerXMLDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE); // Set big icon
	SetIcon(m_hIcon, FALSE); // Set small icon

	// Folder browse edit
	CString strProjectRoot;

	TCHAR currentDir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, &currentDir[0]);
	strProjectRoot = &currentDir[0];

	m_ProjectRoot.SetWindowText(strProjectRoot);
	m_ProjectRoot.EnableFolderBrowseButton(_T("Select Project Root"));

	// XML File name prefix Edit
	m_AnalyzerXMLFileNamePrefix.SetWindowText(_T("analyzer_report"));

	m_GroupByCombo.AddString(_T("Defect ID"));
	m_GroupByCombo.AddString(_T("Project"));
	m_GroupByCombo.AddString(_T("File"));

	m_GroupByCombo.SelectString(0, _T("Defect ID"));

	return TRUE; // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CStaticAnalyzerXMLDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CStaticAnalyzerXMLDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


/////////////////////////////////////////////////////////////////////////
//
//
//
/////////////////////////////////////////////////////////////////////////
void CStaticAnalyzerXMLDlg::PopulateTheTree()
{
	CString strProjectRoot;
	m_ProjectRoot.GetWindowText(strProjectRoot);

	m_WarningsTree.DeleteAllItems();

	size_t nShownDefects = 0;

	// sort the defects according to the user choice
	switch (m_GroupByCombo.GetCurSel())
	{
	case 0:
		std::sort(m_vectorDefects.begin(), m_vectorDefects.end());
		break;
	case 1:
		std::sort(m_vectorDefects.begin(), m_vectorDefects.end(), LessThanByFile);
		break;
	case 2:
		std::sort(m_vectorDefects.begin(), m_vectorDefects.end(), LessThanBySubProjectDir);
		break;
	default:
		ASSERT(FALSE);
	}

	// populate the tree
	for (size_t nIndex = 0; nIndex < m_vectorDefects.size(); nIndex++)
	{
		if (m_vectorDefects[nIndex].bIgnored)
			continue;

		if (m_bOnlyProjectRoot && !m_vectorDefects[nIndex].bIsInProjectRoot)
			continue;

		HTREEITEM hDefect = nullptr;

		switch (m_GroupByCombo.GetCurSel())
		{
		case 0:
			hDefect = m_WarningsTree.InsertItem((m_vectorDefects[nIndex].strDefectCode + _T("  ") + m_vectorDefects[nIndex].strDescription).c_str());
			break;
		case 1:
			{
				auto strFilePath = m_vectorDefects[nIndex].sfa.strFilePath + m_vectorDefects[nIndex].sfa.strFileName;
				if (m_bCollapsePaths)
					strFilePath.replace(0, strProjectRoot.GetLength(), std::basic_string<TCHAR>(_T("..")));

				std::basic_stringstream<TCHAR> output;

				output.width(80);
				output << std::left << strFilePath;
				output.width(0);
				output << _T(" ");
				output << m_vectorDefects[nIndex].strDefectCode;
				output << _T(" ");
				output << m_vectorDefects[nIndex].strDescription;
				hDefect = m_WarningsTree.InsertItem(output.str().c_str());
			}
			break;
		case 2:
			{
				auto strSubProjectDir = m_vectorDefects[nIndex].strSubProjectDir;

				for (auto i = strSubProjectDir.size(); i < 80; i++)
					strSubProjectDir += _T(" ");

				hDefect = m_WarningsTree.InsertItem((strSubProjectDir + _T("  ") + m_vectorDefects[nIndex].strDefectCode + _T("  ") + m_vectorDefects[nIndex].strDescription).c_str());
			}
			break;
		default:
			ASSERT(FALSE);
		}

		m_WarningsTree.SetItemData(hDefect, nIndex);

		HTREEITEM hChild = m_WarningsTree.InsertItem((_T("Defect code: ") + m_vectorDefects[nIndex].strDefectCode).c_str(), hDefect);
		m_WarningsTree.SetItemData(hChild, nIndex);
		hChild = m_WarningsTree.InsertItem((_T("Description: ") + m_vectorDefects[nIndex].strDescription).c_str(), hDefect);
		m_WarningsTree.SetItemData(hChild, nIndex);

		//hChild = m_WarningsTree.InsertItem(m_vectorDefects[nIndex].strFunction.c_str(), hDefect);
		//m_WarningsTree.SetItemData(hChild, nIndex);
		//hChild = m_WarningsTree.InsertItem(m_vectorDefects[nIndex].strDecorated.c_str(), hDefect);
		//m_WarningsTree.SetItemData(hChild, nIndex);
		//hChild = m_WarningsTree.InsertItem(m_vectorDefects[nIndex].strFunctionLine.c_str(), hDefect);
		//m_WarningsTree.SetItemData(hChild, nIndex);

		auto strFile = m_vectorDefects[nIndex].sfa.strFilePath + m_vectorDefects[nIndex].sfa.strFileName;
		if (m_bCollapsePaths)
			strFile.replace(0, strProjectRoot.GetLength(), std::basic_string<TCHAR>(_T("..")));

		hChild = m_WarningsTree.InsertItem((_T("FileName: ") + strFile).c_str(), hDefect);
		m_WarningsTree.SetItemData(hChild, nIndex);
		hChild = m_WarningsTree.InsertItem((_T("Line: ") + m_vectorDefects[nIndex].sfa.strLine).c_str(), hDefect);
		m_WarningsTree.SetItemData(hChild, nIndex);
		hChild = m_WarningsTree.InsertItem((_T("Column: ") + m_vectorDefects[nIndex].sfa.strColumn).c_str(), hDefect);
		m_WarningsTree.SetItemData(hChild, nIndex);
		hChild = m_WarningsTree.InsertItem((_T("XML file: ") + m_vectorDefects[nIndex].strXMLfile).c_str(), hDefect);
		m_WarningsTree.SetItemData(hChild, nIndex);

		++nShownDefects;
	}

	m_strStatus.Format(_T("Showing %Iu defects"), nShownDefects);

	// Refresh
	UpdateData(FALSE);
}

/////////////////////////////////////////////////////////////////////////
//
//
//
/////////////////////////////////////////////////////////////////////////
void CStaticAnalyzerXMLDlg::OnClickedRun()
{
	ScanFoldersForXMLfiles();

	ExtractDefects();

	PopulateTheTree();
}

bool compare_pred(TCHAR a, TCHAR b)
{
	return tolower(a) == tolower(b);
}

bool defect_patch_compare(std::basic_string<TCHAR> const& a, std::basic_string<TCHAR> const& b)
{
	if (b.length() > a.length())
		return false;

	return std::equal(b.begin(), b.end(), a.begin(), compare_pred);
}

/////////////////////////////////////////////////////////////////////////
//
//
//
/////////////////////////////////////////////////////////////////////////
void CStaticAnalyzerXMLDlg::ExtractDefects()
{
	DefectParser defParser;

	for (const auto& file : m_SearchFile.files)
	{
		m_strStatus = _T("Analyzing ") + file;

		UpdateData(FALSE);
		UpdateWindow();

		defParser.m_bFirst = true;
		defParser.Load_XML_Document(file);
		defParser.Final();
	}

	m_vectorDefects.reserve(defParser.m_setDefects.size());
	std::move(defParser.m_setDefects.begin(), defParser.m_setDefects.end(), std::back_inserter(m_vectorDefects));
	m_nCount = defParser.m_nCount;

	CString strProjectRoot;
	m_ProjectRoot.GetWindowText(strProjectRoot);

	CString strAnalyzerFileNamePrefix;
	m_AnalyzerXMLFileNamePrefix.GetWindowText(strAnalyzerFileNamePrefix);

	for (auto& defect : m_vectorDefects)
	{
		if (defect_patch_compare(defect.sfa.strFilePath, (LPCTSTR) strProjectRoot))
			defect.bIsInProjectRoot = true;
		else
			defect.bIsInProjectRoot = false;
	}

	for (auto& defect : m_vectorDefects)
	{
		RemoveReportNameFromPath(defect.strSubProjectDir, (LPCTSTR) strAnalyzerFileNamePrefix);
		ReplaceStringInPlace(defect.strXMLfile, defect.strSubProjectDir.c_str(), std::basic_string<TCHAR>(_T("")));
		defect.strSubProjectDir.replace(0, strProjectRoot.GetLength(), std::basic_string<TCHAR>(_T("")));
	}

	m_strStatus.Format(_T("Analyzed %Iu files and found %Iu defects. Without duplicates %Iu"), m_SearchFile.files.size(), m_nCount, m_vectorDefects.size());
	m_strDefectsCount.Format(_T("%Iu"), m_vectorDefects.size());

	UpdateData(FALSE);
}

/////////////////////////////////////////////////////////////////////////
//
//
//
/////////////////////////////////////////////////////////////////////////
void CStaticAnalyzerXMLDlg::ScanFoldersForXMLfiles()
{
	// Scan for files
	CString strAnalyzerFileNamePrefix;
	m_AnalyzerXMLFileNamePrefix.GetWindowText(strAnalyzerFileNamePrefix);
	m_strStatus.Format(_T("Scanning for files with prefix ( %s )."), (LPCTSTR) strAnalyzerFileNamePrefix);
	UpdateData(FALSE);
	UpdateWindow();

	m_SearchFile.files.clear();

	CString strProjectRoot;
	m_ProjectRoot.GetWindowText(strProjectRoot);

	m_SearchFile(strProjectRoot, strAnalyzerFileNamePrefix);
}

/////////////////////////////////////////////////////////////////////////
//
//
//
/////////////////////////////////////////////////////////////////////////
void CStaticAnalyzerXMLDlg::OnSelchangeGroupByCombo()
{
	PopulateTheTree();
}


/////////////////////////////////////////////////////////////////////////
//
//
//
/////////////////////////////////////////////////////////////////////////
void CStaticAnalyzerXMLDlg::OnDblclkDefectsTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	auto hSelectedItem = m_WarningsTree.GetSelectedItem();
	if (hSelectedItem != NULL && !m_WarningsTree.ItemHasChildren(hSelectedItem))
	{
		auto nIndex = m_WarningsTree.GetItemData(hSelectedItem);
		if (nIndex >= 0 && nIndex < m_vectorDefects.size())
			visual_studio_open_file((m_vectorDefects[nIndex].sfa.strFilePath + m_vectorDefects[nIndex].sfa.strFileName).c_str(), std::stoi(m_vectorDefects[nIndex].sfa.strLine));
	}

	*pResult = 0;
}


/////////////////////////////////////////////////////////////////////////
//
//
//
/////////////////////////////////////////////////////////////////////////
void CStaticAnalyzerXMLDlg::OnClickedHideSelectedWarning()
{
	auto hSelectedItem = m_WarningsTree.GetSelectedItem();
	if (hSelectedItem != NULL)
	{
		auto nIndex = m_WarningsTree.GetItemData(hSelectedItem);
		if (nIndex >= 0 && nIndex < m_vectorDefects.size())
		{
			const auto& selectedDefect = m_vectorDefects[nIndex];
			std::for_each(m_vectorDefects.begin(), m_vectorDefects.end(), [&selectedDefect](Defect& defect)
			              {
				              if (defect.strDefectCode == selectedDefect.strDefectCode)
					              defect.bIgnored = true;
			              });
		}
	}

	PopulateTheTree();
}


/////////////////////////////////////////////////////////////////////////
//
//
//
/////////////////////////////////////////////////////////////////////////
void CStaticAnalyzerXMLDlg::OnClickedResetHiddenWarnings()
{
	std::for_each(m_vectorDefects.begin(), m_vectorDefects.end(), [](Defect& defect)
	              {
		              defect.bIgnored = false;
	              });

	PopulateTheTree();
}


/////////////////////////////////////////////////////////////////////////
//
//
//
/////////////////////////////////////////////////////////////////////////
void CStaticAnalyzerXMLDlg::OnClickedColapsePaths()
{
	m_bCollapsePaths = !m_bCollapsePaths;
	PopulateTheTree();
}


/////////////////////////////////////////////////////////////////////////
//
//
//
/////////////////////////////////////////////////////////////////////////
void CStaticAnalyzerXMLDlg::OnClickedOnlyProjectRoot()
{
	m_bOnlyProjectRoot = !m_bOnlyProjectRoot;
	PopulateTheTree();
}


/////////////////////////////////////////////////////////////////////////
//
//
//
/////////////////////////////////////////////////////////////////////////
void CStaticAnalyzerXMLDlg::OnClickedHideTopWarnings()
{
	UpdateData();

	std::map<std::basic_string<TCHAR>, size_t> m_CountMap;

	std::for_each(m_vectorDefects.begin(), m_vectorDefects.end(), [&m_CountMap] (const Defect& defect)
	              {
		              if (!defect.bIgnored)
			              ++m_CountMap[defect.strDefectCode];
	              });

	using pair_type = decltype(m_CountMap)::value_type;

	for (size_t i = 0; i < (((size_t) m_nTopWarningsToHide) < m_CountMap.size() ? m_nTopWarningsToHide : m_CountMap.size()); i++)
	{
		auto max_el = std::max_element(m_CountMap.begin(), m_CountMap.end(), [] (pair_type const& p1, pair_type const& p2)
		                               {
			                               return (p1.second < p2.second);
		                               });

		std::for_each(m_vectorDefects.begin(), m_vectorDefects.end(), [&max_el] (Defect& defect)
		              {
			              if (defect.strDefectCode == max_el->first)
				              defect.bIgnored = true;
		              });

		m_CountMap[max_el->first] = 0;
	}

	PopulateTheTree();
}


void CStaticAnalyzerXMLDlg::OnClickedRefresh()
{
	PopulateTheTree();
}

