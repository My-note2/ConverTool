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
			// �ֽ��ַ���
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
		// ��ֵ����
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

		// ���ݸ�ʽ
		// ��Դ�¶�|ע�����|�������|������|��Ũ��

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
	float x[MAXNUM + 1];					//�洢�����ϵĵ��x���꣬���51����  
	float* y = pData;						//�洢�����ϵĵ��y���꣬���51����  
	unsigned int point_num = MAXNUM + 1;	//�洢�����ϵ�ʵ�ʵ� �� �ĸ���  
	float begin_k1 = 0;						//��ʼ���һ�׵�����Ϣ  
	float end_k1 = 0;						//��ֹ���һ�׵�����Ϣ  
	//float begin_k2;						//��ʼ��Ķ��׵�����Ϣ  
	//float end_k2;							//��ֹ��Ķ��׵�����Ϣ  
	//�������õ���������S(x)   
	//51����֮����50���Σ�func[]�洢ÿ�εĺ���ϵ��  
	float a3[MAXNUM], a1[MAXNUM];
	float b3[MAXNUM], b1[MAXNUM];

	float H[MAXNUM] = { 0 };			//С����Ĳ���  
	float Fi[MAXNUM] = { 0 };			//�м���  
	float U[MAXNUM + 1] = { 0 };		//�м���  
	float A[MAXNUM + 1] = { 0 };		//�м���  
	float D[MAXNUM + 1] = { 0 };		//�м���  
	float M[MAXNUM + 1] = { 0 };		//M����  
	float B[MAXNUM + 1] = { 0 };		//׷�Ϸ��м���  
	float Y[MAXNUM + 1] = { 0 };		//׷�Ϸ��м����  

	/*m_fWaveL0 = 895.243;
	m_fWaveL1 = 1688.750;*/
	float step = (float)45/4500;// (g_devInfo.m_fWaveL1 - g_devInfo.m_fWaveL0) / (MAXNUM + 1);

	for (int i = 0; i <= MAXNUM; i++)
	{
		//x[i] = g_devInfo.m_fWaveL0 + step * i;
		x[i] = step * i;
	}

	////////////////////////////////////////�����м����
	if ((point_num < 3) || (point_num > MAXNUM + 1))
	{
		return;       //�������ݵ����̫�ٻ�̫��  
	}
	for (int i = 0; i <= point_num - 2; i++)
	{   
		//��H[i]  
		H[i] = x[i + 1] - x[i];
		Fi[i] = (y[i + 1] - y[i]) / H[i]; //��F[x(i),x(i+1)]  
	}
	for (int i = 1; i <= point_num - 2; i++)
	{   
		//��U[i]��A[i]��D[i]  
		U[i] = H[i - 1] / (H[i - 1] + H[i]);
		A[i] = H[i] / (H[i - 1] + H[i]);
		D[i] = 6 * (Fi[i] - Fi[i - 1]) / (H[i - 1] + H[i]);
	}
	//���߽�����Ϊ1����������  
	U[MAXNUM] = 1;
	A[0] = 1;
	D[0] = 6 * (Fi[0] - begin_k1) / H[0];
	D[MAXNUM] = 6 * (end_k1 - Fi[MAXNUM - 1]) / H[MAXNUM - 1];
	//���߽�����Ϊ2����������  
	//U[i] = 0;  
	//A[0] = 0;  
	//D[0] = 2 * begin_k2;  
	//D[i] = 2 * end_k2;

	/////////////////////////////////////////׷�Ϸ����M����  
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

	//////////////////////////////////////////���㷽�������ս��  
	for (int i = 0; i <= point_num - 2; i++)
	{
		a3[i] = M[i] / (6 * H[i]);
		a1[i] = (y[i] - M[i] * H[i] * H[i] / 6) / H[i];
		b3[i] = M[i + 1] / (6 * H[i]);
		b1[i] = (y[i + 1] - M[i + 1] * H[i] * H[i] / 6) / H[i];
	}

	//�����ֵ���125����
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