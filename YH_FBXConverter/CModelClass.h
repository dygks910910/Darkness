#pragma once

#include<d3d11.h>
#include<D3DX10math.h>
#include"CTextureClass.h"
#include<vector>
class CModelClass
{
public:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
	};

public:
	CModelClass();
	CModelClass(const CModelClass&);
	~CModelClass();

	bool Initialize(ID3D11Device*,const wchar_t* textureName);
	bool Initialize(ID3D11Device* device,
 const wchar_t* textureName, const std::vector<VertexType>& vertices,const  std::vector<int> &indices);

	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();
private:
	bool LoadTexture(ID3D11Device*, const wchar_t*);
	void ReleaseTexture();
	bool InitializeBuffers(ID3D11Device*device);
	bool InitializeBuffers(ID3D11Device*device, const std::vector<VertexType>& vertices, const  std::vector<int>& indices);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

private:
	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	int m_vertexCount, m_indexCount;
	CTextureClass* m_Texture;
};

