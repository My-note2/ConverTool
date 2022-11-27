#pragma once

class CTFile
{
public:
	CTFile();
	CTFile(LPCTSTR filepath);

	~CTFile();
	
	// 加载汞蒸气浓度表
	bool LoadTable();

	// 保存查询结果
	bool StoreRecord(LPCTSTR filepath);
private:
	CStdioFile* m_pctFile;

	LPCTSTR	m_pctfilepath;
};
