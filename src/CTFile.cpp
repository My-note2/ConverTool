#include "stdafx.h"
#include "CTFile.h"

CTFile::CTFile()
{
	m_pctFile = NULL;
}

CTFile::~CTFile()
{
}

bool CTFile::LoadTable(LPCTSTR filepath)
{
	try {
		m_pctFile = new CStdioFile(filepath, CFile::modeRead);

		LPTSTR record;
		m_pctFile->ReadString(record, 0);

		if (m_pctFile)
		{
			delete m_pctFile;
			m_pctFile = NULL;
		}
	}
	catch (CFileException* pe)
	{
		TRACE(_T("File could not be opened, cause = %d\n"),
			pe->m_cause);
		pe->Delete();
	}
	return true;
}

bool CTFile::StoreRecord(LPCTSTR filepath)
{
	try {
		m_pctFile = new CStdioFile(filepath, CFile::modeWrite);



		if (m_pctFile)
		{
			delete m_pctFile;
			m_pctFile = NULL;
		}
	}
	catch (CFileException* pe)
	{
		TRACE(_T("File could not be opened, cause = %d\n"),
			pe->m_cause);
		pe->Delete();
	}
	return true;
}