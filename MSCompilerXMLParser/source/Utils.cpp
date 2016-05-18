
// Utils.cpp : implementation file
//

#include "stdafx.h"

#include <map>
#include <atlcomcli.h>


#include "..\include\Utils.h"

/////////////////////////////////////////////////////////////////////////
//
// Call this function to open a file in Visual Studio and select the given line
//
/////////////////////////////////////////////////////////////////////////
// import EnvDTE
#pragma warning(disable : 4278)
#pragma warning(disable : 4146)
#import "libid:80cc9f66-e7d8-4ddd-85b6-d9e6cd0e93e2" version("8.0") lcid("0") raw_interfaces_only named_guids
//#import "libid: 26ad1324-4b7c-44bc-84f8-b86aed45729f" version("10.0") lcid("0") raw_interfaces_only named_guids
#pragma warning(default : 4146)
#pragma warning(default : 4278)

bool visual_studio_open_file(TCHAR const *filename, unsigned int line)
{
	HRESULT result;
	CLSID clsid;
	result = ::CLSIDFromProgID(L"VisualStudio.DTE", &clsid);
	if (FAILED(result))
		return false;

	CComPtr<IUnknown> punk;
	result = ::GetActiveObject(clsid, NULL, &punk);
	if (FAILED(result))
		return false;

	CComPtr<EnvDTE::_DTE> DTE;
	DTE = punk;

	CComPtr<EnvDTE::ItemOperations> item_ops;
	result = DTE->get_ItemOperations(&item_ops);
	if (FAILED(result))
		return false;

	CComBSTR bstrFileName(filename);
	CComBSTR bstrKind(EnvDTE::vsViewKindTextView);
	CComPtr<EnvDTE::Window> window;
	result = item_ops->OpenFile(bstrFileName, bstrKind, &window);
	if (FAILED(result))
		return false;

	CComPtr<EnvDTE::Document> doc;
	result = DTE->get_ActiveDocument(&doc);
	if (FAILED(result))
		return false;

	CComPtr<IDispatch> selection_dispatch;
	result = doc->get_Selection(&selection_dispatch);
	if (FAILED(result))
		return false;

	CComPtr<EnvDTE::TextSelection> selection;
	result = selection_dispatch->QueryInterface(&selection);
	if (FAILED(result))
		return false;

	result = selection->GotoLine(line, TRUE);
	if (FAILED(result))
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////
//
//	Comparison operator for SFA struct
//
/////////////////////////////////////////////////////////////////////////
bool operator==(const SFA& lhs, const SFA& rhs)
{
	return std::tie(lhs.strFileName, lhs.strFilePath, lhs.strColumn, lhs.strLine) ==
         std::tie(rhs.strFileName, rhs.strFilePath, rhs.strColumn, rhs.strLine);
}

/////////////////////////////////////////////////////////////////////////
//
// Greater operator for SFA struct
//
/////////////////////////////////////////////////////////////////////////
bool operator<(const SFA& lhs, const SFA& rhs)
{
  return std::tie(lhs.strFileName, lhs.strFilePath, lhs.strColumn, lhs.strLine) <
         std::tie(rhs.strFileName, rhs.strFilePath, rhs.strColumn, rhs.strLine);
}


/////////////////////////////////////////////////////////////////////////
//
//	Comparison operator for Defect struct
//
/////////////////////////////////////////////////////////////////////////
bool operator==(const Defect& lhs, const Defect& rhs)
{
  return std::tie(lhs.strDefectCode, lhs.strDescription, lhs.strFunction, lhs.strDecorated, lhs.sfa) ==
         std::tie(rhs.strDefectCode, rhs.strDescription, rhs.strFunction, rhs.strDecorated, rhs.sfa);
}

/////////////////////////////////////////////////////////////////////////
//
//	Greater operator for Defect Struct
//
/////////////////////////////////////////////////////////////////////////
bool operator<(const Defect& lhs, const Defect& rhs)
{
	return std::tie(lhs.strDefectCode, lhs.strDescription, lhs.strFunction, lhs.strDecorated, lhs.sfa) <
         std::tie(rhs.strDefectCode, rhs.strDescription, rhs.strFunction, rhs.strDecorated, rhs.sfa);
}

/////////////////////////////////////////////////////////////////////////
//
//
//
/////////////////////////////////////////////////////////////////////////
bool LessThanByFile(const Defect & lhs, const Defect & rhs)
{

  return std::tie(lhs.sfa.strFilePath + lhs.sfa.strFileName, lhs.strDefectCode, lhs.strDescription, lhs.strFunction, lhs.strDecorated, lhs.sfa) <
		     std::tie(rhs.sfa.strFilePath + rhs.sfa.strFileName, rhs.strDefectCode, rhs.strDescription, rhs.strFunction, rhs.strDecorated, rhs.sfa);
}

/////////////////////////////////////////////////////////////////////////
//
//
//
/////////////////////////////////////////////////////////////////////////
bool LessThanBySubProjectDir(const Defect & lhs, const Defect & rhs)
{
  return std::tie(lhs.strSubProjectDir, lhs.strDefectCode, lhs.strDescription, lhs.strFunction, lhs.strDecorated, lhs.sfa) <
         std::tie(rhs.strSubProjectDir, rhs.strDefectCode, rhs.strDescription, rhs.strFunction, rhs.strDecorated, rhs.sfa);
}

/////////////////////////////////////////////////////////////////////////
//
//
//
/////////////////////////////////////////////////////////////////////////
void ReplaceStringInPlace(std::basic_string<TCHAR>& subject, const std::basic_string<TCHAR>& search, const std::basic_string<TCHAR>& replace) {
	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != std::string::npos) {
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
}



/////////////////////////////////////////////////////////////////////////
//
//
//
/////////////////////////////////////////////////////////////////////////
void RemoveReportNameFromPath(std::basic_string<TCHAR>& subject, const std::basic_string<TCHAR>& search) 
{
	size_t pos = 0;
	if ((pos = subject.find(search, pos)) != std::string::npos) 
	{
		subject.replace(pos, subject.length() - pos, std::basic_string<TCHAR>(_T("")));
	}
}

/////////////////////////////////////////////////////////////////////////
//
//
//
/////////////////////////////////////////////////////////////////////////
void DefectParser::Reset()
{
	m_setDefects.clear();

	m_bFirst = true;

	Reset_XML_Document();
}

/////////////////////////////////////////////////////////////////////////
//
//
//
/////////////////////////////////////////////////////////////////////////
void DefectParser::Parse_XML_Document()
{
	//TRACE(Get_CurrentTag() + _T("\n") );
	//TRACE(Get_TextValue()  + _T("\n") );

	// We hit a <DEFECT> start filling a new one.
	if (Get_CurrentTag() == _T("<DEFECT>"))
	{
		if (!m_bFirst)
		{
			++m_nCount;
			m_setDefects.insert(m_currentDefect);
		}

		m_bFirst = false;
		m_bIsSFA = false;
		m_bIsInPath = false;
		m_currentDefect = {};
		m_currentDefect.strSubProjectDir = (LPCTSTR) Get_XML_Document_FileName();
		m_currentDefect.strXMLfile = (LPCTSTR) Get_XML_Document_FileName();
	}

	if (Get_CurrentTag() == _T("<SFA>"))
	{
		m_bIsSFA = true;
		if (m_bIsInPath)
		{
			++m_nCount;
			m_setDefects.insert(m_currentDefect);
		}
	}

	if (m_bIsSFA && Get_CurrentTag() == _T("<FILEPATH>"))
	{
		m_currentDefect.sfa.strFilePath = std::basic_string<TCHAR>(Get_TextValue()); return;
	}

	if (m_bIsSFA && Get_CurrentTag() == _T("<FILENAME>"))
	{
		m_currentDefect.sfa.strFileName = std::basic_string<TCHAR>(Get_TextValue()); return;
	}

	if (m_bIsSFA && Get_CurrentTag() == _T("<LINE>"))
	{
		m_currentDefect.sfa.strLine = std::basic_string<TCHAR>(Get_TextValue()); return;
	}

	if (m_bIsSFA && Get_CurrentTag() == _T("<COLUMN>"))
	{
		m_currentDefect.sfa.strColumn = std::basic_string<TCHAR>(Get_TextValue()); return;
	}

	if (Get_CurrentTag() == _T("<DEFECTCODE>"))
	{
		m_currentDefect.strDefectCode = std::basic_string<TCHAR>(Get_TextValue()); return;
	}

	if (Get_CurrentTag() == _T("<DESCRIPTION>"))
	{
		m_currentDefect.strDescription = std::basic_string<TCHAR>(Get_TextValue()); return;
	}

	if (Get_CurrentTag() == _T("<DECORATED>"))
	{
		m_currentDefect.strDecorated = std::basic_string<TCHAR>(Get_TextValue()); return;
	}

	if (Get_CurrentTag() == _T("<FUNCLINE>"))
	{
		m_currentDefect.strFunctionLine = std::basic_string<TCHAR>(Get_TextValue()); return;
	}

	if (Get_CurrentTag() == _T("<FUNCTION>"))
	{
		m_currentDefect.strFunction = std::basic_string<TCHAR>(Get_TextValue()); return;
	}


	if (Get_CurrentTag() == _T("<PATH>"))
	{
		m_bIsInPath = true;
	}

	//if (Get_CurrentTag() == _T("<>"))
	//{
	//}
}


/////////////////////////////////////////////////////////////////////////
//
//
//
/////////////////////////////////////////////////////////////////////////
void DefectParser::Final()
{
	if (!(m_currentDefect == Defect()))
	{
		++m_nCount;
		m_setDefects.insert(m_currentDefect);
	}
}


