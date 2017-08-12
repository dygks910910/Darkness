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
	fin.close();
	// �����ش�.
	for (int i = 0 ; i < MAX_OBJECT; ++i) {
		
		srand((unsigned)time(NULL));
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
	cout << "Object Creating Success -> " << objects.size() <<  endl;
}



void Server::Init()
{
	
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return;

	// IOCP �ڵ鷯�� �ʱ�ȭ�Ѵ�.
	g_hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	if (!g_hIocp) return;

	Room::GetInstance()->Init();

	thread_running = true;

	CreateObjectPool();
	//LoadObstacle();
		
	// acceptThread ����
	acceptThread = thread{ AcceptThread };

	SYSTEM_INFO si;
	GetSystemInfo(&si);
	for (int i = 0; i<(int)si.dwNumberOfProcessors * 2; i++) {
		workerThreads.push_back(new thread{ WorketThread });
	}


	timer_thread = new thread{ Timer::TimerThread };

}
void Server::GameOver()
{
	cout << "������ �����մϴ�." << endl;

	// ��� NPC�� ����.
	for (int i = Room::GetInstance()->GetMaxPlayer(); i < MAX_OBJECT; ++i) {
		objects[i]->cs.lock();
		objects[i]->SetIsAlive(false);
		objects[i]->cs.unlock();
	}

	sc_packet_game_result game_result;
	game_result.size = sizeof(game_result);
	game_result.type = SC_PACKET_PLAYGAME_GAME_RESULT;
	game_result.max_client = Room::GetInstance()->GetCurPlayer();

	for (int i = 0; i < Room::GetInstance()->GetMaxPlayer(); ++i) {
		if (true == objects[i]->GetIsRender()) {
			game_result.game_result[i].id = 0;
			game_result.game_result[i].NPCKill = 0;
			game_result.game_result[i].playerKill = 0;
			ZeroMemory(game_result.game_result[i].nickName, 20);
		}

	}
	for (int n = 0; n < Room::GetInstance()->GetMaxPlayer(); ++n) {
		if (true == objects[n]->GetIsRender()) {

			game_result.game_result[n].id = n;
			game_result.game_result[n].NPCKill = dynamic_cast<Player*>(objects[n])->GetNPCKill();
			game_result.game_result[n].playerKill = dynamic_cast<Player*>(objects[n])->GetPlayerKill();

			wcscpy(game_result.game_result[n].nickName, dynamic_cast<Player*>(objects[n])->GetNickName());


			cout << "[" << game_result.game_result[n].id << "] Client" << endl;
			cout << "NPC KILL : " << game_result.game_result[n].NPCKill << endl;
			cout << "PLAYER KILL : " << game_result.game_result[n].playerKill << endl << endl;
		}
	}

	for (int m = 0; m < Room::GetInstance()->GetMaxPlayer(); ++m) {
		if (true == objects[m]->GetIsRender()) 
			SendPacket(m, reinterpret_cast<BYTE*>(&game_result));
	}


	



	
	
}

void Server::SendTime()
{
	sc_packet_time game_time;
	game_time.size = sizeof(sc_packet_time);
	game_time.type = SC_PACKET_TIME;
	game_time.min = gameRestTime / 60;
	game_time.sec = gameRestTime % 60;

	for (int i = 0; i < Room::GetInstance()->GetMaxPlayer(); ++i) {
		if (true == objects[i]->GetIsRender()) 
			SendPacket(i, reinterpret_cast<BYTE*>(&game_time));
	}
}


void Server::Progress()
{
	Clock timer;
	timer.startTimer();
	bool one = true;

	while (true)
	{
		// ������ ���»��°� �Ǹ�
		// Object�� �����ϰ� 
		// ���� flag�� false�� set.
		// ������ ���´� ��������, ���ӽ��� �Ѵ� false�� set
		if (true == gameReset) {

			bIsgameOver = false;
			bIsgameStart = false;

			ObjectReset();
			gameOverTime = 0;
			gameReset = false;
			gameRestTime = GAMETIME * 60;
			one = true;
		}
		// ������ ����Ǹ� �����ð� ������ ��� ��Ŷ��
		// ��� Ŭ���̾�Ʈ���� broadCast�Ѵ�.
		if ( true == bIsgameOver) {
			gameOverTime += deltaTime;

			if (gameOverTime >= 30 && true == one) {
				GameOver();
				one = false;
				
			}
		}

		// ������ ���۵Ǹ� TimeCount�� ���۵ǰ�
		// ��� Ŭ���̾�Ʈ�鿡�� �� ������ ��Ŷ�� ������.
		
		if (true == bIsgameStart) {
			// �ܿ� �ð��� 0�̵Ǹ� ������ ����
			if (0 == gameRestTime) {
				bIsgameOver = true;
			}

			// 1�ʸ��� ī��Ʈ
			if (1000 <= GetTickCount64() - gameStartTime ) {
				gameStartTime = GetTickCount64();
				SendTime();
				gameRestTime -= 1; // �ܿ��ð��� 1�ʾ� ����
			}
				
		}


		// deltaTime�� ���Ѵ�.
		if (timer.getElapsedTime() >= 1000 / FPS) {

			timer.lock.lock();
			deltaTime = timer.getElapsedTime()/1000.0f;
			timer.lock.unlock();

			timer.startTimer();


		}
		
	}
}


void Server::ObjectReset()
{
	//// ������Ʈ ����
	//for (int i = Room::GetInstance()->GetMaxPlayer(); i < MAX_OBJECT; ++i) {
	//	objects[i]->cs.lock();
	//	objects[i]->SetIsAlive(false);
	//	objects[i]->cs.unlock();
	//}
	thread_running = false;

	for (auto object : objects)
	{
		delete object;
		object = nullptr;
	}
	objects.clear();

	for (auto obstacle : obstacles)
	{
		delete obstacle;
		obstacle = nullptr;
	}
	obstacles.clear();


	CreateObjectPool();

	Room::GetInstance()->Init();
	

}
void Server::Release()
{
	WSACleanup();

	for (int i = Room::GetInstance()->GetMaxPlayer(); i < MAX_OBJECT; ++i) {
		objects[i]->cs.lock();
		objects[i]->SetIsAlive(false);
		objects[i]->cs.unlock();
	}

	timer_thread->join();

	SYSTEM_INFO si;
	GetSystemInfo(&si);
	for (int i = 0; i<(int)si.dwNumberOfProcessors * 2; i++) {
		PostQueuedCompletionStatus(g_hIocp, 0, 0, nullptr);
	}

	while (true)
	{
		SOCKET tmp_sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);

		SOCKADDR_IN server_addr;
		ZeroMemory(&server_addr, sizeof(SOCKADDR_IN));
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(SERVERPORT);
		server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

		int retval = WSAConnect(tmp_sock, reinterpret_cast<SOCKADDR*>(&server_addr), sizeof(server_addr), NULL, NULL, NULL, NULL);
		if (SOCKET_ERROR != retval)
			break;
	}


	for (auto worker : workerThreads) {
		worker->join();
		delete worker;
	}
	workerThreads.clear();
	
	acceptThread.join();
	

	for (auto object : objects)
	{
		delete object;
		object = nullptr;
	}
	objects.clear();

	for (auto obstacle : obstacles)
	{
		delete obstacle;
		obstacle = nullptr;
	}
	obstacles.clear();

	
	CloseHandle(g_hIocp);

}

