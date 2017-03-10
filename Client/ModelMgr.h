#pragma once
#include<map>
#include<string>
#include "Model.h"
#include "FbxLoader.h"
class CModelMgr
{
	std::map<std::string,CModel*> mData;
public:
	//모델을 삽입하고 해당 모델을 CModel타입으로 리턴받는다.
	CModel* CreateModelFBX(const char* fileName, std::vector<Vertex::Basic32>& vb, std::vector<UINT>& ib);
	const bool& CheckHasModel(const char* modelName);
	void InsertModel(const char* modelName, CModel* model);

	CModel* GetModel(const char* modelName);
	void PrintModelCount() { std::cout <<"로드된 모델 총갯수:" <<mData.size(); }
	CModelMgr();
	~CModelMgr();
};

