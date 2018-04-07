#include "RoomScene.h"

CRoomScene::CRoomScene()
{
}


CRoomScene::~CRoomScene()
{
	//////////////////////////////////////////////////////////////////////////
	//사용한 비트맵 메모리 반환.
	mBackgroundPicture.Shutdown();
	mKingLogo.Shutdown();
}

bool CRoomScene::Init()
{
	mCam.SetPosition(0, 0, 0);
	XMStoreFloat4x4(&mWorldMtx, XMMatrixTranslation(0, 0, 7));

	mDrawText.Init();

	mBackgroundPicture.Initialize( L"UITextures/RoomScene2.png", mClientWidth, mClientHeight);
	starButton.Init(mClientWidth/5,mClientHeight/5, L"UITextures/Start.png", mClientWidth / 1.5f, mClientHeight / 1.5f);
	mKingLogo.Initialize(L"UITextures/CreaterMark.png", FONT_SIZE, FONT_SIZE);
	return true;
}

void CRoomScene::OnKeyboardButtonDown(WPARAM wParam, UINT msg)
{
}

int CRoomScene::UpdateScene(const float dt, MSG & msg)
{
	if (starButton.isClicked)
	{
		return 1;
	}
	return 0;
}

void CRoomScene::Draw()
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

	mBackgroundPicture.Render(0, 0);
	starButton.Draw();
	mKingLogo.Render(mClientWidth / 13, mClientHeight / 10 * 2.15f);

	mDrawText(L"100000", FONT_SIZE,mClientWidth/10, mClientHeight/10*2.15f,mDrawText.RED);

	mDrawText(L"100000", FONT_SIZE, mClientWidth / 10, mClientHeight / 10 * 2.15f +FONT_SIZE*2.5f, mDrawText.RED);

	mDrawText(L"100000", FONT_SIZE, mClientWidth / 10, mClientHeight / 10 * 2.15f + FONT_SIZE*5.0f, mDrawText.RED);

	mDrawText(L"100000", FONT_SIZE, mClientWidth / 10, mClientHeight / 10 * 2.15f + FONT_SIZE*7.5f, mDrawText.RED);

	mDrawText(L"100000", FONT_SIZE, mClientWidth / 10, mClientHeight / 10 * 2.15f + FONT_SIZE*10.0f, mDrawText.RED);

	mDrawText(L"100000", FONT_SIZE, mClientWidth / 10, mClientHeight / 10 * 2.15f + FONT_SIZE*12.5f, mDrawText.RED);

	mDrawText(L"100000", FONT_SIZE, mClientWidth / 10, mClientHeight / 10 * 2.15f + FONT_SIZE*15.0f, mDrawText.RED);



	//////////////////////////////////////////////////////////////////////////

	// restore default states.

	HR(mSwapChain->Present(0, 0));
}

void CRoomScene::OnMouseDown(WPARAM btnState, int x, int y)
{
	starButton.OnMouseDown(x, y);
}

void CRoomScene::OnMouseUp(WPARAM btnState, int x, int y)
{
	starButton.OnMouseUp(x, y);
}

void CRoomScene::OnMouseMove(WPARAM btnState, int x, int y)
{
	starButton.OnMouseMove(x, y);
}

void CRoomScene::OnResize()
{
}
