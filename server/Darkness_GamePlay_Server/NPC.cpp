#include "stdafx.h"
#include "Obstacle.h"
#include "NPC.h"
#include "ThreadFunctions.h"


NPC::NPC()
{
	moveTime = 0.0;
	moveTickTime = 0.0;
	moveTickCount = 0.0;
	restTime = 0.0;

	camLook.x = 0;
	camLook.y = -0.289194;
	camLook.z = 0;

	speedSave = 1.3;
}


NPC::~NPC()
{
}

void NPC::HeartBeat()
{

	if (bIsAlive) {

		XMFLOAT3 campos, charpos, tempCamLook, camRight, temp;

		// Player의 좌표를 charpos.x에 저장 (이동전 좌표)
		charpos.x = GetWorldMatrix()._41;
		charpos.y = GetWorldMatrix()._42;
		charpos.z = GetWorldMatrix()._43;

		// campos와 camlook을 패킷으로 받은 값으로 Set
		tempCamLook = camLook;

		XMMATRIX objoffset;
		XMFLOAT4X4 m;
		XMMATRIX modelScale = XMMatrixScaling(1, 1, 1);
		XMMATRIX modelRot = XMMatrixRotationZ(objects[id]->GetRealAngle());
		XMMATRIX modelOffset = XMMatrixTranslation(0, 0, 0);
		XMMATRIX finalm = modelScale*modelRot*modelOffset;

		// 회전오프셋 을 플레이어의 월드행렬을 곱해줌.
		// 해당 값만큼 플레이어 회전
		objoffset = XMLoadFloat4x4(&worldMatrix);
		objoffset = finalm * objoffset;
		XMStoreFloat4x4(&m, objoffset);
		worldMatrix = m;
		
		temp.x = worldMatrix._11;
		temp.y = worldMatrix._12;
		temp.z = worldMatrix._13;
		XMStoreFloat3(&temp, XMVector3Normalize(XMLoadFloat3(&temp)));
		look = temp;

		XMFLOAT3 ang;
		XMStoreFloat3(&ang, XMVector3Dot(XMLoadFloat3(&tempCamLook), XMLoadFloat3(&look)));

		ang.x = ((tempCamLook.x * look.z) - (tempCamLook.z * look.x) > 0.0f) ? (float)(acos(ang.x)) : 360 - (float)(acos(ang.x));

		// 캐릭터 회전
		if (tempCamLook.x + 0.05 > look.x && tempCamLook.x - 0.05 < look.x)
		{
			if (tempCamLook.z + 0.05 > look.z && tempCamLook.z - 0.05 < look.z)
				realAngle = 0;
		}

		else if (ang.x > 357)
			realAngle = -ROTATE;

		else
			realAngle = ROTATE;


		bool collCheck = false;
		XMFLOAT3 cm;

		// 움직인 플레이어와 집들간의 충돌
		// 6개의 집을 검사
		for (int i = 0; i < MAX_OBSTACLE; ++i) {
			if (true == IntersectSphereOrientedBox(&GetSphereForObstacle(), &obstacles[i]->GetObb())) {
				//VV = 센터의 반대점, PB플레이어가 센터점바라보는 백터

				collCheck = true;

				XMFLOAT3 objcen, objextent, resultvec, BtoP;
				BtoP.x = 0;
				BtoP.y = 0;
				BtoP.z = 1;
				XMVECTOR vBtoP = XMLoadFloat3(&BtoP);

				objcen.x = obstacles[i]->GetObb().Center.x;
				objcen.y = 0;
				objcen.z = obstacles[i]->GetObb().Center.z;

				// 어느 축이랑 충돌 되었는지 검사
				if (worldMatrix._41 > (obstacles[i]->GetObb().Center.x - obstacles[i]->GetObb().Extents.x) &&
					worldMatrix._41 < (obstacles[i]->GetObb().Center.x + obstacles[i]->GetObb().Extents.x))
				{
					//XMVECTOR s = XMVectorReplicate(deltaTime * speed * speedSave);

					XMVECTOR l = XMLoadFloat3(&tempCamLook);
					//XMVECTOR p = XMLoadFloat3(&charpos);
					XMVECTOR p = XMLoadFloat3(&charpos) + (XMVectorSet(camLook.x, camLook.y, camLook.z, 0) *speed*deltaTime);

					XMStoreFloat3(&charpos, p);

					cm.x = charpos.x - worldMatrix._41;
					cm.y = 0;
					cm.z = 0;

					worldMatrix._41 = charpos.x;

					Sphere test;
					test.Center.x = charpos.x;
					test.Center.y = worldMatrix._42;
					test.Center.z = charpos.z;
					test.Radius = GetSphereForObstacle().Radius;
					if (true == IntersectSphereOrientedBox(&test, &obstacles[i]->GetObb()))
						collCheck = true;
					else
						collCheck = false;


					UpdateSphereForObstacle();
					UpdateSphereForPlayer();

				}

				else
				{
					//XMVECTOR s = XMVectorReplicate(deltaTime * speed * speedSave);
					XMVECTOR l = XMLoadFloat3(&tempCamLook);
					XMVECTOR p = XMLoadFloat3(&charpos) + (XMVectorSet(camLook.x,camLook.y,camLook.z,0) *speed*deltaTime) ;
					XMStoreFloat3(&charpos, p);

					cm.x = 0;
					cm.y = 0;
					cm.z = charpos.z - worldMatrix._43;

					worldMatrix._43 = charpos.z;

					Sphere test;
					test.Center.x = charpos.x;
					test.Center.y = worldMatrix._42;
					test.Center.z = charpos.z;
					test.Radius = GetSphereForObstacle().Radius;
					if (true == IntersectSphereOrientedBox(&test, &obstacles[i]->GetObb()))
						collCheck = true;
					else
						collCheck = false;


					UpdateSphereForObstacle();
					UpdateSphereForPlayer();

				}

				sc_packet_playgame_player_pos player_pos;
				player_pos.size = sizeof(player_pos);
				player_pos.type = SC_PACKET_PLAYGAME_PLAYER_POS;
				player_pos.id = id;
				player_pos.camMove = cm;
				player_pos.worldMatrix = worldMatrix;

				for (int i = 0; i < MAX_CLIENT; ++i) {
					SendPacket(i, reinterpret_cast<BYTE*>(&player_pos));
				}

			}
		}


		// 플레이어의 충돌.
		for (int i = 0; i < MAX_OBJECT; ++i) {
			if (true == IntersectSphereSphere(&GetSphereForPlayer(), &objects[i]->GetSphereForPlayer()) && id != i) {
				collCheck = true;

				//VV = 센터의 반대점, PB플레이어가 센터점바라보는 백터
				XMFLOAT3 objcen, objextent, resultvec, BtoP;

				BtoP.x = 0;
				BtoP.y = 0;
				BtoP.z = 1;
				XMVECTOR vBtoP = XMLoadFloat3(&BtoP);

				objcen.x = obstacles[i]->GetObb().Center.x;
				objcen.y = 0;
				objcen.z = obstacles[i]->GetObb().Center.z;

				if (worldMatrix._41 > (objects[i]->GetSphereForPlayer().Center.x - objects[i]->GetSphereForPlayer().Radius) &&
					worldMatrix._41 < (objects[i]->GetSphereForPlayer().Center.x + objects[i]->GetSphereForPlayer().Radius))
				{
					XMVECTOR s = XMVectorReplicate(deltaTime * speed * speedSave);
					XMVECTOR l = XMLoadFloat3(&tempCamLook);
					XMVECTOR p = XMLoadFloat3(&charpos);
					XMStoreFloat3(&charpos, XMVectorMultiplyAdd(s, l, p));

					cm.x = charpos.x - worldMatrix._41;
					cm.y = 0;
					cm.z = 0;

					worldMatrix._41 = charpos.x;

					Sphere test;
					test.Center.x = charpos.x;
					test.Center.y = worldMatrix._42;
					test.Center.z = charpos.z;
					test.Radius = GetSphereForObstacle().Radius;
					if (true == IntersectSphereSphere(&test, &objects[i]->GetSphereForPlayer()))
						collCheck = true;
					else
						collCheck = false;


					UpdateSphereForObstacle();
					UpdateSphereForPlayer();
				}

				else
				{
					XMVECTOR s = XMVectorReplicate(deltaTime * speed * speedSave);
					XMVECTOR l = XMLoadFloat3(&tempCamLook);
					XMVECTOR p = XMLoadFloat3(&charpos);
					XMStoreFloat3(&charpos, XMVectorMultiplyAdd(s, l, p));

					cm.x = 0;
					cm.y = 0;
					cm.z = charpos.z - worldMatrix._43;

					worldMatrix._43 = charpos.z;

					Sphere test;
					test.Center.x = charpos.x;
					test.Center.y = worldMatrix._42;
					test.Center.z = charpos.z;
					test.Radius = GetSphereForObstacle().Radius;
					if (true == IntersectSphereSphere(&test, &objects[i]->GetSphereForPlayer()))
						collCheck = true;
					else
						collCheck = false;


					UpdateSphereForObstacle();
					UpdateSphereForPlayer();

				}

				sc_packet_playgame_player_pos player_pos;
				player_pos.size = sizeof(player_pos);
				player_pos.type = SC_PACKET_PLAYGAME_PLAYER_POS;
				player_pos.id = id;
				player_pos.camMove = cm;
				player_pos.worldMatrix = worldMatrix;

				for (int i = 0; i < MAX_CLIENT; ++i) {
					SendPacket(i, reinterpret_cast<BYTE*>(&player_pos));
				}

			}
		}

		if (!collCheck)
		{
			XMVECTOR s = XMVectorReplicate(deltaTime * speed * speedSave);
			XMVECTOR l = XMLoadFloat3(&tempCamLook);
			XMVECTOR p = XMLoadFloat3(&charpos);
			XMStoreFloat3(&charpos, XMVectorMultiplyAdd(s, l, p));

			cm;
			cm.x = charpos.x - worldMatrix._41;
			cm.y = 0;
			cm.z = charpos.z - worldMatrix._43;
			worldMatrix._41 = charpos.x;
			worldMatrix._43 = charpos.z;

			UpdateSphereForObstacle();
			UpdateSphereForPlayer();

			sc_packet_playgame_player_pos player_pos;
			player_pos.size = sizeof(player_pos);
			player_pos.type = SC_PACKET_PLAYGAME_PLAYER_POS;
			player_pos.id = id;
			player_pos.worldMatrix = worldMatrix;
			player_pos.camMove = cm;

			for (int i = 0; i < MAX_CLIENT; ++i) {
				SendPacket(i, reinterpret_cast<BYTE*>(&player_pos));
			}

		}
	}
}
