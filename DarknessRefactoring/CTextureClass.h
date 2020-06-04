#pragma once
#include <d3d11.h>
#include <d3dx11tex.h>

class CTextureClass
{
public:
	CTextureClass();
	CTextureClass(const CTextureClass&);
	~CTextureClass();

	bool Initialize(ID3D11Device*, const wchar_t* filename);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();

private:
	ID3D11ShaderResourceView* m_texture;
};

