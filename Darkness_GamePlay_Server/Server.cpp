#include "stdafx.h"
#include "Server.h"
#include "ThreadFunctions.h"
#include "Player.h"
#include "NPC.h"
#include "Object.h"
#include "Timer.h"
#include "Obstacle.h"
#include "MathHelper.h"

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
	string ignore , name;
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

		// ��ֹ��� ������İ� center, extent�� set
		obstacles[i]->InitWorldMatrix(XMFLOAT3(1, 1, 1), XMFLOAT3(tX, tY, tZ), XMFLOAT4(rX, rY, rZ, rA));
		obstacles[i]->GetAABB().extent = XMFLOAT3(eX, eY, eZ);
		obstacles[i]->GetAABB().center = XMFLOAT3(cX, cY, cZ);


		XMFLOAT4X4* a = &obstacles[i]->GetWorldMatrix();

		// min, max�� ���ϰ� �ش� ���� ��������� �����ش�.
		XMFLOAT3 vMin1 = obstacles[i]->GetAABB().center - obstacles[i]->GetAABB().extent;
		XMFLOAT3 vMax1 = obstacles[i]->GetAABB().center + obstacles[i]->GetAABB().extent;
		
		XMVECTOR trasformV = XMVector3TransformCoord(XMLoadFloat3(&vMin1), XMLoadFloat4x4(&obstacles[i]->GetWorldMatrix()));
		XMStoreFloat3(&vMin1, trasformV);

		trasformV = XMVector3TransformCoord(XMLoadFloat3(&vMax1), XMLoadFloat4x4(&obstacles[i]->GetWorldMatrix()));
		XMStoreFloat3(&vMax1, trasformV);


		// �ٽ� center�� �ǵ�����.
		obstacles[i]->GetAABB().center.x = (vMin1.x + vMax1.x) / 2;
		obstacles[i]->GetAABB().center.y = (vMin1.y + vMax1.y) / 2;
		obstacles[i]->GetAABB().center.z = (vMin1.z + vMax1.z) / 2;
		
	}


	
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
		if (i < MAX_CLIENT) {
			// MAX Ŭ���̾�Ʈ = 32�� ���� �ʱ�ȭ
			objects.push_back(new Player);
			objects[i]->SetID(i);
			objects[i]->SetWorldMatrix(temp[i].GetWorldMatrix());
			dynamic_cast<Player*>(objects[i])->SetCamPos( temp[i].GetCamPos() );
			objects[i]->SetModel( temp[i].GetModel() );
			
			objects[i]->UpdateCenter();
			objects[i]->GetAABB().extent = XMFLOAT3(0.678452, 2.44222, 2.96222);
		}
		else if (i < MAX_CLIENT + MAX_NPC) {
			// 33 ~ 95�� ���� NPC �ʱ�ȭ
			objects.push_back(new NPC);
			objects[i]->SetID(i);
			objects[i]->SetIsRender(true);
			objects[i]->SetWorldMatrix(temp[i].GetWorldMatrix());
			objects[i]->SetModel(temp[i].GetModel());

			objects[i]->UpdateCenter();
			objects[i]->GetAABB().extent = XMFLOAT3(0.678452, 2.44222, 2.96222);

			Timer::GetInstance()->AddTimer(i, NPC_MOVE, 1000);
		}

	}

}



void Server::Init()
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return;

	// IOCP �ڵ鷯�� �ʱ�ȭ�Ѵ�.
	g_hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	if (!g_hIocp) return;

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
void Server::Progress()
{
	int last = GetTickCount64();
	int now = GetTickCount64();
	int startTick = GetTickCount64();
	int time = 60;
	bool timeCheck = true;
	while (true)
	{
		Sleep(1);

		// ������ ����
		if (GetTickCount64() - startTick >= 1000 / FPS) {
			now = GetTickCount64();
			if (now > last) {
				deltaTime = ((float)(now - last)) / 1000;
				last = now;
			}
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

