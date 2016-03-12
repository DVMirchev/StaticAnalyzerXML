#pragma once

#include <windows.h>
#include <vector>
#include "..\MSCompilerXMLParser.h"


struct MSCOMPILERXMLPARSER_API SearchFileStartingWith
{
	~SearchFileStartingWith() {};

	typedef std::vector<CString> FileNameArray;
	FileNameArray files;

	FileNameArray::iterator begin()
	{
		return files.begin();
	}

	FileNameArray::iterator end()
	{
		return files.end();
	}

	int count() const
	{
		return static_cast<int>(files.size());
	}

	CString operator[](int index)
	{
		return files[index];
	}

	void operator()(const CString &path, const CString &pattern)
	{
		WIN32_FIND_DATA wfd;
		HANDLE hf;
		CString findwhat;
		std::vector<CString> dir;

		findwhat = path + _T("\\*");  // directory

		hf = FindFirstFile(findwhat, &wfd);
		while (hf != INVALID_HANDLE_VALUE)
		{
			if (wfd.cFileName[0] != '.' && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				CString found;

				found = path + _T("\\") + &wfd.cFileName[0];
				dir.push_back(found);
			}

			if (!FindNextFile(hf, &wfd))
			{
				FindClose(hf);
				hf = INVALID_HANDLE_VALUE;
			}
		}

		findwhat = path + _T("\\") + pattern + _T("*.*");  // files

		hf = FindFirstFile(findwhat, &wfd);
		while (hf != INVALID_HANDLE_VALUE)
		{
			if (wfd.cFileName[0] != '.' && !(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				CString found;

				found = path + _T("\\") + &wfd.cFileName[0];
				files.push_back(found);
			}

			if (!FindNextFile(hf, &wfd))
			{
				FindClose(hf);
				hf = INVALID_HANDLE_VALUE;
			}
		}

		// continue with directories
		for (std::vector<CString>::iterator it = dir.begin(); it != dir.end(); ++it)
			this->operator()(*it, pattern);
	}
};

