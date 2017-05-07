#pragma once

#include "Overlapped_Ex.h"
#include "Object.h"

class Player
	: public Object
{
	SOCKET socket;
	string nickName;
	BYTE recv_buf[MAX_BUF_SIZE];
	UINT id;

	int cur_state;		// Client State

	BYTE packet_buf[MAX_PACKET_SIZE];
	BYTE packet_size;
	BYTE saved_size;

	XMFLOAT3 camPos;
	XMFLOAT3 mLook;
public:
	mutex cs;

public:
	Player();
	virtual ~Player();

	void Init();

	void RecvPacket();

	void SetState(const int& state) { cur_state = state; }
	void SetID(const UINT& _id) { id = _id; }
	void SetSocket(const SOCKET& _socket) { socket = _socket; }
	void SetCamPos(const XMFLOAT3& cP) { camPos = cP; }
	void SetLook(XMFLOAT3 look) { mLook = look; }


	//getter
	XMFLOAT3& GetCamPos() { return camPos; }
	UINT& GetID() { return id; }
	SOCKET& GetSocket() { return socket; }
	int& GetState() { return cur_state; }
	void ReadPacket(const UINT& id, const DWORD& transferred);
	
    void Move(const int dir, const UINT id, BYTE* packet);

	XMFLOAT3 GetLook() { return mLook; }

	float mCheckangle = 0;
};

