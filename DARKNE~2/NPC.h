#pragma once

#include "Object.h"


class NPC
	: public Object
{
public:
	NPC();
	virtual ~NPC();

private:
	float moveTime;			// 이동 총 시간
	float moveTickTime;		// 이동 1번에 걸리는 시간.
	float moveTickCount;	// 이동마다 Count 된 시간.
	float restTime;			// 쉬는 시간

	XMFLOAT3 camLook;
public:
	void HeartBeat();

public:
	void SetMoveTime(const float& t) { moveTime = t; }
	void SetMoveTickCount(const float& t) { moveTickCount = t; }
	void SetMoveTickTime(const float& t) { moveTickTime = t; }
	void SetRestTime(const float& t) { restTime = t; }
	void SetCamLook(const XMFLOAT3& dr) { camLook = dr; }

	float& GetMoveTime() { return moveTime; }
	float& GetMoveTickCount() { return moveTickCount; }
	float& GetMoveTickTime() { return moveTickTime; }
	float& GetRestTime() { return restTime; }

};

