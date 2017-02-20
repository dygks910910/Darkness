#include "stdafx.h"
#include "ThreadFunctions.h"
#include "User.h"

void AcceptThread()
{
	int retval;

	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	// socket()
	SOCKET listen_sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (listen_sock == INVALID_SOCKET) return;
	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = ::bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) return;

	::listen(listen_sock, 10);

	UINT new_id = 0;

	while (true)
	{
		SOCKET		client_sock;
		SOCKADDR_IN client_addr;
		int	addr_len = sizeof(client_addr);

		client_sock = WSAAccept(listen_sock, reinterpret_cast<SOCKADDR*>(&client_addr), &addr_len, NULL, NULL);

		if (client_sock == SOCKET_ERROR)
			break;


		CreateIoCompletionPort(reinterpret_cast<HANDLE>(client_sock), g_hIocp, new_id, 0);


		users.push_back(new CUser);
		users[new_id]->socket = client_sock;
		users[new_id]->id = new_id;

		sc_packet_put_user packet;
		packet.id = users[new_id]->id;
		packet.size = sizeof(packet);
		packet.type = PACKET_PUT_USER;

		SendPacket(new_id, reinterpret_cast<BYTE*>(&packet));

		users[new_id]->RecvPacket();
		cout << "[" << new_id << "]" << " 접속" << endl;
		++new_id;
	}

	closesocket(listen_sock);
	WSACleanup();

}
void WorketThread()
{
	DWORD transferred;
	ULONGLONG id = 0;

	OVERLAPPED_EX* over_ex = nullptr;

	while (true)
	{
		BOOL ret_val = GetQueuedCompletionStatus(g_hIocp, &transferred, (PULONG_PTR)&id, reinterpret_cast<LPOVERLAPPED*>(&over_ex), INFINITE);

		// 메세지를 받으면 우선 전부 다 받을때까지 루프를 돌린다
		// 전부다 받았으면 모든 유저들에게 메세지를 보낸다
		// -> 필요한것. (보낸사람의 ID, 패킷길이, 내용)
		// 

		
		cout << "이동 하네?" << endl;

		users[id]->RecvPacket();

	}
}

void SendPacket(const UINT id, BYTE* packet)
{
	DWORD flags = 0;

	OVERLAPPED_EX* over_ex = new OVERLAPPED_EX;
	ZeroMemory(over_ex, sizeof(OVERLAPPED_EX));

	over_ex->wsa_buf.buf = reinterpret_cast<char*>(&over_ex->packet_buf);
	over_ex->wsa_buf.len = packet[0];
	over_ex->operation = IO_SEND;

	memcpy(over_ex->packet_buf, packet, packet[0]);

	int ret_val = WSASend(users[id]->socket, &over_ex->wsa_buf, 1, NULL, flags, &over_ex->overlapped, NULL);

	if (ret_val != 0)
	{
		int error = WSAGetLastError();
		if (error != WSA_IO_PENDING)
			cout << " [error] WSASend() on SendPacket function " << endl;
	}
}