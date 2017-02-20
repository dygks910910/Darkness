#pragma once
#include "d3dUtil.h"
#include "Vertex.h"
#include "GeometryGenerator.h"
#include "Effects.h"
#include<iostream>
#include "Camera.h"
#include "RenderStates.h"
#include "FbxLoader.h"
class CDynamicObject
{
protected:
	ID3D11Device* md3dDevice;
	ID3D11Buffer* mObjVB;
	ID3D11Buffer* mObjIB;
	Material mObjMat;
	XMFLOAT4X4 mObjWorld;
	ID3D11ShaderResourceView* mObjMapSRV;
	UINT indecesCount;
public:

	virtual void SetWorld(const XMFLOAT4X4& worldmtx) { mObjWorld = worldmtx; };
	virtual void Init(ID3D11Device* d3ddevice)PURE;
	virtual void Draw(ID3D11DeviceContext* md3dImmediateContext, const Camera& mCam)PURE;
	CDynamicObject();
	~CDynamicObject();
};

