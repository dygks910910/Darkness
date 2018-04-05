#include "MainScene.h"

namespace
{
	CMainScene* scene = 0;
}

BOOL CALLBACK
InputNickDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return scene->MyDlgProcForInputNickname(hwnd, msg, wParam, lParam);
}


CMainScene::CMainScene()
{
}


CMainScene::~CMainScene()
{
	//////////////////////////////////////////////////////////////////////////
	//사용한 비트맵 메모리 반환.
	mMainLogo.Shutdown();
	mBackground.Shutdown();

}


std::string CMainScene::UpdateScene(const float dt, MSG& msg)
{
	//////////////////////////////////////////////////////////////////////////
	//로고를 비추는 창을 켜주는 시간을 계산.
	static float calculateTime = 0;

	calculateTime += dt;

	if (calculateTime > LOGO_TIME_SEC)
	{
		isLogoTime = false;
		calculateTime = 0;
	}
	else
	{
		if (playButton.isClicked)
		{
			//DialogBox(mhAppInst, MAKEINTRESOURCE(IDD_DIALOG_STARTSETTING), mhMainWnd, (DLGPROC)MainDialogProc);
			playButton.isClicked = false;
			DialogBox(mhAppInst, MAKEINTRESOURCE(IDD_DIALOG_INPUT_NICKNAME), mhMainWnd, (DLGPROC)InputNickDialogProc);
			return "";
		}
		else if (mExitButton.isClicked)
		{
			PostMessage(mhMainWnd, WM_DESTROY, 0, 0);
		}

	}
	return "";
}

void CMainScene::OnMouseUp(WPARAM btnState, int x, int y)
{
	playButton.OnMouseUp(x, y);
	mExitButton.OnMouseUp(x, y);
}

void CMainScene::OnMouseMove(WPARAM btnState, int x, int y)
{
	playButton.OnMouseMove(x, y);
	mExitButton.OnMouseMove(x, y);

}

bool CMainScene::Init()
{
	mCam.SetPosition(0, 0, 0.1f);
	XMStoreFloat4x4(&mWorldMtx, XMMatrixTranslation(0, 0, 0.1f));
	//bitmap
	mMainLogo.Initialize(L"UITextures/DarknessLogo.PNG", mClientWidth, mClientHeight);
	mBackground.Initialize(L"UITextures/backGroundClown.PNG", mClientWidth, mClientHeight);
	
	//button
	playButton.Init(594, 116, L"UITextures/connect.png", mClientWidth / 1.5f, mClientHeight / 1.5f);
	mExitButton.Init(368, 116, L"UITextures/exit.png", mClientWidth / 1.5f, mClientHeight / 1.5f + playButton.GetBitmapHeight());

	//OnResize();
	return true;
}

void CMainScene::OnKeyboardButtonDown(WPARAM wParam, UINT msg)
{
	switch (msg)
	{
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

	if (isLogoTime)
	{
		mMainLogo.Render(0, 0);
	}
	else 
	{
		mBackground.Render(0, 0);
		playButton.Draw();
		mExitButton.Draw();
	}
	//////////////////////////////////////////////////////////////////////////

	// restore default states.

	HR(mSwapChain->Present(0, 0));
}

void CMainScene::OnMouseDown(WPARAM btnState, int x, int y)
{
	playButton.OnMouseDown(x, y);
	mExitButton.OnMouseDown(x, y);

}

BOOL CMainScene::MyDlgProcForInputNickname(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	wchar_t str[20];
	switch (uMsg) {
	case WM_INITDIALOG:
		return TRUE;
	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
			GetDlgItemText(hDlg, IDC_EDIT_INPUT_NICKNAME, str, 20);
			EndDialog(hDlg, 0);
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg, 0);
			return TRUE;
		}
		break;

	}
	return FALSE;
}
