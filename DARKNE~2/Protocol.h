#pragma once

const int SERVERPORT = 9000;				// server port num
const int MAX_BUF_SIZE = 4000;				// max_buf_Size
const int MAX_PACKET_SIZE = 255;			// temp buf

const int CS_UP = 1;						// Client packet move_up
const int CS_DOWN = 2;						// Client packet move_DOWN
const int CS_LEFT = 3;						// Client packet move_LEFT
const int CS_RIGHT = 4;						// Client packet move_RIGHT
const int CS_PACKET_START_ANIMATION = 5;	// Client packet animation
const int CS_PACKET_CLIENT_NICKNAME = 6;	// Client packet sending player nickname
const int CS_PACKET_CLIENT_DRAW_START = 7;	// Client packet player draw starting 
const int CS_PACKET_CREATER_START_CLICK = 8;

const int SC_PACKET_PUT_USER = 0;				// Server packet send ID to login user  
const int SC_PACKET_PLAYGAME_INIT_POS = 1;		// Server packet send first position from player to other players
const int SC_PACKET_PLAYGAME_PLAYER_POS = 2;	// Server packet send move position from player to other players
const int SC_PACKET_PLAYGAME_START_ANIMATION = 3;// Server packet send animation from player to other players
const int SC_PACKET_PLAYGAME_PLAYER_DIE = 4;	// Server packet send die from player to other players
const int SC_PACKET_PLAYGAME_GAME_RESULT = 5;	// Server packet send result to all players
const int SC_PACKET_PLAYGAME_TIMER_START = 6;	// Server packet send game starting point to all players
const int SC_PACKET_ROOM_DATA = 7;				// server packet send room data to all players
const int SC_PACKET_GAME_START = 8;
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

struct cs_packet_player_animation_start
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

struct cs_packet_draw_start
{
	BYTE size;
	BYTE type;
};

struct cs_packet_start_button_click
{
	BYTE size;
	BYTE type;
};

// sc_packet
struct sc_packet_put_user
{
	BYTE size;
	BYTE type;
	WORD id;
	WORD IsCreater;
};

struct sc_packet_playgame_init_pos
{
	BYTE size;
	BYTE type;
	WORD id;
	UINT isRender;
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

struct sc_packet_timer_start
{
	BYTE size;
	BYTE type;
};

struct sc_packet_room_data
{
	BYTE size;
	BYTE type;
	WORD id;
	WCHAR nickName[20];

};

struct sc_packet_game_start
{
	BYTE size;
	BYTE type;
};
#pragma pack(pop)   
