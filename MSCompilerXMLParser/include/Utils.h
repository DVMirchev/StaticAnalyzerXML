#pragma once

#include <string>
#include <unordered_map>
#include <algorithm>
#include <functional>
#include <map>
#include <set>

#include "XML_PARSER.h"

struct MSCOMPILERXMLPARSER_API SFA
{
	std::basic_string<TCHAR> strFilePath;
	std::basic_string<TCHAR> strFileName;
	std::basic_string<TCHAR> strLine;
	std::basic_string<TCHAR> strColumn;

	friend  bool operator==(const SFA& lhs, const SFA& rhs);
	friend  bool operator<(const SFA& lhs, const SFA& rhs);
};

bool operator==(const SFA& lhs, const SFA& rhs);
bool operator<(const SFA& lhs, const SFA& rhs);

// Defect struct
struct MSCOMPILERXMLPARSER_API Defect
{
	std::basic_string<TCHAR> strXMLfile;
	std::basic_string<TCHAR> strDefectCode;
	std::basic_string<TCHAR> strDescription;
	std::basic_string<TCHAR> strFunction;
	std::basic_string<TCHAR> strFunctionLine;
	std::basic_string<TCHAR> strDecorated;
	std::basic_string<TCHAR> strSubProjectDir;
	SFA											 sfa;
	bool										 bIgnored;
	bool										 bIsInProjectRoot;

	friend  bool operator==(const Defect& lhs, const Defect& rhs);
	friend  bool MSCOMPILERXMLPARSER_API operator<(const Defect& lhs, const Defect& rhs);
};

bool operator==(const Defect& lhs, const Defect& rhs);
bool MSCOMPILERXMLPARSER_API operator<(const Defect& lhs, const Defect& rhs);

bool MSCOMPILERXMLPARSER_API LessThanByFile(const Defect& lhs, const Defect& rhs);
bool MSCOMPILERXMLPARSER_API LessThanBySubProjectDir(const Defect& lhs, const Defect& rhs);

void MSCOMPILERXMLPARSER_API RemoveReportNameFromPath(std::basic_string<TCHAR>& subject, const std::basic_string<TCHAR>& search);
void MSCOMPILERXMLPARSER_API ReplaceStringInPlace(std::basic_string<TCHAR>& subject, const std::basic_string<TCHAR>& search, const std::basic_string<TCHAR>& replace);

class MSCOMPILERXMLPARSER_API DefectParser : public XML_PARSER
{
public:
	~DefectParser() override {};

	std::set< Defect > m_setDefects;

	void Reset();
	void Parse_XML_Document() override;
	void Final();

	size_t m_nCount = 0;
	bool m_bFirst = true;
	bool m_bIsSFA = false;
	bool m_bIsInPath = false;
	Defect m_currentDefect;
};

bool MSCOMPILERXMLPARSER_API visual_studio_open_file(TCHAR const *filename, unsigned int line);