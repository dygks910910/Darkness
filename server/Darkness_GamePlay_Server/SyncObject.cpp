#include "stdafx.h"
#include "SyncObject.h"

SyncObject::SyncObject()
{
	InitializeCriticalSection(&cs);
}

SyncObject::~SyncObject()
{
	DeleteCriticalSection(&cs);
}

void SyncObject::Enter()
{
	EnterCriticalSection(&cs);
}

void SyncObject::Leave()
{
	LeaveCriticalSection(&cs);
}