#pragma once
#include "BitMap.h"
#include<string>
#ifdef _DEBUG | DEBUG
#include<iostream>
#endif
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

	bool isMouseOnThisButton;
public:
	/*
	2017 / 4 / 6 / 17:40
	�ۼ���:�ڿ���(dygks910910@daum.net)
	����:������ ��Ʈ�� �̹����� �������� �ϰ� connect.png �� �Ű������� �Է��ϸ� 
	connect1.png,connect2.png,connect3.png �� �ε��.
	*/
	void Init(ID3D11Device* device, const int& bitmapWidth,
		const int& bitmapHeight, const WCHAR* filename,const int& locationX, const int& locationY);
	void Draw(ID3D11DeviceContext* dc);
	void Update();
	void OnMouseDown(const int& x, const int& y);
	void OnMouseMove(const int& x, const int& y);
	void OnMouseUp(const int& x, const int& y);

	CButtonClass();
	~CButtonClass();

	
	bool isClicked;

};
enum BUTTON_STATE
{
	NORMAL_STATE = 0,
	MOUSE_ON_STATE,
	MOUSE_CLICKED_STATE
};
