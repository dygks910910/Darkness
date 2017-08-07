#include "MainScene.h"
extern ID3D11DepthStencilState* tDepthDisableState;

CMainScene::CMainScene()
{
}


CMainScene::~CMainScene()
{
	//////////////////////////////////////////////////////////////////////////
	//����� ��Ʈ�� �޸� ��ȯ.
	mBackgroundPicture.Shutdown();
	mLogo.Shutdown();
	mInputBoard.Shutdown();
	mInputIP.Shutdown();
	mInputPort.Shutdown();
	mInputNickname.Shutdown();
	mMainLogo.Shutdown();
	ReleaseCOM(mDepthDisableState);
	ReleaseCOM(mDepthStencilState);

}

bool CMainScene::Init(ID3D11Device * device, ID3D11DeviceContext * dc,
	IDXGISwapChain * swapChain, const D3D11_VIEWPORT & viewPort, 
	const int & clientWidth, const int & clientHeight)
{
	mClientWidth = clientWidth;
	mClientHeight = clientHeight;

	mCam.SetPosition(0, 0, 0);
	XMStoreFloat4x4(&mWorldMtx, XMMatrixTranslation(0, 0, 7));


	//////////////////////////////////////////////////////////////////////////
	//�ΰ�ȭ�� �ʱ�ȭ.
	mMainLogo.Initialize(device, mClientWidth, mClientHeight, L"UITextures/DarknessLogo.PNG", mClientWidth, mClientHeight);
	//////////////////////////////////////////////////////////////////////////
	//����ȭ�� �ʱ�ȭ.
	mBackgroundPicture.Initialize(device, mClientWidth, mClientHeight, L"UITextures/backGroundClown.PNG", mClientWidth, mClientHeight);
	mLogo.Initialize(device, mClientWidth/2, mClientHeight/1.5f, L"UITextures/Logo.png", 800, 200);
	mConnectButton.Init(device, BUTTON_SIZE_X, BUTTON_SIZE_Y, L"UITextures/connect.png", CONNECT_BUTTON_X, CONNECT_BUTTON_Y, mClientWidth, mClientHeight);
	mExitButton.Init(device, BUTTON_SIZE_X, BUTTON_SIZE_Y, L"UITextures/exit.png", CONNECT_BUTTON_X, EXIT_BUTTON_Y, mClientWidth, mClientHeight);
	//////////////////////////////////////////////////////////////////////////
	//inputboard �ʱ�ȭ.
	mInputBoard.Initialize(device, mClientWidth, mClientHeight, L"UITextures/InputBoard.png", 800, 600);
	mInputIP.Initialize(device, mClientWidth, mClientHeight, L"UITextures/InputIP.png", 700, 150);
	mInputPort.Initialize(device, mClientWidth, mClientHeight, L"UITextures/InputPort.png", 700, 150);
	mInputNickname.Initialize(device, mClientWidth, mClientHeight, L"UITextures/InputNickName.png", 700, 150);
	mReturnButton.Init(device, BUTTON_SIZE_X, BUTTON_SIZE_Y, L"UITextures/cancel.png", RETURN_BUTTON_X, RETURN_BUTTON_Y, mClientWidth, mClientHeight);
	mLobbyConnectButton.Init(device, BUTTON_SIZE_X, BUTTON_SIZE_Y, L"UITextures/connect.png", LOBBY_CONNECT_BUTTON_X, LOBBY_CONNECT_BUTTON_Y, mClientWidth, mClientHeight);

 	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
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
	m_bFocusOnIP = false;
	m_bFocusOnNickName = false;
	m_bFocusOnPort = false;
	m_bLogoTime = true;
	//�Լ���ü �ʱ�ȭ
	DrawText.Init(device, dc);
	//mMinimap.Initialize(device, 800, 600, mCam.View(), 1000, 1000);
	mIpString = L"";
	ZeroMemory(&Text, sizeof(Text));
	ZeroMemory(&Cstr, sizeof(Cstr));
	mTimeForLogo.Start();
	mTimeForLogo.Reset();
	OnResize();
	return true;
}

std::string CMainScene::UpdateScene(const float dt, MSG& msg)
{
	//////////////////////////////////////////////////////////////////////////
	//�ΰ� ���ߴ� â�� ���ִ� �ð��� ���.
	mTimeForLogo.Tick();
	if (mTimeForLogo.TotalTime() > 5)
	{
		m_bLogoTime = false;
	}

	if (bActivedInputBoard)
	{
		if (mLobbyConnectButton.isClicked)
		{
			if (mNicknameString == L"")
			{
				MessageBox(0, L"�г��Ӹ� �Է����ּ���", L"error", 0);
				mLobbyConnectButton.isClicked = false;
				return "";
			}
		
			NetworkMgr::GetInstance()->SetIPAndPortAndNickName(IPADDRESS,mNicknameString);
			NetworkMgr::GetInstance()->Initialize();

			return SceneName::roomScene;
			//////////////////////////////////////////////////////////////////////////
			//�����â����  �̵�.
		}
		if (mReturnButton.isClicked)
		{
			////////////////////////////////////////////////////////////
			//����ȭ������ �̵�.
			bActivedInputBoard = false;
			mConnectButton.isClicked = false;
			mExitButton.isClicked = false;
			mLobbyConnectButton.isClicked = false;
			mReturnButton.isClicked = false;
			m_bFocusOnIP = false;
			m_bFocusOnPort = false;
			m_bFocusOnNickName = false;
			//////////////////////////////////////////////////////////////////////////
			//����ȭ������ ���ƿð�� ����ư�� Isclicked �ʱ�ȭ�������.
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
		}
	}
	return "";
}

void CMainScene::Draw(ID3D11Device* device, ID3D11DeviceContext* dc,
	IDXGISwapChain* swapChain, ID3D11RenderTargetView* rtv,
	ID3D11DepthStencilView* dsv, D3D11_VIEWPORT* viewPort)
{
	dc->ClearRenderTargetView(rtv, reinterpret_cast<const float*>(&Colors::Silver));
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->IASetInputLayout(InputLayouts::Basic32);
	dc->OMSetDepthStencilState(mDepthDisableState, 1);

	tDepthDisableState = mDepthDisableState;
	// center Sky about eye in world space
	XMFLOAT3 eyePos = mCam.GetPosition();
	XMMATRIX world = XMLoadFloat4x4(&mWorldMtx);
	XMMATRIX WVP = XMMatrixMultiply(world, world*mCam.Proj()*mCam.othMtx());

	Effects::BasicFX->SetWorldViewProj(WVP);
	Effects::BasicFX->SetDiffuseMap(mBackgroundPicture.GetTexture());
	Effects::BasicFX->SetTexTransform(XMMatrixScaling(1, 1, 1.0f));
	dc->RSSetState(0);
	//////////////////////////////////////////////////////////////////////////
	//�⺻����ȭ��.
	mBackgroundPicture.Render(dc, 0, 0);
	mLogo.Render(dc, LOGO_X, LOGO_Y);
	mConnectButton.Draw(dc);
	mExitButton.Draw(dc);
	//////////////////////////////////////////////////////////////////////////
	//IP�ּ� �Է�â ȭ��
	if (bActivedInputBoard)
	{
		mInputBoard.Render(dc, INPUT_BOARD_X, INPUT_BOARD_Y);
		mInputIP.Render(dc, INPUT_IP_X, INPUT_IP_Y);
 		DrawText(mIpString, FONT_SIZE, INPUT_IP_X+200, INPUT_IP_Y+80);

		mInputPort.Render(dc, INPUT_PORT_X, INPUT_PORT_Y);
		DrawText(mPortString, FONT_SIZE, INPUT_PORT_X + 200, INPUT_PORT_Y+ 80);

		mInputNickname.Render(dc, INPUT_NICKNAME_X, INPUT_NICKNAME_Y);
		DrawText(mNicknameString, FONT_SIZE, INPUT_NICKNAME_X + 200, INPUT_NICKNAME_Y + 80);

		mLobbyConnectButton.Draw(dc);
		mReturnButton.Draw(dc);
	}

	//////////////////////////////////////////////////////////////////////////
 	if (m_bLogoTime)
 	{
 		mMainLogo.Render(dc, 0, 0);
 	}

// restore default states.
	dc->RSSetState(0);
	dc->OMSetDepthStencilState(0, 0);

	HR(swapChain->Present(0, 0));
}

void CMainScene::OnMouseDown(WPARAM btnState, int x, int y, const HWND & mhMainWnd)
{
  	if (bActivedInputBoard)
  	{
		//ip�Է�â active state check
  		mLobbyConnectButton.OnMouseDown(x, y);
  		mReturnButton.OnMouseDown(x, y);
		if (x > INPUT_IP_X && x < INPUT_IP_X+ INPUT_BAR_WIDTH &&
			y > INPUT_IP_Y && y < INPUT_IP_Y+ INPUT_BAR_HEIGHT)
		{
			m_bFocusOnIP = true;
			m_bFocusOnNickName = false;
			m_bFocusOnPort = false;
		}
		else if (x > INPUT_PORT_X && x < INPUT_PORT_X + INPUT_BAR_WIDTH &&
			y > INPUT_PORT_Y && y < INPUT_PORT_Y + INPUT_BAR_HEIGHT)
		{
			m_bFocusOnIP = false;
			m_bFocusOnNickName = false;
			m_bFocusOnPort = true;
		}
		else if (x > INPUT_NICKNAME_X && x < INPUT_NICKNAME_X + INPUT_BAR_WIDTH &&
			y > INPUT_NICKNAME_Y && y < INPUT_NICKNAME_Y + INPUT_BAR_HEIGHT)
		{
			m_bFocusOnIP = false;
			m_bFocusOnNickName = true;
			m_bFocusOnPort = false;
		}
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
	mCam.SetLens(0.34f*MathHelper::Pi, AspectRatio(), 0, 3000.0f);
}
void CMainScene::OnKeyboardButtonDown(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (m_bFocusOnIP || m_bFocusOnNickName || m_bFocusOnPort) {
		if (GetText(hWnd, msg, wparam, lparam) == 0)
		{
			return;
		}
	}
}
int CMainScene::GetText(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	int len;
	HIMC m_hIMC = NULL;   // IME �ڵ�

	switch (msg)
	{
	case WM_IME_COMPOSITION:
		if (m_bFocusOnNickName)
		{
			m_hIMC = ImmGetContext(hWnd);	// ime�ڵ��� ��°�

			if (lparam & GCS_RESULTSTR)
			{
				if ((len = ImmGetCompositionString(m_hIMC, GCS_RESULTSTR, NULL, 0)) > 0)
				{
					// �ϼ��� ���ڰ� �ִ�.
					ImmGetCompositionString(m_hIMC, GCS_RESULTSTR, Cstr, len);
					Cstr[len] = 0;
					mNicknameString += Cstr;
				}

			}
			else if (lparam & GCS_COMPSTR)
			{
				// ���� ���ڸ� ���� ���̴�.

				// �������� ���̸� ��´�.
				len = ImmGetCompositionString(m_hIMC, GCS_COMPSTR, NULL, 0);
				// str��  �������� ���ڸ� ��´�.
				ImmGetCompositionString(m_hIMC, GCS_COMPSTR, Cstr, len);
				Cstr[len] = 0;
			}

			ImmReleaseContext(hWnd, m_hIMC);	// IME �ڵ� ��ȯ!!
		}
		return 0;
	case WM_CHAR:				// 1byte ���� (ex : ����)
		
		if (m_bFocusOnIP) 
		{
			if ((char)wparam == '\b')
			{
				if(mIpString.size() > 0)
				mIpString.pop_back();
			}
			else
			{
				mIpString += (wchar_t)wparam;
			}
		}
		if (m_bFocusOnPort)
		{
			if ((char)wparam == '\b')
			{
				if (mPortString.size() > 0)
				mPortString.pop_back();
			}
			else
			{
				mPortString += (wchar_t)wparam;
			}
		}
		if (m_bFocusOnNickName)
		{
			if ((char)wparam == '\b')
			{
				if (mNicknameString.size() > 0)
				mNicknameString.pop_back();
			}
			else
			{
			}
		}
		return 0;
	case WM_IME_NOTIFY:			// �����Է�...
		return 0;
	case WM_KEYDOWN:			// Ű�ٿ�..
		return 0;
	}
	return 1;
}
