#include "stdafx.h"
#include "Mynamespace.h"


volatile HANDLE g_hIocp = NULL;
vector<Object*> objects;
vector<Obstacle*> obstacles;

float deltaTime = 1.0;

UINT playingClient = 0;
bool bIsgameOver = false;
float gameOverTime = 0.0;
