#pragma once
#include <winsock2.h>
#include <xnamath.h>
#include "ModelManager.h"
#pragma comment(lib, "ws2_32.lib")

const int MY_SERVER_PORT = 9000;

const int MAX_PACKET_SIZE = 255;
const int MAX_BUF = 4000;

const int WM_SOCKET = WM_USER + 1;

// S -> C packet
const int SC_PACKET_PUT_USER = 0;
const int SC_PACKET_PLAYGAME_INIT_POS = 1;
const int SC_PACKET_PLAYGAME_PLAYER_POS = 2;
const int SC_PACKET_PLAYGAME_START_ANMATION = 3;


#pragma pack(push, 1)   

// sc_packet
struct sc_packet_put_user
{
	BYTE size;
	BYTE type;
	WORD id;
};


struct sc_packet_playgame_init_pos
{
	BYTE size;
	BYTE type;
	WORD id;
	XMFLOAT4X4    worldMatrix;
};



struct sc_packet_playgame_player_pos
{
	BYTE size;
	BYTE type;
	WORD id;
	XMFLOAT4X4 worldMatrix;
};

struct sc_packet_player_anmation_start
{
	BYTE size;
	BYTE type;
	WORD id;
	UINT animationState;
};

#pragma pack(pop)  





class NetworkMgr
{
	SOCKET socket;
	WSABUF recv_wsa_buf;
	BYTE   recv_buf[MAX_BUF];
	BYTE    packet_buf[MAX_PACKET_SIZE];
	HWND mHandle;
	int recv_size;
	int   saved_size;


private:
	static NetworkMgr* instance;


public:
	const SOCKET& GetSock() const { return socket; }

public:
	void Initialize();
	void Release();

public:
	void ReadPacket(const SOCKET& sock);
	void ProcessPacket(BYTE* packet);

public:
	static NetworkMgr* GetInstance()
	{
		if (instance == nullptr)
			instance = new NetworkMgr();

		return instance;

	}

	void DestroyInstance()
	{
		if (instance)
		{
			delete instance;
			instance = nullptr;
		}
	}

public:
	void SetWindowHandle(HWND h) { mHandle = h; };

private:
	NetworkMgr();
public:
	~NetworkMgr();
};