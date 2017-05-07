#pragma once

#define SPEED 0.009
#define ROTATE 0.01


const int FPS = 60;


// IO_OPTION
const int OP_SEND = 1;
const int OP_RECV = 0;

// object pool을 위한 변수 선언
const int MAX_ROOMPLAYER = 8;
const int MAX_ROOMNPC = 16;
const int MAX_ROOM = 4;
//const int MAX_NPC = MAX_ROOM * MAX_ROOMNPC;
const int MAX_NPC = 19;
//const int MAX_CLIENT = MAX_ROOM * MAX_ROOMPLAYER;
const int MAX_CLIENT = 2;
const int MAX_OBJECT = MAX_CLIENT + MAX_NPC;


// Client State
const int STATE_NONE = 0;
const int STATE_LOBBY = 1;
const int STATE_ROOM = 2;
const int STATE_PLAYING = 3;


const int MAX_OBSTACLE = 256;

//AABB 구조체
struct AABB
{
	XMFLOAT3 center;
	XMFLOAT3 extent;
};



// NPC 이벤트 관련
const int NPC_MOVE = 1;

typedef struct Npc_Event
{
	UINT id;
	int	 type;
	UINT time;

}NPC_EVENT;

class Comparison
{
public:
	bool operator() (const NPC_EVENT& e1, const NPC_EVENT& e2)
	{
		return (e1.time > e2.time);
	}
};