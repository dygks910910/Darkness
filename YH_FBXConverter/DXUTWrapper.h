#pragma once
#include"DXUT.h"
#include"CLightShaderClass.h"
#include"CLightClass.h"
#include"CModelClass.h"
#include"DXUTcamera.h"
#include"COpenFBXWrraper.h"
class DXUTWrapper
{
public:
	DXUTWrapper();
	~DXUTWrapper();

public:
	static bool CALLBACK ModifyDeviceSettings(DXUTDeviceSettings* pDeviceSettings, void* pUserContext);
	static void CALLBACK OnFrameMove(double fTime, float fElapsedTime, void* pUserContext);
	static LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing,
		void* pUserContext);
	static bool CALLBACK IsD3D11DeviceAcceptable(const CD3D11EnumAdapterInfo* AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo* DeviceInfo,
		DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext);
	static HRESULT CALLBACK OnD3D11CreateDevice(ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc,
		void* pUserContext);
	static HRESULT CALLBACK OnD3D11ResizedSwapChain(ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain,
		const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext);
	static void CALLBACK OnD3D11ReleasingSwapChain(void* pUserContext);
	static void CALLBACK OnD3D11DestroyDevice(void* pUserContext);
	static void CALLBACK OnD3D11FrameRender(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime,
		float fElapsedTime, void* pUserContext);
	static void CALLBACK OnKeyboard(UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext);
	static void CALLBACK OnMouse(bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown,
		bool bSideButton1Down, bool bSideButton2Down, int nMouseWheelDelta,
		int xPos, int yPos, void* pUserContext);
	static bool CALLBACK OnDeviceRemoved(void* pUserContext);

private:
	static CLightShaderClass		m_LightShader;
	static CLightClass					m_Light;
	static CModelClass					m_Model;
	static CModelViewerCamera m_Camera;                   // A model viewing camera

private:

};
