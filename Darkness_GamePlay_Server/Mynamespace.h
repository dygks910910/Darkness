#pragma once


class Object;
class Obstacle;

extern volatile HANDLE g_hIocp;		// iocp handle
extern vector<Object*> objects;		// objects (player + npc)
extern vector<Obstacle*> obstacles; // obstacles
extern float deltaTime;				// deltaTime
extern UINT playingClient;			// curPlayingPlayers

extern bool bIsgameOver;			// Playing -> false, Ending -> true
extern float gameOverTime;

// 게임 시작을 알리는 변수 좀이따가.
// 총 잔여시간

extern bool bIsgameStart;
extern UINT gameRestTime;
extern UINT gameStartTime;
extern volatile bool thread_running;

extern bool gameReset;
