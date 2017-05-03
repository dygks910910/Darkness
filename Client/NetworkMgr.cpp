#include <iostream>
#include "NetworkMgr.h"

NetworkMgr* NetworkMgr::instance = nullptr;
extern XMFLOAT3 camtest;

NetworkMgr::NetworkMgr()
{
}


NetworkMgr::~NetworkMgr()
{
}

void NetworkMgr::Initialize()
{
	WSADATA   wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);


	socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);

	while (true)
	{
		char ip[128] = { 0 };


		/*cout << " 서버 IP 입력 : ";
		cin >> ip;*/
		strcpy(ip, &m_strIp[0]);

		SOCKADDR_IN serverAddr;
		ZeroMemory(&serverAddr, sizeof(SOCKADDR_IN));
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(MY_SERVER_PORT);
		serverAddr.sin_addr.s_addr = inet_addr(ip);

		int retval = WSAConnect(socket, reinterpret_cast<SOCKADDR*>(&serverAddr), sizeof(serverAddr), NULL, NULL, NULL, NULL);

		if (retval != SOCKET_ERROR)
			break;
	}
	WSAAsyncSelect(socket, mHandle, WM_SOCKET, FD_CLOSE | FD_READ);

	recv_wsa_buf.buf = reinterpret_cast<char*>(recv_buf);
	recv_wsa_buf.len = MAX_BUF_SIZE;

	saved_size = 0;
	recv_size = 0;

}
void NetworkMgr::Release()
{
	closesocket(socket);
	WSACleanup();
}
void NetworkMgr::ReadPacket(const SOCKET& sock)
{
	DWORD io_byte, io_flag = 0;

	int retval = WSARecv(sock, &recv_wsa_buf, 1, &io_byte, &io_flag, NULL, NULL);

	if (retval == SOCKET_ERROR)
	{
		std::cout << " RecvPacket() : WSARecv() -> SOCKET_ERROR " << std::endl;
		return;
	}

	// 메세지를 받는다
	// 다 읽을때까지 받는다.
	// 메세지 사이즈를 먼저 읽는다.

	BYTE* ptr = recv_buf;

	while (io_byte != 0) {

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
void NetworkMgr::ProcessPacket(BYTE* packet)
{
	switch (packet[1]) {
	case SC_PACKET_PUT_USER:
	{
		sc_packet_put_user put_user;
		memcpy(&put_user, packet, packet[0]);
		CModelManager::GetInstance()->GetSkinnedInstanceModels()[5].mId = put_user.id;
		break;
	}
	case SC_PACKET_PLAYGAME_INIT_POS:
	{
		sc_packet_playgame_init_pos init_pos;
		memcpy(&init_pos, packet, packet[0]);

		// 내 아이디와 패킷으로 온 ID가 같으면 내걸 set
		// 아니면. 다른 애걸 set

		if (CModelManager::GetInstance()->GetSkinnedInstanceModels()[5].mId == init_pos.id)
		{
			CModelManager::GetInstance()->GetSkinnedInstanceModels()[5].World = init_pos.worldMatrix;
			CModelManager::GetInstance()->GetSkinnedInstanceModels()[5].mId = init_pos.id;
			camtest.x = CModelManager::GetInstance()->GetSkinnedInstanceModels()[5].World._41 + (-3.731);
			camtest.y = 2.332356;
			camtest.z = CModelManager::GetInstance()->GetSkinnedInstanceModels()[5].World._43 + (-0.001785517);

			std::cout << "아이디는" << CModelManager::GetInstance()->GetSkinnedInstanceModels()[5].mId << std::endl;
		}
		else
		{
			CModelManager::GetInstance()->GetSkinnedInstanceModels()[3].World = init_pos.worldMatrix;
			CModelManager::GetInstance()->GetSkinnedInstanceModels()[3].mId = init_pos.id;
			std::cout << "아이디는" << CModelManager::GetInstance()->GetSkinnedInstanceModels()[3].mId << std::endl;
		}
		break;
	}
	case SC_PACKET_PLAYGAME_PLAYER_POS:
	{
		sc_packet_playgame_player_pos player_pos;
		memcpy(&player_pos, packet, packet[0]);
		if (CModelManager::GetInstance()->GetSkinnedInstanceModels()[5].mId == player_pos.id)
			CModelManager::GetInstance()->GetSkinnedInstanceModels()[5].World = player_pos.worldMatrix;
		else
			CModelManager::GetInstance()->GetSkinnedInstanceModels()[3].World = player_pos.worldMatrix;

	}
	break;

	case SC_PACKET_PLAYGAME_START_ANMATION:
	{
		sc_packet_player_anmation_start player_anim;
		memcpy(&player_anim, packet, packet[0]);
		CModelManager::GetInstance()->GetSkinnedInstanceModels()[3].mAnimstate = player_anim.animationState;
		CModelManager::GetInstance()->GetSkinnedInstanceModels()[3].mAnimCnt = 0;
	}
	break;

	case SC_PACKET_PLAYGAME_COLLISION_TRUE:
	{
		sc_packet_collision_true player_coll;
		memcpy(&player_coll, packet, packet[0]);
		CModelManager::GetInstance()->GetSkinnedInstanceModels()[5].mCollision = player_coll.check;
	}
	break;

	default:
		break;


	}
}