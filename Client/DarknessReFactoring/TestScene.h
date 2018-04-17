#pragma once
#include "Scene.h"
#include"Common\GeometryGenerator.h"
#include"Vertex.h"
#include"Effects.h"
#include"RenderStates.h"
#include "Camera.h"
#include "ModelMgr.h"
class CTestScene :
	public Scene
{
public:
	ModelMgr modelMgr;
	Camera m_Cam;
	CTestScene();
	virtual ~CTestScene();
	// SceneÀ»(¸¦) ÅëÇØ »ó¼ÓµÊ
	virtual bool Init() override;
	virtual void OnKeyboardButtonDown( WPARAM wParam,UINT msg) override;
	virtual int UpdateScene(const float dt, MSG & msg) override;
	virtual void Draw() override;
	virtual void OnMouseDown(WPARAM btnState, int x, int y) override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y) override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y) override;
	virtual void OnResize() override;

	//¸â¹öÇÔ¼ö
private:
	void BuildGeometryBuffers();
	void BuildFBXBuffers(const GeometryGenerator::MeshData& box);


	//¸â¹öº¯¼ö
private:
	ID3D11Buffer* mBoxVB;
	ID3D11Buffer* mBoxIB;


	DirectionalLight mDirLights[3];
	Material mBoxMat;

	XMFLOAT4X4 mTexTransform;
	XMFLOAT4X4 mBoxWorld;

// 	XMFLOAT4X4 mView;
// 	XMFLOAT4X4 mProj;

	int mBoxVertexOffset;
	UINT mBoxIndexOffset;
	UINT mBoxIndexCount;

	/*XMFLOAT3 mEyePosW;*/

	float mTheta;
	float mPhi;
	float mRadius;

	POINT mLastMousePos;
};

