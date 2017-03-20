#pragma once
#include<map>
#include<string>
#include "Model.h"
#include "FbxLoader.h"
class CModelMgr
{
	std::map<std::string,CModel*> mData;
public:
	//���� �����ϰ� �ش� ���� CModelŸ������ ���Ϲ޴´�.
	CModel* CreateModelFBX(const char* fileName, std::vector<Vertex::Basic32>& vb, std::vector<UINT>& ib);
	const bool& CheckHasModel(const char* modelName);
	//���� FBX�� �ƴ϶�� ���� ����.
	void InsertModel(const char* modelName, CModel* model);

	CModel* GetModel(const char* modelName);
	void PrintModelCount() { std::cout <<"�ε�� �� �Ѱ���:" <<mData.size(); }
	CModelMgr();
	~CModelMgr();
};

