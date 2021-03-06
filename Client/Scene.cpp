#include "Scene.h"

CScene::~CScene()
{
}

ID3D11DepthStencilState* CScene::m_depthDisableStencilState = nullptr;
ID3D11DepthStencilState* CScene::m_depthStencilState = nullptr;
ID3D11BlendState* CScene::m_alphaEnableBlendingState = nullptr;
ID3D11BlendState* CScene::m_alphaDisableBlendingState = nullptr;
float CScene::AspectRatio() const
{
	return static_cast<float>(mClientWidth) / mClientHeight;
}

void CScene::TurnZBuffOff(ID3D11DeviceContext* deviceContext)
{
	deviceContext->OMSetDepthStencilState(m_depthDisableStencilState, 1);
}

void CScene::TurnZBuffOn(ID3D11DeviceContext* deviceContext)
{
	deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);
}

void CScene::TurnAlphaOn(ID3D11DeviceContext* deviceContext)
{
	// Setup the blend factor.
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	// Turn on the alpha blending.
	deviceContext->OMSetBlendState(m_alphaEnableBlendingState, blendFactor, 0xffffffff);
}

void CScene::TurnAlphaOff(ID3D11DeviceContext* deviceContext)
{
	// Setup the blend factor.
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	// Turn off the alpha blending.
	deviceContext->OMSetBlendState(m_alphaDisableBlendingState, blendFactor, 0xffffffff);
}

void CScene::ReleaseZbufferState()
{
	ReleaseCOM(m_depthDisableStencilState);
	ReleaseCOM(m_depthStencilState);

}

bool CScene::CreateZbufferState(ID3D11Device* device)
{
		// 이제 2D 렌더링을위한 Z 버퍼를 끄는 두 번째 깊이 스텐실 상태를 만듭니다. 유일한 차이점은
		// DepthEnable을 false로 설정하면 다른 모든 매개 변수는 다른 깊이 스텐실 상태와 동일합니다.
		D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
		ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));

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
		// 장치를 사용하여 상태를 만듭니다.
		if (FAILED(device->CreateDepthStencilState(&depthDisabledStencilDesc, &m_depthDisableStencilState)))
		{
			return false;
		}

		// 스텐실 상태 구조체를 초기화합니다
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

		// 스텐실 상태 구조체를 작성합니다
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

		depthStencilDesc.StencilEnable = true;
		depthStencilDesc.StencilReadMask = 0xFF;
		depthStencilDesc.StencilWriteMask = 0xFF;

		// 픽셀 정면의 스텐실 설정입니다
		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// 픽셀 뒷면의 스텐실 설정입니다
		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// 깊이 스텐실 상태를 생성합니다
		if (FAILED(device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState)))
		{
			return false;
		}

		return true;
}

bool CScene::CreateAlphaBlendingState(ID3D11Device* device)
{
	// Clear the blend state description.
	D3D11_BLEND_DESC blendStateDescription;
	ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));

	// Create an alpha enabled blend state description.
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	// Create the blend state using the description.
	if(FAILED(device->CreateBlendState(&blendStateDescription, &m_alphaEnableBlendingState)))
		return false;
	// Modify the description to create an alpha disabled blend state description.
	blendStateDescription.RenderTarget[0].BlendEnable = FALSE;
	// Create the blend state using the description.
	if (FAILED(device->CreateBlendState(&blendStateDescription, &m_alphaDisableBlendingState)))
		return false;

	return true;
}
