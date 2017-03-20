#pragma once
#include "StaticObject.h"
#include "GeometryGenerator.h"

class CPlane :
	public CStaticObject
{
	const int& PLANE_SIZE = 10;
public:
	virtual void Init(ID3D11Device* device, CModelMgr* modelMgr, TextureMgr* textureMgr) ;
	virtual void Draw(ID3D11DeviceContext* dc, Camera mCam);

	CPlane();
	~CPlane();
};
