#include "DrawText.h"



void CDrawText::Init()
{
	HRESULT hResult = FW1CreateFactory(FW1_VERSION, &pFW1Factory);
	hResult = pFW1Factory->CreateFontWrapper(md3dDevice, L"Arial", &pFontWrapper);
}


CDrawText::~CDrawText()
{
	if(pFontWrapper)
	pFontWrapper->Release();
	if (pFW1Factory)
	pFW1Factory->Release();
}


void CDrawText::operator()(std::wstring text, const float & fontSize, const float & posX, const float & posY)
{

	pFontWrapper->DrawString(
		md3dImmediateContext,
		text.c_str(),// String
		fontSize,// Font size
		posX,// X position
		posY,// Y position
		BLACK,// Text color, 0xAaBbGgRr
		FW1_RESTORESTATE// Flags (for example FW1_RESTORESTATE to keep context states unchanged)
	);

	
}

void CDrawText::operator()(std::wstring text, const float & fontSize, const float & posX, const float & posY, const UINT & color)
{
	pFontWrapper->DrawString(
		md3dImmediateContext,
		text.c_str(),// String
		fontSize,// Font size
		posX,// X position
		posY,// Y position
		color,// Text color, 0xAaBbGgRr
		FW1_RESTORESTATE// Flags (for example FW1_RESTORESTATE to keep context states unchanged)
	);

}
