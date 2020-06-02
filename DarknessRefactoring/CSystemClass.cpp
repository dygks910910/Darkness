#include "CSystemClass.h"

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

	m_pGraphics = new CGraphicsClass;
	IF_NOTX_RTFALSE(m_pGraphics);

	result = m_pGraphics->Initialize(nScreenWidth, nScreenHeight,m_hWnd);
	IF_NOTX_RTFALSE(result);
	m_pInput->Initialize();
	
	return true;
}

void CSystemClass::Shutdown()
{
	if (m_pGraphics)
	{
		m_pGraphics->Shutdown();
		SAFE_DELETE(m_pGraphics);
	}
	if (m_pInput)
		SAFE_DELETE(m_pInput);

	ShutdownWindows();

	return;
}

void CSystemClass::Run()
{
	MSG msg;
	bool bDone, bResult;

	ZeroMemory(&msg, sizeof(MSG));
	bDone = false;

	while (!bDone)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (msg.message == WM_QUIT)
			bDone = true;
		else
		{
			bResult = Frame();
			if (!bResult)
				bDone = true;
		}
	}
}

LRESULT CSystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{ 
	switch (umsg)
	{
	case WM_KEYDOWN:
	{
		m_pInput->KeyDown((unsigned int)wparam);
		return 0;
	}
	case WM_KEYUP:
	{
		m_pInput->KeyUp((unsigned int)wparam);
		return 0;
	}
	default:
		return DefWindowProc(hwnd, umsg, wparam, lparam);
	}

	return LRESULT();
}

bool CSystemClass::Frame()
{
	bool bResult;
	if (m_pInput->IsKeyDown(VK_ESCAPE))
		return false;

	bResult = m_pGraphics->Frame();
	IF_NOTX_RTFALSE(bResult);

	return true;
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
