#pragma once

#include "Object.h"


class NPC
	: public Object
{
public:
	NPC();
	virtual ~NPC();

private:
	float moveTime;			// �̵� �� �ð�
	float moveTickTime;		// �̵� 1���� �ɸ��� �ð�.
	float moveTickCount;	// �̵����� Count �� �ð�.
	float restTime;			// ���� �ð�

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

