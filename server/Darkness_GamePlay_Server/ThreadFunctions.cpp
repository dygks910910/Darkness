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
		cout << "[" << new_id << "] ���� �����ϼ̽��ϴ�." << endl;

		if (client_sock == SOCKET_ERROR)
			break;
		if (new_id == MAX_CLIENT) {
			closesocket(client_sock);
			cout << "���̻� ���� �� �� �����ϴ�." << endl;
			while (true);
		}


		// ���ӵ� Ŭ���̾�Ʈ�� id, socket�� set�ϰ� 
		// IsRender�� true�� set ���ش�.
		Player* player = dynamic_cast<Player*>(objects[new_id]);
		player->SetIsRender(true);
		player->SetSocket(client_sock);
		player->SetID(new_id);

		// Ŭ���̾�Ʈ�� �����ϸ� �ش� Ŭ���̾�Ʈ�� ID ���� ��Ŷ���� �����Ѵ�.
		sc_packet_put_user put_packet;
		put_packet.size = sizeof(put_packet);
		put_packet.type = SC_PACKET_PUT_USER;
		put_packet.id = new_id;
		SendPacket(new_id, reinterpret_cast<BYTE*>(&put_packet));

		// Ŭ���̾�Ʈ�� �ʱ� ��ġ�� Packet���� ����.
		SendPlayerGamePutPacket(new_id, new_id);




		// �ٸ� Ŭ���̾�Ʈ�� �ִٸ�.
		// ���� ������ �ش� Ŭ�󿡰� ������
		// �ش� Ŭ���� ������ ������ ������.
		for (int i = 0; i < MAX_CLIENT; ++i) {
			if (true == objects[i]->GetIsRender() && new_id != i) {
				SendPlayerGamePutPacket(new_id, i);
				cout << i << "�� " << new_id << "���� �����ϴ�" << endl;
				SendPlayerGamePutPacket(i, new_id);
				cout << new_id << "�� " << i << "���� �����ϴ�" << endl;
			}
		}


		// ��� NPC���� ������ ������ Ŭ���̾�Ʈ���� �����ش�.
		for (int i = MAX_CLIENT; i < MAX_OBJECT; ++i) {
			SendPlayerGamePutPacket(new_id, i);
			cout << i << "�� ���� ������  " << new_id << "���� �����ϴ�" << endl;
		}


		SendPlayerAnimationStartBrodcast(new_id, ANIMATION_STATE_IDLE);

		CreateIoCompletionPort(reinterpret_cast<HANDLE>(client_sock), g_hIocp, new_id, 0);
		player->RecvPacket();
		++new_id;
		++playingClient;

		if (MAX_CLIENT == playingClient) {

			// NPC���� �̺�Ʈ�� �߰�.
			for (int i = MAX_CLIENT; i < MAX_OBJECT; ++i) {

				int moveTime = rand() % 5 + 1;
				int tickTime = (moveTime / deltaTime) / FPS;


				// �Ǽ��� ���� �����Ͽ� NPC�� CamLook�� �־��ִ� �κ�
				int sign = rand() % 2;
				double max = 32767;
				double x;
				double z;

				if (0 == sign) {
					x = (rand() / max);
					z = (rand() / max);
				}
				else {
					x = -(rand() / max);
					z = -(rand() / max);
				}

				dynamic_cast<NPC*>(objects[i])->SetCamLook(XMFLOAT3(x, -0.289194, z));

				dynamic_cast<NPC*>(objects[i])->SetMoveTickTime(tickTime);
				dynamic_cast<NPC*>(objects[i])->SetMoveTime(moveTime * 1000);
				Timer::GetInstance()->AddTimer(i, NPC_MOVE, tickTime);



			}


		}


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
			cout << "[" << id << "] �� ���������� ��������" << endl;
			closesocket(dynamic_cast<Player*>(objects[id])->GetSocket());
			dynamic_cast<Player*>(objects[id])->SetIsRender(false);
			dynamic_cast<Player*>(objects[id])->Init();
			while (true);
		}
		if (0 == transferred) {
			cout << "[" << id << "] ��������" << endl;
			closesocket(dynamic_cast<Player*>(objects[id])->GetSocket());
			dynamic_cast<Player*>(objects[id])->SetIsRender(false);
			dynamic_cast<Player*>(objects[id])->Init();
			continue;
		}

		if (over_ex->command == NPC_MOVE) {

			NPC* npc = dynamic_cast<NPC*>(objects[id]);

			if (true == objects[id]->GetIsAlive()) {

				// �̵� �� �浹 ���� �ϴ� �Լ�
				npc->HeartBeat();

				if (npc->GetMoveTickCount() == 0)
					SendPlayerAnimationStartBrodcast(id, ANIMATION_STATE_WORK);


				npc->SetMoveTickCount(npc->GetMoveTickCount() + npc->GetMoveTickTime());


				if (npc->GetMoveTime() <= npc->GetMoveTickCount()) {
					int restTime = rand() % 5 + 1;
					npc->SetRestTime(restTime * 1000);



					Timer::GetInstance()->AddTimer(id, NPC_REST, restTime * 1000);
					npc->SetMoveTickCount(0);

					SendPlayerAnimationStartBrodcast(id, ANIMATION_STATE_IDLE);

				}

				else
					Timer::GetInstance()->AddTimer(id, NPC_MOVE, npc->GetMoveTickTime());
			}
			continue;
		}
		else if (over_ex->command == NPC_REST) {
			NPC* npc = dynamic_cast<NPC*>(objects[id]);

			if (true == npc->GetIsAlive()) {


				int moveTime = rand() % 5 + 1;
				int tickTime = (moveTime / deltaTime) / FPS;

				int sign = rand() % 2;
				double max = 32767;
				double x;
				double z;

				if (0 == sign) {
					x = (rand() / max);
					z = (rand() / max);
				}
				else {
					x = -(rand() / max);
					z = -(rand() / max);
				}



				dynamic_cast<NPC*>(objects[id])->SetCamLook(XMFLOAT3(x, -0.289194, z));

				dynamic_cast<NPC*>(objects[id])->SetMoveTickTime(tickTime);
				dynamic_cast<NPC*>(objects[id])->SetMoveTime(moveTime * 1000);
				Timer::GetInstance()->AddTimer(id, NPC_MOVE, tickTime);

			}
			continue;
		}
		// recv�� ��� ��Ŷ�� �о��ְ�
		// ó���� ���ش�.
		if (over_ex->operation == OP_RECV) {
			ReadPacket(id, transferred);
		}
		// send�� ��� overrapped_ex ����ü �����͸�
		// ���� �Ͽ� �ش�. 
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

// �ش� id�� ���� �÷��̾ �� ��Ŷ�� �а�
// recv�� ȣ���Ѵ�.
void ReadPacket(const UINT& id, const DWORD& transferred)
{

	dynamic_cast<Player*>(objects[id])->ReadPacket(id, transferred);
	dynamic_cast<Player*>(objects[id])->RecvPacket();
}

void PacketProcess(const UINT id, BYTE* packet)
{
	int packetNum = 0;

	switch (packet[1])
	{
	case CS_PACKET_CLIENT_NICKNAME:
	{
		packetNum = CS_PACKET_CLIENT_NICKNAME;

		cs_packet_player_nickname nick;
		memcpy(&nick, packet, packet[0]);

		dynamic_cast<Player*>(objects[nick.id])->SetNickName(nick.nickName);
		wcout << nick.nickName << endl;
		break;
	}
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

		if (ANIMATION_STATE_RUN == ani_start.animationState)
			objects[ani_start.id]->SetSpeed(objects[ani_start.id]->GetSpeedSave() * 1.8);

		else if (ANIMATION_STATE_WORK == ani_start.animationState)
			objects[ani_start.id]->SetSpeed(objects[ani_start.id]->GetSpeedSave());

		else if (ANIMATION_STATE_ATTACK == ani_start.animationState) {

			for (int i = 0; i < MAX_OBJECT; ++i) {
				if (true == IntersectSphereSphere(&objects[ani_start.id]->GetSphereForPlayer(), &objects[i]->GetSphereForPlayer())
					&& ani_start.id != i && objects[i]->GetIsAlive() ) {


					// �ǰݴ��� ������Ʈ�� Client���?
					if (i < MAX_CLIENT) {

						dynamic_cast<Player*>(objects[ani_start.id])->
							SetPlayerKill(dynamic_cast<Player*>(objects[ani_start.id])->GetPlayerKill() + 1);


						objects[i]->cs.lock();
						objects[i]->SetIsAlive(false);
						objects[i]->cs.unlock();

						// �ش� Ŭ���̾�Ʈ���� �׾��ٰ� �˸��� ��Ŷ
						sc_packet_player_die player_die;
						player_die.size = sizeof(player_die);
						player_die.type = SC_PACKET_PLAYGAME_PLAYER_DIE;
						player_die.id = i;
						SendPacket(i, reinterpret_cast<BYTE*>(&player_die));


						SendPlayerAnimationStartBrodcast(i, ANIMATION_STATE_DIE);

						int curPlayerCnt = 0;
						// for���� ���� Ŭ���̾�Ʈ�� ������ ���� 1�̸� ���� ����!
						for (int j = 0; j < MAX_CLIENT; ++j) {
							if (true == objects[j]->GetIsAlive())
								++curPlayerCnt;
						}
						

						// ���� ����
						if (1 == curPlayerCnt) {
							
							cout << "������ �����մϴ�." << endl;

							sc_packet_game_result game_result;  
							game_result.size = sizeof(game_result);
							game_result.type = SC_PACKET_PLAYGAME_GAME_RESULT;
							game_result.max_client = MAX_CLIENT;
							
							for (int n = 0; n < MAX_CLIENT; ++n) {
								game_result.game_result[n].id = n;
								wcscpy(game_result.game_result[n].nickName, dynamic_cast<Player*>(objects[n])->GetNickName() );
								game_result.game_result[n].NPCKill = dynamic_cast<Player*>(objects[n])->GetNPCKill();
								game_result.game_result[n].playerKill = dynamic_cast<Player*>(objects[n])->GetPlayerKill();
							}

							for (int m = 0; m < MAX_CLIENT; ++m) {
								SendPacket(m, reinterpret_cast<BYTE*>(&game_result));
							}

						}

					}
					// ���Ͷ��
					else {
						dynamic_cast<Player*>(objects[ani_start.id])->
							SetNPCKill(dynamic_cast<Player*>(objects[ani_start.id])->GetNPCKill() + 1);

						// NPC�۾��� ������Ű��

						objects[i]->cs.lock();
						objects[i]->SetIsAlive(false);
						objects[i]->cs.unlock();

						SendPlayerAnimationStartBrodcast(i, ANIMATION_STATE_DIE);

						// �÷��̾� 1�� ����ִٸ�. ���� ������ �ڵ� �ϴ� ����
					}


				}
			}




		}
		objects[ani_start.id]->SetAnimationState(ani_start.animationState);
		SendPlayerAnimationStartBrodcast(ani_start.id, ani_start.animationState);
	}
	break;


	}

	if (CS_PACKET_CLIENT_NICKNAME != packetNum) {
		// �ٲ� ���� �ٸ� Client���� ������.
		for (int i = 0; i < MAX_CLIENT; ++i) {
			if (true == objects[i]->GetIsRender() && id != i) {
				SendPlayerGamePlayerPosPacket(i, id);
				//cout << id << "�� " << i << "���� �����ϴ�" << endl;
			}
		}
	}
}
void SendPlayerGamePutPacket(const UINT to, const UINT from)
{
	sc_packet_playgame_init_pos player_init;
	player_init.size = sizeof(player_init);
	player_init.type = SC_PACKET_PLAYGAME_INIT_POS;
	player_init.id = from;

	if (from < MAX_CLIENT)
		player_init.camPos = dynamic_cast<Player*>(objects[from])->GetCamPos();
	player_init.worldMatrix = objects[from]->GetWorldMatrix();

	SendPacket(to, reinterpret_cast<BYTE*>(&player_init));

}

void SendPlayerGamePlayerPosPacket(const UINT to, const UINT from)
{
	sc_packet_playgame_player_pos player_pos;
	player_pos.size = sizeof(player_pos);
	player_pos.type = SC_PACKET_PLAYGAME_PLAYER_POS;
	player_pos.id = from;
	player_pos.camMove = XMFLOAT3(0, 0, 0);
	player_pos.worldMatrix = objects[from]->GetWorldMatrix();

	SendPacket(to, reinterpret_cast<BYTE*>(&player_pos));

}

void SendPlayerAnimationStartBrodcast(const UINT from, const UINT stateNum)
{
	sc_packet_player_animation_start ani_start;
	ani_start.size = sizeof(ani_start);
	ani_start.type = SC_PACKET_PLAYGAME_START_ANMATION;
	ani_start.id = from;
	ani_start.animationState = stateNum;


	for (int i = 0; i < MAX_CLIENT; ++i) {
		if (true == objects[i]->GetIsRender() && from != i) {
			SendPacket(i, reinterpret_cast<BYTE*>(&ani_start));
			//cout << from << "�� " << i << "���� �����ϴ�" << endl;
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
	return XMFLOAT3(a.x - b.x, a.y - b.y, a.z - b.z);
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

	case NPC_REST:
	{
		object->GetOverEx().command = NPC_REST;

	}

	default:
		break;
	}

	PostQueuedCompletionStatus(g_hIocp, 1, object->GetID(), (LPOVERLAPPED)&object->GetOverEx());
}


