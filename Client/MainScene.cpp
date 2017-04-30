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
	mConnectButton.Init(device,100, 33, L"UITextures/connect.png",1000, 450, mClientWidth, mClientHeight);
	mExitButton.Init(device, 100, 33, L"UITextures/exit.png",1000,500, mClientWidth, mClientHeight);
	//////////////////////////////////////////////////////////////////////////
	//inputboard 초기화.
	mInputBoard.Initialize(device, mClientWidth, mClientHeight, L"UITextures/InputBoard.png", 800, 600);
	mInputIP.Initialize(device, mClientWidth, mClientHeight, L"UITextures/InputIP.png", 300, 50);
	mInputPort.Initialize(device, mClientWidth, mClientHeight, L"UITextures/InputPort.png", 300, 50);
	mInputNickname.Initialize(device, mClientWidth, mClientHeight, L"UITextures/InputNickName.png", 300, 50);
	mReturnButton.Init(device, 200, 100, L"UITextures/Lobby나가기.png", 900, 650, mClientWidth, mClientHeight);
	mLobbyConnectButton.Init(device, 200, 100, L"UITextures/Lobby접속.png", 550, 650, mClientWidth, mClientHeight);

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
	mIpString = L"기본";
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
	drawText(mDevice, mDc,mIpString);
	//////////////////////////////////////////////////////////////////////////
	//IP주소 입력창 화면
	if (bActivedInputBoard)
	{
		mInputBoard.Render(mDc, 400, 200);
		mInputIP.Render(mDc, 650, 450);
		mInputPort.Render(mDc, 650, 500);
		mInputNickname.Render(mDc, 650, 550);
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
		Pick(x, y, mConnectButton);
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
	std::cout << "mouse pos :" << "(" << x << "," << y << ")" << std::endl;
	std::cout << "aspectRatio x : " << x * AspectRatio() << std::endl;
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
					wsprintf(szTemp, L"완성된 글자 : %s\r\n", Text);
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
				wsprintf(szTemp, L"조합중인 글자 : %s\r\n", Cstr);
				mIpString += szTemp;
				//std::wcout << szTemp;
				//OutputDebugString(_T(szTemp));
			}
		}

		ImmReleaseContext(hWnd, m_hIMC);	// IME 핸들 반환!!
		return 0;

	case WM_CHAR:				// 1byte 문자 (ex : 영어)
		return 0;
	case WM_IME_NOTIFY:			// 한자입력...
		return 0;
	case WM_KEYDOWN:			// 키다운..
		return 0;
	}
	return 1;
}

void CMainScene::Pick(const int & sx, const int & sy, CButtonClass& button)
{
	XMMATRIX P = mCam.Proj();
	// Compute picking ray in view space.
	float vx = (+2.0f*sx / mClientWidth - 1.0f) / P(0, 0);
	float vy = (-2.0f*sy / mClientHeight + 1.0f) / P(1, 1);

	// Ray definition in view space.
	XMVECTOR rayOrigin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR rayDir = XMVectorSet(vx, vy, 1.0f, 0.0f);

	// Tranform ray to local space of Mesh.
	XMMATRIX V = mCam.View();
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(V), V);

	XMMATRIX W = XMLoadFloat4x4(&button.mWorld);
	XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(W), W);

	XMMATRIX toLocal = XMMatrixMultiply(invView, invWorld);

	rayOrigin = XMVector3TransformCoord(rayOrigin, toLocal);
	rayDir = XMVector3TransformNormal(rayDir, toLocal);

	// Make the ray direction unit length for the intersection tests.
	rayDir = XMVector3Normalize(rayDir);

	// If we hit the bounding box of the Mesh, then we might have picked a Mesh triangle,
	// so do the ray/triangle tests.
	//
	// If we did not hit the bounding box, then it is impossible that we hit 
	// the Mesh, so do not waste effort doing ray/triangle tests.

	// Assume we have not picked anything yet, so init to -1.
	//mPickedTriangle = -1;
	float tmin = 0.0f;
	XNA::AxisAlignedBox tempBox = button.GetBox();
	if (XNA::IntersectRayAxisAlignedBox(rayOrigin, rayDir, &tempBox, &tmin))
	{
		std::cout << "버튼 클릭됨";
			//// Find the nearest ray/triangle intersection.
			//tmin = MathHelper::Infinity;
			//for (UINT i = 0; i < mMeshIndices.size() / 3; ++i)
			//{
			//	// Indices for this triangle.
			//	UINT i0 = mMeshIndices[i * 3 + 0];
			//	UINT i1 = mMeshIndices[i * 3 + 1];
			//	UINT i2 = mMeshIndices[i * 3 + 2];

			//	// Vertices for this triangle.
			//	XMVECTOR v0 = XMLoadFloat3(&mMeshVertices[i0].Pos);
			//	XMVECTOR v1 = XMLoadFloat3(&mMeshVertices[i1].Pos);
			//	XMVECTOR v2 = XMLoadFloat3(&mMeshVertices[i2].Pos);

			//	// We have to iterate over all the triangles in order to find the nearest intersection.
			//	float t = 0.0f;
			//	if (XNA::IntersectRayTriangle(rayOrigin, rayDir, v0, v1, v2, &t))
			//	{
			//		if (t < tmin)
			//		{
			//			// This is the new nearest picked triangle.
			//			tmin = t;
			//			mPickedTriangle = i;
			//		}
			//	}
			//}
	};
}

void CMainScene::drawText(ID3D11Device *pDevice, ID3D11DeviceContext *pContext, std::wstring text)
{
	IFW1Factory *pFW1Factory;
	HRESULT hResult = FW1CreateFactory(FW1_VERSION, &pFW1Factory);

	IFW1FontWrapper *pFontWrapper;
	hResult = pFW1Factory->CreateFontWrapper(pDevice, L"Arial", &pFontWrapper);

	pFontWrapper->DrawString(
		pContext,
		text.c_str(),// String
		20.0f,// Font size
		100.0f,// X position
		50.0f,// Y position
		0xff0099ff,// Text color, 0xAaBbGgRr
		FW1_RESTORESTATE// Flags (for example FW1_RESTORESTATE to keep context states unchanged)
	);

	pFontWrapper->Release();
	pFW1Factory->Release();
}
