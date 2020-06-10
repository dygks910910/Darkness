#include "stdafx.h"
#include "CInputLayouts.h"
#include"CEffect.h"
ID3D11InputLayout* CInputLayouts::m_LightBasic = 0;


const D3D11_INPUT_ELEMENT_DESC CInputLayoutDesc::LightBasic[3] = {
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

CInputLayouts::~CInputLayouts()
{
}

void CInputLayouts::InitInputLayouts(ID3D11Device* device)
{
	D3DX11_PASS_DESC passDesc;

	CEffects::LightFX->LightTexTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(device->CreateInputLayout(CInputLayoutDesc::LightBasic, 3, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &m_LightBasic));
}

void CInputLayouts::Destroy()
{
	SAFE_RELEASE_D3D(m_LightBasic);
}
