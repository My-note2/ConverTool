#pragma once

class CTFile
{
public:
	CTFile();
	~CTFile();
	
	// ���ع�����Ũ�ȱ�
	bool LoadTable(LPCTSTR filepath);

	// �����ѯ���
	bool StoreRecord(LPCTSTR filepath);
private:
	CStdioFile* m_pctFile;
};
