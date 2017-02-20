#include "stdafx.h"
#include "User.h"


CUser::CUser()
{
	ZeroMemory(recv_buf, sizeof(recv_buf));
	ZeroMemory(&over_ex.overlapped, sizeof(over_ex.overlapped));
	over_ex.wsa_buf.buf = reinterpret_cast<char*>(recv_buf);
	over_ex.wsa_buf.len = sizeof(recv_buf);
	nickName = "NONE";
	over_ex.operation = IO_RECV;
	id = -1;
}


CUser::~CUser()
{
}

void CUser::RecvPacket()
{
	DWORD flags = 0;

	int ret_val = WSARecv(socket, &over_ex.wsa_buf, 1, NULL, &flags, &over_ex.overlapped, NULL);

	if (ret_val != 0) {
		int error = WSAGetLastError();
		if (error != WSA_IO_PENDING)
			cout << " [error] WSARecv() on " << socket << " CUser::RecvPacket() " << endl;

	}
}

