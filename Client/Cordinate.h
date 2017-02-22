#pragma once
#include "StaticObject.h"
#include "Vertex.h"
#include "Effects.h"
class CCordinate
{
	ID3D11Buffer* mVB;
	ID3D11Buffer* mIB;
	XMFLOAT4X4 mWorld;
	XMFLOAT4X4 mLocal;
public:
	void CCordinate::Init(ID3D11Device* device, const XMFLOAT4X4& local,
		const XMFLOAT4X4& world,
		const float& lineLength);
	void Draw(ID3D11DeviceContext* md3dImmediateContext, Camera mCam);
	void SetLocal(const XMFLOAT4X4& localmtx);
	CCordinate();
	~CCordinate();
};

