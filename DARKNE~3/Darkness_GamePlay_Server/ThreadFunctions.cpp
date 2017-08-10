#include "stdafx.h"
#include "ThreadFunctions.h"
#include "Object.h"
#include "Obstacle.h"
#include "Player.h"
#include "NPC.h"
#include "Timer.h"
#include "Room.h"

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

		//if (thread_running == false)
		//{
			//closesocket(client_sock);
			//break;
		//}

		// id���� 0~8���� ������ ��� �ִ� ������ set���ش�.
		// MAXPLAYER ��ŭ
		for (int i = 0; i < Room::GetInstance()->GetMaxPlayer(); ++i) {
			if (false == objects[i]->GetIsRender()) {
				new_id = i;
				break;
			}
		}

		// ���� ���ο��� ���Ӱ����ο��� ������ ���� �Ұ���.
		if (Room::GetInstance()->GetCurPlayer() == Room::GetInstance()->GetMaxPlayer() ) {
			closesocket(client_sock);
			cout << "���̻� ���� �� �� �����ϴ�." << endl;
			continue;
		}
		
		if (true == bIsgameStart) {
			closesocket(client_sock);
			cout << "������ �̹� ���۵Ǿ� ���� �� �� �����ϴ�." << endl;
			continue;

		}

		cout << "[" << new_id << "] ���� �����ϼ̽��ϴ�." << endl;

		WORD CheckCreater = 0;

		// ���ο��� 0���̸� �ش� Player�� ����
		if (0 == Room::GetInstance()->GetCurPlayer()) {
			Room::GetInstance()->SetRoomCreater(new_id);
			CheckCreater = 1;
		}
		// ����� �� +1�� ���ش�.
		Room::GetInstance()->SetCurPlayer(Room::GetInstance()->GetCurPlayer() + 1);

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
		put_packet.IsCreater = CheckCreater;
		SendPacket(new_id, reinterpret_cast<BYTE*>(&put_packet));


		CreateIoCompletionPort(reinterpret_cast<HANDLE>(client_sock), g_hIocp, new_id, 0);
		player->RecvPacket();

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

		//if (thread_running == false)
			//break;

		if (FALSE == ret_val) {
			cout << "[" << id << "] �� ���������� ��������" << endl;
			closesocket(dynamic_cast<Player*>(objects[id])->GetSocket());
			
			dynamic_cast<Player*>(objects[id])->cs.lock();
			dynamic_cast<Player*>(objects[id])->SetIsRender(false);
			dynamic_cast<Player*>(objects[id])->cs.unlock();

			dynamic_cast<Player*>(objects[id])->Init();
			Room::GetInstance()->SetCurPlayer(Room::GetInstance()->GetCurPlayer() - 1);
			
			// remove ��Ŷ�� ������.
			sc_packet_player_remove remove_player;
			remove_player.size = sizeof(remove_player);
			remove_player.type = SC_PACKET_PLAYER_REMOVE;
			remove_player.id = id;

			int playerCnt = 0;
			for (int i = 0; i < Room::GetInstance()->GetMaxPlayer(); ++i) {
				if (true == objects[i]->GetIsRender()) {
					++playerCnt;
					SendPacket(i, reinterpret_cast<BYTE*>(&remove_player));
				}
			}
			// �����߿� �÷��̾ 1���� ���� �Ǹ�
			// ������ ���� �ȴ�.
			if (1 == playerCnt && true == bIsgameStart) {
				bIsgameOver = true;

			}

			// ������ ����ǰ� Ŭ���̾�Ʈ�� release�� ȣ���ϸ�
			// ������ ���»��·� ���ư����� ó��.
			else if (0 == playerCnt && true == bIsgameOver) {
				gameReset = true;
			}
			continue;
		}
		if (0 == transferred) {
			cout << "[" << id << "] ��������" << endl;
			closesocket(dynamic_cast<Player*>(objects[id])->GetSocket());

			dynamic_cast<Player*>(objects[id])->cs.lock();
			dynamic_cast<Player*>(objects[id])->SetIsRender(false);
			dynamic_cast<Player*>(objects[id])->cs.unlock();


			dynamic_cast<Player*>(objects[id])->Init();
			Room::GetInstance()->SetCurPlayer(Room::GetInstance()->GetCurPlayer() - 1);
			
			// remove ��Ŷ�� ������.
			sc_packet_player_remove remove_player;
			remove_player.size = sizeof(remove_player);
			remove_player.type = SC_PACKET_PLAYER_REMOVE;
			remove_player.id = id;

			int playerCnt = 0;
			for (int i = 0; i < Room::GetInstance()->GetMaxPlayer(); ++i) {
				if (true == objects[i]->GetIsRender()) {
					++playerCnt;
					SendPacket(i, reinterpret_cast<BYTE*>(&remove_player));
				}
			}
			// �����߿� �÷��̾ 1���� ���� �Ǹ�
			// ������ ���� �ȴ�.
			if (1 == playerCnt && true == bIsgameStart) {
				bIsgameOver = true;

			}
			// ������ ����ǰ� Ŭ���̾�Ʈ�� release�� ȣ���ϸ�
			// ������ ���»��·� ���ư����� ó��.
			else if (0 == playerCnt && true == bIsgameOver) {
				gameReset = true;
			}
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
					int restTime = rand() % 7 + 2;
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
				int tickTime = 1000 / FPS;
				

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

				XMVECTOR tempDir = XMVectorSet(x, 0, z,0);
				tempDir = XMVector3Normalize(tempDir);
				XMFLOAT3 tempfloat3;
				XMStoreFloat3(&tempfloat3, tempDir);


				dynamic_cast<NPC*>(objects[id])->SetCamLook(tempfloat3);
				

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

		// �������� �ο��鿡��
		// �г����� �Ѹ���.
		sc_packet_room_data room_data;
		room_data.size = sizeof(room_data);
		room_data.type = SC_PACKET_ROOM_DATA;
		room_data.id = nick.id;
		wcscpy(room_data.nickName, nick.nickName);

		for (int i = 0; i < Room::GetInstance()->GetMaxPlayer(); ++i) {

			if (true == objects[i]->GetIsRender() && i != nick.id) {
				SendPacket(i, reinterpret_cast<BYTE*>(&room_data));
				cout << "[" << i << "]" << " ���� " << "[" << nick.id << "]" << " �� �г��� ������ �����ϴ�." << endl;
			}
			
			
		}

		// ������ �ο����� 
		// ���� Ŭ���̾�Ʈ���� �г����� �Ѹ���.
		for (int i = 0; i < Room::GetInstance()->GetMaxPlayer(); ++i) {
			if (true == objects[i]->GetIsRender() && i != nick.id) {
				sc_packet_room_data room_data;
				room_data.size = sizeof(room_data);
				room_data.type = SC_PACKET_ROOM_DATA;
				room_data.id = i;
				wcscpy(room_data.nickName, dynamic_cast<Player*>(objects[i])->GetNickName());
				
				SendPacket(nick.id, reinterpret_cast<BYTE*>(&room_data));
				cout << "[" << nick.id << "]" << " ���� " << "["  << i << "]"  <<  " �� �г��� ������ �����ϴ�." << endl;

			}
		}


		break;
	}
	case CS_PACKET_CREATER_START_CLICK:
	{
		packetNum = CS_PACKET_CREATER_START_CLICK;
		// ������ Start�� ���� ������ �����ߴٰ� �������� �˸� ����
		// ������ �ƴ� ��� Ŭ���̾�Ʈ�鿡�� 
		// ������ �˸�
		sc_packet_game_start game_start;
		game_start.size = sizeof(game_start);
		game_start.type = SC_PACKET_GAME_START;

		cout << "������ ������ �����Ͽ����ϴ�." << endl;

		for (int i = 0; i < Room::GetInstance()->GetMaxPlayer(); ++i) {
			//if( i != Room::GetInstance()->GetRoomCreater() )
			if (true == objects[i]->GetIsRender() )
				SendPacket(i, reinterpret_cast<BYTE*>(&game_start));

		}
		break;
	}
	case CS_PACKET_CLIENT_DRAW_START:
	{
		packetNum = CS_PACKET_CLIENT_DRAW_START;

		// �ش� Ŭ���̾�Ʈ���� �ڽ��� ��ġ ������ �˸���.
		SendPlayerGamePutPacket(id, id);
		cout << "Player [" << id << "]���� [" << id << "]�� ���� �ʱ� ��ġ ������ �����ϴ�." << endl;

		// �ش� Ŭ���̾�Ʈ���� �ٸ� Ŭ���̾�Ʈ�� ��ġ ������ ������.
		for (int i = 0; i < Room::GetInstance()->GetMaxPlayer(); ++i) {
			if (id != i && true == objects[i]->GetIsRender()) {
				SendPlayerGamePutPacket(id, i);
				cout << "Player [" << id << "]���� [" << i << "]�� ���� �ʱ� ��ġ ������ �����ϴ�." << endl;
			}
		}
		SendPlayerAnimationStartBrodcast(id, ANIMATION_STATE_IDLE);

		//�ٸ� ��� NPC���� �ʱ� ��ġ�� ������.
		for (int j = Room::GetInstance()->GetMaxPlayer(); j < MAX_OBJECT; ++j) {
			SendPlayerGamePutPacket(id, j);
			cout << "NPC [" << j << "]" << " �� ���� ������  " << "[" << id << "] ���� �����ϴ�" << endl;
		}

		// NPC �̺�Ʈ ����.
		Room::GetInstance()->SetReadyCount(Room::GetInstance()->GetReadyCount() + 1 );


		if(Room::GetInstance()->GetReadyCount() == Room::GetInstance()->GetCurPlayer() ){
			
			Room::GetInstance()->LoadObstacles();

			if (false == thread_running) {
				thread_running = true;
			}


			// �۷ι� Ÿ�̸� ����
			sc_packet_timer_start timer_start;
			timer_start.size = sizeof(timer_start);
			timer_start.type = SC_PACKET_PLAYGAME_TIMER_START;

			for (int i = 0; i < Room::GetInstance()->GetMaxPlayer(); ++i)
				if ( true == objects[i]->GetIsRender() )
					SendPacket(i, reinterpret_cast<BYTE*>(&timer_start));
			
			// �۷ι� Ÿ�̸� ����.
			gameStartTime = GetTickCount64();
			bIsgameStart = true;
			cout << "timer Count�� ���� �˴ϴ�." << endl;

		// NPC���� �̺�Ʈ�� �߰�.
			for (int i = Room::GetInstance()->GetMaxPlayer(); i < MAX_OBJECT; ++i) {
				int moveTime = rand() % 5 + 1;
				int tickTime = 1000 / FPS;


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

				cout << i << "��° NPC Setting �Ϸ�" << endl;


			}

			
		}
		break;
	}
	case CS_UP:
	{
		packetNum = CS_UP;

		dynamic_cast<Player*>(objects[id])->Move(CS_UP, id, packet);
		break;
	}
	case CS_DOWN:
	{
		packetNum = CS_DOWN;
		dynamic_cast<Player*>(objects[id])->Move(CS_DOWN, id, packet);
		break;
	}
	case CS_RIGHT:
	{
		packetNum = CS_RIGHT;
		dynamic_cast<Player*>(objects[id])->Move(CS_RIGHT, id, packet);
		break;
	}
	case CS_LEFT:
	{
		packetNum = CS_LEFT;
		dynamic_cast<Player*>(objects[id])->Move(CS_LEFT, id, packet);
		break;
	}
	case CS_PACKET_START_ANIMATION:
	{
		packetNum = CS_PACKET_START_ANIMATION;
		cs_packet_player_animation_start ani_start;

		memcpy(&ani_start, packet, packet[0]);

		if (ANIMATION_STATE_RUN == ani_start.animationState)
			objects[ani_start.id]->SetSpeedSave(objects[ani_start.id]->GetSpeed() * 10);

		else if (ANIMATION_STATE_WORK == ani_start.animationState)
			objects[ani_start.id]->SetSpeedSave(objects[ani_start.id]->GetSpeed() * 2);

		else if (ANIMATION_STATE_ATTACK == ani_start.animationState) {

			for (int i = 0; i < MAX_OBJECT; ++i) {
				if (true == IntersectSphereSphere(&objects[ani_start.id]->GetSphereForPlayer(), &objects[i]->GetSphereForPlayer())
					&& ani_start.id != i && objects[i]->GetIsAlive() ) {


					// �ǰݴ��� ������Ʈ�� Client���?
					if (i < Room::GetInstance()->GetMaxPlayer()) {

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
						for (int j = 0; j < Room::GetInstance()->GetMaxPlayer(); ++j) {
							if (true == objects[j]->GetIsAlive() && true == objects[j]->GetIsRender())
								++curPlayerCnt;
						}
						

						// ���� ����
						if (1 == curPlayerCnt) {
							
							bIsgameOver = true;

							

						}

					}
					// ���Ͷ��
					else {
						dynamic_cast<Player*>(objects[ani_start.id])->
							SetNPCKill(dynamic_cast<Player*>(objects[ani_start.id])->GetNPCKill() + 1);


						objects[i]->cs.lock();
						objects[i]->SetIsAlive(false);
						objects[i]->cs.unlock();

						SendPlayerAnimationStartBrodcast(i, ANIMATION_STATE_DIE);

					}


				}
			}




		}
		objects[ani_start.id]->SetAnimationState(ani_start.animationState);
		SendPlayerAnimationStartBrodcast(ani_start.id, ani_start.animationState);
	}
	break;


	}
	//cout << "packetNum -> " << packetNum << endl;
	if (CS_PACKET_CLIENT_NICKNAME > packetNum && packetNum > 0 ) {
		// �ٲ� ���� �ٸ� Client���� ������.
		for (int i = 0; i < Room::GetInstance()->GetMaxPlayer(); ++i) {
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

	if (true == objects[from]->GetIsRender())
		player_init.isRender = 1;
	else
		player_init.isRender = 0;

	if (from < Room::GetInstance()->GetMaxPlayer() && from == to ) {
		player_init.camPos = dynamic_cast<Player*>(objects[from])->GetCamPos();
		cout << from << " " << dynamic_cast<Player*>(objects[from])->GetCamPos().x << endl;
		cout << from << " " << dynamic_cast<Player*>(objects[from])->GetCamPos().y << endl;
		cout << from << " " << dynamic_cast<Player*>(objects[from])->GetCamPos().z << endl;
	}
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
	ani_start.type = SC_PACKET_PLAYGAME_START_ANIMATION;
	ani_start.id = from;
	ani_start.animationState = stateNum;


	for (int i = 0; i < Room::GetInstance()->GetMaxPlayer(); ++i) {
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


