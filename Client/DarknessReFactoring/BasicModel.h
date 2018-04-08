#pragma once
#include "Model.h"
#include "Vertex.h"
#include "Effects.h"
class BasicModel :
	public Model
{
public:
	BasicModel();
	virtual ~BasicModel();

	// Model��(��) ���� ��ӵ�
	virtual void Draw(const Camera & cam, ID3D11Buffer* vb, ID3D11Buffer* ib) override;
};

