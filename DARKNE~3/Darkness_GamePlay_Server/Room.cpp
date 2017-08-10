#include "stdafx.h"
#include "Room.h"
#include "Obstacle.h"

 
Room* Room::instance = nullptr;

Room::Room()

{

}

Room::~Room()
{
}

void Room::Init()
{
	roomCreater = 0;
	maxPlayer = 8;
	curPlayer = 0;
	map = 0;
	weather = 0;
	readyCount = 0;
	maxObstacles = 0;
	cout << "방 초기화 완료" << endl;

}

void Room::LoadObstacles()
{
	// 기존의 obstacle 객체가 존재 한다면
	// 비워준다.
	for (auto obstacle : obstacles)
	{
		delete obstacle;
		obstacle = nullptr;
	}
	obstacles.clear();


	// map Num에 따라 읽어오는 텍스트 파일을 다르게함.
	string mapName;
	bool LoadSuccessed = false;

	switch (map) {
	case 0:
	{
		mapName = "obstacle.txt";
		LoadSuccessed = true;
		break;
	}
	case 1:
	{
		mapName = "obstacle.txt";
		LoadSuccessed = true;
		break;
	}
	case 2:
	{
		mapName = "obstacle.txt";
		LoadSuccessed = true;
		break;
	}
	default:
		cout << "This map is not existed" << endl;
		LoadSuccessed = false;
		break;
	}
	
	if (true == LoadSuccessed) {
		ifstream fin(mapName);

		string ignore, name;
		float cX, cY, cZ, eX, eY, eZ;
		float sX, sY, sZ, tX, tY, tZ, rX, rY, rZ, rA;
		int type;

		fin >> ignore >> maxObstacles;

		for (int i = 0; i < maxObstacles; ++i) {
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

		cout << "Obstacle 생성 완료" << endl;
	}
	else
		cout << "Obstacle 생성 실패" << endl;

}
