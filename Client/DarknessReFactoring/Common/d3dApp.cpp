//***************************************************************************************
// d3dApp.cpp by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************
#include "d3dApp.h"
#include <WindowsX.h>
#include <sstream>
#include<D3DX11.h>

namespace
{
	// This is just used to forward Windows messages from a global window
	// procedure to our member function window procedure because we cannot
	// assign a member function to WNDCLASS::lpfnWndProc.
	D3DApp* gd3dApp = 0;
}
LRESULT CALLBACK
MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Forward hwnd on because we can get messages (e.g., WM_CREATE)
	// before CreateWindow returns, and thus before mhMainWnd is valid.
	return gd3dApp->MsgProc(hwnd, msg, wParam, lParam);
}
BOOL CALLBACK
MainDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return gd3dApp->MyDlgProc(hwnd, msg, wParam, lParam);
}
D3DApp::D3DApp(HINSTANCE hInstance)
	: mMainWndCaption(L"D3D11 Application"),
	md3dDriverType(D3D_DRIVER_TYPE_UNKNOWN),
	mEnable4xMsaa(false),
	mAppPaused(false),
	mMinimized(false),
	mMaximized(false),
	mResizing(false),
	m4xMsaaQuality(0)
{
	mhAppInst = hInstance;
	mhMainWnd = 0;
	ZeroMemory(&mScreenViewport, sizeof(D3D11_VIEWPORT));

	// Get a pointer to the application object so we can forward 
	// Windows messages to the object's window procedure through
	// the global window procedure.
	gd3dApp = this;
}

D3DApp::~D3DApp()
{
	ReleaseCOM(mRenderTargetView);
	ReleaseCOM(mDepthStencilView);
	ReleaseCOM(mSwapChain);
	ReleaseCOM(mDepthStencilBuffer);

	// Restore all default settings.
	if (md3dImmediateContext)
		md3dImmediateContext->ClearState();

	ReleaseCOM(md3dImmediateContext);
	ReleaseCOM(md3dDevice);

	SafeDelete(m_pSceneManager);
}

HINSTANCE D3DApp::AppInst()const
{
	return mhAppInst;
}

HWND D3DApp::MainWnd()const
{
	return mhMainWnd;
}

float D3DApp::AspectRatio()const
{
	return static_cast<float>(mClientWidth) / mClientHeight;
}

bool D3DApp::Init()
{

	DialogBox(mhAppInst, MAKEINTRESOURCE(IDD_DIALOG_STARTSETTING), mhMainWnd, (DLGPROC)MainDialogProc);


	if (!InitMainWindow(isFullScreen))
		return false;

	if (!InitDirect3D())
		return false;
	//SceneManager의 참조변수를 d3dApp클래스의 static변수를 통해 초기화되기때문에
	//InitDirect3D이후에 생성해야함 

	m_pSceneManager = new CSceneManager();
	m_pSceneManager->Init();
	OnResize();
	m_pSceneManager->OnResize();

	return true;
}

void D3DApp::OnResize()
{
	assert(md3dImmediateContext);
	assert(md3dDevice);
	assert(mSwapChain);

	// Release the old views, as they hold references to the buffers we
	// will be destroying.  Also release the old depth/stencil buffer.

	ReleaseCOM(mRenderTargetView);
	ReleaseCOM(mDepthStencilView);
	ReleaseCOM(mDepthStencilBuffer);


	// Resize the swap chain and recreate the render target view.

	HR(mSwapChain->ResizeBuffers(1, mClientWidth, mClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0));
	ID3D11Texture2D* backBuffer;
	HR(mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));
	HR(md3dDevice->CreateRenderTargetView(backBuffer, 0, &mRenderTargetView));
	ReleaseCOM(backBuffer);

	// Create the depth/stencil buffer and view.

	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = mClientWidth;
	depthStencilDesc.Height = mClientHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// Use 4X MSAA? --must match swap chain MSAA values.
	if (mEnable4xMsaa)
	{
		depthStencilDesc.SampleDesc.Count = 4;
		depthStencilDesc.SampleDesc.Quality = m4xMsaaQuality - 1;
	}
	// No MSAA
	else
	{
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}

	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	HR(md3dDevice->CreateTexture2D(&depthStencilDesc, 0, &mDepthStencilBuffer));
	HR(md3dDevice->CreateDepthStencilView(mDepthStencilBuffer, 0, &mDepthStencilView));
	// Bind the render target view and depth/stencil view to the pipeline.
	md3dImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);
	// Set the viewport transform.
	mScreenViewport.TopLeftX = 0;
	mScreenViewport.TopLeftY = 0;
	mScreenViewport.Width = static_cast<float>(mClientWidth);
	mScreenViewport.Height = static_cast<float>(mClientHeight);
	mScreenViewport.MinDepth = 0.0f;
	mScreenViewport.MaxDepth = 1.0f;

	md3dImmediateContext->RSSetViewports(1, &mScreenViewport);

}

LRESULT D3DApp::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		// WM_ACTIVATE is sent when the window is activated or deactivated.  
		// We pause the game when the window is deactivated and unpause it 
		// when it becomes active.  
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			mAppPaused = true;
			mTimer.Stop();
		}
		else
		{
			mAppPaused = false;
			mTimer.Start();
		}
		return 0;

		// WM_SIZE is sent when the user resizes the window.  
	case WM_KEYDOWN:
	case WM_KEYUP:
		OnKeyboardButtonDown(wParam);
		return 0;
	case WM_SIZE:
		// Save the new client area dimensions.
		mClientWidth = LOWORD(lParam);
		mClientHeight = HIWORD(lParam);
		if (md3dDevice)
		{
			if (wParam == SIZE_MINIMIZED)
			{
				mAppPaused = true;
				mMinimized = true;
				mMaximized = false;
			}
			else if (wParam == SIZE_MAXIMIZED)
			{
				mAppPaused = false;
				mMinimized = false;
				mMaximized = true;
				OnResize();
			}
			else if (wParam == SIZE_RESTORED)
			{

				// Restoring from minimized state?
				if (mMinimized)
				{
					mAppPaused = false;
					mMinimized = false;
					OnResize();
				}

				// Restoring from maximized state?
				else if (mMaximized)
				{
					mAppPaused = false;
					mMaximized = false;
					OnResize();
				}
				else if (mResizing)
				{
					// If user is dragging the resize bars, we do not resize 
					// the buffers here because as the user continuously 
					// drags the resize bars, a stream of WM_SIZE messages are
					// sent to the window, and it would be pointless (and slow)
					// to resize for each WM_SIZE message received from dragging
					// the resize bars.  So instead, we reset after the user is 
					// done resizing the window and releases the resize bars, which 
					// sends a WM_EXITSIZEMOVE message.
				}
				else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
				{
					OnResize();
				}
			}
		}
		return 0;

		// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
	case WM_ENTERSIZEMOVE:
		mAppPaused = true;
		mResizing = true;
		mTimer.Stop();
		return 0;

		// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
		// Here we reset everything based on the new window dimensions.
	case WM_EXITSIZEMOVE:
		mAppPaused = false;
		mResizing = false;
		mTimer.Start();
		OnResize();
		return 0;

		// WM_DESTROY is sent when the window is being destroyed.
	case WM_DESTROY:
		SafeDelete( m_pSceneManager);
		PostQuitMessage(0);
		return 0;
		// The WM_MENUCHAR message is sent when a menu is active and the user presses 
		// a key that does not correspond to any mnemonic or accelerator key. 
	case WM_MENUCHAR:
		// Don't beep when we alt-enter.
		return MAKELRESULT(0, MNC_CLOSE);

		// Catch this message so to prevent the window from becoming too small.
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
		return 0;
		//case WM_SOCKET:
			//Packet(hwnd, msg, wParam, lParam);
		return 0;
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_CHAR:

		return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}


bool D3DApp::InitMainWindow(const bool& fullScreen)
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = mhAppInst;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"D3DWndClassName";

	if (!RegisterClass(&wc))
	{
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}
	mClientWidth = GetSystemMetrics(SM_CXSCREEN);
	mClientHeight = GetSystemMetrics(SM_CYSCREEN);

	RECT R = { 0, 0, mClientWidth, mClientHeight };
	DEVMODE dmScreenSettings;

	int width;
	int height;
	if (fullScreen)
	{
		mClientWidth = GetSystemMetrics(SM_CXSCREEN);
		mClientHeight = GetSystemMetrics(SM_CYSCREEN);
		// If full screen set the screen to maximum size of the users desktop and 32bit.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)mClientWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)mClientHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner.
		width = height = 0;
	}
	else {
		// Compute window rectangle dimensions based on requested client area dimensions.
		AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
		width = R.right - R.left;
		height = R.bottom - R.top;
	}


	mhMainWnd = CreateWindowEx(WS_EX_APPWINDOW, L"D3DWndClassName", mMainWndCaption.c_str(),
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP, CW_USEDEFAULT, CW_USEDEFAULT,
		mClientWidth, mClientHeight, 0, 0, mhAppInst, 0);
	if (!mhMainWnd)
	{
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return false;
	}

	ShowWindow(mhMainWnd, SW_SHOW);
	SetForegroundWindow(mhMainWnd);
	SetFocus(mhMainWnd);

	UpdateWindow(mhMainWnd);

	return true;
}

bool D3DApp::InitDirect3D()
{
	// Create the device and device context.
	CreateD3DDevice();
	HR(md3dDevice->CheckMultisampleQualityLevels(
		DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m4xMsaaQuality));
	assert(m4xMsaaQuality > 0);
	CreateSwapChain(false);
	return true;
}

void D3DApp::CalculateFrameStats()
{
	// Code computes the average frames per second, and also the 
	// average time it takes to render one frame.  These stats 
	// are appended to the window caption bar.

	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	// Compute averages over one second period.
	if ((mTimer.TotalTime() - timeElapsed) >= 1.0f)
	{
		float fps = (float)frameCnt; // fps = frameCnt / 1
		float mspf = 1000.0f / fps;

		std::wostringstream outs;
		outs.precision(6);
		outs << mMainWndCaption << L"    "
			<< L"FPS: " << fps << L"    "
			<< L"Frame Time: " << mspf << L" (ms)";
		SetWindowText(mhMainWnd, outs.str().c_str());

		// Reset for next average.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}

void D3DApp::CreateD3DDevice()
{
	/*
	2017 / 2 / 27 / 0:23
	작성자:박요한(dygks910910@daum.net)
	설명:지원되는 가장 좋은 어댑터를 설정한다.
	createDevice(pAdapter가 NULL이 아니라면
	md3dDriverType 는 Unknown 으로 설정해야 한다.)
	*/
	IDXGIFactory* factory;
	int result;
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result))
	{
		assert(0);
	}
	DXGI_ADAPTER_DESC desc;
	IDXGIAdapter* pAdapter = nullptr;
	std::vector<IDXGIAdapter*> stdvAdapters;
	int iHighLevelAdapterIndex = 0;
	int iMax = 0;
	for (UINT i = 0; (factory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND); ++i)
	{
		stdvAdapters.push_back(pAdapter);
		pAdapter->GetDesc(&desc);
		if (iMax < desc.DedicatedVideoMemory)
		{
			iHighLevelAdapterIndex = i;
			iMax = desc.DedicatedVideoMemory;
		}
	}
	stdvAdapters[iHighLevelAdapterIndex]->GetDesc(&desc);

#if defined(DEBUG) | defined(_DEBUG)
	std::wcout << desc.Description << std::endl;
#endif

	UINT createDeviceFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)  
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif


	D3D_FEATURE_LEVEL featureLevel;
	HRESULT hr = D3D11CreateDevice(
		stdvAdapters[iHighLevelAdapterIndex],                // default adapter
		md3dDriverType,
		0,                 // no software device
		createDeviceFlags,
		0, 0,              // default feature level array
		D3D11_SDK_VERSION,
		&md3dDevice,
		&featureLevel,
		&md3dImmediateContext);
	if (FAILED(hr))
	{
		MessageBox(0, L"D3D11CreateDevice Failed.", 0, 0);
		return;
	}
	if (featureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		MessageBox(0, L"Direct3D Feature Level 11 unsupported.", 0, 0);
		return;
	}
	for (auto p : stdvAdapters)
	{
		ReleaseCOM(p);
	}
}

void D3DApp::CreateSwapChain(bool use4xaa)
{
	mEnable4xMsaa = use4xaa;

	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = mClientWidth;
	sd.BufferDesc.Height = mClientHeight;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	// Use 4X MSAA? 
	if (mEnable4xMsaa)
	{
		sd.SampleDesc.Count = 4;
		sd.SampleDesc.Quality = m4xMsaaQuality - 1;
	}
	// No MSAA
	else
	{
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
	}

	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = mhMainWnd;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	// To correctly create the swap chain, we must use the IDXGIFactory that was
	// used to create the device.  If we tried to use a different IDXGIFactory instance
	// (by calling CreateDXGIFactory), we get an error: "IDXGIFactory::CreateSwapChain: 
	// This function is being called with a device from a different IDXGIFactory."

	IDXGIDevice* dxgiDevice = 0;
	HR(md3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice));

	IDXGIAdapter* dxgiAdapter = 0;
	HR(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter));

	IDXGIFactory* dxgiFactory = 0;
	HR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory));

	HR(dxgiFactory->CreateSwapChain(md3dDevice, &sd, &mSwapChain));

	ReleaseCOM(dxgiDevice);
	ReleaseCOM(dxgiAdapter);
	ReleaseCOM(dxgiFactory);

}
#include<list>
BOOL  D3DApp::MyDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	wchar_t str[15];
	std::wstring ipAddress;
	HWND comboHandle;
	int selectionResolutionIndex = 0;
	comboHandle = GetDlgItem(hDlg, IDC_COMBO_RESOLUTION);
	std::list<std::wstring> comboStrings{ L"HD",L"FHD" };

	switch (uMsg) {
	case WM_INITDIALOG:
		SetDlgItemText(hDlg, IDC_SERVER_IPADDRESS, L"127.0.0.1");
		SetDlgItemText(hDlg, IDC_COMBO_RESOLUTION, L"default");
		for (auto p = comboStrings.begin(); p != comboStrings.end(); ++p) {
			SendMessage(comboHandle, CB_ADDSTRING, 0, (LPARAM)(*p).c_str());
		}
		return TRUE;


	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
			GetDlgItemText(hDlg, IDC_SERVER_IPADDRESS, str, 15);
			ipAddress = str;
			server_ipAddress.assign(ipAddress.begin(), ipAddress.end());

			//리턴값 = 선택된 인덱스 -1이면 설정안된상태.
			selectionResolutionIndex = SendMessage(comboHandle, CB_GETCURSEL, 0, 0);
			if (selectionResolutionIndex == 0)
			{
				mClientWidth = FHD_WIDTH;
				mClientHeight = FHD_HEIGHT;
			}
			else if (selectionResolutionIndex == 1)
			{
				mClientWidth = HD_WIDTH;
				mClientHeight = HD_HEIGHT;
			}
#if defined DEBUG | _DEBUG
			if (selectionResolutionIndex == 0)
			{
				std::cout << "FHD선택됨" << std::endl;
			}
			else if (selectionResolutionIndex == 1)
			{
				std::cout << "HD선택됨" << std::endl;

			}
			std::cout << "서버 IP주소:";
			std::cout << server_ipAddress;
			std::cout << std::endl;
#endif
			EndDialog(hDlg, 0);
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg, 0);
			return TRUE;
		}
		break;

	}
	return FALSE;
}

