#pragma once
#include "BitMap.h"
#include<string>
#include<iostream>
/*
2017 / 4 / 6 / 17:42
�ۼ���:�ڿ���(dygks910910@daum.net)
����:������ ��Ʈ�� �̹����� ��������  ��.
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
	bool isSingleImg;
public:
	float m_fScaleX = 1;
	float m_fScaleY = 1;
	XMFLOAT4X4 mWorld;
	/*
	2017 / 4 / 6 / 17:40
	�ۼ���:�ڿ���(dygks910910@daum.net)
	����:������ ��Ʈ�� �̹����� �������� �ϰ� connect.png �� �Ű������� �Է��ϸ� 
	connect1.png,connect2.png,connect3.png �� �ε��.
	*/
	void Init(ID3D11Device* device,
		const int& bitmapWidth,
		const int& bitmapHeight, const WCHAR* filename,
		const int& clientWidth,
		const int& clientHeight,const float& scaleX = 1.0f, const float& scaleY = 1.0f, bool singleImg = false);
	void Draw(ID3D11DeviceContext* dc, const int& locationX , const int& locationY);

	void Update();
	void OnMouseDown(const int& x, const int& y);
	void OnMouseMove(const int& x, const int& y);
	void OnMouseUp(const int& x, const int& y);
	float AspectRatio();
	CButtonClass();
	~CButtonClass();
	XNA::AxisAlignedBox GetBox() { return mButton[0].mMeshBox; }
	int GetLocationX() { return mLocationX; };
	int GetLocationY() { return mLocationY; };
	int GetBitmapWidth() { return mBitmapWidth*m_fScaleX; }
	int GetBitmapHeight() { return mBitmapHeight * m_fScaleY; }

	bool isClicked;

};
enum BUTTON_STATE
{
	NORMAL_STATE = 0,
	MOUSE_ON_STATE,
	MOUSE_CLICKED_STATE
};
