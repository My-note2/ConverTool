#pragma once

class CTFile
{
public:
	CTFile();
	CTFile(LPCTSTR filepath);

	~CTFile();
	
	// ���ع�����Ũ�ȱ�
	bool LoadTable();

	// �����ѯ���
	bool StoreRecord(LPCTSTR filepath);
private:
	CStdioFile* m_pctFile;

	LPCTSTR	m_pctfilepath;
};
