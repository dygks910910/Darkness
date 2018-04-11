#pragma once
#include "Model.h"
#include "Vertex.h"
#include "Effects.h"

//클래스간 교차참조를 해결.
class ModelMgr;
class BasicModel :
	public Model
{
public:
	BasicModel();
	virtual ~BasicModel();

	// Model을(를) 통해 상속됨
	virtual void Draw(const Camera & cam, const ModelMgr* mgr) override;
};

