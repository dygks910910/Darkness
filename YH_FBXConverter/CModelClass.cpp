#include "CModelClass.h"
#include "Define.h"
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

bool CModelClass::Initialize(ID3D11Device* device,
 const wchar_t* textureName, const std::vector<VertexType>& vertices, const  std::vector<int>& indices)
{
	if (m_vertexBuffer != nullptr)
		Shutdown();

	bool result;
	// Initialize the vertex and index buffers.
	result = InitializeBuffers(device,vertices,indices);
	IF_NOTX_RTFALSE(result);
	result &= LoadTexture(device, textureName);
	IF_NOTX_RTFALSE(result);
	return true;
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
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;


	// Set the number of vertices in the vertex array.
	m_vertexCount = 3;

	// Set the number of indices in the index array.
	m_indexCount = 3;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	IF_NOTX_RTFALSE(vertices);

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	IF_NOTX_RTFALSE(indices);

	// Load the vertex array with data.
	vertices[0].position = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);  // Bottom left.
	vertices[0].texture = D3DXVECTOR2(0.0f, 1.0f);
	vertices[0].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	vertices[1].position = D3DXVECTOR3(0.0f, 1.0f, 0.0f);  // Top middle.
	vertices[1].texture = D3DXVECTOR2(0.5f, 0.0f);
	vertices[1].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	vertices[2].position = D3DXVECTOR3(1.0f, -1.0f, 0.0f);  // Bottom right.
	vertices[2].texture = D3DXVECTOR2(1.0f, 1.0f);
	vertices[2].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);


	// Load the index array with data.
	indices[0] = 0;  // Bottom left.
	indices[1] = 1;  // Top middle.
	indices[2] = 2;  // Bottom right.

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	IF_FAILED_RTFALSE(result);

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	IF_FAILED_RTFALSE(result);

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	SAFE_DELETE_ARR(vertices);
	SAFE_DELETE_ARR(indices);

	return true;
}

bool CModelClass::InitializeBuffers(ID3D11Device* device, const std::vector<VertexType>& vertices, const  std::vector<int>& indices)
{
	//VertexType* vertices;
	//unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;


	// Set the number of vertices in the vertex array.
	m_vertexCount = vertices.size();

	// Set the number of indices in the index array.
	m_indexCount = indices.size();

	// Create the vertex array.
	//vertices = new VertexType[m_vertexCount];
	//IF_NOTX_RTFALSE(vertices);

	//// Create the index array.
	//indices = new unsigned long[m_indexCount];
	//IF_NOTX_RTFALSE(indices);

	//// Load the vertex array with data.
	//vertices[0].position = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);  // Bottom left.
	//vertices[0].texture = D3DXVECTOR2(0.0f, 1.0f);
	//vertices[0].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	//vertices[1].position = D3DXVECTOR3(0.0f, 1.0f, 0.0f);  // Top middle.
	//vertices[1].texture = D3DXVECTOR2(0.5f, 0.0f);
	//vertices[1].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	//vertices[2].position = D3DXVECTOR3(1.0f, -1.0f, 0.0f);  // Bottom right.
	//vertices[2].texture = D3DXVECTOR2(1.0f, 1.0f);
	//vertices[2].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);


	//// Load the index array with data.
	//indices[0] = 0;  // Bottom left.
	//indices[1] = 1;  // Top middle.
	//indices[2] = 2;  // Bottom right.

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = &vertices[0];
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	IF_FAILED_RTFALSE(result);

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = &indices[0];
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	IF_FAILED_RTFALSE(result);

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
