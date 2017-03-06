#include "ModelMgr.h"



CModel* CModelMgr::CreateModel(const char * fileName)
{
	std::string tempFilename = fileName;
	CModel* model = nullptr;
	if (mData.find(tempFilename) != mData.end())
	{
		model = mData[tempFilename];
	}
	else
	{
		/*
		2017 / 3 / 6 / 11:18
		작성자:박요한(dygks910910@daum.net)
		설명:파일이  FBX라면 여기서 로드하고 
		model->SetBuffer(),model->SetIndexOffset()등을 사용하여 
		mData[fileName] = new CModel;
		삽입.
		*/
		
	}
	return model;
}

CModelMgr::CModelMgr()
{
}


CModelMgr::~CModelMgr()
{
}
