#pragma once
#include "d3dApp.h"
#include "FBXImporter.h"
class CBuilding :
	public D3DApp
{
public:
	CBuilding(HINSTANCE hInstance);
	~CBuilding();
	virtual bool Init();
	virtual void OnResize();
	virtual void UpdateScene(float dt);
	virtual void DrawScene();

	virtual void OnMouseDown(WPARAM btnState, int x, int y);
	virtual void OnMouseUp(WPARAM btnState, int x, int y);
	virtual void OnMouseMove(WPARAM btnState, int x, int y) ;
private:
	void BuildGeometryBuffers() ;
	void BuildFX() ;
	void BuildVertexLayout() ;

private:
	ID3D11Buffer* mBuildingVB;
	ID3D11Buffer* mBuildingIB;

	ID3DX11Effect* mFX;
	ID3DX11EffectTechnique* mTech;
	ID3DX11EffectMatrixVariable* mfxWorldViewProj;

	ID3D11InputLayout* mInputLayout;

	XMFLOAT4X4 mWorld;
	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;

	float mTheta;
	float mPhi;
	float mRadius;

	POINT mLastMousePos;
};

