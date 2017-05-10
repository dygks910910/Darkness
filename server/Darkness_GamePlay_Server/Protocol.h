#pragma once

const int SERVERPORT = 9000;
const int MAX_BUF_SIZE = 4000;
const int MAX_PACKET_SIZE = 255;

const int CS_UP = 1;
const int CS_DOWN = 2;
const int CS_LEFT = 3;
const int CS_RIGHT = 4;
const int CS_PACKET_START_ANIMATION = 5;
const int CS_PACKET_CLIENT_NICKNAME = 6;


const int SC_PACKET_PUT_USER = 0;
const int SC_PACKET_PLAYGAME_INIT_POS = 1;
const int SC_PACKET_PLAYGAME_PLAYER_POS = 2;
const int SC_PACKET_PLAYGAME_START_ANMATION = 3;
const int SC_PACKET_PLAYGAME_PLAYER_DIE = 4;
const int SC_PACKET_PLAYGAME_GAME_RESULT = 5;
#pragma pack(push, 1)   


// cs_packet
struct cs_packet_player_move
{
	BYTE size;
	BYTE type;
	WORD id;
	XMFLOAT3 camPos;
	XMFLOAT3 camLook;
};

const int ANIMATION_STATE_IDLE = 0;
const int ANIMATION_STATE_WORK = 1;
const int ANIMATION_STATE_ATTACK = 2;
const int ANIMATION_STATE_RUN = 3;
const int ANIMATION_STATE_DIE = 4;

struct cs_packet_player_anmation_start
{
	BYTE size;
	BYTE type;
	WORD id;
	UINT animationState;
};

struct cs_packet_player_nickname
{
	BYTE size;
	BYTE type;
	WORD id;
	WCHAR nickName[20];
};


// sc_packet
struct sc_packet_put_user
{
	BYTE size;
	BYTE type;
	WORD id;
};

struct sc_packet_playgame_init_pos
{
	BYTE size;
	BYTE type;
	WORD id;
	XMFLOAT4X4 worldMatrix;
	XMFLOAT3 camPos;
};

struct sc_packet_playgame_player_pos
{
	BYTE size;
	BYTE type;
	WORD id;
	XMFLOAT4X4 worldMatrix;
	XMFLOAT3 camMove;

};

struct sc_packet_player_animation_start
{
	BYTE size;
	BYTE type;
	WORD id;
	UINT animationState;
};

struct sc_packet_player_die
{
	BYTE size;
	BYTE type;
	WORD id;
};


// 게임 결과 구조체
struct game_result
{
	UINT id;
	UINT NPCKill;
	UINT playerKill;
	WCHAR nickName[20];
};


struct sc_packet_game_result
{
	BYTE size;
	BYTE type;
	UINT max_client;
	game_result game_result[MAX_CLIENT];
};

#pragma pack(pop)   
