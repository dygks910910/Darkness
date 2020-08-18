#include"stdafx.h"
#include "CTextureClass.h"
TextureClass::TextureClass()
	:
	m_texture(nullptr)
{
}

TextureClass::TextureClass(const TextureClass&)
	:
	m_texture(nullptr)
{
}

TextureClass::~TextureClass()
{
}



bool TextureClass::Initialize(ID3D11Device* device,const wchar_t* filename)
{
	HRESULT result;


	// Load the texture in.
	result = D3DX11CreateShaderResourceViewFromFile(device, filename, NULL, NULL, &m_texture, NULL);
	IF_FAILED_RTFALSE(result)

		return true;
}


void TextureClass::Shutdown()
{
	// Release the texture resource.
	SAFE_RELEASE_D3D(m_texture);
}


ID3D11ShaderResourceView* TextureClass::GetTexture()
{
	return m_texture;
}