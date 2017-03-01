#pragma once
#include "StaticObject.h"

class CPlane :
	public CStaticObject
{
	const int& PLANE_SIZE = 10;
public:
	virtual void Init(ID3D11Device* device) ;
	virtual void Draw(ID3D11DeviceContext* dc, Camera mCam);
	CPlane();
	~CPlane();
};

