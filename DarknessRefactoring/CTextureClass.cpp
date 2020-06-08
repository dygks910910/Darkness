#include"stdafx.h"
#include "CTextureClass.h"
CTextureClass::CTextureClass():
	m_texture(nullptr)
{
}

CTextureClass::CTextureClass(const CTextureClass&)
{
}

CTextureClass::~CTextureClass()
{
}



bool CTextureClass::Initialize(ID3D11Device* device,const wchar_t* filename)
{
	HRESULT result;


	// Load the texture in.
	result = D3DX11CreateShaderResourceViewFromFile(device, filename, NULL, NULL, &m_texture, NULL);
	IF_FAILED_RTFALSE(result)

		return true;
}


void CTextureClass::Shutdown()
{
	// Release the texture resource.
	SAFE_RELEASE_D3D(m_texture);
}


ID3D11ShaderResourceView* CTextureClass::GetTexture()
{
	return m_texture;
}