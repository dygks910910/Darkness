#pragma once

class MainGame
{
	BYTE	send_buf[MAX_BUF_SIZE];
	WSABUF	send_wsa_buf;

public:
	void Initialize();
	void Progress();
	void Render();
	void Release();

	void ProcessWindowMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

public:
	MainGame();
	~MainGame();
};

