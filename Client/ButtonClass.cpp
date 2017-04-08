#include "ButtonClass.h"



void CButtonClass::Init(ID3D11Device* device, const int& bitmapWidth,
	const int& bitmapHeight, const WCHAR* filename, const int& locationX, const int& locationY)
{
	mIndex = BUTTON_STATE::NORMAL_STATE;
	mLocationX = locationX;
	mLocationY = locationY;
	mBitmapWidth = bitmapWidth;
	mBitmapHeight = bitmapHeight;


	std::wstring temp = filename;
	std::wstring temp1 = filename;
	std::wstring temp2 = filename;

	temp.insert(temp.rfind(L'.'), L"1");
	temp1.insert(temp1.rfind(L'.'), L"2");
	temp2.insert(temp2.rfind(L'.'), L"3");

	mButton[0].Initialize(device, 800, 600, (WCHAR*)temp.c_str(), mBitmapWidth, mBitmapHeight);
	mButton[1].Initialize(device, 800, 600, (WCHAR*)temp1.c_str(), mBitmapWidth, mBitmapHeight);
	mButton[2].Initialize(device, 800, 600, (WCHAR*)temp2.c_str(), mBitmapWidth, mBitmapHeight);
	isMouseOnThisButton = false;
	isClicked = false;
}

void CButtonClass::Draw(ID3D11DeviceContext* dc)
{
	Effects::BasicFX->SetDiffuseMap(mButton[mIndex].GetTexture());
	mButton[mIndex].Render(dc, mLocationX, mLocationY);
}

void CButtonClass::Update()
{

}

void CButtonClass::OnMouseDown(const int & x, const int & y)
{
	std::cout << "down";
	if (isMouseOnThisButton)
	{
		mIndex = BUTTON_STATE::MOUSE_CLICKED_STATE;
	}
	else
	{
		mIndex = BUTTON_STATE::MOUSE_ON_STATE;
	}
}

void CButtonClass::OnMouseMove(const int & x, const int & y)
{
	if (x > mLocationX && x <mLocationX + mBitmapWidth
		&& y > mLocationY && y < mLocationY + mBitmapHeight
		)
	{
		mIndex = BUTTON_STATE::MOUSE_ON_STATE;
		isMouseOnThisButton = true;
	}
	else
	{
		mIndex = BUTTON_STATE::NORMAL_STATE;
		isMouseOnThisButton = false;

	}
}

void CButtonClass::OnMouseUp(const int & x, const int & y)
{
	if (isMouseOnThisButton)
	{
		isClicked = true;
	}
}

CButtonClass::CButtonClass()
{
}


CButtonClass::~CButtonClass()
{
	for (int i = 0; i < 3; ++i)
	{
		mButton[i].Shutdown();
	}
}
