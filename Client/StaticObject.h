#pragma once
#include"d3dUtil.h"
#include "Camera.h"
#include"xnacollision.h"
#include "Cordinate.h"
#include "RenderStates.h"
#include "ModelMgr.h"
#include "TextureMgr.h"
class CStaticObject
{
protected:
	CModel* mModel;
	Material mObjMat;
	XMFLOAT4X4 mObjWorld;
	ID3D11ShaderResourceView* mObjMapSRV;

	bool bHasNormalMap = false;
	ID3D11ShaderResourceView* mObjNormalMapSRV;

	XNA::AxisAlignedBox mColisionBox;
public:
	XNA::AxisAlignedBox GetColisionBox() { return mColisionBox; };
	void SetWorld(const XMFLOAT4X4& world) { mObjWorld = world; };

	virtual void Init(ID3D11Device* device, CModelMgr* modelMgr, TextureMgr* textureMgr) PURE;
	virtual void Draw(ID3D11DeviceContext* dc, Camera mCam)PURE;
	virtual void DrawToShadowMap(ID3D11DeviceContext* dc, Camera mCam);
	virtual void DrawToSsaoNormalDepthMap(ID3D11DeviceContext* dc, Camera mCam);
	CStaticObject() : mModel(nullptr),mObjMapSRV(0)
	{
		ZeroMemory(&mObjMat, sizeof(mObjMat));
	};
	virtual ~CStaticObject() 
	{
	};
};