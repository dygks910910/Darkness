#pragma once

#include<d3d11.h>
#include<xnamath.h>

#include"AlignedAllocationPolicy.h"
#include"CTextureClass.h"
class CModelClass : public AlignedAllocationPolicy<16>
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
	};

	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};
public:
	CModelClass();
	CModelClass(const CModelClass&);
	~CModelClass();

	bool Initialize(ID3D11Device*,const wchar_t* textureName);
	bool Initialize(ID3D11Device* device,const  char* modelFilename, const wchar_t* textureName);

	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();
private:
	bool LoadTexture(ID3D11Device*, const wchar_t*);
	void ReleaseTexture();
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);


	bool LoadModel(const char*);
	void ReleaseModel();
private:
	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	int m_vertexCount, m_indexCount;
	CTextureClass* m_Texture;
	ModelType* m_model = nullptr;
};

