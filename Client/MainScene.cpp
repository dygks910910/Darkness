#include "MainScene.h"



CMainScene::CMainScene()
{
}


CMainScene::~CMainScene()
{
	//////////////////////////////////////////////////////////////////////////
	//사용한 비트맵 메모리 반환.
	mBackgroundPicture.Shutdown();
	mLogo.Shutdown();
	mInputBoard.Shutdown();
	mInputIP.Shutdown();
	mInputPort.Shutdown();
	mInputNickname.Shutdown();

}

bool CMainScene::Init(ID3D11Device * device, ID3D11DeviceContext * dc,
	IDXGISwapChain * swapChain, const D3D11_VIEWPORT & viewPort, 
	const int & clientWidth, const int & clientHeight)
{
	mDevice = device;
	mDc = dc;
	mSwapChain = swapChain;
	mClientWidth = clientWidth;
	mClientHeight = clientHeight;

	mCam.SetPosition(0, 0, 0);
	XMStoreFloat4x4(&mWorldMtx, XMMatrixTranslation(0, 0, 7));

	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;

	//////////////////////////////////////////////////////////////////////////
	//메인화면 초기화.
	mBackgroundPicture.Initialize(device, 800, 600, L"testBack.jpg", 800, 600);
	mLogo.Initialize(device, 800, 600, L"UITextures/Logo.png", 400, 100);
	mConnectButton.Init(device,100, 33, L"UITextures/connect.png",500, 400);
	mExitButton.Init(device, 100, 33, L"UITextures/exit.png",500,450);
	//////////////////////////////////////////////////////////////////////////
	//inputboard 초기화.
	mInputBoard.Initialize(device, 800, 600, L"UITextures/InputBoard.png", 800, 600);
	mInputIP.Initialize(device, 800, 600, L"UITextures/InputIP.png", 300, 50);
	mInputPort.Initialize(device, 800, 600, L"UITextures/InputPort.png", 300, 50);
	mInputNickname.Initialize(device, 800, 600, L"UITextures/InputNickName.png", 300, 50);
	mReturnButton.Init(device, 200, 100, L"UITextures/Lobby나가기.png", 500, 450);
	mLobbyConnectButton.Init(device, 200, 100, L"UITextures/Lobby접속.png", 150, 450);

	// Clear the second depth stencil state before setting the parameters.
	
	ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));

	// Now create a second depth stencil state which turns off the Z buffer for 2D rendering.  The only difference is 
	// that DepthEnable is set to false, all other parameters are the same as the other depth stencil state.
	depthDisabledStencilDesc.DepthEnable = false;
	depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDisabledStencilDesc.StencilEnable = true;
	depthDisabledStencilDesc.StencilReadMask = 0xFF;
	depthDisabledStencilDesc.StencilWriteMask = 0xFF;
	depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	HR(device->CreateDepthStencilState(&depthDisabledStencilDesc, &mDepthDisableState));
	depthDisabledStencilDesc.DepthEnable = true;

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	// Initialize the description of the stencil state.
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// Set up the description of the stencil state.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create the depth stencil state.
	HR(device->CreateDepthStencilState(&depthStencilDesc, &mDepthStencilState));
	dc->OMSetDepthStencilState(mDepthStencilState, 1);
	bActivedInputBoard = false;
	//mMinimap.Initialize(device, 800, 600, mCam.View(), 1000, 1000);
	OnResize();
	return true;
}

void CMainScene::UpdateScene(const float & dt)
{
	if (bActivedInputBoard)
	{
		if (mLobbyConnectButton.isClicked)
		{
			//////////////////////////////////////////////////////////////////////////
			//방생성창으로  이동.
		}
		if (mReturnButton.isClicked)
		{
			////////////////////////////////////////////////////////////
			//메인화면으로 이동.
			bActivedInputBoard = false;
			mConnectButton.isClicked = false;
			mExitButton.isClicked = false;
			mLobbyConnectButton.isClicked = false;
			mReturnButton.isClicked = false;
			//////////////////////////////////////////////////////////////////////////
			//메인화면으로 돌아올경우 모든버튼의 Isclicked 초기화해줘야함.
		}
	}
	else
	{
		if (mConnectButton.isClicked)
		{
			bActivedInputBoard = true;
		}
		else if(mExitButton.isClicked)
		{
			exit(1);
		}
	}


	mCam.UpdateViewMatrix();
}

void CMainScene::Draw(ID3D11RenderTargetView * rtv, ID3D11DepthStencilView * dsv, D3D11_VIEWPORT * viewPort)
{
	mDc->ClearRenderTargetView(rtv, reinterpret_cast<const float*>(&Colors::Silver));
	mDc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	//zBuffer off
	mDc->IASetInputLayout(InputLayouts::Basic32);
	mDc->OMSetDepthStencilState(mDepthDisableState, 1);
	bool result;
	// center Sky about eye in world space
	XMFLOAT3 eyePos = mCam.GetPosition();
	XMMATRIX world = XMLoadFloat4x4(&mWorldMtx);
	XMMATRIX WVP = XMMatrixMultiply(world, world*mCam.Proj()*mCam.othMtx());

	Effects::BasicFX->SetWorldViewProj(WVP);
	Effects::BasicFX->SetDiffuseMap(mBackgroundPicture.GetTexture());
	Effects::BasicFX->SetTexTransform(XMMatrixScaling(1, 1, 1.0f));

	mDc->RSSetState(0);
	//////////////////////////////////////////////////////////////////////////
	//기본메인화면.
	mBackgroundPicture.Render(mDc, 0, 0);
	mLogo.Render(mDc, 250, 100);
	mConnectButton.Draw(mDc);
	mExitButton.Draw(mDc);
	//////////////////////////////////////////////////////////////////////////
	//IP주소 입력창 화면
	if (bActivedInputBoard)
	{
		mInputBoard.Render(mDc, 0, 0);
		mInputIP.Render(mDc, 250, 250);
		mInputPort.Render(mDc, 250, 300);
		mInputNickname.Render(mDc, 250, 350);
		mLobbyConnectButton.Draw(mDc);
		mReturnButton.Draw(mDc);
	}
	//////////////////////////////////////////////////////////////////////////
	mDc->OMSetDepthStencilState(mDepthStencilState, 1);
	// restore default states.
	mDc->RSSetState(0);
	mDc->OMSetDepthStencilState(0, 0);

	HR(mSwapChain->Present(0, 0));
}

void CMainScene::OnMouseDown(WPARAM btnState, int x, int y, const HWND & mhMainWnd)
{
	if (bActivedInputBoard)
	{
		mLobbyConnectButton.OnMouseDown(x, y);
		mReturnButton.OnMouseDown(x, y);
	}
	else
	{
		mConnectButton.OnMouseDown(x, y);
		mExitButton.OnMouseDown(x, y);
	}
}

void CMainScene::OnMouseUp(WPARAM btnState, int x, int y)
{
	if (bActivedInputBoard)
	{
		mLobbyConnectButton.OnMouseUp(x, y);
		mReturnButton.OnMouseUp(x, y);
	}
	else
	{
		mConnectButton.OnMouseUp(x, y);
		mExitButton.OnMouseUp(x, y);
	}
}

void CMainScene::OnMouseMove(WPARAM btnState, int x, int y)
{
	if (bActivedInputBoard)
	{
		mLobbyConnectButton.OnMouseMove(x, y);
		mReturnButton.OnMouseMove(x, y);
	}
	else
	{
		mConnectButton.OnMouseMove(x, y);
		mExitButton.OnMouseMove(x, y);
	}
}

void CMainScene::OnResize()
{
	mCam.SetLens(0.25f*MathHelper::Pi, AspectRatio(), 0, 3000.0f);

}
