#include "stdafx.h"
#include "CTFile.h"

#include "GlobalVariable.h"
CTFile::CTFile()
{
	m_pctFile = NULL;
}

CTFile::~CTFile()
{
}

CTFile::CTFile(LPCTSTR filepath) :
	m_pctfilepath(filepath)
{

}

bool CTFile::LoadTable()
{
	try {
		m_pctFile = new CStdioFile(m_pctfilepath, CFile::modeRead);

		CString strrecord;
		std::list<float> lst;
		while (m_pctFile->ReadString(strrecord))
		{
			// 分解字符串
			CString tempstr = strrecord;
			CString str = _T("");
			for (;;)
			{
				int nPos = tempstr.Find(',');

				if (nPos > 0) {
					str = tempstr.Left(nPos);
					//gVarMap[nEle++] = _ttof(str);
					lst.push_back(_ttof(str));
					tempstr = tempstr.Mid(nPos + 1);
				}
				else {
					break;
				}
			}

		}
		int nEle = lst.size();
		float* pfSrc = new float[nEle];
		ZeroMemory(pfSrc, nEle * sizeof(float));
		float* ptmp = pfSrc;
		std::list<float>::iterator iterlst = lst.begin();
		for (; iterlst != lst.end(); ++iterlst)
		{
			*ptmp++ = *iterlst;
		}
		float* pfDest = new float[nEle];
		ZeroMemory(pfDest, nEle * sizeof(float));
		// 插值测试
		insert(pfSrc, pfDest);
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

	CString outstr = _T("");
	std::map<float, float>::iterator itermap = gVarMap.begin();
	for (; itermap != gVarMap.end(); ++itermap)
	{
		outstr.Format(_T("%.2f, %f\n"), itermap->first, itermap->second);
		OutputDebugString(outstr);
	}
	return true;
}

bool CTFile::StoreRecord(LPCTSTR filepath)
{
	try {
		m_pctFile = new CStdioFile(filepath, CFile::modeWrite);

		// 内容格式
		// 汞源温度|注射体积|采样体积|汞含量|汞浓度

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

////////////////////////////////////////////
void InitMap()
{

}

void insert(__in float *pData, __out float *pResult)
{
	const int MAXNUM = 4499;
	float x[MAXNUM + 1];					//存储样条上的点的x坐标，最多51个点  
	float* y = pData;						//存储样条上的点的y坐标，最多51个点  
	unsigned int point_num = MAXNUM + 1;	//存储样条上的实际的 点 的个数  
	float begin_k1 = 0;						//开始点的一阶导数信息  
	float end_k1 = 0;						//终止点的一阶导数信息  
	//float begin_k2;						//开始点的二阶导数信息  
	//float end_k2;							//终止点的二阶导数信息  
	//计算所得的样条函数S(x)   
	//51个点之间有50个段，func[]存储每段的函数系数  
	float a3[MAXNUM], a1[MAXNUM];
	float b3[MAXNUM], b1[MAXNUM];

	float H[MAXNUM] = { 0 };			//小区间的步长  
	float Fi[MAXNUM] = { 0 };			//中间量  
	float U[MAXNUM + 1] = { 0 };		//中间量  
	float A[MAXNUM + 1] = { 0 };		//中间量  
	float D[MAXNUM + 1] = { 0 };		//中间量  
	float M[MAXNUM + 1] = { 0 };		//M矩阵  
	float B[MAXNUM + 1] = { 0 };		//追赶法中间量  
	float Y[MAXNUM + 1] = { 0 };		//追赶法中间变量  

	/*m_fWaveL0 = 895.243;
	m_fWaveL1 = 1688.750;*/
	float step = (float)45/4500;// (g_devInfo.m_fWaveL1 - g_devInfo.m_fWaveL0) / (MAXNUM + 1);

	for (int i = 0; i <= MAXNUM; i++)
	{
		//x[i] = g_devInfo.m_fWaveL0 + step * i;
		x[i] = step * i;
	}

	////////////////////////////////////////计算中间参数
	if ((point_num < 3) || (point_num > MAXNUM + 1))
	{
		return;       //输入数据点个数太少或太多  
	}
	for (int i = 0; i <= point_num - 2; i++)
	{   
		//求H[i]  
		H[i] = x[i + 1] - x[i];
		Fi[i] = (y[i + 1] - y[i]) / H[i]; //求F[x(i),x(i+1)]  
	}
	for (int i = 1; i <= point_num - 2; i++)
	{   
		//求U[i]和A[i]和D[i]  
		U[i] = H[i - 1] / (H[i - 1] + H[i]);
		A[i] = H[i] / (H[i - 1] + H[i]);
		D[i] = 6 * (Fi[i] - Fi[i - 1]) / (H[i - 1] + H[i]);
	}
	//若边界条件为1号条件，则  
	U[MAXNUM] = 1;
	A[0] = 1;
	D[0] = 6 * (Fi[0] - begin_k1) / H[0];
	D[MAXNUM] = 6 * (end_k1 - Fi[MAXNUM - 1]) / H[MAXNUM - 1];
	//若边界条件为2号条件，则  
	//U[i] = 0;  
	//A[0] = 0;  
	//D[0] = 2 * begin_k2;  
	//D[i] = 2 * end_k2;

	/////////////////////////////////////////追赶法求解M矩阵  
	B[0] = A[0] / 2;
	for (int i = 1; i <= point_num - 2; i++)
	{
		B[i] = A[i] / (2 - U[i] * B[i - 1]);
	}
	Y[0] = D[0] / 2;
	for (int i = 1; i <= point_num - 1; i++)
	{
		Y[i] = (D[i] - U[i] * Y[i - 1]) / (2 - U[i] * B[i - 1]);
	}
	M[point_num - 1] = Y[point_num - 1];
	for (int i = point_num - 1; i > 0; i--)
	{
		M[i - 1] = Y[i - 1] - B[i - 1] * M[i];
	}

	//////////////////////////////////////////计算方程组最终结果  
	for (int i = 0; i <= point_num - 2; i++)
	{
		a3[i] = M[i] / (6 * H[i]);
		a1[i] = (y[i] - M[i] * H[i] * H[i] / 6) / H[i];
		b3[i] = M[i + 1] / (6 * H[i]);
		b1[i] = (y[i + 1] - M[i + 1] * H[i] * H[i] / 6) / H[i];
	}

	//计算差值结果125个点
	//float w11 = 910.4f;
	//float w22 = 1690.1f;
	float step1 = (float) 45 / 4499;
	float xx;
	float* yy = pResult;
	for (int k = 0; k < 4500; k++)
	{
		xx = step1 * k;
		for (int i = 0; i < MAXNUM; i++)
		{
			if (x[i] <= xx && x[i + 1] >= xx)
			{
				float tempx3 = x[i + 1] - xx;
				float tempxx3 = xx - x[i];
				yy[k] = a3[i] * tempx3 * tempx3 * tempx3 + a1[i] * tempx3 + b3[i] * tempxx3 * tempxx3 * tempxx3 + b1[i] * tempxx3;
				gVarMap[x[k]] = yy[k];
				break;
			}
		}
	}
}