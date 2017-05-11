#include "stdafx.h"
#include "Player.h"
#include "Obstacle.h"
#include "ThreadFunctions.h"


Player::Player()
{
	


	Init();

}


Player::~Player()
{
}
void Player::Init()
{
	ZeroMemory(recv_buf, sizeof(recv_buf));
	ZeroMemory(&over_ex.overlapped, sizeof(over_ex.overlapped));
	over_ex.wsa_buf.buf = reinterpret_cast<char*>(recv_buf);
	over_ex.wsa_buf.len = sizeof(recv_buf);
	wcscpy(nickName ,L"NONE") ;
	over_ex.operation = OP_RECV;
	id = -1;
	cur_state = STATE_NONE;


	camPos.x = 0;
	camPos.y = 0;
	camPos.z = 0;

	playerKill = 0;
	NPCKill = 0;

}

void Player::RecvPacket()
{
	DWORD flags = 0;

	int ret_val = WSARecv(socket, &over_ex.wsa_buf, 1, NULL, &flags, &over_ex.overlapped, NULL);

	if (ret_val != 0) {
		int error = WSAGetLastError();
		if (error != WSA_IO_PENDING) {
			cout << " [error] WSARecv() on " << socket << " CUser::RecvPacket() " << endl;
		}

	}
}

void Player::ReadPacket(const UINT& id, const DWORD& transferred)
{
	BYTE* ptr = recv_buf;
	int remained = transferred;

	while (0 < remained) {

		if (0 == packet_size)
			packet_size = ptr[0];

		int required = packet_size - saved_size;

		if (remained >= required)
		{
			memcpy(packet_buf + saved_size, ptr, required);

			PacketProcess(id, packet_buf);

			remained -= required;
			ptr += required;

			packet_size = 0;
			saved_size = 0;
		}

		else
		{
			memcpy(packet_buf + saved_size, ptr, remained);

			saved_size += remained;

			remained = 0;
		}

	}
}

void Player::Move(const int dir, const UINT id, BYTE* packet)
{
	switch (dir)
	{
 	case CS_UP:
	{
		//어떤키가 눌렸는지?
		cs_packet_player_move player_move;
		memcpy(&player_move, packet, packet[0]);

		XMFLOAT3 campos, charpos, camLook, camRight, temp;

		// Player의 좌표를 charpos.x에 저장 (이동전 좌표)
		charpos.x = objects[player_move.id]->GetWorldMatrix()._41;
		charpos.y = objects[player_move.id]->GetWorldMatrix()._42;
		charpos.z = objects[player_move.id]->GetWorldMatrix()._43;
		
		// campos와 camlook을 패킷으로 받은 값으로 Set
		campos = player_move.camPos;
		camLook = player_move.camLook;

		
		XMMATRIX objoffset;
		XMFLOAT4X4 m;
		XMMATRIX modelScale = XMMatrixScaling(1, 1, 1);
		XMMATRIX modelRot = XMMatrixRotationZ(objects[player_move.id]->GetRealAngle());
		XMMATRIX modelOffset = XMMatrixTranslation(0, 0, 0);
		XMMATRIX finalm = modelScale*modelRot*modelOffset;

		// 회전오프셋 을 플레이어의 월드행렬을 곱해줌.
		// 해당 값만큼 플레이어 회전
		objoffset = XMLoadFloat4x4(&objects[player_move.id]->GetWorldMatrix());
		objoffset = finalm * objoffset;
		XMStoreFloat4x4(&m, objoffset);
		objects[player_move.id]->SetWorldMatrix(m);
		XMMATRIX Rot = XMMatrixRotationY(objects[player_move.id]->GetRealAngle());
		XMStoreFloat3(&objects[player_move.id]->GetLook(), XMVector3TransformNormal(XMLoadFloat3(&objects[player_move.id]->GetLook()), Rot));

		XMVECTOR vcharlook, vcamlook;

		vcharlook = XMLoadFloat3(&objects[player_move.id]->GetLook());
		vcamlook = XMLoadFloat3(&camLook);
		vcharlook = XMVector3Normalize(vcharlook);
		vcamlook = XMVector3Normalize(vcamlook);


		temp.x = objects[player_move.id]->GetWorldMatrix()._11;
		temp.y = objects[player_move.id]->GetWorldMatrix()._12;
		temp.z = objects[player_move.id]->GetWorldMatrix()._13;
		XMStoreFloat3(&temp, XMVector3Normalize(XMLoadFloat3(&temp)));
		objects[player_move.id]->SetLook(temp);

		XMFLOAT3 ang;
		XMStoreFloat3(&ang, XMVector3Dot(XMLoadFloat3(&camLook), XMLoadFloat3(&objects[player_move.id]->GetLook())));

		ang.x = ((camLook.x * objects[player_move.id]->GetLook().z) - (camLook.z * objects[player_move.id]->GetLook().x) > 0.0f) ? (float)(acos(ang.x)) : 360 - (float)(acos(ang.x));

		// 캐릭터 회전
		if (camLook.x + 0.05 > objects[player_move.id]->GetLook().x && camLook.x - 0.05 < objects[player_move.id]->GetLook().x)
		{
			if (camLook.z + 0.05 > objects[player_move.id]->GetLook().z && camLook.z - 0.05 < objects[player_move.id]->GetLook().z)
				objects[player_move.id]->SetRealAngle(0);
		}

		else if (ang.x > 357)
			objects[player_move.id]->SetRealAngle(-ROTATE);

		else
			objects[player_move.id]->SetRealAngle(ROTATE);
		bool collCheck = false;
		XMFLOAT3 cm;

		// 움직인 플레이어와 집들간의 충돌
		// 6개의 집을 검사
		for (int i = 0; i < MAX_OBSTACLE; ++i) {
			if (true == IntersectSphereOrientedBox(&objects[player_move.id]->GetSphereForObstacle(), &obstacles[i]->GetObb())) {
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
				if (objects[player_move.id]->GetWorldMatrix()._41 > (obstacles[i]->GetObb().Center.x - obstacles[i]->GetObb().Extents.x) &&
					objects[player_move.id]->GetWorldMatrix()._41 < (obstacles[i]->GetObb().Center.x + obstacles[i]->GetObb().Extents.x))
				{
					//XMVECTOR s = XMVectorReplicate(deltaTime * speed * speedSave);
					XMVECTOR l = XMLoadFloat3(&camLook);
					//XMVECTOR p = XMLoadFloat3(&charpos);
					XMVECTOR p = XMLoadFloat3(&charpos) + (XMVectorSet(camLook.x, camLook.y, camLook.z, 0) *speed*deltaTime);

					XMStoreFloat3(&charpos, p);

					cm.x = charpos.x - objects[player_move.id]->GetWorldMatrix()._41;
					cm.y = 0;
					cm.z = 0;

					objects[player_move.id]->GetWorldMatrix()._41 = charpos.x;
					
					Sphere test;
					test.Center.x = charpos.x;
					test.Center.y = objects[player_move.id]->GetWorldMatrix()._42;
					test.Center.z = charpos.z;
					test.Radius = objects[player_move.id]->GetSphereForObstacle().Radius;
					if (true == IntersectSphereOrientedBox(&test, &obstacles[i]->GetObb()))
						collCheck = true;
					else
						collCheck = false;
					objects[player_move.id]->UpdateSphereForObstacle();
					objects[player_move.id]->UpdateSphereForPlayer();
				}

				else
				{
					XMVECTOR s = XMVectorReplicate(deltaTime * speed * speedSave);
					XMVECTOR l = XMLoadFloat3(&camLook);
// 					XMVECTOR p = XMLoadFloat3(&charpos);
// 					XMStoreFloat3(&charpos, XMVectorMultiplyAdd(s, l, p));
					XMVECTOR p = XMLoadFloat3(&charpos) + (XMVectorSet(camLook.x, camLook.y, camLook.z, 0) *speed*deltaTime);
					XMStoreFloat3(&charpos, p);
					cm.x = 0;
					cm.y = 0;
					cm.z = charpos.z - objects[player_move.id]->GetWorldMatrix()._43;

					objects[player_move.id]->GetWorldMatrix()._43 = charpos.z;
					
					Sphere test;
					test.Center.x = charpos.x;
					test.Center.y = objects[player_move.id]->GetWorldMatrix()._42;
					test.Center.z = charpos.z;
					test.Radius = objects[player_move.id]->GetSphereForObstacle().Radius;
					if (true == IntersectSphereOrientedBox(&test, &obstacles[i]->GetObb()))
						collCheck = true;
					else
						collCheck = false;


					objects[player_move.id]->UpdateSphereForObstacle();
					objects[player_move.id]->UpdateSphereForPlayer();

				}
				
				sc_packet_playgame_player_pos player_pos;
				player_pos.size = sizeof(player_pos);
				player_pos.type = SC_PACKET_PLAYGAME_PLAYER_POS;
				player_pos.id = player_move.id;
				player_pos.camMove = cm;
				player_pos.worldMatrix = objects[player_move.id]->GetWorldMatrix();
				SendPacket(player_pos.id, reinterpret_cast<BYTE*>(&player_pos));

			}
		}


		// 플레이어의 충돌.
		for (int i = 0; i < MAX_OBJECT; ++i) {
			if (true == IntersectSphereSphere(&objects[player_move.id]->GetSphereForPlayer(), &objects[i]->GetSphereForPlayer()) && player_move.id != i ) {
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

				if (objects[player_move.id]->GetWorldMatrix()._41 > (objects[i]->GetSphereForPlayer().Center.x - objects[i]->GetSphereForPlayer().Radius) &&
					objects[player_move.id]->GetWorldMatrix()._41 < (objects[i]->GetSphereForPlayer().Center.x + objects[i]->GetSphereForPlayer().Radius))
				{
					//XMVECTOR s = XMVectorReplicate(deltaTime * speed * speedSave);
					XMVECTOR l = XMLoadFloat3(&camLook);
// 					XMVECTOR p = XMLoadFloat3(&charpos);
// 					XMStoreFloat3(&charpos, XMVectorMultiplyAdd(s, l, p));
					XMVECTOR p = XMLoadFloat3(&charpos) + (XMVectorSet(camLook.x, camLook.y, camLook.z, 0) *speed*deltaTime);
					XMStoreFloat3(&charpos, p);
					cm.x = charpos.x - objects[player_move.id]->GetWorldMatrix()._41;
					cm.y = 0;
					cm.z = 0;

					objects[player_move.id]->GetWorldMatrix()._41 = charpos.x;

					Sphere test;
					test.Center.x = charpos.x;
					test.Center.y = objects[player_move.id]->GetWorldMatrix()._42;
					test.Center.z = charpos.z;
					test.Radius = objects[player_move.id]->GetSphereForObstacle().Radius;
					if (true == IntersectSphereSphere(&test, &objects[i]->GetSphereForPlayer()))
						collCheck = true;
					else
						collCheck = false;


					objects[player_move.id]->UpdateSphereForObstacle();
					objects[player_move.id]->UpdateSphereForPlayer();
				}

				else
				{
					//XMVECTOR s = XMVectorReplicate(deltaTime * speed * speedSave);
					XMVECTOR l = XMLoadFloat3(&camLook);
// 					XMVECTOR p = XMLoadFloat3(&charpos);
// 					XMStoreFloat3(&charpos, XMVectorMultiplyAdd(s, l, p));
					XMVECTOR p = XMLoadFloat3(&charpos) + (XMVectorSet(camLook.x, camLook.y, camLook.z, 0) *speed*deltaTime);
					XMStoreFloat3(&charpos, p);

					cm.x = 0;
					cm.y = 0;
					cm.z = charpos.z - objects[player_move.id]->GetWorldMatrix()._43;

					objects[player_move.id]->GetWorldMatrix()._43 = charpos.z;

					Sphere test;
					test.Center.x = charpos.x;
					test.Center.y = objects[player_move.id]->GetWorldMatrix()._42;
					test.Center.z = charpos.z;
					test.Radius = objects[player_move.id]->GetSphereForObstacle().Radius;
					if (true == IntersectSphereSphere(&test, &objects[i]->GetSphereForPlayer()))
						collCheck = true;
					else
						collCheck = false;


					objects[player_move.id]->UpdateSphereForObstacle();
					objects[player_move.id]->UpdateSphereForPlayer();

				}

				sc_packet_playgame_player_pos player_pos;
				player_pos.size = sizeof(player_pos);
				player_pos.type = SC_PACKET_PLAYGAME_PLAYER_POS;
				player_pos.id = player_move.id;
				player_pos.camMove = cm;
				player_pos.worldMatrix = objects[player_move.id]->GetWorldMatrix();
				SendPacket(player_pos.id, reinterpret_cast<BYTE*>(&player_pos));

			}
		}

		if (!collCheck)
		{
			//XMVECTOR s = XMVectorReplicate(deltaTime * speed * speedSave);
			XMVECTOR l = XMLoadFloat3(&camLook);
// 			XMVECTOR p = XMLoadFloat3(&charpos);
// 			XMStoreFloat3(&charpos, XMVectorMultiplyAdd(s, l, p));
			XMVECTOR p = XMLoadFloat3(&charpos) + (XMVectorSet(camLook.x, camLook.y, camLook.z, 0) *speed*deltaTime);
			XMStoreFloat3(&charpos, p);

			cm;
			cm.x = charpos.x - objects[player_move.id]->GetWorldMatrix()._41;
			cm.y = 0;
			cm.z = charpos.z - objects[player_move.id]->GetWorldMatrix()._43;
			objects[player_move.id]->GetWorldMatrix()._41 = charpos.x;
			objects[player_move.id]->GetWorldMatrix()._43 = charpos.z;

			objects[player_move.id]->UpdateSphereForObstacle();
			objects[player_move.id]->UpdateSphereForPlayer();

			sc_packet_playgame_player_pos player_pos;
			player_pos.size = sizeof(player_pos);
			player_pos.type = SC_PACKET_PLAYGAME_PLAYER_POS;
			player_pos.id = player_move.id;
			player_pos.worldMatrix = objects[player_move.id]->GetWorldMatrix();
			player_pos.camMove = cm;
			SendPacket(player_pos.id, reinterpret_cast<BYTE*>(&player_pos));


		}
		break;
		
	}
	case CS_DOWN:
	{
		cs_packet_player_move player_move;

		memcpy(&player_move, packet, packet[0]);

		XMFLOAT3 campos, charpos, camLook, camRight, temp, cm;

		charpos.x = objects[player_move.id]->GetWorldMatrix()._41;
		charpos.y = objects[player_move.id]->GetWorldMatrix()._42;
		charpos.z = objects[player_move.id]->GetWorldMatrix()._43;

		campos = player_move.camPos;
		camLook = player_move.camLook;

		XMMATRIX objoffset;
		XMFLOAT4X4 m;
		XMMATRIX modelScale = XMMatrixScaling(1, 1, 1);
		XMMATRIX modelRot = XMMatrixRotationZ(objects[player_move.id]->GetRealAngle());
		XMMATRIX modelOffset = XMMatrixTranslation(0, 0, 0);
		XMMATRIX finalm = modelScale*modelRot*modelOffset;
		objoffset = XMLoadFloat4x4(&objects[player_move.id]->GetWorldMatrix());
		objoffset = finalm * objoffset;
		XMStoreFloat4x4(&m, objoffset);
		objects[player_move.id]->SetWorldMatrix(m);

		XMMATRIX Rot = XMMatrixRotationY(objects[player_move.id]->GetRealAngle());
		XMStoreFloat3(&objects[player_move.id]->GetLook(), XMVector3TransformNormal(XMLoadFloat3(&objects[player_move.id]->GetLook()), Rot));

		XMVECTOR vcharlook, vcamlook;

		vcharlook = XMLoadFloat3(&objects[player_move.id]->GetLook());
		vcamlook = XMLoadFloat3(&camLook);
		vcharlook = XMVector3Normalize(vcharlook);
		vcamlook = XMVector3Normalize(vcamlook);


		temp.x = objects[player_move.id]->GetWorldMatrix()._11;
		temp.y = objects[player_move.id]->GetWorldMatrix()._12;
		temp.z = objects[player_move.id]->GetWorldMatrix()._13;
		XMStoreFloat3(&temp, XMVector3Normalize(XMLoadFloat3(&temp)));
		objects[player_move.id]->SetLook(temp);

		XMFLOAT3 ang;
		//////////left이동은 룩의 -방향으로 움직여야함으로 잠시 -를 세팅////////
		camLook.x = -camLook.x;
		camLook.y = -camLook.y;
		camLook.z = -camLook.z;

		XMStoreFloat3(&ang, XMVector3Dot(XMLoadFloat3(&camLook), XMLoadFloat3(&objects[player_move.id]->GetLook())));

		ang.x = ((camLook.x * objects[player_move.id]->GetLook().z) - (camLook.z * objects[player_move.id]->GetLook().x) > 0.0f) ? (float)(acos(ang.x)) : 360 - (float)(acos(ang.x));

		if (camLook.x + 0.05 > objects[player_move.id]->GetLook().x && camLook.x - 0.05 < objects[player_move.id]->GetLook().x)
		{
			if (camLook.z + 0.05 > objects[player_move.id]->GetLook().z && camLook.z - 0.05 < objects[player_move.id]->GetLook().z)
				objects[player_move.id]->SetRealAngle(0);
		}
		else if (ang.x > 357)
			objects[player_move.id]->SetRealAngle(-ROTATE);

		else
			objects[player_move.id]->SetRealAngle(ROTATE);

		//////////다시 look을 원래대로 세팅////////
		camLook.x = -camLook.x;
		camLook.y = -camLook.y;
		camLook.z = -camLook.z;


		// 이동
		bool collCheck = false;

		// 움직인 플레이어와 집들간의 충돌


		// 6개의 집을 검사
		for (int i = 0; i < MAX_OBSTACLE; ++i) {
			if (true == IntersectSphereOrientedBox(&objects[player_move.id]->GetSphereForObstacle(), &obstacles[i]->GetObb())) {
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

				if (objects[player_move.id]->GetWorldMatrix()._41 > (obstacles[i]->GetObb().Center.x - obstacles[i]->GetObb().Extents.x) &&
					objects[player_move.id]->GetWorldMatrix()._41 < (obstacles[i]->GetObb().Center.x + obstacles[i]->GetObb().Extents.x))
				{
					//XMVECTOR s = XMVectorReplicate(deltaTime * speed * speedSave);
					XMVECTOR l = XMLoadFloat3(&camLook);
// 					XMVECTOR p = XMLoadFloat3(&charpos);
// 					XMStoreFloat3(&charpos, XMVectorMultiplyAdd(s, -l, p));
					XMVECTOR p = XMLoadFloat3(&charpos) + (XMVectorSet(camLook.x, camLook.y, camLook.z, 0) *speed*deltaTime);
					XMStoreFloat3(&charpos, p);


					cm.x = charpos.x - objects[player_move.id]->GetWorldMatrix()._41;
					cm.y = 0;
					cm.z = 0;

					objects[player_move.id]->GetWorldMatrix()._41 = charpos.x;
					

					Sphere test;
					test.Center.x = charpos.x;
					test.Center.y = objects[player_move.id]->GetWorldMatrix()._42;
					test.Center.z = charpos.z;
					test.Radius = objects[player_move.id]->GetSphereForObstacle().Radius;
					if (true == IntersectSphereOrientedBox(&test, &obstacles[i]->GetObb()))
						collCheck = true;
					else
						collCheck = false;


					objects[player_move.id]->UpdateSphereForObstacle();
					objects[player_move.id]->UpdateSphereForPlayer();
			
				}

				else
				{
					//XMVECTOR s = XMVectorReplicate(deltaTime * speed * speedSave);
					XMVECTOR l = XMLoadFloat3(&camLook);
// 					XMVECTOR p = XMLoadFloat3(&charpos);
// 					XMStoreFloat3(&charpos, XMVectorMultiplyAdd(s, -l, p));
					XMVECTOR p = XMLoadFloat3(&charpos) + (XMVectorSet(camLook.x, camLook.y, camLook.z, 0) *speed*deltaTime);
					XMStoreFloat3(&charpos, p);

					cm.x = 0;
					cm.y = 0;
					cm.z = charpos.z - objects[player_move.id]->GetWorldMatrix()._43;

					objects[player_move.id]->GetWorldMatrix()._43 = charpos.z;
					


					Sphere test;
					test.Center.x = charpos.x;
					test.Center.y = objects[player_move.id]->GetWorldMatrix()._42;
					test.Center.z = charpos.z;
					test.Radius = objects[player_move.id]->GetSphereForObstacle().Radius;
					if (true == IntersectSphereOrientedBox(&test, &obstacles[i]->GetObb()))
						collCheck = true;
					else
						collCheck = false;


					objects[player_move.id]->UpdateSphereForObstacle();
					objects[player_move.id]->UpdateSphereForPlayer();

				}

				objects[player_move.id]->UpdateSphereForObstacle();
				objects[player_move.id]->UpdateSphereForPlayer();
				sc_packet_playgame_player_pos player_pos;
				player_pos.size = sizeof(player_pos);
				player_pos.type = SC_PACKET_PLAYGAME_PLAYER_POS;
				player_pos.id = player_move.id;
				player_pos.camMove = cm;
				player_pos.worldMatrix = objects[player_move.id]->GetWorldMatrix();

				SendPacket(player_pos.id, reinterpret_cast<BYTE*>(&player_pos));

			}
		}
		
		for (int i = 0; i < MAX_OBJECT; ++i) {
			if (true == IntersectSphereSphere(&objects[player_move.id]->GetSphereForPlayer(), &objects[i]->GetSphereForPlayer()) && player_move.id != i) {
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

				if (objects[player_move.id]->GetWorldMatrix()._41 > (objects[i]->GetSphereForPlayer().Center.x - objects[i]->GetSphereForPlayer().Radius) &&
					objects[player_move.id]->GetWorldMatrix()._41 < (objects[i]->GetSphereForPlayer().Center.x + objects[i]->GetSphereForPlayer().Radius))
				{
					//XMVECTOR s = XMVectorReplicate(deltaTime * speed * speedSave);
					XMVECTOR l = XMLoadFloat3(&camLook);
// 					XMVECTOR p = XMLoadFloat3(&charpos);
// 					XMStoreFloat3(&charpos, XMVectorMultiplyAdd(s, -l, p));
					XMVECTOR p = XMLoadFloat3(&charpos) + (XMVectorSet(camLook.x, camLook.y, camLook.z, 0) *speed*deltaTime);
					XMStoreFloat3(&charpos, p);

					cm.x = charpos.x - objects[player_move.id]->GetWorldMatrix()._41;
					cm.y = 0;
					cm.z = 0;

					objects[player_move.id]->GetWorldMatrix()._41 = charpos.x;
					
					Sphere test;
					test.Center.x = charpos.x;
					test.Center.y = objects[player_move.id]->GetWorldMatrix()._42;
					test.Center.z = charpos.z;
					test.Radius = objects[player_move.id]->GetSphereForObstacle().Radius;
					if (true == IntersectSphereSphere(&test, &objects[i]->GetSphereForPlayer()))
						collCheck = true;
					else
						collCheck = false;


					objects[player_move.id]->UpdateSphereForObstacle();
					objects[player_move.id]->UpdateSphereForPlayer();
				}

				else
				{
					//XMVECTOR s = XMVectorReplicate(deltaTime * speed);
					XMVECTOR l = XMLoadFloat3(&camLook);
// 					XMVECTOR p = XMLoadFloat3(&charpos);
// 					XMStoreFloat3(&charpos, XMVectorMultiplyAdd(s, -l, p));
					XMVECTOR p = XMLoadFloat3(&charpos) + (XMVectorSet(camLook.x, camLook.y, camLook.z, 0) *speed*deltaTime);
					XMStoreFloat3(&charpos, p);

					cm.x = 0;
					cm.y = 0;
					cm.z = charpos.z - objects[player_move.id]->GetWorldMatrix()._43;

					objects[player_move.id]->GetWorldMatrix()._43 = charpos.z;
					
					Sphere test;
					test.Center.x = charpos.x;
					test.Center.y = objects[player_move.id]->GetWorldMatrix()._42;
					test.Center.z = charpos.z;
					test.Radius = objects[player_move.id]->GetSphereForObstacle().Radius;
					if (true == IntersectSphereSphere(&test, &objects[i]->GetSphereForPlayer()))
						collCheck = true;
					else
						collCheck = false;


					objects[player_move.id]->UpdateSphereForObstacle();
					objects[player_move.id]->UpdateSphereForPlayer();

				}

				sc_packet_playgame_player_pos player_pos;
				player_pos.size = sizeof(player_pos);
				player_pos.type = SC_PACKET_PLAYGAME_PLAYER_POS;
				player_pos.id = player_move.id;
				player_pos.camMove = cm;
				player_pos.worldMatrix = objects[player_move.id]->GetWorldMatrix();
				SendPacket(player_pos.id, reinterpret_cast<BYTE*>(&player_pos));

			}
		}

		if (!collCheck)
		{
			//XMVECTOR s = XMVectorReplicate(deltaTime * speed);
			XMVECTOR l = XMLoadFloat3(&camLook);
// 			XMVECTOR p = XMLoadFloat3(&charpos);
// 			XMStoreFloat3(&charpos, XMVectorMultiplyAdd(s, -l, p));
			XMVECTOR p = XMLoadFloat3(&charpos) + (XMVectorSet(camLook.x, camLook.y, camLook.z, 0) *speed*deltaTime);
			XMStoreFloat3(&charpos, p);

			cm.x = charpos.x - objects[player_move.id]->GetWorldMatrix()._41;
			cm.y = 0;
			cm.z = charpos.z - objects[player_move.id]->GetWorldMatrix()._43;

			objects[player_move.id]->GetWorldMatrix()._41 = charpos.x;
			objects[player_move.id]->GetWorldMatrix()._43 = charpos.z;
			objects[player_move.id]->UpdateSphereForObstacle();
			objects[player_move.id]->UpdateSphereForPlayer();


			sc_packet_playgame_player_pos player_pos;
			player_pos.size = sizeof(player_pos);
			player_pos.type = SC_PACKET_PLAYGAME_PLAYER_POS;
			player_pos.id = player_move.id;
			player_pos.camMove = cm;
			player_pos.worldMatrix = objects[player_move.id]->GetWorldMatrix();
			SendPacket(player_pos.id, reinterpret_cast<BYTE*>(&player_pos));


		}

		break;
	}
	case CS_LEFT:
	{
		cs_packet_player_move player_move;

		memcpy(&player_move, packet, packet[0]);

		XMFLOAT3 campos, charpos, camLook, camRight, temp, cm;

		charpos.x = objects[player_move.id]->GetWorldMatrix()._41;
		charpos.y = objects[player_move.id]->GetWorldMatrix()._42;
		charpos.z = objects[player_move.id]->GetWorldMatrix()._43;

		campos = player_move.camPos;
		camLook = player_move.camLook;



		XMMATRIX objoffset;
		XMFLOAT4X4 m;
		XMMATRIX modelScale = XMMatrixScaling(1, 1, 1);
		XMMATRIX modelRot = XMMatrixRotationZ(objects[player_move.id]->GetRealAngle());
		XMMATRIX modelOffset = XMMatrixTranslation(0, 0, 0);
		XMMATRIX finalm = modelScale*modelRot*modelOffset;
		objoffset = XMLoadFloat4x4(&objects[player_move.id]->GetWorldMatrix());
		objoffset = finalm * objoffset;
		XMStoreFloat4x4(&m, objoffset);
		objects[player_move.id]->SetWorldMatrix(m);



		XMMATRIX Rot = XMMatrixRotationY(objects[player_move.id]->GetRealAngle());
		XMStoreFloat3(&objects[player_move.id]->GetLook(), XMVector3TransformNormal(XMLoadFloat3(&objects[player_move.id]->GetLook()), Rot));

		XMVECTOR vcharlook, vcamlook;

		vcharlook = XMLoadFloat3(&objects[player_move.id]->GetLook());
		vcamlook = XMLoadFloat3(&camLook);
		vcharlook = XMVector3Normalize(vcharlook);
		vcamlook = XMVector3Normalize(vcamlook);

		temp.x = objects[player_move.id]->GetWorldMatrix()._11;
		temp.y = objects[player_move.id]->GetWorldMatrix()._12;
		temp.z = objects[player_move.id]->GetWorldMatrix()._13;
		XMStoreFloat3(&temp, XMVector3Normalize(XMLoadFloat3(&temp)));
		objects[player_move.id]->SetLook(temp);

		XMFLOAT3 ang;
		//////////left이동은 룩의 -방향으로 움직여야함으로 잠시 -를 세팅////////
		camLook.x = -camLook.x;
		camLook.y = -camLook.y;
		camLook.z = -camLook.z;

		XMStoreFloat3(&ang, XMVector3Dot(XMLoadFloat3(&camLook), XMLoadFloat3(&objects[player_move.id]->GetLook())));

		ang.x = ((camLook.x * objects[player_move.id]->GetLook().z) - (camLook.z * objects[player_move.id]->GetLook().x) > 0.0f) ? (float)(acos(ang.x)) : 360 - (float)(acos(ang.x));

		if (camLook.x + 0.05 > objects[player_move.id]->GetLook().x && camLook.x - 0.05 < objects[player_move.id]->GetLook().x)
		{
			if (camLook.z + 0.05 > objects[player_move.id]->GetLook().z && camLook.z - 0.05 < objects[player_move.id]->GetLook().z)
				objects[player_move.id]->SetRealAngle(0);
		}
		else if (ang.x > 357)
			objects[player_move.id]->SetRealAngle(-ROTATE);

		else
			objects[player_move.id]->SetRealAngle(ROTATE);

		//////////다시 look을 원래대로 세팅////////
		camLook.x = -camLook.x;
		camLook.y = -camLook.y;
		camLook.z = -camLook.z;

		// 이동
		bool collCheck = false;

		// 움직인 플레이어와 집들간의 충돌


		// 6개의 집을 검사
		for (int i = 0; i < MAX_OBSTACLE; ++i) {
			if (true == IntersectSphereOrientedBox(&objects[player_move.id]->GetSphereForObstacle(), &obstacles[i]->GetObb())) {
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

				if (objects[player_move.id]->GetWorldMatrix()._41 > (obstacles[i]->GetObb().Center.x - obstacles[i]->GetObb().Extents.x) &&
					objects[player_move.id]->GetWorldMatrix()._41 < (obstacles[i]->GetObb().Center.x + obstacles[i]->GetObb().Extents.x))
				{
					//XMVECTOR s = XMVectorReplicate(deltaTime * speed);
					XMVECTOR l = XMLoadFloat3(&camLook);
// 					XMVECTOR p = XMLoadFloat3(&charpos);
// 					
// 					XMStoreFloat3(&charpos, XMVectorMultiplyAdd(s, -l, p));
					XMVECTOR p = XMLoadFloat3(&charpos) + (XMVectorSet(camLook.x, camLook.y, camLook.z, 0) *speed*deltaTime);
					XMStoreFloat3(&charpos, p);
					
					cm.x = charpos.x - objects[player_move.id]->GetWorldMatrix()._41;
					cm.y = 0;
					cm.z = 0;

					objects[player_move.id]->GetWorldMatrix()._41 = charpos.x;
					
					Sphere test;
					test.Center.x = charpos.x;
					test.Center.y = objects[player_move.id]->GetWorldMatrix()._42;
					test.Center.z = charpos.z;
					test.Radius = objects[player_move.id]->GetSphereForObstacle().Radius;
					if (true == IntersectSphereOrientedBox(&test, &obstacles[i]->GetObb()))
						collCheck = true;
					else
						collCheck = false;


					objects[player_move.id]->UpdateSphereForObstacle();
					objects[player_move.id]->UpdateSphereForPlayer();

				}

				else
				{
					//XMVECTOR s = XMVectorReplicate(deltaTime * speed);
					XMVECTOR l = XMLoadFloat3(&camLook);
// 					XMVECTOR p = XMLoadFloat3(&charpos);
// 					XMStoreFloat3(&charpos, XMVectorMultiplyAdd(s, -l, p));
					XMVECTOR p = XMLoadFloat3(&charpos) + (XMVectorSet(camLook.x, camLook.y, camLook.z, 0) *speed*deltaTime);
					XMStoreFloat3(&charpos, p);

					cm.x = 0;
					cm.y = 0;
					cm.z = charpos.z - objects[player_move.id]->GetWorldMatrix()._43;

					objects[player_move.id]->GetWorldMatrix()._43 = charpos.z;
					
					Sphere test;
					test.Center.x = charpos.x;
					test.Center.y = objects[player_move.id]->GetWorldMatrix()._42;
					test.Center.z = charpos.z;
					test.Radius = objects[player_move.id]->GetSphereForObstacle().Radius;
					if (true == IntersectSphereOrientedBox(&test, &obstacles[i]->GetObb()))
						collCheck = true;
					else
						collCheck = false;


					objects[player_move.id]->UpdateSphereForObstacle();
					objects[player_move.id]->UpdateSphereForPlayer();

				}

				objects[player_move.id]->UpdateSphereForObstacle();
				objects[player_move.id]->UpdateSphereForPlayer();
				sc_packet_playgame_player_pos player_pos;
				player_pos.size = sizeof(player_pos);
				player_pos.type = SC_PACKET_PLAYGAME_PLAYER_POS;
				player_pos.id = player_move.id;
				player_pos.camMove = cm;

				player_pos.worldMatrix = objects[player_move.id]->GetWorldMatrix();

				SendPacket(player_pos.id, reinterpret_cast<BYTE*>(&player_pos));

			}
		}

		for (int i = 0; i < MAX_OBJECT; ++i) {
			if (true == IntersectSphereSphere(&objects[player_move.id]->GetSphereForPlayer(), &objects[i]->GetSphereForPlayer()) && player_move.id != i) {
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

				if (objects[player_move.id]->GetWorldMatrix()._41 > (objects[i]->GetSphereForPlayer().Center.x - objects[i]->GetSphereForPlayer().Radius) &&
					objects[player_move.id]->GetWorldMatrix()._41 < (objects[i]->GetSphereForPlayer().Center.x + objects[i]->GetSphereForPlayer().Radius))
				{
					//XMVECTOR s = XMVectorReplicate(deltaTime * speed);
					XMVECTOR l = XMLoadFloat3(&camLook);
// 					XMVECTOR p = XMLoadFloat3(&charpos);
// 					XMStoreFloat3(&charpos, XMVectorMultiplyAdd(s, -l, p));
					XMVECTOR p = XMLoadFloat3(&charpos) + (XMVectorSet(camLook.x, camLook.y, camLook.z, 0) *speed*deltaTime);
					XMStoreFloat3(&charpos, p);

					cm.x = charpos.x - objects[player_move.id]->GetWorldMatrix()._41;
					cm.y = 0;
					cm.z = 0;

					objects[player_move.id]->GetWorldMatrix()._41 = charpos.x;
					
					Sphere test;
					test.Center.x = charpos.x;
					test.Center.y = objects[player_move.id]->GetWorldMatrix()._42;
					test.Center.z = charpos.z;
					test.Radius = objects[player_move.id]->GetSphereForObstacle().Radius;
					if (true == IntersectSphereSphere(&test, &objects[i]->GetSphereForPlayer()))
						collCheck = true;
					else
						collCheck = false;


					objects[player_move.id]->UpdateSphereForObstacle();
					objects[player_move.id]->UpdateSphereForPlayer();
				}

				else
				{
					//XMVECTOR s = XMVectorReplicate(deltaTime * speed);
					XMVECTOR l = XMLoadFloat3(&camLook);
// 					XMVECTOR p = XMLoadFloat3(&charpos);
// 					XMStoreFloat3(&charpos, XMVectorMultiplyAdd(s, -l, p));
					XMVECTOR p = XMLoadFloat3(&charpos) + (XMVectorSet(camLook.x, camLook.y, camLook.z, 0) *speed*deltaTime);
					XMStoreFloat3(&charpos, p);

					cm.x = 0;
					cm.y = 0;
					cm.z = charpos.z - objects[player_move.id]->GetWorldMatrix()._43;

					objects[player_move.id]->GetWorldMatrix()._43 = charpos.z;
					
					Sphere test;
					test.Center.x = charpos.x;
					test.Center.y = objects[player_move.id]->GetWorldMatrix()._42;
					test.Center.z = charpos.z;
					test.Radius = objects[player_move.id]->GetSphereForObstacle().Radius;
					if (true == IntersectSphereSphere(&test, &objects[i]->GetSphereForPlayer()))
						collCheck = true;
					else
						collCheck = false;


					objects[player_move.id]->UpdateSphereForObstacle();
					objects[player_move.id]->UpdateSphereForPlayer();

				}

				sc_packet_playgame_player_pos player_pos;
				player_pos.size = sizeof(player_pos);
				player_pos.type = SC_PACKET_PLAYGAME_PLAYER_POS;
				player_pos.id = player_move.id;
				player_pos.camMove = cm;
				player_pos.worldMatrix = objects[player_move.id]->GetWorldMatrix();
				SendPacket(player_pos.id, reinterpret_cast<BYTE*>(&player_pos));

			}
		}

		if (!collCheck)
		{
			//XMVECTOR s = XMVectorReplicate(deltaTime * speed);
			XMVECTOR l = XMLoadFloat3(&camLook);
// 			XMVECTOR p = XMLoadFloat3(&charpos);
// 			XMStoreFloat3(&charpos, XMVectorMultiplyAdd(s, -l, p));
			XMVECTOR p = XMLoadFloat3(&charpos) + (XMVectorSet(camLook.x, camLook.y, camLook.z, 0) *speed*deltaTime);
			XMStoreFloat3(&charpos, p);

			cm.x = charpos.x - objects[player_move.id]->GetWorldMatrix()._41;
			cm.y = 0;
			cm.z = charpos.z - objects[player_move.id]->GetWorldMatrix()._43;

			objects[player_move.id]->GetWorldMatrix()._41 = charpos.x;
			objects[player_move.id]->GetWorldMatrix()._43 = charpos.z;



			objects[player_move.id]->UpdateSphereForObstacle();
			objects[player_move.id]->UpdateSphereForPlayer();
			sc_packet_playgame_player_pos player_pos;
			player_pos.size = sizeof(player_pos);
			player_pos.type = SC_PACKET_PLAYGAME_PLAYER_POS;
			player_pos.id = player_move.id;
			player_pos.camMove = cm;
			player_pos.worldMatrix = objects[player_move.id]->GetWorldMatrix();
			SendPacket(player_pos.id, reinterpret_cast<BYTE*>(&player_pos));


		}

		break;
	}
	case CS_RIGHT:
	{
		cs_packet_player_move player_move;

		memcpy(&player_move, packet, packet[0]);

		XMFLOAT3 campos, charpos, camLook, camRight, temp, cm;

		charpos.x = objects[player_move.id]->GetWorldMatrix()._41;
		charpos.y = objects[player_move.id]->GetWorldMatrix()._42;
		charpos.z = objects[player_move.id]->GetWorldMatrix()._43;

		campos = player_move.camPos;
		camLook = player_move.camLook;



		XMMATRIX objoffset;
		XMFLOAT4X4 m;
		XMMATRIX modelScale = XMMatrixScaling(1, 1, 1);
		XMMATRIX modelRot = XMMatrixRotationZ(objects[player_move.id]->GetRealAngle());
		XMMATRIX modelOffset = XMMatrixTranslation(0, 0, 0);
		XMMATRIX finalm = modelScale*modelRot*modelOffset;
		objoffset = XMLoadFloat4x4(&objects[player_move.id]->GetWorldMatrix());
		objoffset = finalm * objoffset;
		XMStoreFloat4x4(&m, objoffset);
		objects[player_move.id]->SetWorldMatrix(m);



		XMMATRIX Rot = XMMatrixRotationY(objects[player_move.id]->GetRealAngle());
		XMStoreFloat3(&objects[player_move.id]->GetLook(), XMVector3TransformNormal(XMLoadFloat3(&objects[player_move.id]->GetLook()), Rot));

		XMVECTOR vcharlook, vcamlook;

		vcharlook = XMLoadFloat3(&objects[player_move.id]->GetLook());
		vcamlook = XMLoadFloat3(&camLook);
		vcharlook = XMVector3Normalize(vcharlook);
		vcamlook = XMVector3Normalize(vcamlook);

		temp.x = objects[player_move.id]->GetWorldMatrix()._11;
		temp.y = objects[player_move.id]->GetWorldMatrix()._12;
		temp.z = objects[player_move.id]->GetWorldMatrix()._13;
		XMStoreFloat3(&temp, XMVector3Normalize(XMLoadFloat3(&temp)));
		objects[player_move.id]->SetLook(temp);

		XMFLOAT3 ang;
		XMStoreFloat3(&ang, XMVector3Dot(XMLoadFloat3(&camLook), XMLoadFloat3(&objects[player_move.id]->GetLook())));

		ang.x = ((camLook.x * objects[player_move.id]->GetLook().z) - (camLook.z * objects[player_move.id]->GetLook().x) > 0.0f) ? (float)(acos(ang.x)) : 360 - (float)(acos(ang.x));

		// 캐릭터 회전
		if (camLook.x + 0.05 > objects[player_move.id]->GetLook().x && camLook.x - 0.05 < objects[player_move.id]->GetLook().x)
		{
			if (camLook.z + 0.05 > objects[player_move.id]->GetLook().z && camLook.z - 0.05 < objects[player_move.id]->GetLook().z)
				objects[player_move.id]->SetRealAngle(0);
		}
		else if (ang.x > 357)
			objects[player_move.id]->SetRealAngle(-ROTATE);

		else
			objects[player_move.id]->SetRealAngle(ROTATE);


		// 이동
		bool collCheck = false;

		// 움직인 플레이어와 집들간의 충돌

		// 6개의 집을 검사
		for (int i = 0; i < MAX_OBSTACLE; ++i) {
			if (true == IntersectSphereOrientedBox(&objects[player_move.id]->GetSphereForObstacle(), &obstacles[i]->GetObb())) {
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

				if (objects[player_move.id]->GetWorldMatrix()._41 > (obstacles[i]->GetObb().Center.x - obstacles[i]->GetObb().Extents.x) &&
					objects[player_move.id]->GetWorldMatrix()._41 < (obstacles[i]->GetObb().Center.x + obstacles[i]->GetObb().Extents.x))
				{
					//XMVECTOR s = XMVectorReplicate(deltaTime * speed);
					XMVECTOR l = XMLoadFloat3(&camLook);
// 					XMVECTOR p = XMLoadFloat3(&charpos);
// 					XMStoreFloat3(&charpos, XMVectorMultiplyAdd(s, l, p));
					XMVECTOR p = XMLoadFloat3(&charpos) + (XMVectorSet(camLook.x, camLook.y, camLook.z, 0) *speed*deltaTime);
					XMStoreFloat3(&charpos, p);

					cm.x = charpos.x - objects[player_move.id]->GetWorldMatrix()._41;
					cm.y = 0;
					cm.z = 0;

					objects[player_move.id]->GetWorldMatrix()._41 = charpos.x;
					
					Sphere test;
					test.Center.x = charpos.x;
					test.Center.y = objects[player_move.id]->GetWorldMatrix()._42;
					test.Center.z = charpos.z;
					test.Radius = objects[player_move.id]->GetSphereForObstacle().Radius;
					if (true == IntersectSphereOrientedBox(&test, &obstacles[i]->GetObb()))
						collCheck = true;
					else
						collCheck = false;


					objects[player_move.id]->UpdateSphereForObstacle();
					objects[player_move.id]->UpdateSphereForPlayer();

				}

				else
				{
					//XMVECTOR s = XMVectorReplicate(deltaTime * speed);
					XMVECTOR l = XMLoadFloat3(&camLook);
// 					XMVECTOR p = XMLoadFloat3(&charpos);
// 					XMStoreFloat3(&charpos, XMVectorMultiplyAdd(s, l, p));
					XMVECTOR p = XMLoadFloat3(&charpos) + (XMVectorSet(camLook.x, camLook.y, camLook.z, 0) *speed*deltaTime);
					XMStoreFloat3(&charpos, p);


					cm.x = 0;
					cm.y = 0;
					cm.z = charpos.z - objects[player_move.id]->GetWorldMatrix()._43;


					objects[player_move.id]->GetWorldMatrix()._43 = charpos.z;
					
					Sphere test;
					test.Center.x = charpos.x;
					test.Center.y = objects[player_move.id]->GetWorldMatrix()._42;
					test.Center.z = charpos.z;
					test.Radius = objects[player_move.id]->GetSphereForObstacle().Radius;
					if (true == IntersectSphereOrientedBox(&test, &obstacles[i]->GetObb()))
						collCheck = true;
					else
						collCheck = false;


					objects[player_move.id]->UpdateSphereForObstacle();
					objects[player_move.id]->UpdateSphereForPlayer();

				}

				sc_packet_playgame_player_pos player_pos;
				player_pos.size = sizeof(player_pos);
				player_pos.type = SC_PACKET_PLAYGAME_PLAYER_POS;
				player_pos.id = player_move.id;
				player_pos.camMove = cm;

				player_pos.worldMatrix = objects[player_move.id]->GetWorldMatrix();

				SendPacket(player_pos.id, reinterpret_cast<BYTE*>(&player_pos));

			}
		}
		for (int i = 0; i < MAX_OBJECT; ++i) {
			if (true == IntersectSphereSphere(&objects[player_move.id]->GetSphereForPlayer(), &objects[i]->GetSphereForPlayer()) && player_move.id != i) {
				
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

				if (objects[player_move.id]->GetWorldMatrix()._41 > (objects[i]->GetSphereForPlayer().Center.x - objects[i]->GetSphereForPlayer().Radius) &&
					objects[player_move.id]->GetWorldMatrix()._41 < (objects[i]->GetSphereForPlayer().Center.x + objects[i]->GetSphereForPlayer().Radius))
				{
					//XMVECTOR s = XMVectorReplicate(deltaTime * speed);
					XMVECTOR l = XMLoadFloat3(&camLook);
// 					XMVECTOR p = XMLoadFloat3(&charpos);
// 					XMStoreFloat3(&charpos, XMVectorMultiplyAdd(s, l, p));
					XMVECTOR p = XMLoadFloat3(&charpos) + (XMVectorSet(camLook.x, camLook.y, camLook.z, 0) *speed*deltaTime);
					XMStoreFloat3(&charpos, p);

					cm.x = charpos.x - objects[player_move.id]->GetWorldMatrix()._41;
					cm.y = 0;
					cm.z = 0;

					objects[player_move.id]->GetWorldMatrix()._41 = charpos.x;
					
					Sphere test;
					test.Center.x = charpos.x;
					test.Center.y = objects[player_move.id]->GetWorldMatrix()._42;
					test.Center.z = charpos.z;
					test.Radius = objects[player_move.id]->GetSphereForObstacle().Radius;
					if (true == IntersectSphereSphere(&test, &objects[i]->GetSphereForPlayer()))
						collCheck = true;
					else
						collCheck = false;


					objects[player_move.id]->UpdateSphereForObstacle();
					objects[player_move.id]->UpdateSphereForPlayer();
				}

				else
				{
					//XMVECTOR s = XMVectorReplicate(deltaTime * speed);
					XMVECTOR l = XMLoadFloat3(&camLook);
// 					XMVECTOR p = XMLoadFloat3(&charpos);
// 					XMStoreFloat3(&charpos, XMVectorMultiplyAdd(s, l, p));
					XMVECTOR p = XMLoadFloat3(&charpos) + (XMVectorSet(camLook.x, camLook.y, camLook.z, 0) *speed*deltaTime);
					XMStoreFloat3(&charpos, p);
					cm.x = 0;
					cm.y = 0;
					cm.z = charpos.z - objects[player_move.id]->GetWorldMatrix()._43;

					objects[player_move.id]->GetWorldMatrix()._43 = charpos.z;
					
					Sphere test;
					test.Center.x = charpos.x;
					test.Center.y = objects[player_move.id]->GetWorldMatrix()._42;
					test.Center.z = charpos.z;
					test.Radius = objects[player_move.id]->GetSphereForObstacle().Radius;
					if (true == IntersectSphereSphere(&test, &objects[i]->GetSphereForPlayer()))
						collCheck = true;
					else
						collCheck = false;


					objects[player_move.id]->UpdateSphereForObstacle();
					objects[player_move.id]->UpdateSphereForPlayer();

				}

				sc_packet_playgame_player_pos player_pos;
				player_pos.size = sizeof(player_pos);
				player_pos.type = SC_PACKET_PLAYGAME_PLAYER_POS;
				player_pos.id = player_move.id;
				player_pos.camMove = cm;
				player_pos.worldMatrix = objects[player_move.id]->GetWorldMatrix();
				SendPacket(player_pos.id, reinterpret_cast<BYTE*>(&player_pos));

			}
		}

		if (!collCheck)
		{
			//XMVECTOR s = XMVectorReplicate(deltaTime * speed);
			XMVECTOR l = XMLoadFloat3(&camLook);
// 			XMVECTOR p = XMLoadFloat3(&charpos);
// 			XMStoreFloat3(&charpos, XMVectorMultiplyAdd(s, l, p));
			XMVECTOR p = XMLoadFloat3(&charpos) + (XMVectorSet(camLook.x, camLook.y, camLook.z, 0) *speed*deltaTime);
			XMStoreFloat3(&charpos, p);

			cm.x = charpos.x - objects[player_move.id]->GetWorldMatrix()._41;
			cm.y = 0;
			cm.z = charpos.z - objects[player_move.id]->GetWorldMatrix()._43;

			objects[player_move.id]->GetWorldMatrix()._41 = charpos.x;
			objects[player_move.id]->GetWorldMatrix()._43 = charpos.z;
			objects[player_move.id]->UpdateSphereForPlayer();
			objects[player_move.id]->UpdateSphereForObstacle();


			sc_packet_playgame_player_pos player_pos;
			player_pos.size = sizeof(player_pos);
			player_pos.type = SC_PACKET_PLAYGAME_PLAYER_POS;
			player_pos.id = player_move.id;
			player_pos.camMove = cm;
			player_pos.worldMatrix = objects[player_move.id]->GetWorldMatrix();
			SendPacket(player_pos.id, reinterpret_cast<BYTE*>(&player_pos));

		}



		break;
	}

	}

}

void Player::MovetoDirection(XMFLOAT3 camlook, BYTE * packet)
{
	//어떤키가 눌렸는지?
	cs_packet_player_move player_move;
	memcpy(&player_move, packet, packet[0]);

	XMFLOAT3 campos, charpos, camLook, camRight, temp;

	// Player의 좌표를 charpos.x에 저장 (이동전 좌표)
	charpos.x = objects[player_move.id]->GetWorldMatrix()._41;
	charpos.y = objects[player_move.id]->GetWorldMatrix()._42;
	charpos.z = objects[player_move.id]->GetWorldMatrix()._43;

	// campos와 camlook을 패킷으로 받은 값으로 Set
	campos = player_move.camPos;
	camLook = player_move.camLook;


	XMMATRIX objoffset;
	XMFLOAT4X4 m;
	XMMATRIX modelScale = XMMatrixScaling(1, 1, 1);
	XMMATRIX modelRot = XMMatrixRotationZ(objects[player_move.id]->GetRealAngle());
	XMMATRIX modelOffset = XMMatrixTranslation(0, 0, 0);
	XMMATRIX finalm = modelScale*modelRot*modelOffset;

	// 회전오프셋 을 플레이어의 월드행렬을 곱해줌.
	// 해당 값만큼 플레이어 회전
	objoffset = XMLoadFloat4x4(&objects[player_move.id]->GetWorldMatrix());
	objoffset = finalm * objoffset;
	XMStoreFloat4x4(&m, objoffset);
	objects[player_move.id]->SetWorldMatrix(m);


	XMMATRIX Rot = XMMatrixRotationY(objects[player_move.id]->GetRealAngle());
	XMStoreFloat3(&objects[player_move.id]->GetLook(), XMVector3TransformNormal(XMLoadFloat3(&objects[player_move.id]->GetLook()), Rot));

	XMVECTOR vcharlook, vcamlook;

	vcharlook = XMLoadFloat3(&objects[player_move.id]->GetLook());
	vcamlook = XMLoadFloat3(&camLook);
	vcharlook = XMVector3Normalize(vcharlook);
	vcamlook = XMVector3Normalize(vcamlook);


	temp.x = objects[player_move.id]->GetWorldMatrix()._11;
	temp.y = objects[player_move.id]->GetWorldMatrix()._12;
	temp.z = objects[player_move.id]->GetWorldMatrix()._13;
	XMStoreFloat3(&temp, XMVector3Normalize(XMLoadFloat3(&temp)));
	objects[player_move.id]->SetLook(temp);

	XMFLOAT3 ang;
	XMStoreFloat3(&ang, XMVector3Dot(XMLoadFloat3(&camLook), XMLoadFloat3(&objects[player_move.id]->GetLook())));

	ang.x = ((camLook.x * objects[player_move.id]->GetLook().z) - (camLook.z * objects[player_move.id]->GetLook().x) > 0.0f) ? (float)(acos(ang.x)) : 360 - (float)(acos(ang.x));

	// 캐릭터 회전
	if (camLook.x + 0.05 > objects[player_move.id]->GetLook().x && camLook.x - 0.05 < objects[player_move.id]->GetLook().x)
	{
		if (camLook.z + 0.05 > objects[player_move.id]->GetLook().z && camLook.z - 0.05 < objects[player_move.id]->GetLook().z)
			objects[player_move.id]->SetRealAngle(0);
	}

	else if (ang.x > 357)
		objects[player_move.id]->SetRealAngle(-ROTATE);

	else
		objects[player_move.id]->SetRealAngle(ROTATE);


	bool collCheck = false;
	XMFLOAT3 cm;

	// 움직인 플레이어와 집들간의 충돌
	// 6개의 집을 검사
	for (int i = 0; i < MAX_OBSTACLE; ++i) {
		if (true == IntersectSphereOrientedBox(&objects[player_move.id]->GetSphereForObstacle(), &obstacles[i]->GetObb())) {
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
			if (objects[player_move.id]->GetWorldMatrix()._41 > (obstacles[i]->GetObb().Center.x - obstacles[i]->GetObb().Extents.x) &&
				objects[player_move.id]->GetWorldMatrix()._41 < (obstacles[i]->GetObb().Center.x + obstacles[i]->GetObb().Extents.x))
			{
				//XMVECTOR s = XMVectorReplicate(deltaTime * speed * speedSave);
				XMVECTOR l = XMLoadFloat3(&camLook);
				//XMVECTOR p = XMLoadFloat3(&charpos);
				XMVECTOR p = XMLoadFloat3(&charpos) + (XMVectorSet(camLook.x, camLook.y, camLook.z, 0) *speed*deltaTime);

				XMStoreFloat3(&charpos, p);

				cm.x = charpos.x - objects[player_move.id]->GetWorldMatrix()._41;
				cm.y = 0;
				cm.z = 0;

				objects[player_move.id]->GetWorldMatrix()._41 = charpos.x;

				Sphere test;
				test.Center.x = charpos.x;
				test.Center.y = objects[player_move.id]->GetWorldMatrix()._42;
				test.Center.z = charpos.z;
				test.Radius = objects[player_move.id]->GetSphereForObstacle().Radius;
				if (true == IntersectSphereOrientedBox(&test, &obstacles[i]->GetObb()))
					collCheck = true;
				else
					collCheck = false;


				objects[player_move.id]->UpdateSphereForObstacle();
				objects[player_move.id]->UpdateSphereForPlayer();

			}

			else
			{
				XMVECTOR s = XMVectorReplicate(deltaTime * speed * speedSave);
				XMVECTOR l = XMLoadFloat3(&camLook);
				// 					XMVECTOR p = XMLoadFloat3(&charpos);
				// 					XMStoreFloat3(&charpos, XMVectorMultiplyAdd(s, l, p));
				XMVECTOR p = XMLoadFloat3(&charpos) + (XMVectorSet(camLook.x, camLook.y, camLook.z, 0) *speed*deltaTime);
				XMStoreFloat3(&charpos, p);
				cm.x = 0;
				cm.y = 0;
				cm.z = charpos.z - objects[player_move.id]->GetWorldMatrix()._43;

				objects[player_move.id]->GetWorldMatrix()._43 = charpos.z;

				Sphere test;
				test.Center.x = charpos.x;
				test.Center.y = objects[player_move.id]->GetWorldMatrix()._42;
				test.Center.z = charpos.z;
				test.Radius = objects[player_move.id]->GetSphereForObstacle().Radius;
				if (true == IntersectSphereOrientedBox(&test, &obstacles[i]->GetObb()))
					collCheck = true;
				else
					collCheck = false;


				objects[player_move.id]->UpdateSphereForObstacle();
				objects[player_move.id]->UpdateSphereForPlayer();

			}

			sc_packet_playgame_player_pos player_pos;
			player_pos.size = sizeof(player_pos);
			player_pos.type = SC_PACKET_PLAYGAME_PLAYER_POS;
			player_pos.id = player_move.id;
			player_pos.camMove = cm;
			player_pos.worldMatrix = objects[player_move.id]->GetWorldMatrix();
			SendPacket(player_pos.id, reinterpret_cast<BYTE*>(&player_pos));

		}
	}


	// 플레이어의 충돌.
	for (int i = 0; i < MAX_OBJECT; ++i) {
		if (true == IntersectSphereSphere(&objects[player_move.id]->GetSphereForPlayer(), &objects[i]->GetSphereForPlayer()) && player_move.id != i) {
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

			if (objects[player_move.id]->GetWorldMatrix()._41 > (objects[i]->GetSphereForPlayer().Center.x - objects[i]->GetSphereForPlayer().Radius) &&
				objects[player_move.id]->GetWorldMatrix()._41 < (objects[i]->GetSphereForPlayer().Center.x + objects[i]->GetSphereForPlayer().Radius))
			{
				//XMVECTOR s = XMVectorReplicate(deltaTime * speed * speedSave);
				XMVECTOR l = XMLoadFloat3(&camLook);
				// 					XMVECTOR p = XMLoadFloat3(&charpos);
				// 					XMStoreFloat3(&charpos, XMVectorMultiplyAdd(s, l, p));
				XMVECTOR p = XMLoadFloat3(&charpos) + (XMVectorSet(camLook.x, camLook.y, camLook.z, 0) *speed*deltaTime);
				XMStoreFloat3(&charpos, p);
				cm.x = charpos.x - objects[player_move.id]->GetWorldMatrix()._41;
				cm.y = 0;
				cm.z = 0;

				objects[player_move.id]->GetWorldMatrix()._41 = charpos.x;

				Sphere test;
				test.Center.x = charpos.x;
				test.Center.y = objects[player_move.id]->GetWorldMatrix()._42;
				test.Center.z = charpos.z;
				test.Radius = objects[player_move.id]->GetSphereForObstacle().Radius;
				if (true == IntersectSphereSphere(&test, &objects[i]->GetSphereForPlayer()))
					collCheck = true;
				else
					collCheck = false;


				objects[player_move.id]->UpdateSphereForObstacle();
				objects[player_move.id]->UpdateSphereForPlayer();
			}

			else
			{
				//XMVECTOR s = XMVectorReplicate(deltaTime * speed * speedSave);
				XMVECTOR l = XMLoadFloat3(&camLook);
				// 					XMVECTOR p = XMLoadFloat3(&charpos);
				// 					XMStoreFloat3(&charpos, XMVectorMultiplyAdd(s, l, p));
				XMVECTOR p = XMLoadFloat3(&charpos) + (XMVectorSet(camLook.x, camLook.y, camLook.z, 0) *speed*deltaTime);
				XMStoreFloat3(&charpos, p);

				cm.x = 0;
				cm.y = 0;
				cm.z = charpos.z - objects[player_move.id]->GetWorldMatrix()._43;

				objects[player_move.id]->GetWorldMatrix()._43 = charpos.z;

				Sphere test;
				test.Center.x = charpos.x;
				test.Center.y = objects[player_move.id]->GetWorldMatrix()._42;
				test.Center.z = charpos.z;
				test.Radius = objects[player_move.id]->GetSphereForObstacle().Radius;
				if (true == IntersectSphereSphere(&test, &objects[i]->GetSphereForPlayer()))
					collCheck = true;
				else
					collCheck = false;


				objects[player_move.id]->UpdateSphereForObstacle();
				objects[player_move.id]->UpdateSphereForPlayer();

			}

			sc_packet_playgame_player_pos player_pos;
			player_pos.size = sizeof(player_pos);
			player_pos.type = SC_PACKET_PLAYGAME_PLAYER_POS;
			player_pos.id = player_move.id;
			player_pos.camMove = cm;
			player_pos.worldMatrix = objects[player_move.id]->GetWorldMatrix();
			SendPacket(player_pos.id, reinterpret_cast<BYTE*>(&player_pos));

		}
	}

	if (!collCheck)
	{
		//XMVECTOR s = XMVectorReplicate(deltaTime * speed * speedSave);
		XMVECTOR l = XMLoadFloat3(&camLook);
		// 			XMVECTOR p = XMLoadFloat3(&charpos);
		// 			XMStoreFloat3(&charpos, XMVectorMultiplyAdd(s, l, p));
		XMVECTOR p = XMLoadFloat3(&charpos) + (XMVectorSet(camLook.x, camLook.y, camLook.z, 0) *speed*deltaTime);
		XMStoreFloat3(&charpos, p);

		cm;
		cm.x = charpos.x - objects[player_move.id]->GetWorldMatrix()._41;
		cm.y = 0;
		cm.z = charpos.z - objects[player_move.id]->GetWorldMatrix()._43;
		objects[player_move.id]->GetWorldMatrix()._41 = charpos.x;
		objects[player_move.id]->GetWorldMatrix()._43 = charpos.z;

		objects[player_move.id]->UpdateSphereForObstacle();
		objects[player_move.id]->UpdateSphereForPlayer();

		sc_packet_playgame_player_pos player_pos;
		player_pos.size = sizeof(player_pos);
		player_pos.type = SC_PACKET_PLAYGAME_PLAYER_POS;
		player_pos.id = player_move.id;
		player_pos.worldMatrix = objects[player_move.id]->GetWorldMatrix();
		player_pos.camMove = cm;
		SendPacket(player_pos.id, reinterpret_cast<BYTE*>(&player_pos));


	}
}
