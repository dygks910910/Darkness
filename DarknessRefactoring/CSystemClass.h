#pragma once
#include<Windows.h>
#include"CInputClass.h"
#include"CGraphicsClass.h"
#include"Define.h"


class CSystemClass
{
public:
	CSystemClass();
	CSystemClass(const CSystemClass&);
	~CSystemClass();

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);
private:
	bool Frame();
	void InitializeWindows(int&,int&);
	void ShutdownWindows();

private:
	LPCWSTR m_applicationName;
	HINSTANCE m_hInstance;
	HWND m_hWnd;

	CInputClass* m_pInput;
	CGraphicsClass* m_pGraphics;

};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static CSystemClass* g_ApplicationHandle = 0;