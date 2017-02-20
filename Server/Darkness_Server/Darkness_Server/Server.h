#pragma once
class CServer
{
private:
	vector<thread*> workerThreads;
	thread*          acceptThread;

public:
	CServer();
	~CServer();


public:
	void Init();
	void Progress();
	void Release();

};

