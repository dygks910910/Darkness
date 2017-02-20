#pragma once
#include "Box.h"
class CPlane :
	public CBox
{
public:
	CPlane();
	~CPlane();
	virtual void Init(ID3D11Device* d3ddevice);
	virtual void Draw(ID3D11DeviceContext* md3dImmediateContext, Camera mCam);
};

