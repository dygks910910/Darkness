#include "stdafx.h"
#include "Mynamespace.h"


volatile HANDLE g_hIocp = NULL;
vector<Object*> objects;
vector<Obstacle*> obstacles;

float deltaTime = 1.0;

UINT playingClient = 0;
bool bIsgameOver = false;
float gameOverTime = 0.0;

bool bIsgameStart = false;
UINT gameRestTime = GAMETIME * 60;
UINT gameStartTime = 0;

volatile bool thread_running = false;

extern bool gameReset = false;
