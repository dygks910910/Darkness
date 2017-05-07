#pragma once
class Server
{
private:
	vector<thread*> workerThreads;
	thread*         acceptThread;
	thread			timer_thread;

public:
	Server();
	~Server();


public:
	void Init();
	void Progress();
	void Release();

public:
	void LoadObstacle();
	void CreateObjectPool();
};

