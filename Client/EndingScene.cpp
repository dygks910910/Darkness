#include "EndingScene.h"


CEndingScene::CEndingScene()
{
}


CEndingScene::~CEndingScene()
{
	//////////////////////////////////////////////////////////////////////////
	//사용한 비트맵 메모리 반환.
	
	ReleaseCOM(mDepthDisableState);
	ReleaseCOM(mDepthStencilState);
	mResultBoard.Shutdown();
}

bool CEndingScene::Init(ID3D11Device * device, ID3D11DeviceContext * dc,
	IDXGISwapChain * swapChain, const D3D11_VIEWPORT & viewPort, 
	const int & clientWidth, const int & clientHeight)
{
	mClientWidth = clientWidth;
	mClientHeight = clientHeight;

	mCam.SetPosition(0, 0, -1);
	XMStoreFloat4x4(&mWorldMtx, XMMatrixTranslation(0, 0, 7));

	//////////////////////////////////////////////////////////////////////////
	//로고화면 초기화.
	mResultBoard.Initialize(device, mClientWidth, mClientHeight, TEXT("UITextures/gameResultBoard.png"), mClientWidth, mClientHeight);
	mHomeButton.Init(device, 100, 100, TEXT("UITextures/HomeButton.png"), mClientWidth, mClientHeight);
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
	//////////////////////////////////////////////////////////////////////////스코어 초기화.
	mScores.resize(NetworkMgr::GetInstance()->getGameResult().max_client);
	for (int i = 0; i < NetworkMgr::GetInstance()->getGameResult().max_client; ++i) {
		mScores[i].nickname = NetworkMgr::GetInstance()->getGameResult().game_result[i].nickName;
		mScores[i].monsterKill = NetworkMgr::GetInstance()->getGameResult().game_result[i].NPCKill;
		mScores[i].playerKill = NetworkMgr::GetInstance()->getGameResult().game_result[i].playerKill;
	}
	/*mScores.resize(3);
	for (int i = 0; i < 3; ++i) {
		mScores[i].nickname = L"123";
		mScores[i].monsterKill =123;
		mScores[i].playerKill = 123;
	}*/
	sort(mScores.begin(), mScores.end(), [](const playerScore& score1, const playerScore& score2) {
		if (score1.playerKill > score2.playerKill)
		{
			return true;
		}
		return false;
	});
	for (int i = 0; i < mScores.size(); ++i)
	{
		if (i == 0)
		{
			mScores[i].fontColor = FontColorForFW::GOLD;
		}
		else if (i == 1)
		{
			mScores[i].fontColor = FontColorForFW::SILVER;
		}
		else if (i == 2)
		{
			mScores[i].fontColor = FontColorForFW::BRONZE;
		}
		else
			mScores[i].fontColor = FontColorForFW::BRONZE;
		
		mScores[i].NIckposX = OUTPUT_NICKNAME_LOCATION_X;
		mScores[i].NickposY = OUTPUT_NICKNAME_LOCATION_Y+ i * OUTPUT_Y_OFFSET;

		mScores[i].PKposX = OUTPUT_PLAYER_KILL_LOCATION_X;
		mScores[i].PKposY = OUTPUT_NICKNAME_LOCATION_Y + i * OUTPUT_Y_OFFSET;

		mScores[i].MKposX = OUTPUT_MONSTER_KILL_LOCATION_X;
		mScores[i].MKkposY = OUTPUT_NICKNAME_LOCATION_Y + i * OUTPUT_Y_OFFSET;
	}
	//함수객체 초기화
	mDrawText.Init(device, dc);
	NetworkMgr::GetInstance()->Release();
	OnResize();
	//CreateZbufferState(device);
	return true;
}

std::string CEndingScene::UpdateScene(const float dt, MSG& msg)
{
	//////////////////////////////////////////////////////////////////////////
	//로고를 비추는 창을 켜주는 시간을 계산.
	if (mHomeButton.isClicked)
	{
		PlaySound(NULL, 0, 0);
		return SceneName::mainScene;
	}
	mCam.UpdateViewMatrix();
	return "";
}

void CEndingScene::Draw(ID3D11Device* device, ID3D11DeviceContext* dc,
	IDXGISwapChain* swapChain, ID3D11RenderTargetView* rtv,
	ID3D11DepthStencilView* dsv, D3D11_VIEWPORT* viewPort)
{
	dc->ClearRenderTargetView(rtv, reinterpret_cast<const float*>(&Colors::Black));
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->IASetInputLayout(InputLayouts::PosTex);
	//ZbufferOff();
	//dc->OMSetDepthStencilState(mDepthDisableState, 1);

	// center Sky about eye in world space
	XMFLOAT3 eyePos = mCam.GetPosition();
	XMMATRIX world = XMLoadFloat4x4(&mWorldMtx);
	XMMATRIX WVP = XMMatrixMultiply(world, world*mCam.View()*mCam.othMtx());

	Effects::TextureFX->SetWorldViewProj(WVP);
	TurnZBuffOff(dc);
	mHomeButton.Draw(dc, 0, 0);
	/*mHomeButton.Draw(dc, mClientWidth*0.3f, mClientHeight * 0.2f );
	mHomeButton.Draw(dc, mClientWidth * 0.5f, mClientHeight * 0.2f );
	mHomeButton.Draw(dc, mClientWidth * 0.7f, mClientHeight * 0.2f);*/

	mResultBoard.Render(dc, 0, 0, true);

	DrawAllScore();

// restore default states.
	//dc->RSSetState(0);
	HR(swapChain->Present(0, 0));
}

void CEndingScene::OnMouseDown(WPARAM btnState, int x, int y, const HWND & mhMainWnd)
{
	mHomeButton.OnMouseDown(x, y);
}

void CEndingScene::OnMouseUp(WPARAM btnState, int x, int y)
{
	mHomeButton.OnMouseUp(x, y);
}

void CEndingScene::OnMouseMove(WPARAM btnState, int x, int y)
{
	mHomeButton.OnMouseMove(x, y);
}

void CEndingScene::OnResize()
{
	mCam.SetLens(0.34f*MathHelper::Pi, AspectRatio(), 0, 3000.0f);
}

void CEndingScene::OnKeyboardButtonDown(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	
}

int CEndingScene::GetText(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	return 0;
}

void CEndingScene::DrawAllScore()
{
	std::wstring temp;
	wchar_t tempbuff[100];
	for (int i = 0; i < mScores.size(); ++i) {
		mDrawText(mScores[i].nickname, 50, mClientWidth * 0.3f, mClientHeight * 0.2f + i*OUTPUT_Y_OFFSET,mScores[i].fontColor);
		
		wsprintf(tempbuff, TEXT("%d"), mScores[i].playerKill);
		temp = tempbuff;
		mDrawText(temp, 50, mClientWidth * 0.5f, mClientHeight * 0.2f + i*OUTPUT_Y_OFFSET, mScores[i].fontColor);
		
		wsprintf(tempbuff, TEXT("%d"), mScores[i].monsterKill);
		temp = tempbuff;
		mDrawText(temp, 50, mClientWidth * 0.7f, mClientHeight * 0.2f + i*OUTPUT_Y_OFFSET, mScores[i].fontColor);
	}

}
