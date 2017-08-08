#include "stdafx.h"
#include "Obstacle.h"
#include "NPC.h"
#include "ThreadFunctions.h"
#include "Room.h"

NPC::NPC()
{
	moveTime = 0.0;
	moveTickTime = 0.0;
	moveTickCount = 0.0;
	restTime = 0.0;

	camLook.x = 0;
	camLook.y = -0.289194;
	camLook.z = 0;

	speedSave = 2;
}


NPC::~NPC()
{
}

void NPC::HeartBeat()
{

	if (bIsAlive) {

		XMFLOAT3 campos, charpos, tempCamLook, camRight, temp;

		// Player�� ��ǥ�� charpos.x�� ���� (�̵��� ��ǥ)
		charpos.x = GetWorldMatrix()._41;
		charpos.y = GetWorldMatrix()._42;
		charpos.z = GetWorldMatrix()._43;

		// campos�� camlook�� ��Ŷ���� ���� ������ Set
		tempCamLook = camLook;


		XMMATRIX objoffset;
		XMFLOAT4X4 m;
		XMMATRIX modelScale = XMMatrixScaling(1, 1, 1);
		XMMATRIX modelRot = XMMatrixRotationZ(objects[id]->GetRealAngle());
		XMMATRIX modelOffset = XMMatrixTranslation(0, 0, 0);
		XMMATRIX finalm = modelScale*modelRot*modelOffset;

		// ȸ�������� �� �÷��̾��� ��������� ������.
		// �ش� ����ŭ �÷��̾� ȸ��
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

		// ĳ���� ȸ��
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

		// ������ �÷��̾�� ���鰣�� �浹
		// 6���� ���� �˻�
		for (int i = 0; i < MAX_OBSTACLE; ++i) {
			if (true == IntersectSphereOrientedBox(&GetSphereForObstacle(), &obstacles[i]->GetObb())) {
				//VV = ������ �ݴ���, PB�÷��̾ �������ٶ󺸴� ����

				collCheck = true;

				XMFLOAT3 objcen, objextent, resultvec, BtoP;
				BtoP.x = 0;
				BtoP.y = 0;
				BtoP.z = 1;
				XMVECTOR vBtoP = XMLoadFloat3(&BtoP);

				objcen.x = obstacles[i]->GetObb().Center.x;
				objcen.y = 0;
				objcen.z = obstacles[i]->GetObb().Center.z;

				float objWX;
				float objWZ;
				float obtCX;
				float obtEX;
				float obtEZ;
				float obtCZ;

				if (1 == obstacles[i]->GetObbType()) {
					objWX = GetSphereForObstacle().Center.x;
					obtCX = obstacles[i]->GetObb().Center.x;
					obtCZ = obstacles[i]->GetObb().Center.z;

					obtEX = obstacles[i]->GetObb().Extents.x;
					obtEZ = obstacles[i]->GetObb().Extents.z;
				}
				else if (2 == obstacles[i]->GetObbType()) {
					objWX = GetSphereForObstacle().Center.x;
					obtCX = obstacles[i]->GetObb().Center.x;
					obtCZ = obstacles[i]->GetObb().Center.z;

					obtEX = obstacles[i]->GetObb().Extents.z;
					obtEZ = obstacles[i]->GetObb().Extents.x;
				}

				if (objWX >= (obtCX - obtEX) &&
					objWX <= (obtCX + obtEX))
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
					if (true == IntersectSphereOrientedBox(&test, &obstacles[i]->GetObb()))
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

				for (int i = 0; i < Room::GetInstance()->GetCurPlayer(); ++i) {
					SendPacket(i, reinterpret_cast<BYTE*>(&player_pos));
				}

			}
		}


		// �÷��̾��� �浹.
		for (int i = 0; i < MAX_OBJECT; ++i) {
			if (true == IntersectSphereSphere(&GetSphereForPlayer(), &objects[i]->GetSphereForPlayer()) && id != i) {
				collCheck = true;

				//VV = ������ �ݴ���, PB�÷��̾ �������ٶ󺸴� ����
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

				for (int i = 0; i < Room::GetInstance()->GetCurPlayer(); ++i) {
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


			for (int i = 0; i < Room::GetInstance()->GetCurPlayer(); ++i) {
				SendPacket(i, reinterpret_cast<BYTE*>(&player_pos));
			}

		}
	}
}
