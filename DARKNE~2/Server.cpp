#include "stdafx.h"
#include "Server.h"
#include "ThreadFunctions.h"
#include "Player.h"
#include "NPC.h"
#include "Object.h"
#include "Timer.h"
#include "Obstacle.h"
#include "Clock.h"
#include "Room.h"

Server::Server()
{
	Init();
}


Server::~Server()
{
	Release();
}

void Server::LoadObstacle()
{
	// �ؽ�Ʈ ���Ϸ� ���� �о� �´�.
	ifstream fin("obstacle.txt");
	string ignore, name;
	float cX, cY, cZ, eX, eY, eZ;
	float sX, sY, sZ, tX, tY, tZ, rX, rY, rZ, rA;
	int type;

	


	for (int i = 0; i < MAX_OBSTACLE; ++i) {
		fin >> name;
		fin >> ignore >> tX >> tY >> tZ;
		fin >> ignore >> rX >> rY >> rZ >> rA;
		fin >> ignore >> sX >> sY >> sZ;
		fin >> ignore >> eX >> eY >> eZ;
		fin >> ignore >> cX >> cY >> cZ;
		fin >> ignore >> type;

		obstacles.push_back(new Obstacle);

		obstacles[i]->InitWorldMatrix(XMFLOAT3(1, 1, 1), XMFLOAT3(tX, tY, tZ), XMFLOAT4(rX, rY, rZ, rA));


		XMFLOAT4X4* a = &obstacles[i]->GetWorldMatrix();
		obstacles[i]->SetName(name);
		obstacles[i]->GetObb().Center = XMFLOAT3(cX, cY, cZ);

		obstacles[i]->GetObb().Extents = XMFLOAT3(eX, eY, eZ);

		obstacles[i]->GetObb().Orientation = XMFLOAT4(rX, rY, rZ, rA);
		obstacles[i]->SetObbType(type);


		XMVECTOR trasformV = XMVector3TransformCoord(XMLoadFloat3(&obstacles[i]->GetObb().Center), XMLoadFloat4x4(&obstacles[i]->GetWorldMatrix()));
		XMStoreFloat3(&obstacles[i]->GetObb().Center, trasformV);


	}

	cout << "Obstacle ���� �Ϸ�" << endl;

	
}

void  Server::CreateObjectPool()
{
	// �ؽ�Ʈ ���Ϸ� ���� �о� �´�.
	ifstream fin("player.txt");
	string ignore;
	float ctX, ctY, ctZ;
	float sX, sY, sZ, tX, tY, tZ, rX, rY, rZ, rA;
	
	Player temp[MAX_OBJECT];

	// 0~MAX_Object���� ������� �������ش�.
	for (int i = 0; i < MAX_OBJECT; i++) {
		// �ӽ� �迭�� ī�޶�, ��������� set���ش�.
		fin >> ignore;

		fin >> ignore >> tX >> tY >> tZ;
		fin >> ignore >> rX >> rY >> rZ >> rA;
		fin >> ignore >> sX >> sY >> sZ;

		fin >> ignore;
		fin >> ignore >> ctX >> ctY >> ctZ;
		// �ٲ�
		temp[i].InitWorldMatrix(XMFLOAT3(sX, sY, sZ), XMFLOAT3(tX, tY, tZ), XMFLOAT4(rX, rY, rZ, rA));
		temp[i].SetCamPos(XMFLOAT3(ctX, ctY, ctZ));
		temp[i].SetModel( rand()%2 );
	}
	// �����ش�.
	for (int i = 0 ; i < MAX_OBJECT; ++i) {
		
		//srand((unsigned)time(NULL));
		int j = rand() % MAX_OBJECT;
		Player p;
		// temp[i]�� temp[j]�� Swap
		p.SetWorldMatrix(temp[i].GetWorldMatrix());
		p.SetCamPos(temp[i].GetCamPos() );
		p.SetModel(temp[i].GetModel() );
		
		temp[i].SetWorldMatrix(temp[j].GetWorldMatrix());
		temp[i].SetCamPos(temp[j].GetCamPos());
		temp[i].SetModel(temp[j].GetModel());

		temp[j].SetWorldMatrix(p.GetWorldMatrix());
		temp[j].SetCamPos(p.GetCamPos());
		temp[j].SetModel(p.GetModel());
	}

	// �ִ� Object ������ŭ Reserve
	objects.reserve(MAX_OBJECT);

	// Object pool ����
	for (int i = 0; i < MAX_OBJECT; ++i) {
		if (i < Room::GetInstance()->GetMaxPlayer()) {
			// MAX Ŭ���̾�Ʈ�� ���� �ʱ�ȭ
			objects.push_back(new Player);
			objects[i]->SetID(i);
			objects[i]->SetWorldMatrix(temp[i].GetWorldMatrix());
			dynamic_cast<Player*>(objects[i])->SetCamPos( temp[i].GetCamPos() );
			objects[i]->SetModel( temp[i].GetModel() );


			objects[i]->UpdateSphereForObstacle();
			objects[i]->UpdateSphereForPlayer();
		}
		else{
			// NPC �ʱ�ȭ
			objects.push_back(new NPC);
			objects[i]->SetID(i);
			objects[i]->SetIsRender(true);
			objects[i]->SetWorldMatrix(temp[i].GetWorldMatrix());
			objects[i]->SetModel(temp[i].GetModel());

			objects[i]->UpdateSphereForObstacle();
			objects[i]->UpdateSphereForPlayer();

			
		}

	}
	cout << "Object Creating Success" << endl;
}



void Server::Init()
{
	
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return;

	// IOCP �ڵ鷯�� �ʱ�ȭ�Ѵ�.
	g_hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	if (!g_hIocp) return;

	Room::GetInstance()->Init();


	CreateObjectPool();
	LoadObstacle();
		
	// acceptThread ����
	acceptThread = new thread{ AcceptThread };

	SYSTEM_INFO si;
	GetSystemInfo(&si);
	for (int i = 0; i<(int)si.dwNumberOfProcessors * 2; i++) {
		workerThreads.push_back(new thread{ WorketThread });
	}


	timer_thread = thread{ Timer::TimerThread };

}
void Server::GameOver()
{
	cout << "������ �����մϴ�." << endl;

	sc_packet_game_result game_result;
	game_result.size = sizeof(game_result);
	game_result.type = SC_PACKET_PLAYGAME_GAME_RESULT;
	game_result.max_client = Room::GetInstance()->GetCurPlayer();

	for (int i = 0; i < MAX_CLIENT; ++i) {
		game_result.game_result[i].id = 0;
		game_result.game_result[i].NPCKill = 0;
		game_result.game_result[i].playerKill = 0;
		ZeroMemory(game_result.game_result[i].nickName, 20);

	}
	for (int n = 0; n < Room::GetInstance()->GetCurPlayer(); ++n) {
		game_result.game_result[n].id = n;
		game_result.game_result[n].NPCKill = dynamic_cast<Player*>(objects[n])->GetNPCKill();
		game_result.game_result[n].playerKill = dynamic_cast<Player*>(objects[n])->GetPlayerKill();

		
		wcscpy(game_result.game_result[n].nickName, dynamic_cast<Player*>(objects[n])->GetNickName() );


		cout << "[" << game_result.game_result[n].id << "] Client" << endl;
		cout << "NPC KILL : " << game_result.game_result[n].NPCKill << endl;
		cout << "PLAYER KILL : " << game_result.game_result[n].playerKill << endl << endl;
	}

	for (int m = 0; m < Room::GetInstance()->GetCurPlayer(); ++m) {
		SendPacket(m, reinterpret_cast<BYTE*>(&game_result));
	}

	
	for (int i = 0; i < Room::GetInstance()->GetCurPlayer(); ++i) {
		//if( true == dynamic_cast<Player*>(objects[i])->GetIsRender() )
			//closesocket(dynamic_cast<Player*>(objects[i])->GetSocket());
	}
	Release();
	Init();
}

void Server::Progress()
{
	Clock timer;
	timer.startTimer();



	while (true)
	{
		if (bIsgameOver) {
			gameOverTime += deltaTime;

			cout << gameOverTime << endl;
			if ( gameOverTime >= 30)
				GameOver();
		}

		if (timer.getElapsedTime() >= 1000 / FPS) {

			timer.lock.lock();
			deltaTime = timer.getElapsedTime()/1000.0f;
			timer.lock.unlock();

			timer.startTimer();


		}
		
	}
}



void Server::Release()
{
	WSACleanup();
	acceptThread->join();

	for (auto worket : workerThreads)
		worket->join();

	timer_thread.join();

	workerThreads.clear();
	CloseHandle(g_hIocp);

}

