#pragma once


class CInputClass;
class CGraphicsClass;
class SoundClass;
class FpsClass;
class CpuClass;
class TimerClass;

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

	CInputClass* m_pInput = nullptr;
	CGraphicsClass* m_pGraphics = nullptr;
	SoundClass* m_Sound = nullptr;
	FpsClass* m_Fps = nullptr;
	CpuClass* m_Cpu = nullptr;
	TimerClass* m_Timer = nullptr;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static CSystemClass* g_ApplicationHandle = 0;