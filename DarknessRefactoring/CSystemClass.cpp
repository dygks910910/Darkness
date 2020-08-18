#include"stdafx.h"
#include "CSystemClass.h"
#include"CInputClass.h"
#include"CGraphicsClass.h"
#include"SoundClass.h"
#include"FpsClass.h"
#include"TimerClass.h"
#include"CpuClass.h"
#include"PositionClass.h"
SystemClass::SystemClass()
	:m_pInput(nullptr),m_pGraphics(nullptr)
{
	
}

SystemClass::SystemClass(const SystemClass&)
{
}

SystemClass::~SystemClass()
{
}

bool SystemClass::Initialize()
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
	m_pGraphics = new GraphicsClass;
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
	m_Position = new PositionClass();
	IF_NOTX_RTFALSE(m_Position);
	
	return true;
}

void SystemClass::Shutdown()
{
	SAFE_DELETE_SHUTDOWN(m_pGraphics);
	SAFE_DELETE_SHUTDOWN(m_pInput);
	SAFE_DELETE_SHUTDOWN(m_Sound);
	SAFE_DELETE_SHUTDOWN(m_Cpu);
	SAFE_DELETE(m_Fps);
	SAFE_DELETE(m_Timer);
	SAFE_DELETE(m_Position);


	ShutdownWindows();

	return;
}

void SystemClass::Run()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

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

LRESULT SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{ 
	
	return DefWindowProc(hwnd,umsg,wparam,lparam);
}

bool SystemClass::Frame()
{
	bool bResult;
	int mouseX = 0, mouseY = 0;

	m_Timer->Frame();
	m_Fps->Frame();
	m_Cpu->Frame();

	m_Position->SetFrameTime(m_Timer->GetTime());
	if (!m_pInput->Frame())
		return false;
	m_pInput->GetMouseLocation(mouseX, mouseY);
	

	bool keyDown = m_pInput->IsRightArrowPressed();
	m_Position->TurnRight(keyDown);

	keyDown = m_pInput->IsLeftArrowPressed();
	m_Position->TurnLeft(keyDown);

	float rotationY = 0;
	m_Position->GetRotation(rotationY);
	
	//IF_NOTX_RTFALSE(m_pGraphics->Frame(m_Timer->GetTime()));
	IF_NOTX_RTFALSE(m_pGraphics->Frame());

	return m_pGraphics->Render();
}

void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc; 
	DEVMODE dmScreenSettings; 
	int posX, posY; 
	// 외부 포인터를 이 객체로 설정합니다. 
	g_ApplicationHandle = this; 
	// 이 어플리케이션의 인스턴스를 가져옵니다. 
	m_hInstance = GetModuleHandle(NULL); 
	// 어플리케이션의 이름을 설정합니다. 
	m_applicationName = L"Engine";
	// 윈도우 클래스를 기본 설정으로 맞춥니다.
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; 
	wc.lpfnWndProc = WndProc; 
	wc.cbClsExtra = 0; 
	wc.cbWndExtra = 0; 
	wc.hInstance = m_hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO); 
	wc.hIconSm = wc.hIcon; wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH); wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName; wc.cbSize = sizeof(WNDCLASSEX); 
	// 윈도우 클래스를 등록합니다. 
	RegisterClassEx(&wc); 
	// 모니터 화면의 해상도를 알아옵니다. 
	screenWidth = GetSystemMetrics(SM_CXSCREEN); screenHeight = GetSystemMetrics(SM_CYSCREEN); 
	// 풀스크린 모드 변수의 값에 따라 화면 설정을 합니다. 
	if(FULL_SCREEN)
	{ 
		// 만약 풀스크린 모드라면 화면 크기를 데스크톱 크기에 맞추고 색상을 32bit로 합니다.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings); 
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth; 
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32; 
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT; 
		// 풀스크린에 맞는 디스플레이 설정을 합니다. 
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
		// 윈도우의 위치를 화면의 왼쪽 위로 맞춥니다.
		posX = posY = 0;
	} else 
	{ 
		// 윈도우 모드라면 800x600의 크기를 가지게 합니다. 
		screenWidth = 800; screenHeight = 600; 
		// 창을 모니터의 중앙에 오도록 합니다. 
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2; 
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	} 
	// 설정한 것을 가지고 창을 만들고 그 핸들을 가져옵니다. 
	m_hWnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP, posX, posY, screenWidth, screenHeight,
		NULL, NULL, m_hInstance, NULL); 
	// 윈도우를 화면에 표시하고 포커스를 줍니다. 
	ShowWindow(m_hWnd, SW_SHOW); 
	SetForegroundWindow(m_hWnd); 
	SetFocus(m_hWnd); 
	// 마우스 커서를 표시하지 않습니다. 
	ShowCursor(false);
	
}

void SystemClass::ShutdownWindows()
{
	// 마우스 커서를 표시합니다. 
	ShowCursor(true); 
	// 풀스크린 모드를 빠져나올 때 디스플레이 설정을 바꿉니다. 
	if(FULL_SCREEN) { ChangeDisplaySettings(0, 0); } 
	// 창을 제거합니다. 
	DestroyWindow(m_hWnd);
	m_hWnd = nullptr; 
	// 어플리케이션 인스턴스를 제거합니다. 
	UnregisterClass(m_applicationName, m_hInstance); m_hInstance = NULL; 
	// 이 클래스에 대한 외부 포인터 참조를 제거합니다.
	g_ApplicationHandle = nullptr; 
	return;

}


LRESULT CALLBACK WndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{ 
		// 윈도우가 제거되었는지 확인합니다. 
	case WM_DESTROY: { PostQuitMessage(0); return 0; } 
				   // 윈도우가 닫히는지 확인합니다. 
	case WM_CLOSE: { PostQuitMessage(0); return 0; } 
				 // 다른 모든 메세지들은 system 클래스의 메세지 처리기에 전달합니다.
	default: { return g_ApplicationHandle->MessageHandler(hwnd, umsg, wparam, lparam); } }
}
