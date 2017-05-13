#include "stdafx.h"
#include "Server.h"
#include "ThreadFunctions.h"
#include "Player.h"
#include "NPC.h"
#include "Object.h"
#include "Timer.h"
#include "Obstacle.h"
#include "Clock.h"

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
	// 텍스트 파일로 부터 읽어 온다.
	ifstream fin("obstacle.txt");
	string ignore, name;
	float cX, cY, cZ, eX, eY, eZ;
	float sX, sY, sZ, tX, tY, tZ, rX, rY, rZ, rA;



	for (int i = 0; i < MAX_OBSTACLE; ++i) {
		fin >> name;
		fin >> ignore >> tX >> tY >> tZ;
		fin >> ignore >> rX >> rY >> rZ >> rA;
		fin >> ignore >> sX >> sY >> sZ;
		fin >> ignore >> eX >> eY >> eZ;
		fin >> ignore >> cX >> cY >> cZ;

		obstacles.push_back(new Obstacle);

		// 장애물의 월드행렬과 center, extent를 set

		if (name == "Building_b" || name == "Building_c" || name == "Building_d" 
			|| name == "Building_e" || name == "Tent" || name == "wallAABB" ) {

			obstacles[i]->InitWorldMatrix(XMFLOAT3(1, 1, 1), XMFLOAT3(tX, tY, tZ), XMFLOAT4(rX, rY, rZ, rA));


			XMFLOAT4X4* a = &obstacles[i]->GetWorldMatrix();
			obstacles[i]->SetName(name);
			obstacles[i]->GetObb().Center = XMFLOAT3(cX, cY, cZ);
			obstacles[i]->GetObb().Extents = XMFLOAT3(eX, eY, eZ);
			obstacles[i]->GetObb().Orientation = XMFLOAT4(rX, rY, rZ, rA);

			XMVECTOR trasformV = XMVector3TransformCoord(XMLoadFloat3(&obstacles[i]->GetObb().Center), XMLoadFloat4x4(&obstacles[i]->GetWorldMatrix()));
			XMStoreFloat3(&obstacles[i]->GetObb().Center, trasformV);
		}

		else if (name == "Sewers_entrance" || name == "Hay_b" || name == "Hay_c" || name == "wall" || name == "Building_f" 
			     || name == "Sack_a" || name == "Sack_b" || name == "Crate" || name == "Barrel" || name == "Well") {
			obstacles[i]->InitWorldMatrix(XMFLOAT3(sX, sY, sZ), XMFLOAT3(tX, tY, tZ), XMFLOAT4(rX, rY, rZ, rA));

			obstacles[i]->SetName(name);

			XMFLOAT4X4* a = &obstacles[i]->GetWorldMatrix();

			obstacles[i]->GetObb().Center = XMFLOAT3(cX, cY, cZ);
			obstacles[i]->GetObb().Extents = XMFLOAT3(eX, eY, eZ);
			obstacles[i]->GetObb().Orientation = XMFLOAT4(rX, rY, rZ, rA);

			XMVECTOR trasformV = XMVector3TransformCoord(XMLoadFloat3(&obstacles[i]->GetObb().Center), XMLoadFloat4x4(&obstacles[i]->GetWorldMatrix()));
			XMStoreFloat3(&obstacles[i]->GetObb().Center, trasformV);
		}
		else if( name == "angelStatue"){
			obstacles[i]->InitWorldMatrix(XMFLOAT3(1, 1, 1), XMFLOAT3(tX, tY, tZ), XMFLOAT4(rX, rY, rZ, rA));

			// 여기아니야 ------------------------------------
			obstacles[i]->SetName(name);
			// 여기아니야 ------------------------------------
			XMFLOAT4X4* a = &obstacles[i]->GetWorldMatrix();

			obstacles[i]->GetObb().Center = XMFLOAT3(cX, cY, cZ);
			obstacles[i]->GetObb().Extents = XMFLOAT3(eX, eY, eZ);
			obstacles[i]->GetObb().Orientation = XMFLOAT4(rX, rY, rZ, rA);

			XMVECTOR trasformV = XMVector3TransformCoord(XMLoadFloat3(&obstacles[i]->GetObb().Center), XMLoadFloat4x4(&obstacles[i]->GetWorldMatrix()));
			XMStoreFloat3(&obstacles[i]->GetObb().Center, trasformV);
		}
		else {
			obstacles[i]->InitWorldMatrix(XMFLOAT3(1, 1, 1), XMFLOAT3(tX, tY, tZ), XMFLOAT4(rX, rY, rZ, rA));

			// 여기아니야 ------------------------------------
			obstacles[i]->SetName(name);
			// 여기아니야 ------------------------------------
			XMFLOAT4X4* a = &obstacles[i]->GetWorldMatrix();

			obstacles[i]->GetObb().Center = XMFLOAT3(cZ, cY, cX);
			obstacles[i]->GetObb().Extents = XMFLOAT3(eZ, eY, eX);
			obstacles[i]->GetObb().Orientation = XMFLOAT4(rX, rY, rZ, rA);

			//XMVECTOR trasformV = XMVector3TransformCoord(XMLoadFloat3(&obstacles[i]->GetObb().Center), XMLoadFloat4x4(&obstacles[i]->GetWorldMatrix()));
			//XMStoreFloat3(&obstacles[i]->GetObb().Center, trasformV);

		}

	}


	
}

void  Server::CreateObjectPool()
{
	// 텍스트 파일로 부터 읽어 온다.
	ifstream fin("player.txt");
	string ignore;
	float ctX, ctY, ctZ;
	float sX, sY, sZ, tX, tY, tZ, rX, rY, rZ, rA;
	
	Player temp[MAX_OBJECT];

	// 0~MAX_Object까지 순서대로 대입해준다.
	for (int i = 0; i < MAX_OBJECT; i++) {
		// 임시 배열로 카메라, 월드행렬을 set해준다.
		fin >> ignore;

		fin >> ignore >> tX >> tY >> tZ;
		fin >> ignore >> rX >> rY >> rZ >> rA;
		fin >> ignore >> sX >> sY >> sZ;

		fin >> ignore;
		fin >> ignore >> ctX >> ctY >> ctZ;
		// 바꿔
		temp[i].InitWorldMatrix(XMFLOAT3(sX, sY, sZ), XMFLOAT3(tX, tY, tZ), XMFLOAT4(rX, rY, rZ, rA));
		temp[i].SetCamPos(XMFLOAT3(ctX, ctY, ctZ));
		temp[i].SetModel( rand()%2 );
	}
	// 섞어준다.
	for (int i = 0 ; i < MAX_OBJECT; ++i) {
		
		//srand((unsigned)time(NULL));
		int j = rand() % MAX_OBJECT;
		Player p;
		// temp[i]와 temp[j]의 Swap
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

	// 최대 Object 갯수만큼 Reserve
	objects.reserve(MAX_OBJECT);

	// Object pool 생성
	for (int i = 0; i < MAX_OBJECT; ++i) {
		if (i < MAX_CLIENT) {
			// MAX 클라이언트 = 32에 대한 초기화
			objects.push_back(new Player);
			objects[i]->SetID(i);
			objects[i]->SetWorldMatrix(temp[i].GetWorldMatrix());
			dynamic_cast<Player*>(objects[i])->SetCamPos( temp[i].GetCamPos() );
			objects[i]->SetModel( temp[i].GetModel() );


			objects[i]->UpdateSphereForObstacle();
			objects[i]->UpdateSphereForPlayer();
		}
		else if (i < MAX_CLIENT + MAX_NPC) {
			// 33 ~ 95에 대한 NPC 초기화
			objects.push_back(new NPC);
			objects[i]->SetID(i);
			objects[i]->SetIsRender(true);
			objects[i]->SetWorldMatrix(temp[i].GetWorldMatrix());
			objects[i]->SetModel(temp[i].GetModel());

			objects[i]->UpdateSphereForObstacle();
			objects[i]->UpdateSphereForPlayer();

			
		}

	}

}



void Server::Init()
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return;

	// IOCP 핸들러를 초기화한다.
	g_hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	if (!g_hIocp) return;

	CreateObjectPool();
	LoadObstacle();
		
	// acceptThread 생성
	acceptThread = new thread{ AcceptThread };

	SYSTEM_INFO si;
	GetSystemInfo(&si);
	for (int i = 0; i<(int)si.dwNumberOfProcessors * 2; i++) {
		workerThreads.push_back(new thread{ WorketThread });
	}


	timer_thread = thread{ Timer::TimerThread };

}
void Server::Progress()
{
	int last = GetTickCount64();
	int now = GetTickCount64();
	int startTick = GetTickCount64();
	
	Clock timer;
	timer.startTimer();



	while (true)
	{

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

