#pragma once
#include "d3dUtil.h"
#include "FW1FontWrapper.h"
#include<tchar.h>

class CDrawText
{
public:
	CDrawText();
	~CDrawText();
	void operator()(ID3D11Device *pDevice, ID3D11DeviceContext *pContext, std::wstring text);
	void operator()(ID3D11Device *pDevice, ID3D11DeviceContext *pContext, std::wstring text,
		const float& fontSize, const float& posX, const float& posY);
};

