#include "stdafx.h"
#include "Network.h"

Network* Network::instance = nullptr;

Network::Network()
{
}


Network::~Network()
{
}

void Network::Initialize()
{
	WSADATA	wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);


	socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);

	while (true)
	{
		char ip[128] = { 0 };


		/*cout << " 서버 IP 입력 : ";
		cin >> ip;*/
		strcpy(ip, "127.0.0.1");

		SOCKADDR_IN serverAddr;
		ZeroMemory(&serverAddr, sizeof(SOCKADDR_IN));
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(MY_SERVER_PORT);
		serverAddr.sin_addr.s_addr = inet_addr(ip);

		int retval = WSAConnect(socket, reinterpret_cast<SOCKADDR*>(&serverAddr), sizeof(serverAddr), NULL, NULL, NULL, NULL);

		if (retval != SOCKET_ERROR)
			break;
	}
	WSAAsyncSelect(socket, g_WHandle, WM_SOCKET, FD_CLOSE | FD_READ);

	recv_wsa_buf.buf = reinterpret_cast<char*>(recv_buf);
	recv_wsa_buf.len = MAX_BUF_SIZE;

	saved_size = 0;
	recv_size = 0;
}
void Network::Release()
{
	closesocket(socket);
	WSACleanup();
}
void Network::ReadPacket(const SOCKET& sock)
{
	DWORD io_byte, io_flag = 0;

	int retval = WSARecv(sock, &recv_wsa_buf, 1, &io_byte, &io_flag, NULL, NULL);
	
	if (retval == SOCKET_ERROR)
	{
		cout << " RecvPacket() : WSARecv() -> SOCKET_ERROR " << endl;
		return;
	}
	
	// 메세지를 받는다
	// 다 읽을때까지 받는다.
	// 메세지 사이즈를 먼저 읽는다.

	BYTE* ptr = recv_buf;

	while ( io_byte != 0 ){

		if (recv_size == 0)
			recv_size = ptr[0];

		if (io_byte + saved_size >= recv_size)
		{
			memcpy(packet_buf + saved_size, ptr, recv_size - saved_size);

			ProcessPacket(packet_buf);
			ptr += recv_size - saved_size;
			io_byte -= recv_size - saved_size;

			recv_size = 0;
			saved_size = 0;
		}

		else
		{
			memcpy(packet_buf + saved_size, ptr, io_byte);

			saved_size += io_byte;
			io_byte = 0;
		}

	}

}
void Network::ProcessPacket(BYTE* packet)
{
	switch (packet[1]) {
	
	case PACKET_PUT_USER:
		sc_packet_put_user client;
		
		memcpy(&client, packet, packet[0]);
		cout << "유저 정보가 입력 되었습니다." << endl;
		cout << "본 계정의 id는 " << (int)client.id << endl;
		break;

	default:
		break;


	}
}