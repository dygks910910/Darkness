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
	/*ReleaseCOM(mDepthDisableState);
	ReleaseCOM(mDepthStencilState);*/

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

	mCam.SetPosition(0, 0, -10);
	XMStoreFloat4x4(&mWorldMtx, XMMatrixTranslation(0, 0, 7));


	//////////////////////////////////////////////////////////////////////////
	//방화면 초기화.
	mBackgroundPicture.Initialize(device, mClientWidth, mClientHeight, L"UITextures/RoomScene2.png", mClientWidth, mClientHeight);

	mKingLogo.Initialize(device, mClientWidth, mClientHeight, L"UITextures/CreaterMark.png", KINGLOGO_SIZE_X, KINGLOGO_SIZE_Y);


	mStartButton.Init(device, BUTTON_SIZE_X, BUTTON_SIZE_Y, L"UITextures/Start.png", mClientWidth, mClientHeight);

	

	m_bStartCheck = false;
	
	CreateZbufferState(device);

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
	mCam.UpdateViewMatrix();
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
		//dc->OMSetDepthStencilState(mDepthDisableState, 1);
		bool result;

		// center Sky about eye in world space
		XMFLOAT3 eyePos = mCam.GetPosition();
		XMMATRIX world = XMLoadFloat4x4(&mWorldMtx);
		XMMATRIX WVP = XMMatrixMultiply(world, world*mCam.View()*mCam.othMtx());

		Effects::TextureFX->SetWorldViewProj(WVP);
		Effects::TextureFX->SetDiffuseMap(mBackgroundPicture.GetTexture());
		dc->RSSetState(0);
		//////////////////////////////////////////////////////////////////////////
		//기본메인화면.
		TurnZBuffOff(dc);
		dc->IASetInputLayout(InputLayouts::PosTex);
		mStartButton.Draw(dc, mClientWidth *0.6f, mClientHeight * 0.7f);

		//화살표 버튼

		mBackgroundPicture.Render(dc, 0, 0, true);


		//방장 왕관표시
		mKingLogo.Render(dc, KINGLOGO_LOCATION_X, KINGLOGO_LOCATION_Y, false);
		TurnZBuffOn(dc);

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
