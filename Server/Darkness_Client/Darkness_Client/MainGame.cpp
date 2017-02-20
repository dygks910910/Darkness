#include "stdafx.h"
#include "MainGame.h"
#include "Network.h"


MainGame::MainGame()
{
}

MainGame::~MainGame()
{
}

void MainGame::Initialize()
{

	send_wsa_buf.buf = reinterpret_cast<char*>(send_buf);
	send_wsa_buf.len = MAX_BUF_SIZE;



	Network::GetInstance()->Initialize();
}

void MainGame::Progress()
{

}

void MainGame::Render()
{
}

void MainGame::Release()
{
	Network::GetInstance()->DestroyInstance();

}


void MainGame::ProcessWindowMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_KEYDOWN:
	{
		if (wParam == VK_ESCAPE)
		{
			PostQuitMessage(0);
			break;
		}


		BYTE dir = 0;

		if (wParam == VK_UP)
		{
			dir = CS_UP;
		}

		else if (wParam == VK_DOWN)
		{
			dir = CS_DOWN;

		}

		else if (wParam == VK_LEFT)
		{
			dir = CS_LEFT;

		}

		else if (wParam == VK_RIGHT)
		{
			dir = CS_RIGHT;

		}
		else break;

		cout << "Å°´­¸²" << endl;
		cs_packet_move* move = reinterpret_cast<cs_packet_move*>(&send_buf);
		move->size = sizeof(cs_packet_move);
		move->type = dir;

		send_wsa_buf.len = sizeof(cs_packet_move);

		DWORD io_byte;

		int ret_val = WSASend(Network::GetInstance()->GetSock(), &send_wsa_buf, 1, &io_byte, 0, NULL, NULL);
		if (ret_val == SOCKET_ERROR)
			cout << " [error] WSASend() " << endl;

		break;
	}

	case WM_SOCKET:
	{
		
		if (WSAGETSELECTERROR(lParam))
		{
			closesocket((SOCKET)wParam);
			exit(-1);
		}

		switch (WSAGETSELECTEVENT(lParam)) {
		case FD_READ:
			Network::GetInstance()->ReadPacket((SOCKET)wParam);
			break;

		case FD_CLOSE:
			closesocket((SOCKET)wParam);
			exit(-1);
			break;
		}
	}

	default:
		break;
	}
}
