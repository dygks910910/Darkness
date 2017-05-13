#include "stdafx.h"
#include "Mynamespace.h"


volatile HANDLE g_hIocp = NULL;
vector<Object*> objects;
vector<Obstacle*> obstacles;
float deltaTime = 1.0;
int globalTimer = 60 * GAME_TIME;

UINT playingClient = 0;
