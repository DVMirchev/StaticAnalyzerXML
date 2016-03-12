#pragma once
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the MSCOMPILERXMLPARSER_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// MSCOMPILERXMLPARSER_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef MSCOMPILERXMLPARSER_EXPORTS
#define MSCOMPILERXMLPARSER_API __declspec(dllexport)
#else
#define MSCOMPILERXMLPARSER_API __declspec(dllimport)
#endif

#include "include\SearchFile.h"
#include "include\Utils.h"
#include "include\XML_PARSER.h"

// This class is exported from the MSCompilerXMLParser.dll
class MSCOMPILERXMLPARSER_API CMSCompilerXMLParser {
public:
	CMSCompilerXMLParser(void);
	// TODO: add your methods here.
};

extern MSCOMPILERXMLPARSER_API int nMSCompilerXMLParser;

MSCOMPILERXMLPARSER_API int fnMSCompilerXMLParser(void);
