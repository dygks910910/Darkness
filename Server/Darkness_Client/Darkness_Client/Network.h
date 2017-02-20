#pragma once
class Network
{
	SOCKET socket;
	WSABUF recv_wsa_buf;
	BYTE	recv_buf[MAX_BUF_SIZE];
	BYTE    packet_buf[MAX_PACKET_SIZE];

	int recv_size;
	int	saved_size;


private:
	static Network* instance;


public:
	const SOCKET& GetSock() const { return socket; }

public:
	void Initialize();
	void Release();

public:
	void ReadPacket(const SOCKET& sock);
	void ProcessPacket(BYTE* packet);

public:
	static Network* GetInstance() 
	{
		if (instance == nullptr)
			instance = new Network();

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

private:
	Network();
public:
	~Network();
};

