#include "ModelMgr.h"



CModel* CModelMgr::CreateModelFBX(const char* fileName, std::vector<Vertex::Basic32>& vb, std::vector<UINT>& ib)
{
	std::string tempFilename = fileName;
	CModel* model = nullptr;
	CFbxLoader loader;

	if (mData.find(tempFilename) != mData.end())
	{
		model = mData[tempFilename];
	}
	else
	{
		loader.LoadFBX("true_clownTri.FBX", vb, ib);
		model = new CModel;
		std::cout << "모델생성" << std::endl;
		/*
		2017 / 3 / 6 / 11:18
		작성자:박요한(dygks910910@daum.net)
		설명:파일이  FBX라면 여기서 로드하고 
		model->SetBuffer(),model->SetIndexOffset()등을 사용하여 
		mData[fileName] = new CModel;
		삽입.
		*/
		mData.insert(make_pair(tempFilename, model));
		loader.Destroy();
	}
	return model;
}

const bool & CModelMgr::CheckHasModel(const char * modelName)
{
	if (mData.find(modelName) != mData.end())
	{
		return true;
	}
	else
		return false;
}

void CModelMgr::InsertModel(const char* modelName, CModel* model)
{
	std::string tempModelName = modelName;
	CModel* tempModel;
	if (mData.find(modelName) != mData.end())
	{
		tempModel = mData[modelName];
	}
	else 
	{
		tempModel = model;
		mData.insert(make_pair(tempModelName, tempModel));
		std::cout << "모델삽입." << std::endl;
	}
}

CModel * CModelMgr::GetModel(const char * modelName)
{
	std::string tempModelName = modelName;
	if (mData.find(tempModelName) != mData.end())
	{
		return mData[tempModelName];
	}

	return nullptr;
}

CModelMgr::CModelMgr()
{
}


CModelMgr::~CModelMgr()
{
	for (auto p = mData.begin(); p != mData.end(); ++p)
	{
		delete p->second;
	}
}
