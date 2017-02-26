#include "TestScene.h"


CTestScene::CTestScene()
{
}


CTestScene::~CTestScene()
{
	for (auto p = mvDynamicObject.begin(); p != mvDynamicObject.end();)
	{
		delete *p;
		p=mvDynamicObject.erase(p);
	}
	for (auto p = mvStaticObject.begin(); p != mvStaticObject.end();)
	{
		delete *p;
		p = mvStaticObject.erase(p);
	}
	SafeDelete(mSky);
}

bool CTestScene::Init(ID3D11Device * device, ID3D11DeviceContext * dc, 
	IDXGISwapChain * swapChain, ID3D11RenderTargetView * renderTargetView)
{
	mLastMousePos.x = 0;
	mLastMousePos.y = 0;
	mDirLights[0].Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	mDirLights[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mDirLights[0].Specular = XMFLOAT4(0.8f, 0.8f, 0.7f, 1.0f);
	mDirLights[0].Direction = XMFLOAT3(0.407f, -0.407f, 0.0f);

	mDirLights[1].Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[1].Diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[1].Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[1].Direction = XMFLOAT3(0.47735f, -0.47735f, 0.57735f);

	mDirLights[2].Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[2].Diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[2].Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[2].Direction = XMFLOAT3(-0.47735f, -0.47735f, -0.57735f);

	mSky = new Sky(device, L"Textures/sunsetcube1024.dds", 5000.0f);
	Effects::BasicFX->SetDirLights(mDirLights);
	Effects::BasicFX->SetFogColor(Colors::Silver);
	Effects::BasicFX->SetFogStart(15.0f);
	Effects::BasicFX->SetFogRange(175.0f);

	//월드좌표계 그려주기.
	XMFLOAT4X4 temp4x4;
	XMStoreFloat4x4(&temp4x4, XMMatrixIdentity());
	mCordWorld.Init(device, temp4x4, 5000);

	std::ifstream ifs;
	ifs.open("MapData.txt");
	XMFLOAT3 scale;
	XMFLOAT4 rotation;
	XMFLOAT3 position;
	CDynamicObject* pTempDynamicObject;
	CStaticObject* pTempStaticObject;
	char objectName[50];
	char cIgnore[100];
	int StaticObjCount;
	ifs >> cIgnore >> StaticObjCount;
	for (int i = 0; i < StaticObjCount; ++i)
	{
		ifs >> objectName >> cIgnore>>  cIgnore >> cIgnore >> cIgnore >> position.x >> position.y>> position.z	>> cIgnore;
		ifs >> cIgnore >> cIgnore >> cIgnore >> rotation.x >> rotation.y>> rotation.z >> rotation.w >> cIgnore;
		ifs >> cIgnore >> cIgnore >> scale.x	 >> scale.y >> scale.z >> cIgnore;


		if (!strcmp(objectName, "Cube"))
		{
			pTempStaticObject = new CBox;
			pTempStaticObject->Init(device);
			XMVECTOR S = XMLoadFloat3(&scale);
			XMVECTOR P = XMLoadFloat3(&position);
			XMVECTOR Q = XMLoadFloat4(&rotation);
			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			XMFLOAT4X4 M;
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
			pTempStaticObject->SetWorld(M);
			mvStaticObject.push_back(pTempStaticObject);
		}
		else if (!strcmp(objectName, "crawler"))
		{
			pTempDynamicObject = new CBuilding;
			pTempDynamicObject->Init(device);
			XMVECTOR S = XMLoadFloat3(&scale);
			XMVECTOR P = XMLoadFloat3(&position);
			XMVECTOR Q = XMLoadFloat4(&rotation);
			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			XMFLOAT4X4 M;
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));

			pTempDynamicObject->SetWorld(M);
			mvDynamicObject.push_back(pTempDynamicObject);
		}
		else if (!strcmp(objectName, "MainCamera"))
		{
			mCam.SetPosition(position.x, position.y, position.z);
		}
		
		std::cout << objectName << std::endl;
		std::cout << position.x << " " << position.y << " " << position.z << std::endl;
		std::cout << rotation.x << " " << rotation.y << " " << rotation.z<< " " << rotation.w<< std::endl;
		std::cout << scale.x << " " << scale.y << " " << scale.z << std::endl << std::endl;
		ZeroMemory(&objectName, sizeof(objectName));
		ZeroMemory(&cIgnore, sizeof(cIgnore));
		ZeroMemory(&position, sizeof(position));
		ZeroMemory(&rotation	, sizeof(rotation));
		ZeroMemory(&scale, sizeof(scale));

	}
	ifs.close();
	XMStoreFloat4x4(&temp4x4, XMMatrixIdentity());
	mCordWorld.Init(device,temp4x4, 5000);
	return true;
}

void CTestScene::UpdateScene(const float & dt)
{
// 	mTimer.Tick();
	//
	// Control the camera.
	//
	if (GetAsyncKeyState('W') & 0x8000)
		mCam.Walk(10.0f*dt);

	if (GetAsyncKeyState('S') & 0x8000)
		mCam.Walk(-10.0f*dt);

	if (GetAsyncKeyState('A') & 0x8000)
		mCam.Strafe(-10.0f*dt);

	if (GetAsyncKeyState('D') & 0x8000)
		mCam.Strafe(10.0f*dt);

	//
	// Walk/fly mode
	//

// 	if (GetAsyncKeyState('R') & 0x8000)
	{
		// 		mFire.Reset();
		// 		mRain.Reset();
	}
	// 
	// Clamp camera to terrain surface in walk mode.
	//

	//
	// Reset particle systems.
	//

	mCam.UpdateViewMatrix();
}

void CTestScene::Draw(ID3D11Device * device, ID3D11DeviceContext * dc,
	IDXGISwapChain * swapChain, ID3D11RenderTargetView * renderTargetView,
	ID3D11DepthStencilView * depthStencilView)
{
	dc->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
	dc->ClearRenderTargetView(renderTargetView, reinterpret_cast<const float*>(&Colors::Silver));

	dc->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	for (auto p : mvDynamicObject)
	{
		p->Draw(dc, mCam);
	}
	for (auto p : mvStaticObject)
	{
		p->Draw(dc, mCam);
	}

	mCordWorld.Draw(dc, mCam);
	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

	if (GetAsyncKeyState('1') & 0x8000)
		dc->RSSetState(RenderStates::WireframeRS);
	
	dc->RSSetState(0);
	mSky->Draw(dc, mCam);
	//불 파티클.
	dc->OMSetBlendState(0, blendFactor, 0xffffffff); // restore default
	


	// restore default states.
	dc->RSSetState(0);
	dc->OMSetDepthStencilState(0, 0);
	dc->OMSetBlendState(0, blendFactor, 0xffffffff);


	HR(swapChain->Present(0, 0));
}

void CTestScene::OnMouseDown(WPARAM btnState, int x, int y, const HWND& mhMainWnd)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(mhMainWnd);
}

void CTestScene::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void CTestScene::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));

		mCam.Pitch(dy);
		mCam.RotateY(dx);
	}
	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void CTestScene::OnResize(const float& aspectRatio)
{
	mCam.SetLens(0.25f*MathHelper::Pi, aspectRatio, 0.3f, 3000.0f);
}
