#pragma once
#include "DynamicObject.h"
#include "Model.h"

class CCrown : public CDynamicObject
{
	CCordinate mCoord;
public:
	CCrown();
	~CCrown();
	void SetWorld(const XMFLOAT4X4& worldmtx);
	void Init(ID3D11Device* d3ddevice, CModelMgr* modelMgr);
	void Draw(ID3D11DeviceContext* dc, const Camera& mCam);
};

#ifdef _DEBUG
std::ostream& operator<<(std::ostream& os, const XMFLOAT3& float3);
#endif