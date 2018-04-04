#include "MainScene.h"

CMainScene::CMainScene()
{
}


CMainScene::~CMainScene()
{
	//////////////////////////////////////////////////////////////////////////
	//사용한 비트맵 메모리 반환.
	mMainLogo.Shutdown();
}


std::string CMainScene::UpdateScene(const float dt, MSG& msg)
{
	//////////////////////////////////////////////////////////////////////////
	//로고를 비추는 창을 켜주는 시간을 계산.
	return "";
}

void CMainScene::OnMouseUp(WPARAM btnState, int x, int y)
{
}

void CMainScene::OnMouseMove(WPARAM btnState, int x, int y)
{
}

bool CMainScene::Init()
{
	mCam.SetPosition(0, 0, 0.1f);
	XMStoreFloat4x4(&mWorldMtx, XMMatrixTranslation(0, 0, 0.1f));
	//XMStoreFloat4x4(&mWorldMtx,  XMMatrixMultiply(XMLoadFloat4x4(&mWorldMtx), XMMatrixRotationAxis(XMVectorSet(0, 1, 0,0), 90)));

	//////////////////////////////////////////////////////////////////////////
	//로고화면 초기화.
	mMainLogo.Initialize(L"UITextures/DarknessLogo.PNG", mClientWidth, mClientHeight);

	//OnResize();
	return true;
}

void CMainScene::OnKeyboardButtonDown(WPARAM wParam, UINT msg)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_UP)
		{
			mCam.SetPosition(0, 0, mCam.GetPosition().z +1);
		}
		break;
	default:
		break;
	}
}

void CMainScene::OnResize()
{
	mCam.SetLens(0.34f*MathHelper::Pi, AspectRatio(), 0, 3000.0f);

}

void CMainScene::Draw()
{
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::Silver));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	md3dImmediateContext->OMSetDepthStencilState(RenderStates::DepthDisableState, 1);
	md3dImmediateContext->IASetInputLayout(InputLayouts::PosTex);
	
	XMFLOAT3 eyePos = mCam.GetPosition();
	XMMATRIX world = XMLoadFloat4x4(&mWorldMtx);
	XMMATRIX view = mCam.View();
	XMMATRIX proj = mCam.OthoProj();

	XMMATRIX WVP = world * view * proj;

	Effects::TextureFX->SetWorld(world);
	Effects::TextureFX->SetView(view);
	Effects::TextureFX->SetProj(proj);


	mMainLogo.Render(0,0);
	//////////////////////////////////////////////////////////////////////////

	// restore default states.

	HR(mSwapChain->Present(0, 0));
}

void CMainScene::OnMouseDown(WPARAM btnState, int x, int y)
{
}
