#pragma once
#include "Common/d3dUtil.h"


#include "FW1FontWrapper.h"
#include<tchar.h>

class CDrawText
{
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext *m_pDeviceContext = nullptr;
	IFW1Factory *pFW1Factory = nullptr;
	IFW1FontWrapper *pFontWrapper = nullptr;
public:
	CDrawText() {};
	~CDrawText();
	void Init(ID3D11Device* device, ID3D11DeviceContext *context);
	void operator()( std::wstring text,const float& fontSize, const float& posX, const float& posY);
	void operator()(std::wstring text, const float& fontSize, const float& posX, const float& posY,const UINT& color);

};
namespace FontColorForFW
{
	// Text color, 0xAaBbGgRr
	const UINT32 RED = 0xff0000ff;
	const UINT32 GRREN = 0xff00ff00;
 	const UINT32 BLUE = 0xffff0000;
 	const UINT32 WHITE = 0xffffffff;
 	const UINT32 SKYBLUE = 0xffffd700;
	const UINT32 PINK = 0xffd700ff;
	const UINT32 GOLD = 0xff00d7ff;
 	const UINT32 SILVER = 0xffc0c0c0;
	const UINT32 BRONZE = 0xff327FCD;
	const UINT32 BLACK = 0xff000000;

}
