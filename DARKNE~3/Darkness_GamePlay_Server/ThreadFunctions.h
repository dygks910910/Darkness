#pragma once


void AcceptThread();
void WorketThread();

void SendPacket(const UINT id, BYTE* packet);
void ReadPacket(const UINT& id, const DWORD& transferred);
void PacketProcess(const UINT id, BYTE* packet);
void SendPlayerGamePutPacket(const UINT to, const UINT from);
void SendPlayerGamePlayerPosPacket(const UINT to, const UINT from);
void SendPlayerAnimationStartBrodcast(const UINT from, const UINT stateNum);
void ProcessEvent(const NPC_EVENT& e);
