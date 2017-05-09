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
	isGameOver = false;
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
#ifdef _DEBUG	
		std::cout << " RecvPacket() : WSARecv() -> SOCKET_ERROR " << std::endl;
#endif
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
		setId(put_user.id);
		CModelManager::GetInstance()->GetSkinnedInstanceModels()[getId()].mId = put_user.id;

		cs_packet_player_nickname* nick
			= reinterpret_cast<cs_packet_player_nickname*>(&CModelManager::GetInstance()->send_buf);
		nick->size = sizeof(cs_packet_player_nickname);
		nick->type = CS_PACKET_CLEINT_NICKNAME;
		nick->id = CModelManager::GetInstance()->GetSkinnedInstanceModels()[put_user.id].mId;
		wcscpy(nick->nickName,&mNickName[0]);

		CModelManager::GetInstance()->send_wsa_buf.len = sizeof(cs_packet_player_nickname);
		DWORD io_byte2;

		int ret_val = WSASend(NetworkMgr::GetInstance()->GetSock(), 
			&CModelManager::GetInstance()->send_wsa_buf,
			1, &io_byte2, 0, NULL, NULL);
		if (ret_val == SOCKET_ERROR)
			std::cout << " [error] WSASend() " << std::endl;
		break;
	}
	case SC_PACKET_PLAYGAME_INIT_POS:
	{
		sc_packet_playgame_init_pos init_pos;
		memcpy(&init_pos, packet, packet[0]);

		// 내 아이디와 패킷으로 온 ID가 같으면 내걸 set
		// 아니면. 다른 애걸 set
		if (CModelManager::GetInstance()->GetSkinnedInstanceModels()[getId()].mId == init_pos.id)
		{
			CModelManager::GetInstance()->GetSkinnedInstanceModels()[getId()].World = init_pos.worldMatrix;


			camtest.x = init_pos.campos.x;
			camtest.y = 2.332356;
			camtest.z = init_pos.campos.z;
			std::cout << camtest.x << " " << camtest.y << " " << camtest.z << std::endl;
			std::cout << "아이디는" << CModelManager::GetInstance()->GetSkinnedInstanceModels()[getId()].mId << std::endl;
		}
		else
		{
			CModelManager::GetInstance()->GetSkinnedInstanceModels()[init_pos.id].mId = init_pos.id;
			CModelManager::GetInstance()->GetSkinnedInstanceModels()[init_pos.id].World = init_pos.worldMatrix;
			std::cout << "아이디는" << CModelManager::GetInstance()->GetSkinnedInstanceModels()[init_pos.id].mId << std::endl;
		}
		break;
	}
	case SC_PACKET_PLAYGAME_PLAYER_POS:
	{
		sc_packet_playgame_player_pos player_pos;
		memcpy(&player_pos, packet, packet[0]);
		if (CModelManager::GetInstance()->GetSkinnedInstanceModels()[getId()].mId == player_pos.id)
		{
			//std::cout << player_pos.cammove.x << ' ' << player_pos.cammove.z << std::endl;
			CModelManager::GetInstance()->GetSkinnedInstanceModels()[getId()].mcammove = player_pos.cammove;
			CModelManager::GetInstance()->GetSkinnedInstanceModels()[getId()].World = player_pos.worldMatrix;
		}
		else
			CModelManager::GetInstance()->GetSkinnedInstanceModels()[player_pos.id].World = player_pos.worldMatrix;

	}
	break;

	case SC_PACKET_PLAYGAME_START_ANMATION:
	{
		sc_packet_player_anmation_start player_anim;
		memcpy(&player_anim, packet, packet[0]);
		CModelManager::GetInstance()->GetSkinnedInstanceModels()[player_anim.id].mAnimstate = player_anim.animationState;
		CModelManager::GetInstance()->GetSkinnedInstanceModels()[player_anim.id].mAnimCnt = 0;
	}
	break;

	case SC_PACKET_PLAYGAME_PLAYER_DIE:
	{
		sc_packet_player_anmation_start player_die;
		memcpy(&player_die, packet, packet[0]);
		CModelManager::GetInstance()->GetSkinnedInstanceModels()[player_die.id].mAlive = false;
	}
	break;
	case SC_PACKET_PLAYGAME_GAME_RESULT:
	{
		isGameOver = true;
		//sc_packet_game_result player_result;
		memcpy(&mGameResult, packet, packet[0]);
	}
	break;
	default:
		break;


	}
}