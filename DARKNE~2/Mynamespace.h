#pragma once


class Lobby;
class Object;
class Obstacle;

extern volatile HANDLE g_hIocp;		// iocp handle
extern vector<Object*> objects;		// objects (player + npc)
extern vector<Obstacle*> obstacles; // obstacles
extern float deltaTime;				// deltaTime
extern UINT playingClient;			// curPlayingPlayers
extern bool bIsgameOver;			// Playing -> false, Ending -> true
extern float gameOverTime;			


