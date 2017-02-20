#pragma once
#include "Overlapped_Ex.h"

class CUser
{

public:
	Overlapped_Ex over_ex;
	SOCKET socket;
	string nickName;
	BYTE recv_buf[MAX_BUF_SIZE];
	UINT id;

public:
	CUser();
	~CUser();


public:
	void RecvPacket();
};



