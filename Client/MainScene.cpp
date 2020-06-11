#include "MainScene.h"

CMainScene::CMainScene()
{
}


CMainScene::~CMainScene()
{
	//////////////////////////////////////////////////////////////////////////
	//����� ��Ʈ�� �޸� ��ȯ.
	mBackgroundPicture.Shutdown();
	mInputBoard.Shutdown();
	mInputNickname.Shutdown();
	mMainLogo.Shutdown();
	mLogo.Shutdown();
}

bool CMainScene::Init(ID3D11Device * device, ID3D11DeviceContext * dc,
	IDXGISwapChain * swapChain, const D3D11_VIEWPORT & viewPort, 
	const int & clientWidth, const int & clientHeight)
{
	mClientWidth = clientWidth;
	mClientHeight = clientHeight;
	mCam.SetPosition(0, 0, -10);
	mCam.UpdateViewMatrix();
	XMStoreFloat4x4(&mWorldMtx, XMMatrixTranslation(0, 0, 7));

	CModelManager::GetInstance()->m_bFinishInit = false;
	NetworkMgr::GetInstance()->mCheckPacket = false;
	NetworkMgr::GetInstance()->isGameOver = false;
	NetworkMgr::GetInstance()->isGameStart = false;


	//////////////////////////////////////////////////////////////////////////
	//�ΰ�ȭ�� �ʱ�ȭ.
	mMainLogo.Initialize(device, mClientWidth, mClientHeight, L"UITextures/DarknessLogo.PNG", 800, 600);
	//////////////////////////////////////////////////////////////////////////
	//����ȭ�� �ʱ�ȭ.
	mBackgroundPicture.Initialize(device, mClientWidth, mClientHeight, L"UITextures/backGroundClown.PNG", 1839,1014);
	mConnectButton.Init(device, BUTTON_SIZE_X, BUTTON_SIZE_Y, L"UITextures/connect.png",clientWidth,clientHeight);
	mExitButton.Init(device, BUTTON_SIZE_X, BUTTON_SIZE_Y, L"UITextures/exit.png", mClientWidth, mClientHeight);
	mNicknameButton.Init(device, 850, 235, L"UITextures/InputNickName.png", mClientWidth, mClientHeight, 0.3f, 0.3f, true);;
	//////////////////////////////////////////////////////////////////////////
	
	
//inputboard �ʱ�ȭ.
	mInputBoard.Initialize(device,mClientWidth, mClientHeight,
		L"UITextures/InputBoard.png",1023 ,1026);

	mInputNickname.Initialize(device, mClientWidth, mClientHeight,
		L"UITextures/InputNickName.png", 700, 150);
	
	mReturnButton.Init(device, RETURN_BUTTON_SIZE_X,
 RETURN_BUTTON_SIZE_Y,
		L"UITextures/X.png", mClientWidth,
 mClientHeight);

	mLobbyConnectButton.Init(device, LOGIN_BUTTON_SIZE_X,
 LOGIN_BUTTON_SIZE_Y,
		L"UITextures/Login.png", mClientWidth,
 mClientHeight);

 	//D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
 	//ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));
 
 	//// Now create a second depth stencil state which turns off the Z buffer for 2D rendering.  The only difference is 
 	//// that DepthEnable is set to false, all other parameters are the same as the other depth stencil state.
 	//depthDisabledStencilDesc.DepthEnable = false;
 	//depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
 	//depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
 	//depthDisabledStencilDesc.StencilEnable = true;
 	//depthDisabledStencilDesc.StencilReadMask = 0xFF;
 	//depthDisabledStencilDesc.StencilWriteMask = 0xFF;
 	//depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
 	//depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
 	//depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
 	//depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
 	//depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
 	//depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
 	//depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
 	//depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
 
 	//HR(device->CreateDepthStencilState(&depthDisabledStencilDesc, &mDepthDisableState));
 	//depthDisabledStencilDesc.DepthEnable = false;
 
 	//D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
 	//// Initialize the description of the stencil state.
 	//ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
 
 	//// Set up the description of the stencil state.
 	//depthStencilDesc.DepthEnable = true;
 	//depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
 	//depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
 
 	//depthStencilDesc.StencilEnable = true;
 	//depthStencilDesc.StencilReadMask = 0xFF;
 	//depthStencilDesc.StencilWriteMask = 0xFF;
 
 	//// Stencil operations if pixel is front-facing.
 	//depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
 	//depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
 	//depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
 	//depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
 
 	//// Stencil operations if pixel is back-facing.
 	//depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
 	//depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
 	//depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
 	//depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
 
 	//// Create the depth stencil state.
 	//HR(device->CreateDepthStencilState(&depthStencilDesc, &mDepthStencilState));
 	//dc->OMSetDepthStencilState(mDepthStencilState, 1);
	if (!CreateZbufferState(device))
	{
		return false;
	}
	if (!CreateAlphaBlendingState(device))
	{
		return false;
	}
	bActivedInputBoard = false;
	m_bFocusOnNickName = false;
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
			std::ifstream fin("ServerIpAddress.txt",std::ios::in);
			char addr[25];
			fin >> addr;

			fin.close();
			NetworkMgr::GetInstance()->SetIPAndPortAndNickName(addr,mNicknameString);
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
			m_bFocusOnNickName = false;
			//////////////////////////////////////////////////////////////////////////
			//����ȭ������ ���ƿð�� ����ư�� Isclicked �ʱ�ȭ�������.
		}
		if (mNicknameButton.isClicked)
		{
			m_bFocusOnNickName = true;
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
	//mCam.UpdateViewMatrix();

	return "";
}

void CMainScene::Draw(ID3D11Device* device, ID3D11DeviceContext* dc,
	IDXGISwapChain* swapChain, ID3D11RenderTargetView* rtv,
	ID3D11DepthStencilView* dsv, D3D11_VIEWPORT* viewPort)
{
	
	dc->ClearRenderTargetView(rtv, reinterpret_cast<const float*>(&Colors::White));
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH, 1.0f, 0);
	
	dc->IASetInputLayout(InputLayouts::PosTex);
	
	//dc->OMSetDepthStencilState(mDepthDisableState, 1);
	// center Sky about eye in world space
	XMFLOAT3 eyePos = mCam.GetPosition();
	XMMATRIX world = XMLoadFloat4x4(&mWorldMtx);
	XMMATRIX WVP = world * mCam.View() * mCam.othMtx();

	//XMMATRIX worldTp = XMMatrixTranspose(world);
	//XMMATRIX viewTp = XMMatrixTranspose(mCam.View());
	//XMMATRIX projTp = XMMatrixTranspose(mCam.othMtx());
	//XMMATRIX WVPTp = XMMatrixMultiply(worldTp, (worldTp * viewTp) * projTp);

	Effects::TextureFX->SetWorldViewProj(WVP);
	Effects::TextureFX->SetDiffuseMap(mBackgroundPicture.GetTexture());
	//dc->RSSetState(0);
	//dc->RSSetState(RenderStates::SolidRS);
	TurnZBuffOff(dc);
	TurnAlphaOn(dc);
	
	//////////////////////////////////////////////////////////////////////////
	/*if (m_bLogoTime)
	{
		mMainLogo.Render(dc, 0, 0, true);
	}*/
	//////////////////////////////////////////////////////////////////////////
	//�⺻����ȭ��.
	
	mBackgroundPicture.Render(dc, 0, 0, true);
	mExitButton.Draw(dc, mClientWidth * 0.8f, mClientHeight * 0.7f + mConnectButton.GetBitmapHeight());
	mConnectButton.Draw(dc, mClientWidth * 0.8f, mClientHeight * 0.7f);

	if (bActivedInputBoard)
	{
		int x, y, bmpwidth, bmpheight;
		mInputBoard.GetPos(x, y);
		mInputBoard.GetBmpWidth(bmpwidth);
		mInputBoard.GetBmpHeight(bmpheight);

		mInputBoard.Render(dc, mClientWidth * 0.3f, mClientHeight * 0.3f, false, 0.4f, 0.4f);
		mNicknameButton.Draw(dc, x, y + bmpheight / 2);
		mLobbyConnectButton.Draw(dc, x, y + bmpheight - mLobbyConnectButton.GetBitmapHeight());
		mReturnButton.Draw(dc, x + bmpwidth - mReturnButton.GetBitmapWidth(), y + bmpheight - mReturnButton.GetBitmapHeight());
		DrawText(mNicknameString, mNicknameButton.GetBitmapHeight() / 2,
			mNicknameButton.GetLocationX() + (x / 4), mNicknameButton.GetLocationY() + mNicknameButton.GetBitmapHeight() * 0.3f);

	}
	TurnAlphaOff(dc);
	TurnZBuffOn(dc);
	//////////////////////////////////////////////////////////////////////////
	


// restore default states.
	//dc->OMSetDepthStencilState(0, 0);

	HR(swapChain->Present(0, 0));
	//TurnZBuffOn(dc);

}

void CMainScene::OnMouseDown(WPARAM btnState, int x, int y, const HWND & mhMainWnd)
{
  	if (bActivedInputBoard)
  	{
		//ip�Է�â active state check
  		mLobbyConnectButton.OnMouseDown(x, y);
  		mReturnButton.OnMouseDown(x, y);
		
		if (x > INPUT_NICKNAME_X && x < INPUT_NICKNAME_X + INPUT_BAR_WIDTH &&
			y > INPUT_NICKNAME_Y && y < INPUT_NICKNAME_Y + INPUT_BAR_HEIGHT)
		{
			m_bFocusOnNickName = true;
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
	if (m_bFocusOnNickName) {
		if (GetText	(hWnd, msg, wparam, lparam) == 0)
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
 				//if ((len = ImmGetCompositionString(m_hIMC, GCS_RESULTSTR, NULL, 0)) > 0)
 				//{
 				//	// �ϼ��� ���ڰ� �ִ�.
 				//	ImmGetCompositionString(m_hIMC, GCS_RESULTSTR, Cstr, len);
 				//	Cstr[len] = 0;
 				//	mNicknameString += Cstr;
 				//}
 			}
 			else if (lparam & GCS_COMPSTR)
 			{
 				//// ���� ���ڸ� ���� ���̴�.
 				//// �������� ���̸� ��´�.
 				//len = ImmGetCompositionString(m_hIMC, GCS_COMPSTR, NULL, 0);
 				//// str��  �������� ���ڸ� ��´�.
 				//ImmGetCompositionString(m_hIMC, GCS_COMPSTR, Cstr, len);
 				//Cstr[len] = 0;
 			}
 
 			ImmReleaseContext(hWnd, m_hIMC);	// IME �ڵ� ��ȯ!!
 		}
 		return 0;
 	case WM_CHAR:				// 1byte ���� (ex : ����)
	{
 		if (wparam == '\b' && mNicknameString.size() > 0)
 		{
 			mNicknameString.pop_back();
 		}
 		else
 			mNicknameString += wparam;
	}
 		return 0;
 	case WM_IME_NOTIFY:			// �����Է�...
 		return 0;
 	case WM_KEYDOWN:			// Ű�ٿ�..
 		return 0;
 	}
	return 1;
}
