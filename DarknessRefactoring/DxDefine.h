#pragma once

/////////////
// LINKING //
/////////////
//#pragma comment(lib, "d3d11.lib")
//#pragma comment(lib, "dxgi.lib")
//#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "Effects11d.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dsound.lib")



//////////////
// INCLUDES //
//////////////
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include<xnamath.h>
//#include <directxmath.h>
#include<d3dcompiler.h>
#include "DDSTextureLoader.h"	// DDS 颇老 贸府
#include <d3dx11tex.h>
#include <d3dx11async.h>
#include<dinput.h>
#include <mmsystem.h>
#include <dsound.h>
#include"dxerr.h"
#include"d3dx11effect.h"

using namespace DirectX;


#if defined(DEBUG) | defined(_DEBUG)
#ifndef HR
#define HR(x)                                              \
	{                                                          \
		HRESULT hr = (x);                                      \
		if(FAILED(hr))                                         \
		{                                                      \
			DXTrace(WFILE, (DWORD)__LINE__, hr, L#x, true); \
		}                                                      \
	}
#endif

#else
#ifndef HR
#define HR(x) (x)
#endif
#endif 


///////////////////////////
//  warning C4316 贸府侩  //
///////////////////////////
#include "AlignedAllocationPolicy.h"

