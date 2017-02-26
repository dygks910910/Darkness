#pragma once
#include "Vertex.h"
#include "GeometryGenerator.h"
#include<iostream>
#include "RenderStates.h"
#include "StaticObject.h"
class CBox : public CStaticObject
{
public:
	CBox();
	~CBox();
	virtual void Init(ID3D11Device* d3ddevice);
	virtual void Draw(ID3D11DeviceContext* dc, Camera mCam);

};

