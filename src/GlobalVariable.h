#pragma once

#include <map>
#include <list>
// 保存汞蒸气浓度表中的数据
// TODO: 因为是对照表(温度-浓度) 数据结构使用key值对
std::map<float, float> gVarMap;

// 初始化map
void InitMap();

// 插值操作
void insert(__in float *pData, __out float *pResult);

// 根据温度查找对应的浓度

// 计算最终结果