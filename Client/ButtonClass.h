#pragma once
#include "BitMap.h"
#include<string>
#ifdef _DEBUG | DEBUG
#include<iostream>
#endif
/*
2017 / 4 / 6 / 17:42
작성자:박요한(dygks910910@daum.net)
설명:무조건 비트맵 이미지가 세개여야  함.
*/
class CButtonClass
{
private:
	CBitMap mButton[3];

	int mLocationX;
	int mLocationY;
	int mBitmapWidth;
	int mBitmapHeight;
	int mIndex;
	int mClientWidth;
	int mClientHeight;
	bool isMouseOnThisButton;
public:
	XMFLOAT4X4 mWorld;
	/*
	2017 / 4 / 6 / 17:40
	작성자:박요한(dygks910910@daum.net)
	설명:무조건 비트맵 이미지가 세개여야 하고 connect.png 를 매개변수로 입력하면 
	connect1.png,connect2.png,connect3.png 가 로드됨.
	*/
	void Init(ID3D11Device* device, const int& bitmapWidth,
		const int& bitmapHeight, const WCHAR* filename,const int& locationX, 
		const int& locationY, const int& clientWidth, const int& clientHeight);
	void Draw(ID3D11DeviceContext* dc);
	void Update();
	void OnMouseDown(const int& x, const int& y);
	void OnMouseMove(const int& x, const int& y);
	void OnMouseUp(const int& x, const int& y);
	float AspectRatio();
	CButtonClass();
	~CButtonClass();
	XNA::AxisAlignedBox GetBox() { return mButton[0].mMeshBox; }
	
	bool isClicked;

};
enum BUTTON_STATE
{
	NORMAL_STATE = 0,
	MOUSE_ON_STATE,
	MOUSE_CLICKED_STATE
};
