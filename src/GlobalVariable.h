#pragma once

#include <map>
#include <list>
// ���湯����Ũ�ȱ��е�����
// TODO: ��Ϊ�Ƕ��ձ�(�¶�-Ũ��) ���ݽṹʹ��keyֵ��
std::map<float, float> gVarMap;

// ��ʼ��map
void InitMap();

// ��ֵ����
void insert(__in float *pData, __out float *pResult);

// �����¶Ȳ��Ҷ�Ӧ��Ũ��

// �������ս��