#include"Common\d3dUtil.h"
#include<string>
ID3D11Device* md3dDevice = nullptr;
ID3D11DeviceContext* md3dImmediateContext = nullptr;
IDXGISwapChain* mSwapChain = nullptr;
ID3D11Texture2D* mDepthStencilBuffer = nullptr;
ID3D11Texture2D* mDepthDisableStencilBuffer = nullptr;
ID3D11RenderTargetView* mRenderTargetView = nullptr;
ID3D11DepthStencilView* mDepthStencilView = nullptr;
D3D11_VIEWPORT mScreenViewport{ 0 };
int mClientWidth = 0;
int mClientHeight = 0;

HINSTANCE mhAppInst=nullptr;
HWND      mhMainWnd = nullptr;
