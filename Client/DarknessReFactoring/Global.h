#pragma once
#include"Common\d3dUtil.h"
//#include "Material.h"

extern ID3D11Device* md3dDevice;
extern ID3D11DeviceContext* md3dImmediateContext ;
extern IDXGISwapChain* mSwapChain ;
extern ID3D11Texture2D* mDepthStencilBuffer ;

extern ID3D11RenderTargetView* mRenderTargetView ;
extern ID3D11DepthStencilView* mDepthStencilView ;
extern D3D11_VIEWPORT mScreenViewport;
extern int mClientWidth ;
extern int mClientHeight;

extern HINSTANCE mhAppInst;
extern HWND      mhMainWnd;
