#pragma once


class Lobby;
class Object;
class Obstacle;

extern volatile HANDLE g_hIocp;
extern vector<Object*> objects;
extern vector<Obstacle*> obstacles;
extern float deltaTime;
extern int globalTimer;
extern UINT playingClient;


