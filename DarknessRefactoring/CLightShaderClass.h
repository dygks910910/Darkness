#pragma once

class CLightShaderClass
{
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};
	class CameraBufferType
	{
	public:
		XMFLOAT3 cameraPosition;
		float padding;
	};
	struct LightBufferType
	{
		XMFLOAT4 ambientColor;
		XMFLOAT4 diffuseColor;
		XMFLOAT3 lightDirection;
		float specularPower;
		XMFLOAT4 speculatColor;
	};

public:
	CLightShaderClass();
	CLightShaderClass(const CLightShaderClass&);
	~CLightShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*,
		XMFLOAT3, XMFLOAT4, XMFLOAT4, XMFLOAT3, XMFLOAT4, float);

private:
	bool InitializeShader(ID3D11Device*, HWND, const wchar_t*, const wchar_t*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, const wchar_t*);

	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
		XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT3 lightDirection, XMFLOAT4 ambientColor,
		XMFLOAT4 diffuseColor, XMFLOAT3 cameraPosition, XMFLOAT4 specularColor, float specularPower);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_cameraBuffer;
	ID3D11Buffer* m_lightBuffer;
};

