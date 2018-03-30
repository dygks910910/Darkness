#pragma once
#include "Scene.h"
#include"Common\GeometryGenerator.h"
#include"Vertex.h"
#include"Effects.h"
class CTestScene :
	public Scene
{
public:
	CTestScene();
	virtual ~CTestScene();
	// SceneÀ»(¸¦) ÅëÇØ »ó¼ÓµÊ
	virtual bool Init() override;
	virtual void OnKeyboardButtonDown( WPARAM wParam) override;
	virtual std::string UpdateScene(const float dt, MSG & msg) override;
	virtual void Draw() override;
	virtual void OnMouseDown(WPARAM btnState, int x, int y) override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y) override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y) override;
	virtual void OnResize() override;

	//¸â¹öÇÔ¼ö
private:
	void BuildGeometryBuffers();


	//¸â¹öº¯¼ö
private:
	ID3D11Buffer* mBoxVB;
	ID3D11Buffer* mBoxIB;

	ID3D11ShaderResourceView* mDiffuseMapSRV;

	DirectionalLight mDirLights[3];
	Material mBoxMat;

	XMFLOAT4X4 mTexTransform;
	XMFLOAT4X4 mBoxWorld;

	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;

	int mBoxVertexOffset;
	UINT mBoxIndexOffset;
	UINT mBoxIndexCount;

	XMFLOAT3 mEyePosW;

	float mTheta;
	float mPhi;
	float mRadius;

	POINT mLastMousePos;
};

