#include "DrawText.h"



CDrawText::CDrawText()
{
}


CDrawText::~CDrawText()
{
}

void CDrawText::operator()(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, std::wstring text)
{
	IFW1Factory *pFW1Factory;
		HRESULT hResult = FW1CreateFactory(FW1_VERSION, &pFW1Factory);

		IFW1FontWrapper *pFontWrapper;
		hResult = pFW1Factory->CreateFontWrapper(pDevice, L"Arial", &pFontWrapper);

		pFontWrapper->DrawString(
			pContext,
			text.c_str(),// String
			20.0f,// Font size
			100.0f,// X position
			50.0f,// Y position
			0xff0099ff,// Text color, 0xAaBbGgRr
			FW1_RESTORESTATE// Flags (for example FW1_RESTORESTATE to keep context states unchanged)
		);

		pFontWrapper->Release();
		pFW1Factory->Release();
}

void CDrawText::operator()(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, std::wstring text, const float & fontSize, const float & posX, const float & posY)
{
	IFW1Factory *pFW1Factory;
	HRESULT hResult = FW1CreateFactory(FW1_VERSION, &pFW1Factory);

	IFW1FontWrapper *pFontWrapper;
	hResult = pFW1Factory->CreateFontWrapper(pDevice, L"Arial", &pFontWrapper);

	pFontWrapper->DrawString(
		pContext,
		text.c_str(),// String
		fontSize,// Font size
		posX,// X position
		posY,// Y position
		0xff0099ff,// Text color, 0xAaBbGgRr
		FW1_RESTORESTATE// Flags (for example FW1_RESTORESTATE to keep context states unchanged)
	);

	pFontWrapper->Release();
	pFW1Factory->Release();
}
