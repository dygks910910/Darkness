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
	nickName = "NONE";
	over_ex.operation = OP_RECV;
	id = -1;
	cur_state = STATE_NONE;


	camPos.x = 0;
	camPos.y = 0;
	camPos.z = 0;

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

		float angle = XMConvertToDegrees(XMVectorGetX(XMVector3AngleBetweenNormals(vcharlook, vcamlook)));


		temp.x = objects[player_move.id]->GetWorldMatrix()._11;
		temp.y = objects[player_move.id]->GetWorldMatrix()._12;
		temp.z = objects[player_move.id]->GetWorldMatrix()._13;
		XMStoreFloat3(&temp, XMVector3Normalize(XMLoadFloat3(&temp)));
		objects[player_move.id]->SetLook(temp);

		XMFLOAT3 ang; 
		XMStoreFloat3(&ang, XMVector3Dot(XMLoadFloat3(&camLook), XMLoadFloat3(&objects[player_move.id]->GetLook())));

		ang.x = ((camLook.x * objects[player_move.id]->GetLook().z) - (camLook.z * objects[player_move.id]->GetLook().x) > 0.0f) ? (float)(acos(ang.x)) : 360-(float)(acos(ang.x));
		std::cout << ang.x << std::endl;

		// 캐릭터 회전
		if (camLook.x + 0.05 > objects[player_move.id]->GetLook().x && camLook.x - 0.05 < objects[player_move.id]->GetLook().x)
		{
			if (camLook.z + 0.05 > objects[player_move.id]->GetLook().z && camLook.z - 0.05 < objects[player_move.id]->GetLook().z)
				objects[player_move.id]->SetRealAngle(0);
		}
	
		else if(ang.x > 357)
			objects[player_move.id]->SetRealAngle(-ROTATE);

		else
			objects[player_move.id]->SetRealAngle(ROTATE);


		// 충돌 안했을때
		bool collCheck = false;

		// 움직인 플레이어와 집들간의 충돌
		// 6개의 집을 검사
		for (int i = 0; i < MAX_OBSTACLE; ++i) {
			if (true == intersectAABBAABB(&objects[player_move.id]->GetAABB(), &obstacles[i]->GetAABB())) {
				collCheck = true;
				//VV = 센터의 반대점, PB플레이어가 센터점바라보는 백터
				XMFLOAT3 objcen, objextent, resultvec, BtoP;
				
				BtoP.x = 0;
				BtoP.y = 0;
				BtoP.z = 1;
				XMVECTOR vBtoP = XMLoadFloat3(&BtoP);

				objcen.x = obstacles[i]->GetAABB().center.x;
				objcen.y = 0;
				objcen.z = obstacles[i]->GetAABB().center.z;

				if (objects[player_move.id]->GetWorldMatrix()._41 > (obstacles[i]->GetAABB().center.x - obstacles[i]->GetAABB().extent.x) &&
					objects[player_move.id]->GetWorldMatrix()._41 < (obstacles[i]->GetAABB().center.x + obstacles[i]->GetAABB().extent.x))
				{
					XMVECTOR s = XMVectorReplicate(deltaTime);
					XMVECTOR l = XMLoadFloat3(&camLook);
					XMVECTOR p = XMLoadFloat3(&charpos);
					XMStoreFloat3(&charpos, XMVectorMultiplyAdd(s, l, p));

					objects[player_move.id]->GetWorldMatrix()._41 = charpos.x;
					objects[player_move.id]->UpdateCenter();


				}

				else
				{
					XMVECTOR s = XMVectorReplicate(deltaTime);
					XMVECTOR l = XMLoadFloat3(&camLook);
					XMVECTOR p = XMLoadFloat3(&charpos);
					XMStoreFloat3(&charpos, XMVectorMultiplyAdd(s, l, p));

					objects[player_move.id]->GetWorldMatrix()._43 = charpos.z;
					objects[player_move.id]->UpdateCenter();


				}


				sc_packet_playgame_player_pos player_pos;
				player_pos.size = sizeof(player_pos);
				player_pos.type = SC_PACKET_PLAYGAME_PLAYER_POS;
				player_pos.id = player_move.id;

				player_pos.worldMatrix = objects[player_move.id]->GetWorldMatrix();

				SendPacket(player_pos.id, reinterpret_cast<BYTE*>(&player_pos));

			}
		}

		if (!collCheck)
		{
			XMVECTOR s = XMVectorReplicate(deltaTime);
			XMVECTOR l = XMLoadFloat3(&camLook);
			XMVECTOR p = XMLoadFloat3(&charpos);
			XMStoreFloat3(&charpos, XMVectorMultiplyAdd(s, l, p));

			objects[player_move.id]->GetWorldMatrix()._41 = charpos.x;
			objects[player_move.id]->GetWorldMatrix()._43 = charpos.z;
			objects[player_move.id]->UpdateCenter();
			sc_packet_playgame_player_pos player_pos;
			player_pos.size = sizeof(player_pos);
			player_pos.type = SC_PACKET_PLAYGAME_PLAYER_POS;
			player_pos.id = player_move.id;
			player_pos.worldMatrix = objects[player_move.id]->GetWorldMatrix();
			SendPacket(player_pos.id, reinterpret_cast<BYTE*>(&player_pos));


		}
		break;
	}
	case CS_DOWN:
	{
		cs_packet_player_move player_move;

		memcpy(&player_move, packet, packet[0]);

		XMFLOAT3 campos, charpos, camLook, camRight, temp;

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
		std::cout << ang.x << std::endl;
		float angle = XMConvertToDegrees(XMVectorGetX(XMVector3AngleBetweenNormals(vcharlook, vcamlook)));

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
			if (true == intersectAABBAABB(&objects[player_move.id]->GetAABB(), &obstacles[i]->GetAABB())) {
				collCheck = true;

				//VV = 센터의 반대점, PB플레이어가 센터점바라보는 백터
				XMFLOAT3 objcen, objextent, resultvec, BtoP;

				BtoP.x = 0;
				BtoP.y = 0;
				BtoP.z = 1;
				XMVECTOR vBtoP = XMLoadFloat3(&BtoP);

				objcen.x = obstacles[i]->GetAABB().center.x;
				objcen.y = 0;
				objcen.z = obstacles[i]->GetAABB().center.z;

				if (objects[player_move.id]->GetWorldMatrix()._41 > (obstacles[i]->GetAABB().center.x - obstacles[i]->GetAABB().extent.x) &&
					objects[player_move.id]->GetWorldMatrix()._41 < (obstacles[i]->GetAABB().center.x + obstacles[i]->GetAABB().extent.x))
				{
					XMVECTOR s = XMVectorReplicate(deltaTime);
					XMVECTOR l = XMLoadFloat3(&camLook);
					XMVECTOR p = XMLoadFloat3(&charpos);
					XMStoreFloat3(&charpos, XMVectorMultiplyAdd(s, -l, p));

					objects[player_move.id]->GetWorldMatrix()._41 = charpos.x;
					objects[player_move.id]->UpdateCenter();

					AABB test;
					test.center.x = charpos.x;
					test.center.y = objects[player_move.id]->GetWorldMatrix()._42;
					test.center.z = charpos.z;
					test.extent = objects[player_move.id]->GetAABB().extent;
					if (true == intersectAABBAABB(&test, &obstacles[i]->GetAABB()))
						collCheck = true;
					else
						collCheck = false;


				}

				else
				{
					XMVECTOR s = XMVectorReplicate(deltaTime);
					XMVECTOR l = XMLoadFloat3(&camLook);
					XMVECTOR p = XMLoadFloat3(&charpos);
					XMStoreFloat3(&charpos, XMVectorMultiplyAdd(s, -l, p));

					objects[player_move.id]->GetWorldMatrix()._43 = charpos.z;
					objects[player_move.id]->UpdateCenter();

					AABB test;
					test.center.x = charpos.x;
					test.center.y = objects[player_move.id]->GetWorldMatrix()._42;
					test.center.z = charpos.z;
					test.extent = objects[player_move.id]->GetAABB().extent;
					if (true == intersectAABBAABB(&test, &obstacles[i]->GetAABB()))
						collCheck = true;
					else
						collCheck = false;


				}

				//////////////사이각 구하는부분//////////////////

				sc_packet_playgame_player_pos player_pos;
				player_pos.size = sizeof(player_pos);
				player_pos.type = SC_PACKET_PLAYGAME_PLAYER_POS;
				player_pos.id = player_move.id;

				player_pos.worldMatrix = objects[player_move.id]->GetWorldMatrix();

				SendPacket(player_pos.id, reinterpret_cast<BYTE*>(&player_pos));

			}
		}

		if (!collCheck)
		{
			XMVECTOR s = XMVectorReplicate(deltaTime);
			XMVECTOR l = XMLoadFloat3(&camLook);
			XMVECTOR p = XMLoadFloat3(&charpos);
			XMStoreFloat3(&charpos, XMVectorMultiplyAdd(s, -l, p));

			objects[player_move.id]->GetWorldMatrix()._41 = charpos.x;
			objects[player_move.id]->GetWorldMatrix()._43 = charpos.z;
			objects[player_move.id]->UpdateCenter();
			sc_packet_playgame_player_pos player_pos;
			player_pos.size = sizeof(player_pos);
			player_pos.type = SC_PACKET_PLAYGAME_PLAYER_POS;
			player_pos.id = player_move.id;
			player_pos.worldMatrix = objects[player_move.id]->GetWorldMatrix();
			SendPacket(player_pos.id, reinterpret_cast<BYTE*>(&player_pos));


		}

		break;
	}
	case CS_LEFT:
	{
		cs_packet_player_move player_move;

		memcpy(&player_move, packet, packet[0]);

		XMFLOAT3 campos, charpos, camLook, camRight, temp;

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
		std::cout << ang.x << std::endl;
		float angle = XMConvertToDegrees(XMVectorGetX(XMVector3AngleBetweenNormals(vcharlook, vcamlook)));

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
		// 바꿔야되는 부분
		/*if (angle < 89.6213)
			objects[player_move.id]->SetRealAngle(ROTATE);

		else if (angle > 89.6213)
			objects[player_move.id]->SetRealAngle(-ROTATE);

		else
			objects[player_move.id]->SetRealAngle(0);*/

		// 이동
		bool collCheck = false;

		// 움직인 플레이어와 집들간의 충돌


		// 6개의 집을 검사
		for (int i = 0; i < MAX_OBSTACLE; ++i) {
			if (true == intersectAABBAABB(&objects[player_move.id]->GetAABB(), &obstacles[i]->GetAABB())) {
				collCheck = true;


				//VV = 센터의 반대점, PB플레이어가 센터점바라보는 백터
				XMFLOAT3 objcen, objextent, resultvec, BtoP;

				BtoP.x = 0;
				BtoP.y = 0;
				BtoP.z = 1;
				XMVECTOR vBtoP = XMLoadFloat3(&BtoP);

				objcen.x = obstacles[i]->GetAABB().center.x;
				objcen.y = 0;
				objcen.z = obstacles[i]->GetAABB().center.z;

				if (objects[player_move.id]->GetWorldMatrix()._41 > (obstacles[i]->GetAABB().center.x - obstacles[i]->GetAABB().extent.x) &&
					objects[player_move.id]->GetWorldMatrix()._41 < (obstacles[i]->GetAABB().center.x + obstacles[i]->GetAABB().extent.x))
				{
					XMVECTOR s = XMVectorReplicate(deltaTime);
					XMVECTOR l = XMLoadFloat3(&camLook);
					XMVECTOR p = XMLoadFloat3(&charpos);
					XMStoreFloat3(&charpos, XMVectorMultiplyAdd(s, -l, p));

					objects[player_move.id]->GetWorldMatrix()._41 = charpos.x;
					objects[player_move.id]->UpdateCenter();

					AABB test;
					test.center.x = charpos.x;
					test.center.y = objects[player_move.id]->GetWorldMatrix()._42;
					test.center.z = charpos.z;
					test.extent = objects[player_move.id]->GetAABB().extent;
					if (true == intersectAABBAABB(&test, &obstacles[i]->GetAABB()))
						collCheck = true;
					else
						collCheck = false;


				}

				else
				{
					XMVECTOR s = XMVectorReplicate(deltaTime);
					XMVECTOR l = XMLoadFloat3(&camLook);
					XMVECTOR p = XMLoadFloat3(&charpos);
					XMStoreFloat3(&charpos, XMVectorMultiplyAdd(s, -l, p));

					objects[player_move.id]->GetWorldMatrix()._43 = charpos.z;
					objects[player_move.id]->UpdateCenter();

					AABB test;
					test.center.x = charpos.x;
					test.center.y = objects[player_move.id]->GetWorldMatrix()._42;
					test.center.z = charpos.z;
					test.extent = objects[player_move.id]->GetAABB().extent;
					if (true == intersectAABBAABB(&test, &obstacles[i]->GetAABB()))
						collCheck = true;
					else
						collCheck = false;


				}

				//////////////사이각 구하는부분//////////////////

				sc_packet_playgame_player_pos player_pos;
				player_pos.size = sizeof(player_pos);
				player_pos.type = SC_PACKET_PLAYGAME_PLAYER_POS;
				player_pos.id = player_move.id;

				player_pos.worldMatrix = objects[player_move.id]->GetWorldMatrix();

				SendPacket(player_pos.id, reinterpret_cast<BYTE*>(&player_pos));

			}
		}


		if (!collCheck)
		{
			XMVECTOR s = XMVectorReplicate(deltaTime);
			XMVECTOR l = XMLoadFloat3(&camLook);
			XMVECTOR p = XMLoadFloat3(&charpos);
			XMStoreFloat3(&charpos, XMVectorMultiplyAdd(s, -l, p));

			objects[player_move.id]->GetWorldMatrix()._41 = charpos.x;
			objects[player_move.id]->GetWorldMatrix()._43 = charpos.z;
			objects[player_move.id]->UpdateCenter();
			sc_packet_playgame_player_pos player_pos;
			player_pos.size = sizeof(player_pos);
			player_pos.type = SC_PACKET_PLAYGAME_PLAYER_POS;
			player_pos.id = player_move.id;
			player_pos.worldMatrix = objects[player_move.id]->GetWorldMatrix();
			SendPacket(player_pos.id, reinterpret_cast<BYTE*>(&player_pos));


		}

		break;
	}
	case CS_RIGHT:
	{
		cs_packet_player_move player_move;

		memcpy(&player_move, packet, packet[0]);

		XMFLOAT3 campos, charpos, camLook, camRight, temp;

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

		float angle = XMConvertToDegrees(XMVectorGetX(XMVector3AngleBetweenNormals(vcharlook, vcamlook)));

		temp.x = objects[player_move.id]->GetWorldMatrix()._11;
		temp.y = objects[player_move.id]->GetWorldMatrix()._12;
		temp.z = objects[player_move.id]->GetWorldMatrix()._13;
		XMStoreFloat3(&temp, XMVector3Normalize(XMLoadFloat3(&temp)));
		objects[player_move.id]->SetLook(temp);

		XMFLOAT3 ang;
		XMStoreFloat3(&ang, XMVector3Dot(XMLoadFloat3(&camLook), XMLoadFloat3(&objects[player_move.id]->GetLook())));

		ang.x = ((camLook.x * objects[player_move.id]->GetLook().z) - (camLook.z * objects[player_move.id]->GetLook().x) > 0.0f) ? (float)(acos(ang.x)) : 360 - (float)(acos(ang.x));
		std::cout << ang.x << std::endl;


		// 캐릭터 회전
		if (camLook.x + 0.05 > objects[player_move.id]->GetLook().x && camLook.x - 0.05 < objects[player_move.id]->GetLook().x)
		{
			if (camLook.z + 0.05 > objects[player_move.id]->GetLook().z && camLook.z - 0.05 < objects[player_move.id]->GetLook().z)
				objects[player_move.id]->SetRealAngle(0);
		}
		else if(ang.x > 357)
			objects[player_move.id]->SetRealAngle(-ROTATE);

		else
			objects[player_move.id]->SetRealAngle(ROTATE);

		// 바꿔야되는 부분
		/*if (angle > 89.6213)
			objects[player_move.id]->SetRealAngle(ROTATE);

		else if (angle < 89.6213)
			objects[player_move.id]->SetRealAngle(-ROTATE);

		else
			objects[player_move.id]->SetRealAngle(0);*/


		// 이동
		bool collCheck = false;

		// 움직인 플레이어와 집들간의 충돌


		// 6개의 집을 검사
		for (int i = 0; i < MAX_OBSTACLE; ++i) {
			if (true == intersectAABBAABB(&objects[player_move.id]->GetAABB(), &obstacles[i]->GetAABB())) {
				collCheck = true;


				//VV = 센터의 반대점, PB플레이어가 센터점바라보는 백터
				XMFLOAT3 objcen, objextent, resultvec, BtoP;

				BtoP.x = 0;
				BtoP.y = 0;
				BtoP.z = 1;
				XMVECTOR vBtoP = XMLoadFloat3(&BtoP);

				objcen.x = obstacles[i]->GetAABB().center.x;
				objcen.y = 0;
				objcen.z = obstacles[i]->GetAABB().center.z;

				if (objects[player_move.id]->GetWorldMatrix()._41 > (obstacles[i]->GetAABB().center.x - obstacles[i]->GetAABB().extent.x) &&
					objects[player_move.id]->GetWorldMatrix()._41 < (obstacles[i]->GetAABB().center.x + obstacles[i]->GetAABB().extent.x))
				{
					XMVECTOR s = XMVectorReplicate(deltaTime);
					XMVECTOR l = XMLoadFloat3(&camLook);
					XMVECTOR p = XMLoadFloat3(&charpos);
					XMStoreFloat3(&charpos, XMVectorMultiplyAdd(s, l, p));

					objects[player_move.id]->GetWorldMatrix()._41 = charpos.x;
					objects[player_move.id]->UpdateCenter();

					AABB test;
					test.center.x = charpos.x;
					test.center.y = objects[player_move.id]->GetWorldMatrix()._42;
					test.center.z = charpos.z;
					test.extent = objects[player_move.id]->GetAABB().extent;
					if (true == intersectAABBAABB(&test, &obstacles[i]->GetAABB()))
						collCheck = true;
					else
						collCheck = false;


				}

				else
				{
					XMVECTOR s = XMVectorReplicate(deltaTime);
					XMVECTOR l = XMLoadFloat3(&camLook);
					XMVECTOR p = XMLoadFloat3(&charpos);
					XMStoreFloat3(&charpos, XMVectorMultiplyAdd(s, l, p));

					objects[player_move.id]->GetWorldMatrix()._43 = charpos.z;
					objects[player_move.id]->UpdateCenter();

					AABB test;
					test.center.x = charpos.x;
					test.center.y = objects[player_move.id]->GetWorldMatrix()._42;
					test.center.z = charpos.z;
					test.extent = objects[player_move.id]->GetAABB().extent;
					if (true == intersectAABBAABB(&test, &obstacles[i]->GetAABB()))
						collCheck = true;
					else
						collCheck = false;


				}

				//////////////사이각 구하는부분//////////////////

				sc_packet_playgame_player_pos player_pos;
				player_pos.size = sizeof(player_pos);
				player_pos.type = SC_PACKET_PLAYGAME_PLAYER_POS;
				player_pos.id = player_move.id;

				player_pos.worldMatrix = objects[player_move.id]->GetWorldMatrix();

				SendPacket(player_pos.id, reinterpret_cast<BYTE*>(&player_pos));

			}
		}


		if (!collCheck)
		{
			XMVECTOR s = XMVectorReplicate(deltaTime);
			XMVECTOR l = XMLoadFloat3(&camLook);
			XMVECTOR p = XMLoadFloat3(&charpos);
			XMStoreFloat3(&charpos, XMVectorMultiplyAdd(s, l, p));

			objects[player_move.id]->GetWorldMatrix()._41 = charpos.x;
			objects[player_move.id]->GetWorldMatrix()._43 = charpos.z;
			objects[player_move.id]->UpdateCenter();
			sc_packet_playgame_player_pos player_pos;
			player_pos.size = sizeof(player_pos);
			player_pos.type = SC_PACKET_PLAYGAME_PLAYER_POS;
			player_pos.id = player_move.id;
			player_pos.worldMatrix = objects[player_move.id]->GetWorldMatrix();
			SendPacket(player_pos.id, reinterpret_cast<BYTE*>(&player_pos));

		}

		break;
	}
	}
}
