#pragma once
#include "d3dUtil.h"
#include "FW1FontWrapper.h"
#include<tchar.h>

class CDrawText
{
	ID3D11Device* pDevice;
	ID3D11DeviceContext *pContext;
	IFW1Factory *pFW1Factory;
	IFW1FontWrapper *pFontWrapper;
public:
	CDrawText() {};
	~CDrawText();
	void Init(ID3D11Device* device, ID3D11DeviceContext *context);
	void operator()(std::wstring text);
	void operator()( std::wstring text,const float& fontSize, const float& posX, const float& posY);
};

