#include "Scene.h"

CScene::~CScene()
{
}

ID3D11DepthStencilState* CScene::m_depthDisableStencilState;
ID3D11DepthStencilState* CScene::m_depthStencilState;

float CScene::AspectRatio() const
{
	return static_cast<float>(mClientWidth) / mClientHeight;
}

void CScene::TurnZBuffOff(ID3D11DeviceContext* deviceContext)
{
	deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);
}

void CScene::TurnZBuffOn(ID3D11DeviceContext* deviceContext)
{
	deviceContext->OMSetDepthStencilState(m_depthDisableStencilState, 1);
}

void CScene::ReleaseZbufferState()
{
	ReleaseCOM(m_depthDisableStencilState);
	ReleaseCOM(m_depthStencilState);

}

bool CScene::CreateZbufferState(ID3D11Device* device)
{
		// ���� 2D ������������ Z ���۸� ���� �� ��° ���� ���ٽ� ���¸� ����ϴ�. ������ ��������
		// DepthEnable�� false�� �����ϸ� �ٸ� ��� �Ű� ������ �ٸ� ���� ���ٽ� ���¿� �����մϴ�.
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
		// ��ġ�� ����Ͽ� ���¸� ����ϴ�.
		if (FAILED(device->CreateDepthStencilState(&depthDisabledStencilDesc, &m_depthDisableStencilState)))
		{
			return false;
		}

		// ���ٽ� ���� ����ü�� �ʱ�ȭ�մϴ�
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

		// ���ٽ� ���� ����ü�� �ۼ��մϴ�
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

		depthStencilDesc.StencilEnable = true;
		depthStencilDesc.StencilReadMask = 0xFF;
		depthStencilDesc.StencilWriteMask = 0xFF;

		// �ȼ� ������ ���ٽ� �����Դϴ�
		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// �ȼ� �޸��� ���ٽ� �����Դϴ�
		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// ���� ���ٽ� ���¸� �����մϴ�
		if (FAILED(device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState)))
		{
			return false;
		}

		return true;
}
