#pragma once

typedef struct Overlapped_Ex
{
	WSAOVERLAPPED	overlapped;
	WSABUF			wsa_buf;
	BYTE		    packet_buf[255];
	int operation;

}OVERLAPPED_EX;