#pragma once
class Room
{
private:
	static Room* instance;

public:
	static Room* GetInstance()
	{
		if (instance == nullptr)
			instance = new Room();

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
	WORD roomCreater;
	WORD maxPlayer;
	WORD curPlayer;
	WORD map;
	WORD weather;
	int readyCount;
	int maxObstacles;
	
private:
	Room();
	~Room();

public:
	//getter 
	WORD& GetRoomCreater() { return roomCreater; }
	WORD& GetMaxPlayer() { return maxPlayer; }
	WORD& GetCurPlayer() { return curPlayer; }
	WORD& GetMap() { return map; }
	WORD& GetWeather() { return weather; }
	int& GetReadyCount() { return readyCount; }

public:
	//setter
	void SetRoomCreater(const WORD& rc) { roomCreater = rc; }
	void SetMaxPlayer(const WORD& player) { maxPlayer = player; }
	void SetCurPlayer(const WORD& cp) { curPlayer = cp; }
	void SetMap(const WORD& m) { map = m; }
	void SetWeahter(const WORD& w) { weather = w; }
	void SetReadyCount(const int& n) { readyCount = n; }
public:
	void Init();
};

