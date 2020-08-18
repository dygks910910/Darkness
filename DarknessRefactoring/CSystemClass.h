#pragma once


class CInputClass;
class GraphicsClass;
class SoundClass;
class FpsClass;
class CpuClass;
class TimerClass;
class PositionClass;
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
	GraphicsClass* m_pGraphics = nullptr;
	SoundClass* m_Sound = nullptr;
	FpsClass* m_Fps = nullptr;
	CpuClass* m_Cpu = nullptr;
	TimerClass* m_Timer = nullptr;
	PositionClass* m_Position = nullptr;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static CSystemClass* g_ApplicationHandle = 0;