#pragma once

#define ROTATE 0.03
#define GAMETIME 8

const int FPS = 67;

// IO_OPTION
const int OP_SEND = 1;
const int OP_RECV = 0;

const int MAX_OBJECT = 21;
const int MAX_CLIENT = 8;

// Client State
const int STATE_NONE = 0;
const int STATE_LOBBY = 1;
const int STATE_ROOM = 2;
const int STATE_PLAYING = 3;

const int MAX_OBSTACLE = 67;

// NPC Events
const int NPC_MOVE = 1;
const int NPC_REST = 2;

typedef struct Npc_Event
{
	UINT id;
	int	 type;
	UINT time;

}NPC_EVENT;


// NPC EVENT COMP
class Comparison
{
public:
	bool operator() (const NPC_EVENT& e1, const NPC_EVENT& e2)
	{
		return (e1.time > e2.time);
	}
};