#include "stdafx.h"
#include "Server.h"
#include "ThreadFunctions.h"


CServer::CServer()
{
	Init();

}


CServer::~CServer()
{
	Release();
}

void CServer::Init()
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return;

	// IOCP �ڵ鷯�� �ʱ�ȭ�Ѵ�.
	g_hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	if (!g_hIocp) return;

	// acceptThread ����
	acceptThread = new thread{ AcceptThread };


	
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	for (int i = 0; i<(int)si.dwNumberOfProcessors * 2; i++) {
		workerThreads.push_back(new thread{ WorketThread });
	}


}
void CServer::Progress()
{
}
void CServer::Release()
{
	WSACleanup();

	acceptThread->join();

	for (auto worket : workerThreads)
		worket->join();

	workerThreads.clear();
}