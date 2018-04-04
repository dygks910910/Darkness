#include "Texture.h"



CTexture::CTexture()
{
	m_texture = 0;
}

CTexture::CTexture(const CTexture &)
{
}


CTexture::~CTexture()
{
}

bool CTexture::Initialize(WCHAR* filename)
{
	HRESULT result;


	// Load the texture in.
	result = D3DX11CreateShaderResourceViewFromFile(md3dDevice, filename, NULL, NULL, &m_texture, NULL);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void CTexture::Shutdown()
{
	// Release the texture resource.
	if (m_texture)
	{
		m_texture->Release();
		m_texture = 0;
	}

	return;
}

ID3D11ShaderResourceView * CTexture::GetTexture()
{
	return m_texture;
}
