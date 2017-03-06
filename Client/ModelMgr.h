#pragma once
#include<map>
#include<string>
#include "Model.h"
class CModelMgr
{
	std::map<std::string,CModel*> mData;
public:
	//���� �����ϰ� �ش� ���� CModelŸ������ ���Ϲ޴´�.
	CModel* CreateModel(const char* fileName);
	CModelMgr();
	~CModelMgr();
};

