#pragma once
#include "Vertex.h"
#include "Effects.h"
#include "Camera.h"
class CCordinate
{
	ID3D11Buffer* mVB;
	ID3D11Buffer* mIB;
	XMFLOAT4X4 mWorld;
public:
	void SetWorld(const XMFLOAT4X4& world) { mWorld = world; }
	void CCordinate::Init(ID3D11Device* device,
		const XMFLOAT4X4& world,
		const float& lineLength);
	void Draw(ID3D11DeviceContext* md3dImmediateContext, Camera mCam);
	CCordinate();
	~CCordinate();
};

