#pragma once

class CTFile
{
public:
	CTFile();
	~CTFile();
	
	// 加载汞蒸气浓度表
	bool LoadTable(LPCTSTR filepath);

	// 保存查询结果
	bool StoreRecord(LPCTSTR filepath);
private:
	CStdioFile* m_pctFile;
};
