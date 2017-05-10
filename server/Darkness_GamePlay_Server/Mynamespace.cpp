#include "stdafx.h"
#include "Mynamespace.h"


volatile HANDLE g_hIocp = NULL;
vector<Object*> objects;
vector<Obstacle*> obstacles;
float deltaTime = 0.0;
UINT playingClient = 0;
