#pragma once
#include <d3d11.h>
#include <d3dx11tex.h>
#include"Global.h"
class CTexture
{
public:
	CTexture();
	CTexture(const CTexture&);
	~CTexture();

	bool Initialize(WCHAR* filename);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();
private:
	ID3D11ShaderResourceView* m_texture;
};

