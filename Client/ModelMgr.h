#pragma once
#include<map>
#include<string>
#include "Model.h"
class CModelMgr
{
	std::map<std::string,CModel*> mData;
public:
	//모델을 삽입하고 해당 모델을 CModel타입으로 리턴받는다.
	CModel* CreateModel(const char* fileName);
	CModelMgr();
	~CModelMgr();
};

