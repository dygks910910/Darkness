#pragma once

#include <winsock2.h>
#include <xnamath.h>
#include "ModelManager.h"
#pragma comment(lib, "ws2_32.lib")
#include<string.h>
#ifndef _DEBUG | DEBUG
#include<iostream>
#endif // !_DEBUG | DEBUG

extern XMFLOAT3 camtest;

#include<string>
#include<iostream>
const int MY_SERVER_PORT = 9000;

const int MAX_PACKET_SIZE = 255;
const int MAX_BUF = 4000;

const int WM_SOCKET = WM_USER + 1;

// S -> C packet
const int SC_PACKET_PUT_USER = 0;
const int SC_PACKET_PLAYGAME_INIT_POS = 1;
const int SC_PACKET_PLAYGAME_PLAYER_POS = 2;
const int SC_PACKET_PLAYGAME_START_ANMATION = 3;
const int SC_PACKET_PLAYGAME_COLLISION_TRUE = 4;


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
	XMFLOAT3 campos;
};



struct sc_packet_playgame_player_pos
{
	BYTE size;
	BYTE type;
	WORD id;
	XMFLOAT4X4 worldMatrix;
	XMFLOAT3 cammove;
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
	std::string m_strIp;
	std::string m_portNum;
	std::wstring mNickName;
	int mId;

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
	void setId(int id){mId = id;}
	int getId() { return mId; }

public:
	void SetWindowHandle(HWND h) { mHandle = h; };
	void SetIPAndPort(std::string ip, std::string port = "9000") { 
		this->m_strIp = ip;
		std::cout << this->m_strIp << std::endl; };
	void SetIPAndPortAndNickName(std::string ip, std::wstring nickName, std::string port = "9000")
	{
		this->m_strIp = ip;
		this->m_portNum = port;
		this->mNickName = nickName;
		std::cout << this->m_strIp << std::endl;
		std::cout << this->m_portNum << std::endl;
		std::wcout << this->mNickName << std::endl;

	}
private:
	NetworkMgr();
public:
	~NetworkMgr();
};