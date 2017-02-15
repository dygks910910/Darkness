#pragma once
#include"d3dUtil.h"
#include "Camera.h"
#include"xnacollision.h"
class CStaticObject
{
protected:
	ID3D11Buffer* mObjVB;
	ID3D11Buffer* mObjIB;
	Material mObjMat;
	XMFLOAT4X4 mObjWorld;
	ID3D11ShaderResourceView* mObjMapSRV;
	XNA::AxisAlignedBox mColisionBox;
public:
	XNA::AxisAlignedBox GetColisionBox() { return mColisionBox; };

	virtual void Init(ID3D11Device* device) PURE;
	virtual void Draw(ID3D11DeviceContext* md3dImmediateContext, Camera mCam)PURE;
	CStaticObject() : mObjVB(0),mObjIB(0),mObjMapSRV(0)
	{
		ZeroMemory(&mObjMat, sizeof(mObjMat));
	};
	~CStaticObject() 
	{
		ReleaseCOM(mObjVB);
		ReleaseCOM(mObjIB);
		ReleaseCOM(mObjMapSRV);
	};
};