#include "stdafx.h"
#include "Room.h"


 
Room* Room::instance = nullptr;

Room::Room()

{

}

Room::~Room()
{
}

void Room::Init()
{
	roomCreater = 0;
	maxPlayer = 8;
	curPlayer = 0;
	map = 0;
	weather = 0;
	readyCount = 0;
}