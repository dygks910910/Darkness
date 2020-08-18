#pragma once


class D3DClass : public AlignedAllocationPolicy<16>
{
public:
	D3DClass();
	D3DClass(const D3DClass&);
	~D3DClass();

	bool Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen,
		float screenDepth, float screenNear);
	void Shutdown();

	void BeginScene(float red, float green, float blue, float alpha);
	void EndScene();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	void GetProjectionMatrix(XMMATRIX&);
	void GetWorldMatrix(XMMATRIX&);
	void GetOrthoMatrix(XMMATRIX&);

	void GetVideoCardInfo(char*, int&);
	void TurnZBufferOn();
	void TurnZBufferOff();


	void TurnOnAlphaBlending();
	void TurnOffAlphaBlending();
	void SetFirstViewport();
	void SetSecondViewport();

private:
	bool CreateSwapChain(int screenW,int screenH,unsigned int numerator,
		unsigned int denominator,bool fullscreen,HWND hwnd);
	bool CreateRenderTargerView();
	bool CreateDepthBuffer(int screenWidth, int screenHeight);
	bool CreateDepthStencilState();
	bool CreateDepthStencilView();
	bool CreateRasterizeState();
	bool CreateDepthDisableStencilState();
	bool CreateBlendState();
private:
	bool m_vsync_enabled;
	int m_videoCardMemory;
	char m_videoCardDescription[128];
	IDXGISwapChain*					m_swapChain;
	ID3D11Device*						m_device;
	ID3D11DeviceContext*			m_deviceContext;
	ID3D11RenderTargetView*	m_renderTargetView;
	ID3D11Texture2D*					m_depthStencilBuffer;
	ID3D11DepthStencilState*	m_depthStencilState;
	ID3D11DepthStencilView*		m_depthStencilView;
	ID3D11RasterizerState*			m_rasterState;
	XMMATRIX								m_projectionMatrix;
	XMMATRIX								m_worldMatrix;
	XMMATRIX								m_orthoMatrix;
	ID3D11DepthStencilState*	m_depthDisabledStencilState;
	ID3D11BlendState*					m_alphaEnableBlendingState = nullptr;
	ID3D11BlendState*					m_alphaDisableBlendingState = nullptr;
	D3D11_VIEWPORT m_viewports[2];

};

