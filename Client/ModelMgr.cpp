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
		�ۼ���:�ڿ���(dygks910910@daum.net)
		����:������  FBX��� ���⼭ �ε��ϰ� 
		model->SetBuffer(),model->SetIndexOffset()���� ����Ͽ� 
		mData[fileName] = new CModel;
		����.
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
