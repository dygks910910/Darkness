//***************************************************************************************
// RenderStates.cpp by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#include "RenderStates.h"

ID3D11RasterizerState* RenderStates::WireframeRS = 0;
ID3D11RasterizerState* RenderStates::NoCullRS    = 0;
ID3D11RasterizerState* RenderStates::SolidRS = 0;
ID3D11BlendState*      RenderStates::AlphaToCoverageBS = 0;
ID3D11BlendState*      RenderStates::TransparentBS     = 0;
ID3D11DepthStencilState* RenderStates::EqualsDSS = 0;
ID3D11DepthStencilState* RenderStates::DepthDisableState = 0;

void RenderStates::InitAll(ID3D11Device* device)
{
	//
	// WireframeRS
	//
	D3D11_RASTERIZER_DESC wireframeDesc;
	ZeroMemory(&wireframeDesc, sizeof(D3D11_RASTERIZER_DESC));
	wireframeDesc.FillMode = D3D11_FILL_WIREFRAME;
	wireframeDesc.CullMode = D3D11_CULL_BACK;
	wireframeDesc.FrontCounterClockwise = false;
	wireframeDesc.DepthClipEnable = true;

	HR(device->CreateRasterizerState(&wireframeDesc, &WireframeRS));

	//
	// NoCullRS
	//
	/*
	2017 / 1 / 10 / 6:52
	작성자:박요한(dygks910910@daum.net)
	설명:와이어펜스같은 경우사용.
	*/
	D3D11_RASTERIZER_DESC noCullDesc;
	ZeroMemory(&noCullDesc, sizeof(D3D11_RASTERIZER_DESC));
	noCullDesc.FillMode = D3D11_FILL_SOLID;
	noCullDesc.CullMode = D3D11_CULL_NONE;
	noCullDesc.FrontCounterClockwise = false;
	noCullDesc.DepthClipEnable = true;

	HR(device->CreateRasterizerState(&noCullDesc, &NoCullRS));

	//
	// EqualsDSS
	//
	D3D11_DEPTH_STENCIL_DESC equalsDesc;
	ZeroMemory(&equalsDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	equalsDesc.DepthEnable = true;
	equalsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	equalsDesc.DepthFunc = D3D11_COMPARISON_EQUAL;

	HR(device->CreateDepthStencilState(&equalsDesc, &EqualsDSS));


	//depthDisableState
	D3D11_DEPTH_STENCIL_DESC depthDisableDesc;
	ZeroMemory(&equalsDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	depthDisableDesc.DepthEnable = false;
	depthDisableDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDisableDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDisableDesc.StencilEnable = true;
	depthDisableDesc.StencilReadMask = 0xFF;
	depthDisableDesc.StencilWriteMask = 0xFF;
	depthDisableDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisableDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthDisableDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisableDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthDisableDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisableDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthDisableDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisableDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	HR(device->CreateDepthStencilState(&depthDisableDesc, &DepthDisableState));

	/*
	2017 / 1 / 10 / 6:49
	작성자:박요한(dygks910910@daum.net)
	설명:FILL_SOLID,CULL_BACK,
	*/
	D3D11_RASTERIZER_DESC solidCullDesc;
	ZeroMemory(&solidCullDesc, sizeof(D3D11_RASTERIZER_DESC));
	solidCullDesc.FillMode = D3D11_FILL_SOLID;
	solidCullDesc.CullMode = D3D11_CULL_BACK;
	solidCullDesc.FrontCounterClockwise = false;
	solidCullDesc.DepthClipEnable = true;

	HR(device->CreateRasterizerState(&solidCullDesc, &SolidRS));
	//
	// AlphaToCoverageBS
	//

	D3D11_BLEND_DESC alphaToCoverageDesc = {0};
	alphaToCoverageDesc.AlphaToCoverageEnable = true;
	alphaToCoverageDesc.IndependentBlendEnable = false;
	alphaToCoverageDesc.RenderTarget[0].BlendEnable = false;
	alphaToCoverageDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	HR(device->CreateBlendState(&alphaToCoverageDesc, &AlphaToCoverageBS));

	//
	// TransparentBS
	//

	D3D11_BLEND_DESC transparentDesc = {0};
	transparentDesc.AlphaToCoverageEnable = false;
	transparentDesc.IndependentBlendEnable = false;

	transparentDesc.RenderTarget[0].BlendEnable = true;
	transparentDesc.RenderTarget[0].SrcBlend       = D3D11_BLEND_SRC_ALPHA;
	transparentDesc.RenderTarget[0].DestBlend      = D3D11_BLEND_INV_SRC_ALPHA;
	transparentDesc.RenderTarget[0].BlendOp        = D3D11_BLEND_OP_ADD;
	transparentDesc.RenderTarget[0].SrcBlendAlpha  = D3D11_BLEND_ONE;
	transparentDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	transparentDesc.RenderTarget[0].BlendOpAlpha   = D3D11_BLEND_OP_ADD;
	transparentDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	HR(device->CreateBlendState(&transparentDesc, &TransparentBS));
}

void RenderStates::DestroyAll()
{
	ReleaseCOM(WireframeRS);
	ReleaseCOM(NoCullRS);
	ReleaseCOM(AlphaToCoverageBS);
	ReleaseCOM(TransparentBS);
	ReleaseCOM(EqualsDSS);
	ReleaseCOM(DepthDisableState);

}