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
	mMainLogo.Shutdown();
	ReleaseCOM(mDepthDisableState);
	ReleaseCOM(mDepthStencilState);

}

bool CRoomScene::Init(ID3D11Device * device, ID3D11DeviceContext * dc,
	IDXGISwapChain * swapChain, const D3D11_VIEWPORT & viewPort,
	const int & clientWidth, const int & clientHeight)
{

	send_wsa_buf1.buf = reinterpret_cast<char*>(send_buf1);
	send_wsa_buf1.len = MAX_BUF_SIZE;

	mDrawText.Init(device, dc);


	mClientWidth = clientWidth;
	mClientHeight = clientHeight;

	mCam.SetPosition(0, 0, 0);
	XMStoreFloat4x4(&mWorldMtx, XMMatrixTranslation(0, 0, 7));


	//////////////////////////////////////////////////////////////////////////
	//방화면 초기화.
	mBackgroundPicture.Initialize(device, mClientWidth, mClientHeight, L"UITextures/RoomScene2.png", mClientWidth, mClientHeight);

	mKingLogo.Initialize(device, mClientWidth, mClientHeight, L"UITextures/CreaterMark.png", KINGLOGO_SIZE_X, KINGLOGO_SIZE_Y);


	mStartButton.Init(device, BUTTON_SIZE_X, BUTTON_SIZE_Y, L"UITextures/Start.png", START_BUTTON_X, START_BUTTON_Y, mClientWidth, mClientHeight);

	

	m_bStartCheck = false;
	
	// Clear the second depth stencil state before setting the parameters.


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
	//함수객체 초기화
	DrawText.Init(device, dc);

	OnResize();
	return true;
}

std::string CRoomScene::UpdateScene(const float dt, MSG& msg)
{
	if (m_bStartCheck == true)
		return SceneName::gameScene;

	else if (CModelManager::GetInstance()->mIsStart == true)
	{
		CModelManager::GetInstance()->mIsStart = false;
		return SceneName::gameScene;
	}

	return "";
}

void CRoomScene::Draw(ID3D11Device* device, ID3D11DeviceContext* dc,
	IDXGISwapChain* swapChain, ID3D11RenderTargetView* rtv,
	ID3D11DepthStencilView* dsv, D3D11_VIEWPORT* viewPort)
{
	if (CModelManager::GetInstance()->mCheck)
	{
		dc->ClearRenderTargetView(rtv, reinterpret_cast<const float*>(&Colors::Silver));
		dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		dc->IASetInputLayout(InputLayouts::PosTex);
		dc->OMSetDepthStencilState(mDepthDisableState, 1);
		bool result;

		// center Sky about eye in world space
		XMFLOAT3 eyePos = mCam.GetPosition();
		XMMATRIX world = XMLoadFloat4x4(&mWorldMtx);
		XMMATRIX WVP = XMMatrixMultiply(world, world*mCam.Proj()*mCam.othMtx());

		Effects::TextureFX->SetWorldViewProj(WVP);
		Effects::TextureFX->SetDiffuseMap(mBackgroundPicture.GetTexture());
		dc->RSSetState(0);
		//////////////////////////////////////////////////////////////////////////
		//기본메인화면.
		mBackgroundPicture.Render(dc, 0, 0);

		for (int i = 0; i < 8; ++i)
		{
			if (!CModelManager::GetInstance()->mMyNick[i].empty() && NetworkMgr::GetInstance()->mPlayerExist[i] == true)
			{
				if (i == 0)
					mDrawText(CModelManager::GetInstance()->mMyNick[i], 30, PLAYER1_NICKNAME_LOCATION_X, PLAYER1_NICKNAME_LOCATION_Y + OUTPUT_Y_OFFSET, FontColorForFW::GOLD);
				else if (i == 1)
					mDrawText(CModelManager::GetInstance()->mMyNick[i], 30, PLAYER2_NICKNAME_LOCATION_X, PLAYER2_NICKNAME_LOCATION_Y + OUTPUT_Y_OFFSET, FontColorForFW::GOLD);
				else if (i == 2)
					mDrawText(CModelManager::GetInstance()->mMyNick[i], 30, PLAYER3_NICKNAME_LOCATION_X, PLAYER3_NICKNAME_LOCATION_Y + OUTPUT_Y_OFFSET, FontColorForFW::GOLD);
				else if (i == 3)
					mDrawText(CModelManager::GetInstance()->mMyNick[i], 30, PLAYER4_NICKNAME_LOCATION_X, PLAYER4_NICKNAME_LOCATION_Y + OUTPUT_Y_OFFSET, FontColorForFW::GOLD);
				else if (i == 4)
					mDrawText(CModelManager::GetInstance()->mMyNick[i], 30, PLAYER5_NICKNAME_LOCATION_X, PLAYER5_NICKNAME_LOCATION_Y + OUTPUT_Y_OFFSET, FontColorForFW::GOLD);
				else if (i == 5)
					mDrawText(CModelManager::GetInstance()->mMyNick[i], 30, PLAYER6_NICKNAME_LOCATION_X, PLAYER6_NICKNAME_LOCATION_Y + OUTPUT_Y_OFFSET, FontColorForFW::GOLD);
				else if (i == 6)
					mDrawText(CModelManager::GetInstance()->mMyNick[i], 30, PLAYER7_NICKNAME_LOCATION_X, PLAYER7_NICKNAME_LOCATION_Y + OUTPUT_Y_OFFSET, FontColorForFW::GOLD);
				else if (i == 7)
					mDrawText(CModelManager::GetInstance()->mMyNick[i], 30, PLAYER8_NICKNAME_LOCATION_X, PLAYER8_NICKNAME_LOCATION_Y + OUTPUT_Y_OFFSET, FontColorForFW::GOLD);
			}
		}
		if (NetworkMgr::GetInstance()->getId() == 0)
			mDrawText(NetworkMgr::GetInstance()->mNickName, 30, PLAYER1_NICKNAME_LOCATION_X, PLAYER1_NICKNAME_LOCATION_Y + OUTPUT_Y_OFFSET, FontColorForFW::GOLD);
		else if (NetworkMgr::GetInstance()->getId() == 1)
			mDrawText(NetworkMgr::GetInstance()->mNickName, 30, PLAYER2_NICKNAME_LOCATION_X, PLAYER2_NICKNAME_LOCATION_Y + OUTPUT_Y_OFFSET, FontColorForFW::GOLD);
		else if (NetworkMgr::GetInstance()->getId() == 2)
			mDrawText(NetworkMgr::GetInstance()->mNickName, 30, PLAYER3_NICKNAME_LOCATION_X, PLAYER3_NICKNAME_LOCATION_Y + OUTPUT_Y_OFFSET, FontColorForFW::GOLD);
		else if (NetworkMgr::GetInstance()->getId() == 3)
			mDrawText(NetworkMgr::GetInstance()->mNickName, 30, PLAYER4_NICKNAME_LOCATION_X, PLAYER4_NICKNAME_LOCATION_Y + OUTPUT_Y_OFFSET, FontColorForFW::GOLD);
		else if (NetworkMgr::GetInstance()->getId() == 4)
			mDrawText(NetworkMgr::GetInstance()->mNickName, 30, PLAYER5_NICKNAME_LOCATION_X, PLAYER5_NICKNAME_LOCATION_Y + OUTPUT_Y_OFFSET, FontColorForFW::GOLD);
		else if (NetworkMgr::GetInstance()->getId() == 5)
			mDrawText(NetworkMgr::GetInstance()->mNickName, 30, PLAYER6_NICKNAME_LOCATION_X, PLAYER6_NICKNAME_LOCATION_Y + OUTPUT_Y_OFFSET, FontColorForFW::GOLD);
		else if (NetworkMgr::GetInstance()->getId() == 6)
			mDrawText(NetworkMgr::GetInstance()->mNickName, 30, PLAYER7_NICKNAME_LOCATION_X, PLAYER7_NICKNAME_LOCATION_Y + OUTPUT_Y_OFFSET, FontColorForFW::GOLD);
		else if (NetworkMgr::GetInstance()->getId() == 7)
			mDrawText(NetworkMgr::GetInstance()->mNickName, 30, PLAYER8_NICKNAME_LOCATION_X, PLAYER8_NICKNAME_LOCATION_Y + OUTPUT_Y_OFFSET, FontColorForFW::GOLD);

		//스타트 버튼
		mStartButton.Draw(dc);

		//화살표 버튼
		


		//방장 왕관표시
		mKingLogo.Render(dc, KINGLOGO_LOCATION_X, KINGLOGO_LOCATION_Y);

		
		dc->RSSetState(0);
		dc->OMSetDepthStencilState(0, 0);

		HR(swapChain->Present(0, 0));
	}
}

void CRoomScene::OnMouseDown(WPARAM btnState, int x, int y, const HWND & mhMainWnd)
{
	if (CModelManager::GetInstance()->mMyId == 0)
	{
		mStartButton.OnMouseDown(x, y);
		if (x > START_BUTTON_X && x < START_BUTTON_X + BUTTON_SIZE_X && y > START_BUTTON_Y && y < START_BUTTON_Y + BUTTON_SIZE_Y)
		{
			cs_packet_start_button_click* start = reinterpret_cast<cs_packet_start_button_click*>(&send_buf1);
			start->size = sizeof(cs_packet_start_button_click);
			start->type = CS_PACKET_CREATER_START_CLICK;
			send_wsa_buf1.len = sizeof(cs_packet_start_button_click);
			DWORD io_byte2;

			int ret_val = WSASend(NetworkMgr::GetInstance()->GetSock(), &send_wsa_buf1, 1, &io_byte2, 0, NULL, NULL);
			if (ret_val == SOCKET_ERROR)
				std::cout << " [error] WSASend() " << std::endl;

		}
	}

	

}

void CRoomScene::OnMouseUp(WPARAM btnState, int x, int y)
{
	if (CModelManager::GetInstance()->mMyId == 0)
	{
		mStartButton.OnMouseUp(x, y);
		
	}

}

void CRoomScene::OnMouseMove(WPARAM btnState, int x, int y)
{
	if (CModelManager::GetInstance()->mMyId == 0)
	{
		mStartButton.OnMouseMove(x, y);
		
	}
}
void CRoomScene::OnResize()
{
	mCam.SetLens(0.34f*MathHelper::Pi, AspectRatio(), 0, 3000.0f);
}
void CRoomScene::OnKeyboardButtonDown(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	
}
