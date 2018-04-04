#include "DrawText.h"



void CDrawText::Init(ID3D11Device* device, ID3D11DeviceContext *context)
{
	this->pDevice = device;
	this->pContext = context;
	HRESULT hResult = FW1CreateFactory(FW1_VERSION, &pFW1Factory);
	hResult = pFW1Factory->CreateFontWrapper(device, L"Arial", &pFontWrapper);
}


CDrawText::~CDrawText()
{
	pFontWrapper->Release();
	pFW1Factory->Release();
}


void CDrawText::operator()(std::wstring text, const float & fontSize, const float & posX, const float & posY)
{

	pFontWrapper->DrawString(
		pContext,
		text.c_str(),// String
		fontSize,// Font size
		posX,// X position
		posY,// Y position
		FontColorForFW::BLACK,// Text color, 0xAaBbGgRr
		FW1_RESTORESTATE// Flags (for example FW1_RESTORESTATE to keep context states unchanged)
	);

	
}

void CDrawText::operator()(std::wstring text, const float & fontSize, const float & posX, const float & posY, const UINT & color)
{
	pFontWrapper->DrawString(
		pContext,
		text.c_str(),// String
		fontSize,// Font size
		posX,// X position
		posY,// Y position
		color,// Text color, 0xAaBbGgRr
		FW1_RESTORESTATE// Flags (for example FW1_RESTORESTATE to keep context states unchanged)
	);

}
