#include"stdafx.h"
#include "CSystemClass.h"
#include"CInputClass.h"
#include"CGraphicsClass.h"
#include"SoundClass.h"
#include"FpsClass.h"
#include"TimerClass.h"
#include"CpuClass.h"
CSystemClass::CSystemClass()
	:m_pInput(nullptr),m_pGraphics(nullptr)
{
	
}

CSystemClass::CSystemClass(const CSystemClass&)
{
}

CSystemClass::~CSystemClass()
{
}

bool CSystemClass::Initialize()
{
	int nScreenWidth = 0, nScreenHeight = 0;
	bool result;

	InitializeWindows(nScreenWidth, nScreenHeight);

	m_pInput = new CInputClass;
	IF_NOTX_RTFALSE(m_pInput);
	if (!m_pInput->Initialize(m_hInstance, m_hWnd, nScreenWidth, nScreenHeight))
	{
		MessageBox(m_hWnd, L"could not initialize the input obj", L"Error", MB_OK);
		return false;
	}
	m_pGraphics = new CGraphicsClass;
	IF_NOTX_RTFALSE(m_pGraphics);

	result = m_pGraphics->Initialize(nScreenWidth, nScreenHeight,m_hWnd);
	IF_NOTX_RTFALSE(result);
	m_pInput->Initialize(m_hInstance, m_hWnd,nScreenWidth,nScreenHeight);
	m_Sound = new SoundClass;
	if (!m_Sound->Initialize(m_hWnd))
	{
		MessageBox(m_hWnd, L"could not initialize the Sound obj", L"Error", MB_OK);
		return false;
	}
	m_Fps = new FpsClass;
	IF_NOTX_RTFALSE(m_Fps);
	m_Fps->Initialize();

	m_Cpu = new CpuClass;
	IF_NOTX_RTFALSE(m_Cpu);
	m_Cpu->Initialize();

	m_Timer = new TimerClass;
	IF_NOTX_RTFALSE(m_Timer);
	if (!m_Timer->Initialize())
	{
		MessageBox(m_hWnd, L"could not initialize the Timer obj", L"Error", MB_OK);
		return false;

	}
	return true;
}

void CSystemClass::Shutdown()
{
	SAFE_DELETE_SHUTDOWN(m_pGraphics);
	SAFE_DELETE_SHUTDOWN(m_pInput);
	SAFE_DELETE_SHUTDOWN(m_Sound);
	SAFE_DELETE_SHUTDOWN(m_Cpu);
	SAFE_DELETE(m_Fps);
	SAFE_DELETE(m_Timer);


	ShutdownWindows();

	return;
}

void CSystemClass::Run()
{
	MSG msg;
	bool bDone, bResult;

	ZeroMemory(&msg, sizeof(MSG));
	bDone = false;

	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			if (!Frame())
			{
				MessageBox(m_hWnd, L"Frame Processing faild", L"Error", MB_OK);
				break;
			}
		}
		if (m_pInput->IsEscapePressed())
			break;
	}
}

LRESULT CSystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{ 
	
	return DefWindowProc(hwnd,umsg,wparam,lparam);
}

bool CSystemClass::Frame()
{
	bool bResult;
	int mouseX = 0, mouseY = 0;

	m_Timer->Frame();
	m_Fps->Frame();
	m_Cpu->Frame();

	if (!m_pInput->Frame())
		return false;
	m_pInput->GetMouseLocation(mouseX, mouseY);
	
	bResult = m_pGraphics->Frame(mouseX, mouseY, m_Cpu->GetCpuPercentage(),m_Fps->GetFps() ,m_Timer->GetTime() );
	IF_NOTX_RTFALSE(bResult);

	return m_pGraphics->Render();
}

void CSystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc; 
	DEVMODE dmScreenSettings; 
	int posX, posY; 
	// �ܺ� �����͸� �� ��ü�� �����մϴ�. 
	g_ApplicationHandle = this; 
	// �� ���ø����̼��� �ν��Ͻ��� �����ɴϴ�. 
	m_hInstance = GetModuleHandle(NULL); 
	// ���ø����̼��� �̸��� �����մϴ�. 
	m_applicationName = L"Engine";
	// ������ Ŭ������ �⺻ �������� ����ϴ�.
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; 
	wc.lpfnWndProc = WndProc; 
	wc.cbClsExtra = 0; 
	wc.cbWndExtra = 0; 
	wc.hInstance = m_hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO); 
	wc.hIconSm = wc.hIcon; wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH); wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName; wc.cbSize = sizeof(WNDCLASSEX); 
	// ������ Ŭ������ ����մϴ�. 
	RegisterClassEx(&wc); 
	// ����� ȭ���� �ػ󵵸� �˾ƿɴϴ�. 
	screenWidth = GetSystemMetrics(SM_CXSCREEN); screenHeight = GetSystemMetrics(SM_CYSCREEN); 
	// Ǯ��ũ�� ��� ������ ���� ���� ȭ�� ������ �մϴ�. 
	if(FULL_SCREEN)
	{ 
		// ���� Ǯ��ũ�� ����� ȭ�� ũ�⸦ ����ũ�� ũ�⿡ ���߰� ������ 32bit�� �մϴ�.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings); 
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth; 
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32; 
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT; 
		// Ǯ��ũ���� �´� ���÷��� ������ �մϴ�. 
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
		// �������� ��ġ�� ȭ���� ���� ���� ����ϴ�.
		posX = posY = 0;
	} else 
	{ 
		// ������ ����� 800x600�� ũ�⸦ ������ �մϴ�. 
		screenWidth = 800; screenHeight = 600; 
		// â�� ������� �߾ӿ� ������ �մϴ�. 
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2; 
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	} 
	// ������ ���� ������ â�� ����� �� �ڵ��� �����ɴϴ�. 
	m_hWnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP, posX, posY, screenWidth, screenHeight,
		NULL, NULL, m_hInstance, NULL); 
	// �����츦 ȭ�鿡 ǥ���ϰ� ��Ŀ���� �ݴϴ�. 
	ShowWindow(m_hWnd, SW_SHOW); 
	SetForegroundWindow(m_hWnd); 
	SetFocus(m_hWnd); 
	// ���콺 Ŀ���� ǥ������ �ʽ��ϴ�. 
	ShowCursor(false);
	
}

void CSystemClass::ShutdownWindows()
{
	// ���콺 Ŀ���� ǥ���մϴ�. 
	ShowCursor(true); 
	// Ǯ��ũ�� ��带 �������� �� ���÷��� ������ �ٲߴϴ�. 
	if(FULL_SCREEN) { ChangeDisplaySettings(0, 0); } 
	// â�� �����մϴ�. 
	DestroyWindow(m_hWnd);
	m_hWnd = nullptr; 
	// ���ø����̼� �ν��Ͻ��� �����մϴ�. 
	UnregisterClass(m_applicationName, m_hInstance); m_hInstance = NULL; 
	// �� Ŭ������ ���� �ܺ� ������ ������ �����մϴ�.
	g_ApplicationHandle = nullptr; 
	return;

}


LRESULT CALLBACK WndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{ 
		// �����찡 ���ŵǾ����� Ȯ���մϴ�. 
	case WM_DESTROY: { PostQuitMessage(0); return 0; } 
				   // �����찡 �������� Ȯ���մϴ�. 
	case WM_CLOSE: { PostQuitMessage(0); return 0; } 
				 // �ٸ� ��� �޼������� system Ŭ������ �޼��� ó���⿡ �����մϴ�.
	default: { return g_ApplicationHandle->MessageHandler(hwnd, umsg, wparam, lparam); } }
}
