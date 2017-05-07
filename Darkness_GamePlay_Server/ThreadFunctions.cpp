#include "stdafx.h"
#include "ThreadFunctions.h"
#include "Object.h"
#include "Obstacle.h"
#include "Player.h"
#include "NPC.h"
#include "Timer.h"

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
		cout << "[" << new_id << "] 님이 접속하셨습니다." << endl;

		if (client_sock == SOCKET_ERROR)
			break;
		if (new_id == MAX_CLIENT) {
			closesocket( client_sock );
			cout << "더이상 접속 할 수 없습니다." << endl;
			while (true);
		}


		// 접속된 클라이언트의 id, socket을 set하고 
		// IsRender를 true로 set 해준다.
		Player* player = dynamic_cast<Player*>(objects[new_id]);
		player->SetIsRender(true);
		player->SetSocket(client_sock);
		player->SetID(new_id);

		// 클라이언트가 접속하면 해당 클라이언트의 ID 값을 패킷으로 전달한다.
		sc_packet_put_user put_packet;
		put_packet.size = sizeof(put_packet);
		put_packet.type = SC_PACKET_PUT_USER;
		put_packet.id = new_id;
		SendPacket(new_id, reinterpret_cast<BYTE*>(&put_packet));

		// 클라이언트의 초기 위치를 Packet으로 전달.
		SendPlayerGamePutPacket(new_id, new_id);

		// 다른 클라이언트가 있다면.
		// 나의 정보를 해당 클라에게 보내고
		// 해당 클라의 정보를 나에게 보낸다.
		for (int i = 0; i < MAX_CLIENT; ++i) {
			if (true == objects[i]->GetIsRender() && new_id != i) {
				SendPlayerGamePutPacket(new_id, i);
				cout << i << "가 " << new_id << "에게 보냅니다" << endl;
				SendPlayerGamePutPacket(i, new_id);
				cout << new_id << "가 " << i << "에게 보냅니다" << endl;
			}
		}


		// 모든 NPC들의 정보를 접속한 클라이언트에게 보내준다.
		for (int i = MAX_CLIENT; i < MAX_OBJECT; ++i) {
				SendPlayerGamePutPacket(new_id, i);
				cout << i << "에 대한 정보를  " << new_id << "에게 보냅니다" << endl;
		}


		SendPlayerAnimationStartBrodcast(new_id, ANIMATION_STATE_IDLE);

		CreateIoCompletionPort(reinterpret_cast<HANDLE>(client_sock), g_hIocp, new_id, 0);
		player->RecvPacket();
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

		if (FALSE == ret_val) {
			cout << "[" << id << "] 의 비정상적인 접속해제" << endl;
			closesocket(dynamic_cast<Player*>(objects[id])->GetSocket());
			dynamic_cast<Player*>(objects[id])->SetIsRender(false);
			dynamic_cast<Player*>(objects[id])->Init();
			while (true);
		}
		if (0 == transferred) {
			cout << "[" << id << "] 접속해제" << endl;
			closesocket(dynamic_cast<Player*>(objects[id])->GetSocket());
			dynamic_cast<Player*>(objects[id])->SetIsRender(false);
			dynamic_cast<Player*>(objects[id])->Init();
			continue;
		}

		if (over_ex->command == NPC_MOVE)
		{
			NPC* npc = dynamic_cast<NPC*>(objects[id]);

			Timer::GetInstance()->AddTimer(id, NPC_MOVE, 1000);

			continue;
		}

		// recv일 경우 패킷을 읽어주고
		// 처리를 해준다.
		if (over_ex->operation == OP_RECV) {
			ReadPacket(id, transferred);
		}
		// send일 경우 overrapped_ex 구조체 포인터를
		// 해제 하여 준다. 
		else if (over_ex->operation == OP_SEND) {
			if (transferred != over_ex->wsa_buf.len) {
				cout << "send Incomplete Error" << endl;
				closesocket(dynamic_cast<Player*>(objects[id])->GetSocket());
				dynamic_cast<Player*>(objects[id])->SetIsRender(false);
			}
			delete over_ex;
		}
		else {
			cout << "Unknown GQCS event!" << endl;
			while (true);
		}
		



	}
}

// 해당 id에 대한 플레이어가 이 패킷을 읽고
// recv를 호출한다.
void ReadPacket(const UINT& id, const DWORD& transferred)
{

	dynamic_cast<Player*>(objects[id])->ReadPacket(id, transferred);
	dynamic_cast<Player*>(objects[id])->RecvPacket();
}

void PacketProcess(const UINT id, BYTE* packet)
{
	switch (packet[1])
	{
	case CS_UP:
	{
		dynamic_cast<Player*>(objects[id])->Move(CS_UP, id, packet);
		break;
	}
	case CS_DOWN:
	{
		dynamic_cast<Player*>(objects[id])->Move(CS_DOWN, id, packet);
		break;
	}
	case CS_RIGHT:
	{
		dynamic_cast<Player*>(objects[id])->Move(CS_RIGHT, id, packet);
		break;
	}
	case CS_LEFT:
	{
		dynamic_cast<Player*>(objects[id])->Move(CS_LEFT, id, packet);
		break;
	}
	case CS_PACKET_START_ANIMATION:
	{
		cs_packet_player_anmation_start ani_start;

		memcpy(&ani_start, packet, packet[0]);

		SendPlayerAnimationStartBrodcast(ani_start.id, ani_start.animationState);

	}
	default:
		break;

	}

	// 바뀐 값을 다른 Client에게 보낸다.
	for (int i = 0; i < MAX_CLIENT; ++i) {
		if (true == objects[i]->GetIsRender() && id != i) {
			SendPlayerGamePlayerPosPacket(i, id);
			//cout << id << "가 " << i << "에게 보냅니다" << endl;

		}
	}

}

void SendPlayerGamePutPacket(const UINT to, const UINT from)
{
	sc_packet_playgame_init_pos player_init;
	player_init.size = sizeof(player_init);
	player_init.type = SC_PACKET_PLAYGAME_INIT_POS;
	player_init.id = from;

	if( from < MAX_CLIENT )
		player_init.camPos = dynamic_cast<Player*>(objects[from])->GetCamPos();
	player_init.worldMatrix = objects[from]->GetWorldMatrix();

	SendPacket(to, reinterpret_cast<BYTE*>(&player_init));

}

void SendPlayerGamePlayerPosPacket(const UINT to, const UINT from)
{
	sc_packet_playgame_init_pos player_init;
	player_init.size = sizeof(player_init);
	player_init.type = SC_PACKET_PLAYGAME_PLAYER_POS;
	player_init.id = from;
	player_init.worldMatrix = objects[from]->GetWorldMatrix();

	SendPacket(to, reinterpret_cast<BYTE*>(&player_init));

}

void SendPlayerAnimationStartBrodcast(const UINT from, const UINT stateNum)
{
	sc_packet_player_anmation_start ani_start;
	ani_start.size = sizeof(ani_start);
	ani_start.type = SC_PACKET_PLAYGAME_START_ANMATION;
	ani_start.id = from;
	ani_start.animationState = stateNum;

	for (int i = 0; i < MAX_CLIENT; ++i) {
		if (true == objects[i]->GetIsRender() && from != i) {
			SendPacket(i, reinterpret_cast<BYTE*>(&ani_start));
			cout << from << "가 " << i << "에게 보냅니다" << endl;
		}
	}

}

void SendPacket(const UINT id, BYTE* packet)
{
	DWORD flags = 0;

	OVERLAPPED_EX* over_ex = new OVERLAPPED_EX;
	ZeroMemory(over_ex, sizeof(OVERLAPPED_EX));

	over_ex->wsa_buf.buf = reinterpret_cast<char*>(&over_ex->packet_buf);
	over_ex->wsa_buf.len = packet[0];
	over_ex->operation = OP_SEND;

	memcpy(over_ex->packet_buf, packet, packet[0]);

	Player* player = dynamic_cast<Player*>(objects[id]);

	int ret_val = WSASend(player->GetSocket(), &over_ex->wsa_buf, 1, NULL, flags, &over_ex->overlapped, NULL);

	if (ret_val != 0)
	{
		int error = WSAGetLastError();
		if (error != WSA_IO_PENDING)
			cout << " [error] WSASend() on SendPacket function " << endl;
	}
}

XMFLOAT3 operator-(XMFLOAT3& a, XMFLOAT3& b)

{
	return XMFLOAT3(a.x - b.x, a.y- b.y, a.z - b.z);
}
XMFLOAT3 operator+(XMFLOAT3& a, XMFLOAT3& b)

{
	return XMFLOAT3(a.x + b.x, a.y + b.y, a.z + b.z);
}


bool intersectAABBAABB(AABB* pAABB1, AABB* pAABB2)
{
	XMFLOAT3 vMin1 = (pAABB1->center - pAABB1->extent);
	XMFLOAT3 vMax1 = (pAABB1->center + pAABB1->extent);
	XMFLOAT3 vMin2 = (pAABB2->center - pAABB2->extent);
	XMFLOAT3 vMax2 = (pAABB2->center + pAABB2->extent);

	if (vMin1.x > vMax2.x) return(false);
	if (vMin1.y > vMax2.y) return(false);
	if (vMin1.z > vMax2.z) return(false);
	if (vMin2.x > vMax1.x) return(false);
	if (vMin2.y > vMax1.y) return(false);
	if (vMin2.z > vMax1.z) return(false);

	return true;

}
void ProcessEvent(const NPC_EVENT& e)
{
	Object* object = objects[e.id];
	ZeroMemory(&object->GetOverEx(), sizeof(object->GetOverEx()));

	switch (e.type) {
	case NPC_MOVE:
	{
		object->GetOverEx().command = NPC_MOVE;
		break;
	}

	default:
		break;
	}

	PostQueuedCompletionStatus(g_hIocp, 1, object->GetID(), (LPOVERLAPPED)&object->GetOverEx());
}