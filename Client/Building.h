#pragma once
#include "DynamicObject.h"

class CBuilding : public CDynamicObject
{
public:
	CBuilding();
	~CBuilding();
	void SetWorld(const XMFLOAT4X4& worldmtx);
	void Init(ID3D11Device* d3ddevice);
	void Draw(ID3D11DeviceContext* md3dImmediateContext, const Camera& mCam);
};

