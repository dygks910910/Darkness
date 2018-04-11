#pragma once
#include "Model.h"
#include "Vertex.h"
#include "Effects.h"

//Ŭ������ ���������� �ذ�.
class ModelMgr;
class BasicModel :
	public Model
{
public:
	BasicModel();
	virtual ~BasicModel();

	// Model��(��) ���� ��ӵ�
	virtual void Draw(const Camera & cam, const ModelMgr* mgr) override;
};

