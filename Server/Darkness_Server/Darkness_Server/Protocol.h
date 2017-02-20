#pragma once

const int SERVERPORT = 9000;
const int MAX_BUF_SIZE = 4000;
const int MAX_PACKET_SIZE = 255;


const int PACKET_PUT_USER = 0;

const int CS_UP = 1;
const int CS_DOWN = 2;
const int CS_LEFT = 3;
const int CS_RIGHT = 4;


struct sc_packet_put_user
{
	BYTE size;
	BYTE type;
	WORD id;
};

struct cs_packet_move {
	BYTE size;
	BYTE type;
};
