#include "LogoScene.h"



CLogoScene::CLogoScene()
{
}


CLogoScene::~CLogoScene()
{
	mBitmap.Shutdown();
	//ReleaseCOM()
}

bool CLogoScene::Init(ID3D11Device * device, ID3D11DeviceContext * dc, IDXGISwapChain * swapChain, const D3D11_VIEWPORT & viewPort, const int & clientWidth, const int & clientHeight)
{
	mDevice = device;
	mDc = dc;
	mSwapChain = swapChain;

	mCam.SetPosition(0, 0, 0);
	mWorldMtx = XMMatrixTranslation(0, 0, 10);
	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
	mBitmap.Initialize(device, 800, 600, L"DarknessLogo.PNG", 800, 600);
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

	//mMinimap.Initialize(device, 800, 600, mCam.View(), 1000, 1000);


	return true;
}

void CLogoScene::UpdateScene(const float & dt)
{
	mCam.UpdateViewMatrix();
}

void CLogoScene::Draw(ID3D11RenderTargetView * rtv, ID3D11DepthStencilView * dsv, D3D11_VIEWPORT * viewPort)
{
	mDc->ClearRenderTargetView(rtv, reinterpret_cast<const float*>(&Colors::Silver));
	mDc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	//zBuffer off
	mDc->IASetInputLayout(InputLayouts::Basic32);
	mDc->OMSetDepthStencilState(mDepthDisableState, 1);
	bool result;
	// center Sky about eye in world space
	XMFLOAT3 eyePos = mCam.GetPosition();

	XMMATRIX WVP = XMMatrixMultiply(XMMatrixTranslation(0,0,1), 
		XMMatrixTranslation(0, 0, 1)*mCam.Proj()*mCam.othMtx());
	Effects::BasicFX->SetWorldViewProj(WVP);
	Effects::BasicFX->SetDiffuseMap(mBitmap.GetTexture());
	Effects::BasicFX->SetTexTransform(XMMatrixScaling(5,7, 5.0f));
	mBitmap.Render(mDc,0,0);
	mDc->RSSetState(0);

	D3DX11_TECHNIQUE_DESC techDesc;
	Effects::BasicFX->Light0TexTech->GetDesc(&techDesc);

	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		ID3DX11EffectPass* pass = Effects::BasicFX->Light0TexTech->GetPassByIndex(p);
		pass->Apply(0, mDc);

		mDc->DrawIndexed(6, 0, 0);
	}


	mDc->OMSetDepthStencilState(mDepthStencilState, 1);
	// restore default states.
	mDc->RSSetState(0);
	mDc->OMSetDepthStencilState(0, 0);

	HR(mSwapChain->Present(0, 0));
}

void CLogoScene::OnMouseDown(WPARAM btnState, int x, int y, const HWND & mhMainWnd)
{
}

void CLogoScene::OnMouseUp(WPARAM btnState, int x, int y)
{
}

void CLogoScene::OnMouseMove(WPARAM btnState, int x, int y)
{
}

void CLogoScene::OnResize(const float & aspectRatio)
{
	mCam.SetLens(0.25f*MathHelper::Pi, aspectRatio, 0.3f, 3000.0f);
}
