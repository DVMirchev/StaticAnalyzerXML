
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
	return lhs.strFileName == rhs.strFileName && lhs.strFilePath == rhs.strFilePath && lhs.strColumn == rhs.strColumn && lhs.strLine == rhs.strLine;// && lhs. == rhs. &&
}

/////////////////////////////////////////////////////////////////////////
//
// Greater operator for SFA struct
//
/////////////////////////////////////////////////////////////////////////
bool operator<(const SFA& lhs, const SFA& rhs)
{
	if (lhs.strFileName < rhs.strFileName)
		return true;

	if (lhs.strFileName == rhs.strFileName)
	{
		if (lhs.strFilePath < rhs.strFilePath)
			return true;

		if (lhs.strFilePath == rhs.strFilePath)
		{
			if (lhs.strColumn < rhs.strColumn)
				return true;

			if (lhs.strColumn == rhs.strColumn)
				if (lhs.strLine < rhs.strLine)
					return true;
		}
	}

	return false;
}


/////////////////////////////////////////////////////////////////////////
//
//	Comparison operator for Defect struct
//
/////////////////////////////////////////////////////////////////////////
bool operator==(const Defect& lhs, const Defect& rhs)
{
	return	lhs.strDefectCode == rhs.strDefectCode	&&
		lhs.strDescription == rhs.strDescription &&
		lhs.strFunction == rhs.strFunction		&&
		lhs.strDecorated == rhs.strDecorated		&&
		lhs.sfa == rhs.sfa;
}

/////////////////////////////////////////////////////////////////////////
//
//	Greater operator for Defect Struct
//
/////////////////////////////////////////////////////////////////////////
bool operator<(const Defect& lhs, const Defect& rhs)
{
	if (lhs.strDefectCode < rhs.strDefectCode)
		return true;

	if (lhs.strDefectCode == rhs.strDefectCode)
	{
		if (lhs.strDescription < rhs.strDescription)
			return true;

		if (lhs.strDescription == rhs.strDescription)
		{
			if (lhs.strFunction < rhs.strFunction)
				return true;

			if (lhs.strFunction == rhs.strFunction)
			{
				if (lhs.strDecorated < rhs.strDecorated)
					return true;

				if (lhs.strDecorated == rhs.strDecorated)
				{
					if (lhs.sfa < rhs.sfa)
						return true;
				}
			}
		}
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////
//
//
//
/////////////////////////////////////////////////////////////////////////
bool LessThankByFile(const Defect & lhs, const Defect & rhs)
{
	if ((lhs.sfa.strFilePath + lhs.sfa.strFileName) < (rhs.sfa.strFilePath + rhs.sfa.strFileName))
		return true;

	if ((lhs.sfa.strFilePath + lhs.sfa.strFileName) == (rhs.sfa.strFilePath + rhs.sfa.strFileName))
	{
		if (lhs.strDefectCode < rhs.strDefectCode)
			return true;

		if (lhs.strDefectCode == rhs.strDefectCode)
		{
			if (lhs.strDescription < rhs.strDescription)
				return true;

			if (lhs.strDescription == rhs.strDescription)
			{
				if (lhs.strFunction < rhs.strFunction)
					return true;

				if (lhs.strFunction == rhs.strFunction)
				{
					if (lhs.strDecorated < rhs.strDecorated)
						return true;

					if (lhs.strDecorated == rhs.strDecorated)
					{
						if (lhs.sfa < rhs.sfa)
							return true;
					}
				}
			}
		}
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////
//
//
//
/////////////////////////////////////////////////////////////////////////
bool LessThankBySubProjectDir(const Defect & lhs, const Defect & rhs)
{
	if (lhs.strSubProjectDir < rhs.strSubProjectDir)
		return true;

	if (lhs.strSubProjectDir == rhs.strSubProjectDir)
	{
		if (lhs.strDefectCode < rhs.strDefectCode)
			return true;

		if (lhs.strDefectCode == rhs.strDefectCode)
		{
			if (lhs.strDescription < rhs.strDescription)
				return true;

			if (lhs.strDescription == rhs.strDescription)
			{
				if (lhs.strFunction < rhs.strFunction)
					return true;

				if (lhs.strFunction == rhs.strFunction)
				{
					if (lhs.strDecorated < rhs.strDecorated)
						return true;

					if (lhs.strDecorated == rhs.strDecorated)
					{
						if (lhs.sfa < rhs.sfa)
							return true;
					}
				}
			}
		}
	}

	return false;
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
template < typename  K, typename V >
typename std::multimap<K, V>::const_iterator find_pair(const std::multimap<K, V>& map, const std::pair<K, V>& pair)
{
	auto range = map.equal_range(pair.first);
	for (auto p = range.first; p != range.second; ++p)
		if (p->second == pair.second)
			return p;
	return map.end();
}

/////////////////////////////////////////////////////////////////////////
//
//
//
/////////////////////////////////////////////////////////////////////////
template<typename K, typename V>
bool insert_if_not_present(std::multimap<K, V>& map, const std::pair<K, V>& pair)
{
	if (find_pair(map, pair) == map.end()) {
		map.insert(pair);
		return true;
	}
	return false;
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
			//insert_if_not_present(m_Defects, std::pair<std::basic_string<TCHAR>, Defect>(m_currentDefect.strDefectCode, m_currentDefect));
			//m_mapDefects.insert(std::pair<std::basic_string<TCHAR>, Defect>(m_currentDefect.strDefectCode, m_currentDefect));
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

	//insert_if_not_present(m_Defects, std::pair<std::basic_string<TCHAR>, Defect>(m_currentDefect.strDefectCode, m_currentDefect));
}


