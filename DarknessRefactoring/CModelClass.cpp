#include"stdafx.h"
#include "CModelClass.h"
#include <fstream>
CModelClass::CModelClass():
	m_vertexBuffer(nullptr)
	,m_indexBuffer(nullptr)
	,m_Texture(nullptr)
{
}

CModelClass::CModelClass(const CModelClass&)
{
}

CModelClass::~CModelClass()
{
}


bool CModelClass::Initialize(ID3D11Device* device, const wchar_t* textureName)
{
	bool result;
	// Initialize the vertex and index buffers.
	result = InitializeBuffers(device);
	IF_NOTX_RTFALSE(result);
	result &= LoadTexture(device, textureName);
	IF_NOTX_RTFALSE(result);
	return true;
}

bool CModelClass::Initialize(ID3D11Device* device,const char* modelFilename, const wchar_t* textureName)
{
	// �� �����͸� �ε��մϴ�.
	if (!LoadModel(modelFilename))
	{
		return false;
	}

	// ���� �� �ε��� ���۸� �ʱ�ȭ�մϴ�.
	if (!InitializeBuffers(device))
	{
		return false;
	}

	// �� ���� �ؽ�ó�� �ε��մϴ�.
	return LoadTexture(device, textureName);
}

void CModelClass::Shutdown()
{
	ReleaseTexture();
	ShutdownBuffers();
}

void CModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);
}

int CModelClass::GetIndexCount()
{
	return m_indexCount;
}

ID3D11ShaderResourceView* CModelClass::GetTexture()
{
	return m_Texture->GetTexture();
}

bool CModelClass::LoadTexture(ID3D11Device* device, const wchar_t* fineName)
{
	bool result;
	m_Texture = new CTextureClass;
	IF_NOTX_RTFALSE(m_Texture);

	result = m_Texture->Initialize(device, fineName);
	IF_NOTX_RTFALSE(m_Texture);

	return true;
}

void CModelClass::ReleaseTexture()
{
	SAFE_DELETE_SHUTDOWN(m_Texture);
}

bool CModelClass::InitializeBuffers(ID3D11Device* device)
{
	// ���� �迭�� ����ϴ�.
	VertexType* vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// �ε��� �迭�� ����ϴ�.
	unsigned long* indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	// ���� �迭�� �ε��� �迭�� �����ͷ� �о�ɴϴ�.
	for (int i = 0; i < m_vertexCount; i++)
	{
		vertices[i].position = XMFLOAT3(m_model[i].x, m_model[i].y, m_model[i].z);
		vertices[i].texture = XMFLOAT2(m_model[i].tu, m_model[i].tv);
		vertices[i].normal = XMFLOAT3(m_model[i].nx, m_model[i].ny, m_model[i].nz);

		indices[i] = i;
	}

	// ���� ���� ������ ����ü�� �����մϴ�.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// subresource ������ ���� �����Ϳ� ���� �����͸� �����մϴ�.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// ���� ���� ���۸� ����ϴ�.
	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer)))
	{
		return false;
	}

	// ���� �ε��� ������ ����ü�� �����մϴ�.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// �ε��� �����͸� ����Ű�� ���� ���ҽ� ����ü�� �ۼ��մϴ�.
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// �ε��� ���۸� �����մϴ�.
	if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer)))
	{
		return false;
	}

	// �����ǰ� ���� �Ҵ�� ���� ���ۿ� �ε��� ���۸� �����մϴ�.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}

void CModelClass::ShutdownBuffers()
{
	// Release the index buffer.
	SAFE_RELEASE_D3D(m_indexBuffer);
	SAFE_RELEASE_D3D(m_vertexBuffer);
}

void CModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool CModelClass::LoadModel(const char* filename)
{
	// �� ������ ���ϴ�.
	std::ifstream fin;
	fin.open(filename);

	// ������ �� �� ������ �����մϴ�.
	if (fin.fail())
	{
		return false;
	}

	// ���ؽ� ī��Ʈ�� ������ �д´�.
	char input = 0;
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// ���ؽ� ī��Ʈ�� �д´�.
	fin >> m_vertexCount;

	// �ε����� ���� ���� ���� ���� �����մϴ�.
	m_indexCount = m_vertexCount;

	// �о� ���� ���� ������ ����Ͽ� ���� ����ϴ�.
	m_model = new ModelType[m_vertexCount];
	if (!m_model)
	{
		return false;
	}

	// �������� ���� �κб��� �д´�.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	// ���ؽ� �����͸� �н��ϴ�.
	for (int i = 0; i < m_vertexCount; i++)
	{
		fin >> m_model[i].x >> m_model[i].y >> m_model[i].z;
		fin >> m_model[i].tu >> m_model[i].tv;
		fin >> m_model[i].nx >> m_model[i].ny >> m_model[i].nz;
	}

	// �� ������ �ݴ´�.
	fin.close();

	return true;
}

void CModelClass::ReleaseModel()
{
	SAFE_DELETE_ARR(m_model);
}
