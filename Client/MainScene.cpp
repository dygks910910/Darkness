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
	mBackgroundPicture.Initialize(device, mClientWidth, mClientHeight, L"UITextures/testBack.jpg", mClientWidth, mClientHeight);
	mLogo.Initialize(device, mClientWidth/2, mClientHeight/1.5f, L"UITextures/Logo.png", 400, 100);
	mConnectButton.Init(device,100, 33, L"UITextures/connect.png", CONNECT_BUTTON_X, CONNECT_BUTTON_Y, mClientWidth, mClientHeight);
	mExitButton.Init(device, 100, 33, L"UITextures/exit.png", CONNECT_BUTTON_X, EXIT_BUTTON_Y, mClientWidth, mClientHeight);
	//////////////////////////////////////////////////////////////////////////
	//inputboard 초기화.
	mInputBoard.Initialize(device, mClientWidth, mClientHeight, L"UITextures/InputBoard.png", 800, 600);
	mInputIP.Initialize(device, mClientWidth, mClientHeight, L"UITextures/InputIP.png", 300, 50);
	mInputPort.Initialize(device, mClientWidth, mClientHeight, L"UITextures/InputPort.png", 300, 50);
	mInputNickname.Initialize(device, mClientWidth, mClientHeight, L"UITextures/InputNickName.png", 300, 50);
	mReturnButton.Init(device, 200, 100, L"UITextures/cancel.png", RETURN_BUTTON_X, RETURN_BUTTON_Y, mClientWidth, mClientHeight);
	mLobbyConnectButton.Init(device, 200, 100, L"UITextures/connect.png", LOBBY_CONNECT_BUTTON_X, LOBBY_CONNECT_BUTTON_Y, mClientWidth, mClientHeight);

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
	m_bFocusOnIP = false;
	m_bFocusOnNickName = false;
	m_bFocusOnPort = false;

	//mMinimap.Initialize(device, 800, 600, mCam.View(), 1000, 1000);
	mIpString = L"";
	ZeroMemory(&Text, sizeof(Text));
	ZeroMemory(&Cstr, sizeof(Cstr));

	OnResize();
	return true;
}

void CMainScene::UpdateScene(const float & dt)
{
// 	char a = getKey();
// 	if (a != '\0')
// 	{
// 		mIpString += ConverCtoWC(a);
// 	}

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
			m_bFocusOnIP = false;
			m_bFocusOnPort = false;
			m_bFocusOnNickName = false;
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
// 			~CMainScene();
// 			exit(1);
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
		mInputBoard.Render(mDc, INPUT_BOARD_X, INPUT_BOARD_Y);
		mInputIP.Render(mDc, INPUT_IP_X, INPUT_IP_Y);
		DrawText(mDevice, mDc, mIpString, FONT_SIZE, INPUT_IP_X+100, INPUT_IP_Y+10);

		mInputPort.Render(mDc, INPUT_PORT_X, INPUT_PORT_Y);
		DrawText(mDevice, mDc, mPortString, FONT_SIZE, INPUT_PORT_X + 100, INPUT_PORT_Y+10);

		mInputNickname.Render(mDc, INPUT_NICKNAME_X, INPUT_NICKNAME_Y);
		DrawText(mDevice, mDc, mNicknameString, FONT_SIZE, INPUT_NICKNAME_X + 100, INPUT_NICKNAME_Y+10);

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
		//ip입력창 active state check
  		mLobbyConnectButton.OnMouseDown(x, y);
  		mReturnButton.OnMouseDown(x, y);
		if (x > 650 && x < 950 && y > 450 && y < 500)
		{
			//std::cout << "m_bFocusOnIP = true" <<std::endl;
			m_bFocusOnIP = true;
			m_bFocusOnNickName = false;
			m_bFocusOnPort = false;
		}
		else if (x > 650 && x < 950 && y > 500 && y < 550)
		{
			//std::cout << "m_bFocusOnPort = true" << std::endl;
			m_bFocusOnIP = false;
			m_bFocusOnNickName = false;
			m_bFocusOnPort = true;
		}
		else if (x > 650 && x < 950 && y > 550 && y < 600)
		{
			//std::cout << "m_bFocusOnNickname = true" << std::endl;
			m_bFocusOnIP = false;
			m_bFocusOnNickName = true;
			m_bFocusOnPort = false;
		}
  	}
  	else
  	{
		//Pick(x, y, mConnectButton);
//   		mConnectButton.OnMouseDown(x, y);
//   		mExitButton.OnMouseDown(x, y);
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
// 	std::cout << "mouse pos :" << "(" << x << "," << y << ")" << std::endl;
// 	std::cout << "aspectRatio x : " << x * AspectRatio() << std::endl;
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
	mCam.SetLens(0.36f*MathHelper::Pi, AspectRatio(), 0, 3000.0f);
	//std::cout << AspectRatio() << std::endl;

}

void CMainScene::OnKeyboardButtonDown(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (GetText(hWnd, msg, wparam, lparam) == 0)
	{
		return;
	}
// 	if (focusOnInIP)
// 	{
// 
// 	}
// 	else if (fouseOnInPort)
// 	{
// 
// 	}
// 	else if (focusOnInNickName)
// 	{
// 
// 	}
}

int CMainScene::GetText(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	int len;
	HIMC m_hIMC = NULL;   // IME 핸들

	switch (msg)
	{
	case WM_IME_COMPOSITION:
		if (m_bFocusOnNickName)
		{
			m_hIMC = ImmGetContext(hWnd);	// ime핸들을 얻는것

			if (lparam & GCS_RESULTSTR)
			{
				if ((len = ImmGetCompositionString(m_hIMC, GCS_RESULTSTR, NULL, 0)) > 0)
				{
					// 완성된 글자가 있다.
					ImmGetCompositionString(m_hIMC, GCS_RESULTSTR, Cstr, len);
					Cstr[len] = 0;
					wcscpy(Text + wcslen(Text), Cstr);
					memset(Cstr, 0, 10);
					{
						WCHAR szTemp[256] = _T("");
						wsprintf(szTemp, L"%s", Text);
						mIpString = szTemp;
						//std::wcout << szTemp;
						//OutputDebugString(_T(szTemp));
					}
				}

			}
			else if (lparam & GCS_COMPSTR)
			{
				// 현재 글자를 조합 중이다.

				// 조합중인 길이를 얻는다.
				// str에  조합중인 문자를 얻는다.
				len = ImmGetCompositionString(m_hIMC, GCS_COMPSTR, NULL, 0);
				ImmGetCompositionString(m_hIMC, GCS_COMPSTR, Cstr, len);
				Cstr[len] = 0;
				{
					wchar_t szTemp[256] = L"";
					wsprintf(szTemp, L"%s", Cstr);
					//mIpString += szTemp;
					//std::wcout << szTemp;
					//OutputDebugString(_T(szTemp));
				}
			}

			ImmReleaseContext(hWnd, m_hIMC);	// IME 핸들 반환!!
		}
		return 0;
	case WM_CHAR:				// 1byte 문자 (ex : 영어)
		
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
				mNicknameString += (wchar_t)wparam;
			}
		}
		/*Text[wcslen(Text)] = (wchar_t)wparam;*/
		//mIpString += (wchar_t)wparam;
		return 0;
	case WM_IME_NOTIFY:			// 한자입력...
		return 0;
	case WM_KEYDOWN:			// 키다운..
		return 0;
	}
	return 1;
}
