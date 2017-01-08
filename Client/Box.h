#pragma once
#include "d3dUtil.h"
#include "Vertex.h"
#include "GeometryGenerator.h"
#include "Effects.h"
#include<iostream>
#include "Camera.h"
#include "RenderStates.h"
class CBox
{
	ID3D11Device* md3dDevice;
	ID3D11Buffer* mBoxVB;
	ID3D11Buffer* mBoxIB;
	Material mBoxMat;
	XMFLOAT4X4 mBoxWorld;
	ID3D11ShaderResourceView* mBoxMapSRV;

public:
	CBox();
	~CBox();
	void Init(ID3D11Device* d3ddevice);
	void Draw(ID3D11DeviceContext* md3dImmediateContext, Camera mCam);


};

